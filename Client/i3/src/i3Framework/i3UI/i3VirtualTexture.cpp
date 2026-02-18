#include "i3FrameworkPCH.h"
#include "i3UIManager.h"
#include "i3VirtualTexture.h"
#include "i3VirtualTextureIO.h"
#include "i3Base/itl/algorithm_util.h"


#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_char.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/substr.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"

#include "i3Base/string/compare/generic_is_iless.h"

#include "i3Base/itl/algorithm/stable_sort.h"

#include "i3VTFileFormat.h"

#include "i3UIFilePathMgr.h"


I3_CLASS_INSTANCE( i3VirtualTexture);

static INT32	_TexInfoFind( I3VT_TEXINFO * p1, UINT128 * pKey)
{
	if( p1->m_CODE > *pKey)
		return 1;
	else if( p1->m_CODE < *pKey)
		return -1;

	return 0;
}

namespace
{
	struct TexInfoFindComp
	{
		bool operator()( const I3VT_TEXINFO * p1, const UINT128& Key) const  { return  p1->m_CODE < Key ; }
		bool operator()( const UINT128& Key, const I3VT_TEXINFO * p1 ) const  { return   Key < p1->m_CODE; }
	};
}

I3VT_WEAK_TEX*	i3VTWeakTex_Create(i3Texture* pRealTex)			// 최소한 pRealTex는 레퍼런스카운트 1은 들어있어야함~.
{
	I3VT_WEAK_TEX* weak_obj = new I3VT_WEAK_TEX;

	weak_obj->m_pRealTex = pRealTex;	
	weak_obj->m_weakCount = 1;

	return weak_obj;
}

void			i3VTWeakTex_AddRef(const I3VT_WEAK_TEX* weak_tex)
{
	if (weak_tex)
		++weak_tex->m_weakCount;
}

void			i3VTWeakTex_WeakRelease(I3VT_WEAK_TEX*& weak_tex)
{
	if (!weak_tex) return;

	if ( --weak_tex->m_weakCount == 0)
	{
		if (weak_tex->m_pRealTex)
			weak_tex->m_pRealTex->Release();
		delete weak_tex;
	}
	weak_tex = nullptr;
}

void			i3VTWeakTex_StrongRelease(I3VT_WEAK_TEX*& weak_tex)
{
	if (!weak_tex) return;
	I3_SAFE_RELEASE(weak_tex->m_pRealTex);

	if (--weak_tex->m_weakCount == 0)
	{
		delete weak_tex;
	}
	weak_tex = nullptr;
}



namespace vt_detail
{
	void GetVariousUIFileName(char* res, const i3::literal_range& strDefault, int idx);

	I3VT_TILE * getTileByIndex(I3VT_TILE* TileTable, INT32 xidx, INT32 yidx)
	{
		if (xidx >= I3VT_TILE_COUNTX)
			xidx = I3VT_TILE_COUNTX - 1;

		if (yidx >= I3VT_TILE_COUNTY)
			yidx = I3VT_TILE_COUNTY - 1;

		return TileTable + (yidx * I3VT_TILE_COUNTX) + xidx;
	}

	I3VT_TILE * getTileByIndex(const i3VirtualTextureData* pData, INT32 xidx, INT32 yidx)
	{
		return vt_detail::getTileByIndex(pData->m_pTiles, xidx, yidx);
	}

	void getIndex_ByPixel(INT32 px, INT32 py, INT32 * idxX, INT32 * idxY)
	{
		*idxX = px / I3VT_TILE_SIZE;
		*idxY = py / I3VT_TILE_SIZE;
	}
	
	I3VT_TILE* getTileByPixel(const i3VirtualTextureData* pData, INT32 px, INT32 py)
	{
		return getTileByIndex(pData, px / I3VT_TILE_SIZE, py / I3VT_TILE_SIZE);
	}

	void RemoveAllRefTextureImp(i3VirtualTextureData* inoutData, i3VirtualTextureCache* inoutCache)
	{
		const size_t numRefTex = inoutData->m_linearTexInfoList.size();

		for (size_t i = 0; i < numRefTex; ++i)
		{
			I3VT_TEXINFO * pInfo = inoutData->m_linearTexInfoList[i];
			i3VTWeakTex_StrongRelease(pInfo->m_pWeakTex);
			I3_MUST_DELETE(pInfo);
		}

		inoutData->m_linearTexInfoList.clear();
		inoutData->m_str_refTexIdx_set.clear();

		if (inoutCache != nullptr)
			inoutCache->ResetCache();
	}

	// void _setRes(INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, INT32 mipmaps, INT32 TileSz)

	void setVTexInfoToCtx(i3VirtualTexture* owner, INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, INT32 mipmaps, INT32 TileSz)
	{
		i3VirtualTextureCtx& ctx = *owner->getContext();

		ctx.m_Width = w;
		ctx.m_Height = h;
		ctx.m_Format = fmt;
		ctx.m_MipmapLevel = mipmaps;

		ctx.m_TileSize = TileSz;
		ctx.m_DataSizePerTile = i3Gfx::CalcRasterBufferSize(TileSz, TileSz, 1, fmt);

		ctx.m_cntTileX = w / TileSz;
		ctx.m_cntTileY = h / TileSz;
		ctx.m_cntTileTotal = ctx.m_cntTileX * ctx.m_cntTileY;

		ctx.m_revWidth = 1.0f / w;
		ctx.m_revHeight = 1.0f / h;

		ctx.m_pVTex = owner;
	}



}


i3VirtualTexture::i3VirtualTexture(void)
{
	//  다음은 임시코드이다..
	m_pFileFormat = new i3VTFileFormat(this);// 이 변수는 널값을 허용하면 안된다..

	m_pIO = i3VirtualTextureIO::new_object();
	m_pIO->Create();

	m_Data.m_pTiles = nullptr;
	m_Data.m_linearTexInfoList.reserve(128);
}



i3VirtualTexture::~i3VirtualTexture(void)
{
	m_pIO->DestroyThread();

	delete m_pFileFormat;

	I3_SAFE_RELEASE( m_pCache);

	I3MEM_SAFE_FREE( m_Data.m_pTiles);



	vt_detail::RemoveAllRefTextureImp(&m_Data, m_pCache);

	I3_SAFE_RELEASE(m_pIO);
}


bool i3VirtualTexture::PrepareForRendering( INT32 cx, INT32 cy, bool bLegacyMode)
{
	if( m_pCache == nullptr)
	{
		m_pCache = i3VirtualTextureCache::new_object();

		// RAM Cache의 경우, VRAM Cache의 2배 크기로 설정.
		INT32 cntX, cntY;

		cntX = cx / m_Ctx.m_TileSize;
		cntY = cy / m_Ctx.m_TileSize;

		INT32 cntRAM = (cntX * cntY) * 8;

		return m_pCache->Create( cx, cy, cntRAM, this, bLegacyMode);
	}
	else
	{
		m_pCache->Resize( cx, cy);
	}

	return true;
}

#define MAXRETRIES 100
#define RETRYDELAY 100


bool	i3VirtualTexture::isReadyToWork(void) const 
{ 
	return m_pFileFormat->isReadyToWork(); 
}


void i3VirtualTexture::RemoveRefTexture( i3Texture * pTex)
{
	// 위에 AddRef에서 일부 긁어왓음....투스트라익으로 한번만 더 반복되면 함수를 뺀다..
	i3::stack_string uniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(pTex->getResourceLoadPath(), uniquePath);
	i3::make_unix_path(uniquePath);

	typedef I3VT_TEXINFO_INDEX_SET::iterator set_itearator;


	set_itearator it = m_Data.m_str_refTexIdx_set.find_other(uniquePath);

	if (it == m_Data.m_str_refTexIdx_set.end())		// 없어요...
	{
		I3PRINTLOG(I3LOG_WARN, "Given texture is not contained in the texture list (%s).", pTex->GetName());
		return;
	}

	I3VT_TEXINFO* pFoundInfo = *it;

	m_Data.m_str_refTexIdx_set.erase(it);

	if (pFoundInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "I3VT_TEXINFO* pFoundInfo == nullptr.");
		return;
	}

	i3::vu::remove(m_Data.m_linearTexInfoList, pFoundInfo);

	i3VTWeakTex_StrongRelease(pFoundInfo->m_pWeakTex);
	
	// 이때 타일 정보도 원래 함께 갱신되었어야한다..문제가 되면....해당 부분을 채워줄것...(sx,sy,ex, ey 류의 이중루프에 setRefTexture)...

	INT32 sx, sy, ex, ey;

	vt_detail::getIndex_ByPixel(pFoundInfo->m_X, pFoundInfo->m_Y, &sx, &sy);
	vt_detail::getIndex_ByPixel(pFoundInfo->m_X + pFoundInfo->m_Width - 1, 
		pFoundInfo->m_Y + pFoundInfo->m_Height - 1, &ex, &ey);

	for (INT32 y = sy; y <= ey; y++)
	{
		for (INT32 x = sx; x <= ex; x++)
		{
			I3VT_TILE* pTile = vt_detail::getTileByIndex(&m_Data, x, y);

			pTile->setTexInfo(nullptr);
			// pTile->setFileOffset(0);		// 일단 주석...
		}
	}

	delete pFoundInfo;


}


I3VT_TEXINFO* i3VirtualTexture::FindRefTexture( const char * pszPath) const
{
	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(pszPath, strUniquePath);
	i3::make_unix_path(strUniquePath);

	I3VT_TEXINFO_INDEX_SET::const_iterator it = m_Data.m_str_refTexIdx_set.find_other(strUniquePath);

	if (it == m_Data.m_str_refTexIdx_set.end())
	{
		return nullptr;
	}

	return *it;
}


INT32 i3VirtualTexture::FindRefTexture( INT32 px, INT32 py)
{
	INT32 i;
	POINT pt;

	pt.x = px;
	pt.y = py;

	for( i = 0; i < getRefTextureCount(); i++)
	{
		I3VT_TEXINFO * pTex = getRefTexture( i);

		RECT rt;

		rt.left = pTex->m_X;
		rt.top	= pTex->m_Y;
		rt.right = pTex->m_X + pTex->m_Width - 1;
		rt.bottom = pTex->m_Y + pTex->m_Height - 1;

		if( PtInRect( &rt, pt))
		{
			return i;
		}
	}

	return -1;
}




bool i3VirtualTexture::ExtractTexture( i3Texture * pTex, INT32 px, INT32 py, INT32 cx, INT32 cy)
{
	INT32 x, y;
	REAL32 scaleX, scaleY;
	i3Surface * pSurface = pTex->getSurface( 0);
	char *	pTempBuf;
	HRESULT rv;
	D3DFORMAT dformat;
	RECT destRt, srcRt;
	INT32 pitch;

	scaleX = (REAL32) pTex->GetWidth() / cx;
	scaleY = (REAL32) pTex->GetHeight() / cy;

	{
		UINT32 sz = i3Gfx::CalcRasterBufferSize(I3VT_TILE_SIZE, I3VT_TILE_SIZE, 1, getFormat());

		pTempBuf = (char *) i3MemAlloc( sz);
	}

	dformat = i3DXUT_GetNativeImageFormat( getFormat());

	if( getFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED)
	{
		// DXT 형식인 경우에 대한 처리.
		INT32 blockSz = i3Gfx::CalcRasterBufferSize( 4, 4, 1, getFormat());
		INT32 cxBlock = (m_Ctx.m_TileSize + 3) / 4;

		pitch = blockSz * cxBlock;
	}
	else
	{
		pitch = i3Gfx::CalcRasterBufferSize(I3VT_TILE_SIZE, 1, 1, getFormat());
	}

	for( y = py; y < (py + cy);)
	{
		for( x = px; x < (px + cx); )
		{
			// Load a tile
			{
				INT32 idxx, idxy;

				vt_detail::getIndex_ByPixel( x, y, &idxx, &idxy);

				INT32 idxTile = getTileIndex( idxx, idxy);

				LoadATile( idxTile, pTempBuf);
			}

			INT32 w, h;

			w = MIN(I3VT_TILE_SIZE, ((px + cx) - x));
			h = MIN(I3VT_TILE_SIZE, ((py + cy) - y));

			{
				srcRt.left		= (x % I3VT_TILE_SIZE);
				srcRt.top		= (y % I3VT_TILE_SIZE);
				srcRt.right		= min( (srcRt.left + w), I3VT_TILE_SIZE);
				srcRt.bottom	= min( (srcRt.top + h), I3VT_TILE_SIZE);
			}

			{
				destRt.left		= (x - px);
				destRt.top		= (y - py);
				destRt.right	= (destRt.left + (srcRt.right - srcRt.left));
				destRt.bottom	= (destRt.top + (srcRt.bottom - srcRt.top));

				destRt.left		= (INT32)( destRt.left * scaleX);
				destRt.top		= (INT32)( destRt.top * scaleY);
				destRt.right	= (INT32)( destRt.right * scaleX);
				destRt.bottom	= (INT32)( destRt.bottom * scaleY);
			}

			#if defined( I3G_DX)
			{
				rv = ::D3DXLoadSurfaceFromMemory( pSurface->getSurface(), NULL, &destRt,
						pTempBuf, dformat, pitch, NULL, &srcRt, D3DX_FILTER_TRIANGLE, 0);

				if( FAILED( rv))
				{
					I3TRACE( "Error !!!!!!! D3DXLoadSurfaceFromMemory\n");
				}
			}
			#else
				#error	"You have to implement the function for the platform!");
			#endif

			x = ((x + (I3VT_TILE_SIZE)) / I3VT_TILE_SIZE) * I3VT_TILE_SIZE;
		}

		y = ((y + (I3VT_TILE_SIZE)) / I3VT_TILE_SIZE) * I3VT_TILE_SIZE;
	}

	i3MemFree( pTempBuf);

	return true;
}



INT32 i3VirtualTexture::getNumberOfTiles(void)
{
	return  I3VT_TILE_COUNTXY;
}

void i3VirtualTexture::LoadATile( INT32 idx, char * pBuf)
{
	I3VT_TILE * pTile;
	UINT32 sz;

	I3_BOUNDCHK( idx, I3VT_TILE_COUNTXY);

	pTile = & m_Data.m_pTiles[ idx];

	sz = i3Gfx::CalcRasterBufferSize( I3VT_TILE_SIZE, I3VT_TILE_SIZE, 4, m_Ctx.m_Format);

	I3VT_IOHANDLE * pHandle = m_pFileFormat->getFileHandle( pTile->getSubset());
	if( pHandle == nullptr)
	{
		I3TRACE(L"Invalid subset of the virtual texture : %d (Total %d subsets)\n", pTile->getSubset(), m_pFileFormat->getFileHandleCount());
		return;
	}

	I3VT_REQ_HNDL req = m_pIO->ReqLoad( pHandle, pTile->getFileOffset(), sz, pBuf, 0);

	m_pIO->Wait( req);
}

void i3VirtualTexture::ReqLoad( I3VT_HTILE idx, I3VT_HRTILE idxRTile)
{
	I3VT_TILE * pTile;
	UINT32 sz;

	I3_BOUNDCHK( idx, I3VT_TILE_COUNTXY);

	pTile = &m_Data.m_pTiles[ idx];

	I3VT_IOHANDLE * pHandle = m_pFileFormat->getFileHandle( pTile->getSubset());

	sz = m_Ctx.m_DataSizePerTile;

	char * pLoadBuf = m_pCache->GetBufferOfRTile( idxRTile);

	VTX_LOG( "[VTX] ReqLoad( TILE(%d) -> RAM(%d)\n", idx, idxRTile);

	m_pIO->ReqLoad( pHandle, pTile->getFileOffset(), sz, pLoadBuf, idxRTile, m_pCache);
}


void i3VirtualTexture::ChangeFormat( I3G_IMAGE_FORMAT fmt)
{
	vt_detail::setVTexInfoToCtx(this, m_Ctx.m_Width, m_Ctx.m_Height, fmt, m_Ctx.m_MipmapLevel, m_Ctx.m_TileSize);

	m_pFileFormat->Defragment();
	m_pFileFormat->Flush();

	if( m_pCache != nullptr)
		m_pCache->ResetCache();
}

void i3VirtualTexture::FlushVRAM(void)
{
	// VRAM의 할당 기록을 모두 해제한다.
	if( m_pCache != nullptr)
	{
		// RAM Cache는 해제하지 않고, VRAM 및 Allocator 정보만 Flush를 한다.
		m_pCache->ResetCache( false, false);
	}
}

//
// TODO : 수정본이 제대로 동작하는지 확인해야함..
//
void i3VirtualTexture::DeleteWrongTexture( void)
{
	
	for(INT32 i = 0; i < getRefTextureCount(); i++)
	{
		I3VT_TEXINFO* pInfo = getRefTexture( i);

		if( i3System::IsFile( pInfo->m_strName.c_str()) == false)
		{
			i3::stack_string strUniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(pInfo->m_strName, strUniquePath);

			i3::stack_string resPath = i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(true);
			resPath += '/';
			resPath += strUniquePath;
			
			if( i3System::IsFile(resPath.c_str()) == false)
			{
				// 파일이 삭제된 경우
				i3VTWeakTex_StrongRelease(pInfo->m_pWeakTex);
								
				m_Data.m_linearTexInfoList.erase(m_Data.m_linearTexInfoList.begin() + i);
				m_Data.m_str_refTexIdx_set.erase(pInfo);		// 맵도 함께 지워야함..

				// 이때 타일 정보도 원래 함께 갱신되었어야한다..문제가 되면....해당 부분을 채워줄것...(sx,sy,ex, ey 류의 이중루프에 setRefTexture)...

				INT32 sx, sy, ex, ey;

				vt_detail::getIndex_ByPixel(pInfo->m_X, pInfo->m_Y, &sx, &sy);
				vt_detail::getIndex_ByPixel(pInfo->m_X + pInfo->m_Width - 1,
					pInfo->m_Y + pInfo->m_Height - 1, &ex, &ey);

				for (INT32 y = sy; y <= ey; y++)
				{
					for (INT32 x = sx; x <= ex; x++)
					{
						I3VT_TILE* pTile = vt_detail::getTileByIndex(&m_Data, x, y);

						pTile->setTexInfo(nullptr);
						// pTile->setFileOffset(0);		// 일단 주석...
					}
				}

				I3_MUST_DELETE(pInfo);

				--i;
			}
		}
	}

	//Defragment();
}


void i3VirtualTexture::Verify(void)
{
	// 전체 Tile을 돌면서..
	INT32 i;
	I3VT_TILE * pTile;
	I3VT_RTILE * pRTile;
	I3VT_VTILE * pVTile;
	I3VT_HRTILE hRTile;
	I3VT_HVTILE hVTile;

	I3TRACE( "-------------------- Verifing virtual texture -------------------\n");

	for( i = 0; i < m_Ctx.m_cntTileTotal; i++)
	{
		pTile = getTileBySerial( i);

		hRTile	= pTile->getRTile();
		hVTile	= pTile->getVTile();

		if( hRTile != -1)
		{
			if( pTile->isLoaded_RAM() == false)
			{
				I3TRACE( "INVALID TILE : [%d] : RAM : isLoaded_RAM() == false\n", i);
			}

			pRTile = m_pCache->getRTile( hRTile);

			if( pRTile->m_idxTile != i)
			{
				I3TRACE( "INVALID TILE : [%d] : RAM : RTile != Tile\n", i);
			}
		}
		else
		{
			if( pTile->isLoaded_RAM() == true)
			{
				I3TRACE( "INVALID TILE : [%d] : RAM : isLoaded_RAM() == true\n", i);
			}

			if( m_pCache->FindRTileByTile( i) != -1)
			{
				I3TRACE( "INVALID TILE : [%d] : RAM : Still RAM \n", i);
			}
		}

		if( hVTile != -1)
		{
			if( pTile->isLoaded_VRAM() == false)
			{
				I3TRACE( "INVALID TILE : [%d] : VRAM : isLoaded_VRAM() == false\n", i);
			}

			pVTile = m_pCache->getVTile( hVTile);

			if( pVTile->m_idxTile != i)
			{
				I3TRACE( "INVALID TILE : [%d] : VRAM : VTile != Tile\n", i);
			}
		}
		else
		{
			if( pTile->isLoaded_VRAM() == true)
			{
				I3TRACE( "INVALID TILE : [%d] : VRAM : isLoaded_VRAM() == true\n", i);
			}

			if( m_pCache->FindVTileByTile( i) != -1)
			{
				I3TRACE( "INVALID TILE : [%d] : VRAM : Still VRAM \n", i);
			}
		}

	}

	I3TRACE( "-------------------- End of verifing virtual texture -------------------\n");
}

void i3VirtualTexture::DumpTiles( const char * pszDir)
{
	INT32 i, cnt, x, y;
	i3Texture * pTex;

	cnt = getNumberOfTiles();

	pTex = i3Texture::new_object();
#if defined( I3_DEBUG)
	pTex->SetName( "VirtualDumpTile");
#endif
	pTex->Create( m_Ctx.m_TileSize, m_Ctx.m_TileSize, 1, getFormat(), 0);

	for( i = 0; i < cnt; i++)
	{
		I3VT_TILE * pTile = getTileBySerial( i);

		if( pTile->hasData())
		{
			pTex->Lock( 0);

			LoadATile( i, pTex->GetLockedBuffer( 0));

			pTex->Unlock( 0);

			{
				i3ImageFile file;
				char conv[256];

				x = i % I3VT_TILE_COUNTX;
				y = i / I3VT_TILE_COUNTX;

				i3::snprintf( conv, sizeof(conv), "%s/%03d_%03d_(%06d).TGA", pszDir, x, y, i);

				file.SaveTGA( conv, pTex);
			}
		}
	}

	I3_MUST_RELEASE( pTex);
}

void i3VirtualTexture::SaveVRAMCache( const char * pszPath)
{
	m_pCache->SaveVRAMCache( pszPath);
}

void i3VirtualTexture::ExtractAllContainedTextures( const char * pszPath)
{
	INT32 i;
	i3Texture * pTex;
	char szFull[ MAX_PATH], szName[MAX_PATH];

	for( i = 0; i < getRefTextureCount(); i++)
	{
		I3VT_TEXINFO * pInfo = getRefTexture( i);

		pTex = i3Texture::new_object();
#if defined( I3_DEBUG)
		pTex->SetName( "VirtualRefExtract");
#endif
		pTex->Create( pInfo->m_Width, pInfo->m_Height, 1, getFormat(), 0);

		ExtractTexture( pTex, pInfo->m_X, pInfo->m_Y, pInfo->m_Width, pInfo->m_Height);

		i3TextureDX * pDXTex = (i3TextureDX *) pTex;

		i3::extract_filetitle(pInfo->m_strName, szName);

		i3::snprintf( szFull, _countof( szFull), "%s/%s.jpg", pszPath, szName);

		::D3DXSaveTextureToFile( szFull, D3DXIFF_JPG, pDXTex->GetDXImage(), NULL);
	}
}

void i3VirtualTexture::VerifyRef2Tile(void)
{
	INT32 i, x, y, j;
	INT32 sx, sy, ex, ey;

	for( i = 0; i < getRefTextureCount(); i++)
	{
		I3VT_TEXINFO * pRef = getRefTexture( i);

		sx = pRef->m_X;
		sy = pRef->m_Y;
		ex = sx + pRef->m_Width - 1;
		ey = sy + pRef->m_Height - 1;

		// 영역이 겹치는 다른 Ref Texture가 있는지 확인
		{
			i3::pack::RECT rt0, rt1;

			rt0.left = (REAL32) sx;
			rt0.top = (REAL32) sy;
			rt0.right = (REAL32) ex;
			rt0.bottom = (REAL32) ey;

			for( j = 0; j < getRefTextureCount(); j++)
			{
				if( j == i)
					continue;

				I3VT_TEXINFO * pOther = getRefTexture( j);

				rt1.left = (REAL32) (pOther->m_X);
				rt1.top = (REAL32) (pOther->m_Y);
				rt1.right = (REAL32) (pOther->m_X + pOther->m_Width - 1);
				rt1.bottom = (REAL32) (pOther->m_Y + pOther->m_Height - 1);

				if( i3Math::RectInRect( &rt0, &rt1) != TOUCH_NONE)
				{
					I3TRACE( "Regions of two textures are overlapped. %d:(%s) - %d:(%s)\n", i, pRef->m_strName, j, pOther->m_strName);
				}
			}
		}

		for( y = sy; y < ey; y += I3VT_TILE_SIZE)
		{
			for( x = sx; x < ex; x += I3VT_TILE_SIZE)
			{
				INT32 xidx, yidx;

				xidx = (x / I3VT_TILE_SIZE);
				yidx = (y / I3VT_TILE_SIZE);

				I3VT_TILE * pTile = vt_detail::getTileByPixel(&m_Data, x, y);

				if( pTile->hasData() == false)
				{
					I3TRACE( "No Data!!!! - %s (%d, %d - %d, %d)\n", pRef->m_strName, pRef->m_X, pRef->m_Y, pRef->m_Width, pRef->m_Height);
				}

				if( pRef != pTile->getTexInfo() ) // pTile->getRefTexture() != i)
				{
					I3TRACE( "Invalid!!!! - %s (%d, %d - %d, %d)\n", pRef->m_strName, pRef->m_X, pRef->m_Y, pRef->m_Width, pRef->m_Height);
				}
			}
		}
	}
}
