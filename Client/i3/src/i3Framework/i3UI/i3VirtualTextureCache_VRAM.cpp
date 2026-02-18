#include "i3FrameworkPCH.h"
#include "i3VirtualTextureCache_VRAM.h"
#include "i3VirtualTexture.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/string/ext/generic_string_cat.h"

i3VirtualTextureCache_VRAM::i3VirtualTextureCache_VRAM(void)
{
	m_LRU.reserve(128);
	m_UpdateList.reserve(256);
}

i3VirtualTextureCache_VRAM::~i3VirtualTextureCache_VRAM(void)
{
	// m_pCtx는 해제되어선 안되는 Pointer

	I3_SAFE_DELETE( m_pTable);

	_DestroyTextures();
}


bool i3VirtualTextureCache_VRAM::Create( INT32 width, INT32 height, i3VirtualTextureCtx * pCtx)
{
	I3ASSERT( pCtx != nullptr);
	I3ASSERT( (width > 0) && ((width % pCtx->m_TileSize) == 0));
	I3ASSERT( (height > 0) && ((height % pCtx->m_TileSize) == 0));

	m_pCtx = pCtx;

	m_pCtx->m_CacheWidth = width;
	m_pCtx->m_CacheHeight = height;

	m_cntTileX = width / pCtx->m_TileSize;
	m_cntTileY = height / pCtx->m_TileSize;
	m_cntTile = m_cntTileX * m_cntTileY;

	// Legacy Mode에서는 이하의 생성이 모두 필요없다.
	if( pCtx->m_bLegacyMode == false)
	{
		// Constructor가 호출되도록 하기 위해 new operator 사용.
		m_pTable = new I3VT_VTILE[ m_cntTile];
	
		NetworkDump_Malloc( m_pTable, sizeof( I3VT_RTILE) * m_cntTile, __FILE__, __LINE__);

		// LRU Ring buffer
		{
			m_LRU.clear();
		}
	}

	return _CreateTextures();
}

bool i3VirtualTextureCache_VRAM::Resize( INT32 width, INT32 height)
{
	Reset();

	I3_SAFE_DELETE( m_pTable);
	//I3MEM_SAFE_FREE( m_pLRU);

	_DestroyTextures();

	return Create( width, height, m_pCtx);
}

VOID WINAPI ColorFill (D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
    *pOut = D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f);
}

void _Clear( i3Texture * pTex)
{
	IDirect3DTexture9 * pDXTex;

	pDXTex = ((i3TextureDX *) pTex)->GetDXImage();

	CHECKRESULT( ::D3DXFillTexture( pDXTex, ColorFill, NULL));
}

bool i3VirtualTextureCache_VRAM::_CreateTextures(void)
{
	INT32 w, h;

	I3ASSERT( m_pCtx != nullptr);

	w = m_cntTileX * m_pCtx->m_TileSize;
	h = m_cntTileY * m_pCtx->m_TileSize;

	if( m_pCtx->m_bLegacyMode == false)
	{
		m_pCacheTex = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pCacheTex->SetName( "VirtualCacheTex");
#endif

		//  Virtual Texture Cache를 생성할 때, I3G_USAGE_RENDERTARGET을 추가하지 않으면
		// CopySurfaceTo() 함수의 동작이 매우 느려집니다.
		I3G_USAGE usage = I3G_USAGE_FORCE_VRAM;

		if( (m_pCtx->m_Format & I3G_IMAGE_FORMAT_MASK_COMPRESSED) == 0)
			usage |= I3G_USAGE_RENDERTARGET;

		m_pCacheTex->Create( w, h, 1, m_pCtx->m_Format, usage);

		_Clear( m_pCacheTex);

		INT32 iCX, iCY;

		iCX = m_pCtx->m_cntTileX;
		iCY = m_pCtx->m_cntTileY;

		m_pIndexTex = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pIndexTex->SetName( "VirtualIndexTex");
#endif
		m_pIndexTex->Create( iCX, iCY, 1, I3G_IMAGE_FORMAT_ABGR_16F, 0);

		REAL32	cw, ch;

		cw = (REAL32) (m_pIndexTex->GetWidth() - 0);
		ch = (REAL32) (m_pIndexTex->GetHeight() - 0);
		i3Vector::Set( &m_RedirFactor, cw, ch, 1.0f / cw, 1.0f / ch);

		cw = (REAL32) (m_pCacheTex->GetWidth() - 0);
		ch = (REAL32) (m_pCacheTex->GetHeight() - 0);
		i3Vector::Set( &m_CacheFactor, cw, ch, 1.0f / cw, 1.0f / ch);

		for( INT32 i = 0; i < 2; i++)
		{
			m_pTempTex[i] = i3Texture::new_object();
#if defined( I3_DEBUG)
			m_pTempTex[i]->SetName( "VirtaulTempTex");
#endif
			m_pTempTex[i]->Create( m_pCtx->m_TileSize, m_pCtx->m_TileSize, 1, m_pCtx->m_Format, I3G_USAGE_FREQUENT_UPDATE);
		}
	}

	return true;
}

void i3VirtualTextureCache_VRAM::_DestroyTextures(void)
{
	I3_SAFE_RELEASE( m_pCacheTex);
	I3_SAFE_RELEASE( m_pIndexTex);
	I3_SAFE_RELEASE( m_pTempTex[0]);
	I3_SAFE_RELEASE( m_pTempTex[1]);
}

void i3VirtualTextureCache_VRAM::Reset(void)
{
	INT32 i;

	if( m_pTable != nullptr)
	{
		// 모든 VTile을 Free List로 구성.
		for( i = 0; i < m_cntTile; i++)
		{
			if( m_pTable[i].m_idxTile != -1)
			{
				// 만약 m_idxTile이 -1이 아닌 경우라면, VirtualTexture에 Flush 사실을 갱신해야 한다!!!
				i3VirtualTexture * pVTex = m_pCtx->m_pVTex;

				I3VT_TILE * pTile = pVTex->getTileBySerial( m_pTable[i].m_idxTile);

				pTile->clearVTile();
			}

			m_pTable[i].m_idxTile	= -1;
			m_pTable[i].m_linkNext	= i + 1;
		}

		// Link의 시작과 끝 정리.
		m_pTable[ m_cntTile - 1].m_linkNext = -1;
	}

	m_idxFreeHead = 0;

	m_LRU.clear();

	// Update List는 빈 상태로
	m_UpdateList.clear();

	m_idxUsedHead = -1;
}

static bool		s_bDump = false;

I3VT_HVTILE i3VirtualTextureCache_VRAM::AllocTile( bool bThresh)
{
	INT32 idx = -1;

	if( m_idxFreeHead != -1)
	{
		// Free List에서 가져온다.
		idx = m_idxFreeHead;

		m_idxFreeHead = m_pTable[ idx].m_linkNext;

		// Used List로 연결
		m_pTable[idx].m_linkNext = m_idxUsedHead;
		m_idxUsedHead = idx;

		m_AllocCount++;
	}
	else
	{
		// 가장 마지막으로 사용된 Tile를 검색한다.
		//I3ASSERT( m_LRU_Head != -1);
		
		I3VT_TILE * pTile = nullptr;
		i3VirtualTexture * pVTex = m_pCtx->m_pVTex;

		for(size_t idxLRU = 0; idxLRU < m_LRU.size(); idxLRU++)
		{
			idx = m_LRU[ idxLRU];

			I3ASSERT( m_pTable[idx].m_idxTile != -1);
			
			pTile = pVTex->getTileBySerial( m_pTable[idx].m_idxTile);

			if( pTile->getLastFrameID() != pVTex->getFrameID())
			{
				// 이전에 Loading되었지만, 사용되지 않는 Tile을 찾았다!
				break;
			}
		}

		I3ASSERT_RETURN(pTile, -1);

		if( pTile->getLastFrameID() == pVTex->getFrameID())
		{
			// LRU에 추가된 시점과 현재 시점이 같다는 뜻은 VRAM Cache가 Overflow할 정도로
			// 많은 Texture가 사용된다는 뜻이다.
			// 이 경우, 현재까지의 Cache로 Draw를 하고, 다시 Texture를 Loading해야 하는 경우...
			return -1;
		}

		// 기존 Tile이 사라지기 때문에 Tile 정보를 갱신해 주어야 한다.
		pTile->clearVTile();
	}

	I3ASSERT_RETURN( idx != -1, idx);

	addLRU( idx);

	return idx;
}

void i3VirtualTextureCache_VRAM::addLRU( I3VT_HVTILE idx)
{
	i3VirtualTexture * pVTex = m_pCtx->m_pVTex;
	I3VT_TILE * pTile;

	// 기존 LRU에 있다면 제거한다.
	//m_LRU.Remove( (void *) idx);
	//m_LRU.Add( (void *) idx);

	i3::vu::remove(m_LRU, idx);
	m_LRU.push_back(idx);

	if( m_pTable[idx].m_idxTile != -1)
	{
		pTile = pVTex->getTileBySerial( m_pTable[idx].m_idxTile);
		pTile->setLastFrameID(pVTex->getFrameID());
	}
}

void i3VirtualTextureCache_VRAM::DumpLRU(void)
{
	
	I3TRACE( "---- Dump VRAM LRU ----\n");

	for(size_t i = 0; i < m_LRU.size(); i++)
	{
		I3TRACE( "%d : %d\n", i, m_LRU[i]);
	}
}

void i3VirtualTextureCache_VRAM::UpdateLastUsedTile( I3VT_HVTILE hVTile)
{
	i3VirtualTexture * pVTex = m_pCtx->m_pVTex;
	I3VT_TILE * pTile;
	I3VT_HTILE idxTile;

	idxTile = m_pTable[hVTile].m_idxTile;

	I3ASSERT( idxTile != -1);

	pTile = pVTex->getTileBySerial( idxTile);

	pTile->setLastFrameID(pVTex->getFrameID());

	addLRU( hVTile);
}

void i3VirtualTextureCache_VRAM::Update(void)
{
	m_timeLocal += m_Timer.GetDeltaSec();

	if( m_timeLocal > 1.0f)
	{
		m_TPS = (m_TransferSize / m_timeLocal);
		m_TPS *= 1.0f / (1024.0f * 1024.0f);

		m_timeLocal = 0.0f;
		m_TransferCount = 0;
		m_TransferSize = 0;
	}
}

bool i3VirtualTextureCache_VRAM::Upload( char * pBuf, I3VT_HTILE idxTile, I3VT_HVTILE idxVTile)
{
	i3Texture * pSrcTex;
	I3VT_VTILE * pVTile;
	I3VT_TILE * pTile;

	{
		I3ASSERT( m_pCtx != nullptr);

		 pTile = m_pCtx->m_pVTex->getTileBySerial( idxTile);
	}

	// CTile 할당
	{
		if( idxVTile == -1)
		{
			idxVTile = AllocTile( true);

			if( idxVTile == -1)
			{
				// Cache 영역이 부족하다!
				//I3TRACE( "!!!!!!!!!!!!!!!!!! Not enough virtual texture cache !!!!!!!!!!! \n");
				return false;
			}
			else
			{
				pVTile = getTile( idxVTile);
			}
		}
		else
		{
			pVTile = getTile( idxVTile);
		}

		pVTile->m_idxTile = idxTile;

		// Tile에 VTile 할당을 기록
		pTile->setVTile( idxVTile);
		pTile->setLastFrameID(m_pCtx->m_pVTex->getFrameID());

		// Update List로 추가.
		m_UpdateList.push_back( idxVTile);

		pTile->removeState( I3VT_TILE_STATE_LOADING);
	}

	// Texture Data의 전송을 위해 Memory Texture 준비.
	pSrcTex = m_pTempTex[ m_idxTempTex];

	m_idxTempTex = (m_idxTempTex + 1) & 1;

	{
		pSrcTex->Lock( 0, I3G_LOCK_OVERWRITE | I3G_LOCK_NO_DIRTY_UPDATE);									// 5.6 ~ 6.6
		//pSrcTex->Lock( 0, I3G_LOCK_OVERWRITE);
	
		i3mem::Copy( pSrcTex->GetLockedBuffer( 0), pBuf, pSrcTex->GetDataSize());

		pSrcTex->Unlock( 0);
	}

	// CTile 영역에 Pixel data를 전송
	{
		i3Surface * pDestSurf = m_pCacheTex->getSurface( 0);
		i3Surface * pSrcSurf = pSrcTex->getSurface( 0);
		
		INT32 x, y;

		x = (idxVTile % m_cntTileX) * m_pCtx->m_TileSize;
		y = (idxVTile / m_cntTileX) * m_pCtx->m_TileSize;

		pSrcSurf->CopySurfaceTo( 0, 0, m_pCtx->m_TileSize, m_pCtx->m_TileSize, pDestSurf, x, y);
	}

	m_TransferSize += pSrcTex->GetDataSize();

	m_TransferCount++;

	return true;
}

bool i3VirtualTextureCache_VRAM::UploadToTex( char * pSrcBuf, I3VT_HTILE idxTile, i3Texture * pTex, INT32 xIdx, INT32 yIdx)
{
	I3VT_TILE * pTile;

	{
		I3ASSERT( m_pCtx != nullptr);

		 pTile = m_pCtx->m_pVTex->getTileBySerial( idxTile);
	}

	// CTile 할당
	{
		// Tile에 VTile 할당을 기록
		pTile->setVTile( 0);			// 실질적인 VTile의 Index는 아니다. 단지, VRAM에 올라갔다는 것을 의미하기 위한 상태 갱신을 위한 처리.
		pTile->setLastFrameID(m_pCtx->m_pVTex->getFrameID());
		pTile->removeState( I3VT_TILE_STATE_LOADING);
	}

	//
	{
		INT32 i;
		RECT destRt;
		UINT32 srcPitch, destPitch;
		char * pDest, * pSrc;

		destRt.left		= (xIdx * I3VT_TILE_SIZE);
		destRt.top		= (yIdx * I3VT_TILE_SIZE);
		destRt.right	= destRt.left + I3VT_TILE_SIZE;
		destRt.bottom	= destRt.top + I3VT_TILE_SIZE;

		pSrc = pSrcBuf;
		pDest = pTex->Lock( 0, 0, &destRt);
		I3ASSERT( pDest != nullptr);

		if( (pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED))
		{
			// DXT 압축
			INT32 cntBlock, y;
			INT32 blockBytes;

			// DXT Image는 항상 4x4 block 단위로 저장되기 때문에
			// 복사하는 단위도 1 pixel 높이가 아니라, 4 pixel 높이 단위로 복사해야 한다.
			blockBytes	= i3Gfx::CalcRasterBufferSize( 4, 4, 1, pTex->GetFormat());
			cntBlock = (m_pCtx->m_TileSize + 3) / 4;

			srcPitch	= blockBytes * cntBlock;
			destPitch	= blockBytes * ((pTex->GetWidth() + 3) / 4);

			for( y = 0; y < cntBlock; y++)
			{
				i3mem::Copy( pDest, pSrc, srcPitch);

				pSrc += srcPitch;
				pDest += destPitch;
			}
		}
		else
		{
			srcPitch	= i3Gfx::CalcRasterBufferSize( I3VT_TILE_SIZE, 1, 1, pTex->GetFormat());
			destPitch	= i3Gfx::CalcRasterBufferSize( pTex->GetWidth(), 1, 1, pTex->GetFormat());

			for( i = 0; i < I3VT_TILE_SIZE; i++)
			{
				i3mem::Copy( pDest, pSrc, srcPitch);

				pSrc += srcPitch;
				pDest += destPitch;
			}
		}

		pTex->Unlock( 0);

		m_TransferSize += srcPitch * I3VT_TILE_SIZE;
	}

	m_TransferCount++;

	return true;
}

void i3VirtualTextureCache_VRAM::UpdateIndirectTexture(void)
{
	INT32 idxTemp;
	INT32 x, y;
	I3VT_VTILE * pTable;

	if( m_UpdateList.size() > 0)
	{
		m_pIndexTex->Lock( 0, 0);

		for(size_t i = 0; i < m_UpdateList.size(); i++)
		{
			idxTemp = m_UpdateList[ i];

			pTable = & m_pTable[ idxTemp];

			REAL16 * pDest = (REAL16 *) (m_pIndexTex->GetLockedBuffer( 0) + (pTable->m_idxTile << 3));

			x = (idxTemp % m_cntTileX) * m_pCtx->m_TileSize;
			y = (idxTemp / m_cntTileX) * m_pCtx->m_TileSize;

			pDest[0] = REAL16((REAL32)x / m_pCacheTex->GetWidth());
			pDest[1] = REAL16((REAL32)y / m_pCacheTex->GetHeight());

			//pDest[0] = REAL16( (REAL32)x );
			//pDest[1] = REAL16( (REAL32)y );
			pDest[2] = REAL16( (REAL32) 1.0f);
		}

		m_pIndexTex->Unlock( 0);

		// 더 이상 Update될 것이 없음으로...
		m_UpdateList.clear();
	}
}

void i3VirtualTextureCache_VRAM::GetAllocatedTiles( i3::vector<INT32>& List)
{
	// 현재 할당되어 있는 Tile들의 Index를 담아 반환한다.
	//
	// 이 함수가 사용되는 경우 : Lost Device로 인해, Texture들을 새로 생성한 경우. (Only)
	INT32 idxTemp = m_idxUsedHead;
	I3VT_VTILE * pTable;

	while( idxTemp != -1)
	{
		pTable = & m_pTable[ idxTemp];

		List.push_back(pTable->m_idxTile);
		
		idxTemp = pTable->m_linkNext;
	}
}

void i3VirtualTextureCache_VRAM::OnDestroy(void)
{
	_DestroyTextures();
}

void i3VirtualTextureCache_VRAM::OnRevive(void)
{
	_CreateTextures();
}

void i3VirtualTextureCache_VRAM::SaveCache( const char * pszName)
{
	if( m_pCacheTex != nullptr)
	{
		i3TextureDX * pDXTex = (i3TextureDX *) m_pCacheTex;

		::D3DXSaveTextureToFile( pszName, D3DXIFF_PNG, pDXTex->GetDXImage(), NULL);
	}
}

void i3VirtualTextureCache_VRAM::Dump(void)
{
	INT32 idx, cnt;
	I3VT_VTILE * pVTile;
	i3VirtualTexture * pVTex = m_pCtx->m_pVTex;

	I3TRACE( "---------------- VCache Dump -------------------\n");
	I3TRACE( "* Total tile count : %d\n", m_cntTileX * m_cntTileY);
	I3TRACE( "* Total Allocation count : %d\n", m_AllocCount);
	I3TRACE( "* Total Transfer count : %d\n", m_TransferCount);

	// Used blocks
	{
		for( idx = m_idxUsedHead, cnt = 0; idx != -1;)
		{
			cnt++;

			pVTile = getTile( idx);
			
			idx = pVTile->m_linkNext;
		}

		I3TRACE( "* Used tile count : %d\n", cnt);
	}

	// Free blocks
	{
		for( idx = m_idxFreeHead, cnt = 0; idx != -1;)
		{
			cnt++;

			pVTile = getTile( idx);
			if( pVTile == nullptr)
				break;
			idx = pVTile->m_linkNext;
		}

		I3TRACE( "* Free tile count : %d\n", cnt);
	}

	if( m_pCacheTex != nullptr)
	{
		i3TextureDX * pDXTex = (i3TextureDX *) m_pCacheTex;

		::D3DXSaveTextureToFile( "D:/Dump/VTex_Cache.TGA", D3DXIFF_TGA, pDXTex->GetDXImage(), NULL);
	}

	if( m_pIndexTex != nullptr)
	{
		i3TextureDX * pDXTex = (i3TextureDX *) m_pIndexTex;

		::D3DXSaveTextureToFile( "D:/Dump/VTex_Index.HDR", D3DXIFF_HDR, pDXTex->GetDXImage(), NULL);
	}

#if defined( I3_WINDOWS) && defined( I3_DEBUG)
	{
		HDC dc, srcDC;
		HBITMAP hBmp, hOldBmp;
		HBRUSH hRedBrush, hGrayBrush, hGreenBrush, hBlueBrush, hDarkGrayBrush;
		void * pPixel;

		INT32 w = m_cntTileX * m_pCtx->m_TileSize;
		INT32 h = m_cntTileY * m_pCtx->m_TileSize;

		srcDC = ::GetDC( NULL);

		dc = CreateCompatibleDC( srcDC);

		::ReleaseDC( NULL, srcDC);

		{
			BITMAPINFO bmi;

			memset( &bmi, 0, sizeof(bmi));

			bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = w;
			bmi.bmiHeader.biHeight = h;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;

			hBmp = ::CreateDIBSection( dc, &bmi, DIB_RGB_COLORS, &pPixel, NULL, 0);

			hOldBmp = (HBITMAP) ::SelectObject( dc, hBmp);
		}

		{
			hRedBrush = ::CreateSolidBrush( RGB( 255, 0, 0));
			hGreenBrush = ::CreateSolidBrush( RGB( 0, 255, 0));
			hBlueBrush = ::CreateSolidBrush( RGB( 0, 0, 255));
			hGrayBrush = ::CreateSolidBrush( RGB( 128, 128, 128));
			hDarkGrayBrush = ::CreateSolidBrush( RGB( 32, 32, 32));
		}

		{
			CRect rt( 0, 0, w, h);

			::FillRect( dc, &rt, hDarkGrayBrush);
		}

		{
			char conv[32];
			CRect rt;
			HBRUSH hBrush;

			SetBkMode( dc, TRANSPARENT);

			for( idx = m_idxUsedHead; idx != -1;)
			{
				pVTile = getTile( idx);

				I3VT_TILE * pTile = pVTex->getTileBySerial( pVTile->m_idxTile);

				rt.left		= (idx % m_cntTileX) * m_pCtx->m_TileSize;
				rt.top		= (idx / m_cntTileX) * m_pCtx->m_TileSize;
				rt.right	= rt.left + m_pCtx->m_TileSize;
				rt.bottom	= rt.top + m_pCtx->m_TileSize;

				if( pTile->isLoaded_VRAM())
					hBrush = hRedBrush;
				else if( pTile->isLoaded_RAM())
					hBrush = hGreenBrush;
				else if( pTile->isLoading())
					hBrush = hBlueBrush;
				else
					hBrush = hGrayBrush;

				::FillRect( dc, rt, hBrush);
				
				i3::snprintf( conv, sizeof(conv), "%d", idx);
				::DrawText( dc, conv, -1, rt, DT_CENTER | DT_VCENTER);

				pVTile = getTile( idx);
				idx = pVTile->m_linkNext;
			}
		}

		{
			UINT32 sz;

			i3Texture * pTex = i3Texture::new_object();
#if defined( I3_DEBUG)
			pTex->SetName( "VirtaulDumpFileTex");
#endif

			pTex->Create( w, h, 1, I3G_IMAGE_FORMAT_BGRX_8888, 0);
			pTex->Lock( 0);

			sz = i3Gfx::CalcRasterBufferSize( w, h, 1, I3G_IMAGE_FORMAT_BGRX_8888);
			memcpy( pTex->GetLockedBuffer( 0), pPixel, sz);

			pTex->Unlock( 0);

			i3ImageFile file;

			file.SaveTGA( "D:/Dump/VTex_CacheUsage.TGA", pTex);

			I3_MUST_RELEASE( pTex);
		}

		{
			::SelectObject( dc, hOldBmp);

			::DeleteObject( hRedBrush);
			::DeleteObject( hGreenBrush);
			::DeleteObject( hBlueBrush);
			::DeleteObject( hGrayBrush);
			::DeleteObject( hDarkGrayBrush);
			::DeleteObject( hBmp);

			::DeleteDC( dc);
		}

	}
#endif
}

I3VT_HVTILE i3VirtualTextureCache_VRAM::FindVTileByTile( I3VT_HTILE hTile)
{
	INT32 i;

	for( i = 0; i < m_cntTile; i++)
	{
		if( m_pTable[ i].m_idxTile == hTile)
			return i;
	}

	return -1;
}

void i3VirtualTextureCache_VRAM::RefreshTile( I3VT_HTILE idxTile, char * pData)
{
	I3VT_TILE * pTile;

	{
		I3ASSERT( m_pCtx != nullptr);

		pTile = m_pCtx->m_pVTex->getTileBySerial( idxTile);
	}

	I3ASSERT( pTile != nullptr);
	I3ASSERT( pTile->isLoaded_VRAM());

	Upload( pData, idxTile, pTile->getVTile());
}

void i3VirtualTextureCache_VRAM::GetProfileString( char * pszStr, INT32 len)
{
	REAL32 totalMem, allocMem;
	char conv[256];


	totalMem = (REAL32) i3Gfx::CalcRasterBufferSize( m_cntTileX * m_pCtx->m_TileSize, m_cntTileY * m_pCtx->m_TileSize, 1, m_pCtx->m_Format);
	allocMem = (REAL32) i3Gfx::CalcRasterBufferSize( m_AllocCount * m_pCtx->m_TileSize, m_pCtx->m_TileSize, 1, m_pCtx->m_Format);

	totalMem *= 1.0f / (1024.0f * 1024.0f);
	allocMem *= 1.0f / (1024.0f * 1024.0f);

	// Memory Usage State (Allocated / Total)
	i3::snprintf( conv, sizeof(conv), "VRAM - Memory Usage : %.1f/%.1f MB (%dx%d)\n", allocMem, totalMem, m_pCtx->m_CacheWidth, m_pCtx->m_CacheHeight);
	i3::generic_string_cat( pszStr, conv);

	// Transfer Rate (mps)
	i3::snprintf( conv, sizeof(conv), "RAM to VRAM  Transfer : %.1f MB/sec (%d tiles)\n", m_TPS, m_TransferCount);
	i3::generic_string_cat( pszStr, conv);
}

void i3VirtualTextureCache_VRAM::GetProfileString( i3::string& inout)
{
	REAL32 totalMem, allocMem;
	char conv[256];


	totalMem = (REAL32) i3Gfx::CalcRasterBufferSize( m_cntTileX * m_pCtx->m_TileSize, m_cntTileY * m_pCtx->m_TileSize, 1, m_pCtx->m_Format);
	allocMem = (REAL32) i3Gfx::CalcRasterBufferSize( m_AllocCount * m_pCtx->m_TileSize, m_pCtx->m_TileSize, 1, m_pCtx->m_Format);

	totalMem *= 1.0f / (1024.0f * 1024.0f);
	allocMem *= 1.0f / (1024.0f * 1024.0f);

	// Memory Usage State (Allocated / Total)
	i3::snprintf( conv, sizeof(conv), "VRAM - Memory Usage : %.1f/%.1f MB (%dx%d)\n", allocMem, totalMem, m_pCtx->m_CacheWidth, m_pCtx->m_CacheHeight);
	inout += conv;

	// Transfer Rate (mps)
	i3::snprintf( conv, sizeof(conv), "RAM to VRAM  Transfer : %.1f MB/sec (%d tiles)\n", m_TPS, m_TransferCount);
	inout += conv;
}
