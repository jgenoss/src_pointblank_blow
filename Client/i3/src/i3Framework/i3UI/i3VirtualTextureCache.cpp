#include "i3FrameworkPCH.h"
#include "i3VirtualTextureCache.h"
#include "i3VirtualTexture.h"

#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "i3Base/profile/profile.h"


namespace vt_detail
{
	I3VT_TILE * getTileByIndex(const i3VirtualTextureData* pData, INT32 xidx, INT32 yidx);
}


I3_CLASS_INSTANCE( i3VirtualTextureCache);

i3VirtualTextureCache::i3VirtualTextureCache(void)
{
	i3mem::Fill( m_TransQueue, -1, sizeof( m_TransQueue));
}

i3VirtualTextureCache::~i3VirtualTextureCache(void)
{
	ResetCache();
}

bool i3VirtualTextureCache::Create( INT32 cx, INT32 cy, INT32 cntRAMCache, i3VirtualTexture * pVTex, bool bLegacyMode)
{
	I3ASSERT( (cx % I3VT_TILE_SIZE) == 0);
	I3ASSERT( (cy % I3VT_TILE_SIZE) == 0);

	m_bLegacyMode = bLegacyMode;

	m_pVTex = pVTex;

	m_pCtx = pVTex->getContext();

	m_pCtx->m_bLegacyMode = bLegacyMode;

	// Cached Tile Information Table
	if( m_VRAM.Create( cx, cy, m_pCtx) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not initialize the Virtual Texture VRAM Cache System.");
		return false;
	}

	if( m_RAM.Create( this, cntRAMCache, m_pCtx) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not initialize the Virtual Texture RAM Cache System.");
		return false;
	}

	{
		m_LRU.Create( pVTex->getRefTextureCount());
	}

	ResetCache();

	return true;
}

bool i3VirtualTextureCache::Resize( INT32 cx, INT32 cy)
{


	bool bRv = m_VRAM.Resize( cx, cy);

	if( bRv)
	{
		ResetCache( false, false);
	}

	return bRv;
}

void i3VirtualTextureCache::ResetCache( bool bResetRAMCache, bool bByLostDevice)
{

	INT32 i;

	// 모든 Cached-Tile을 Free List로 추가.
	m_VRAM.Reset();

	m_RAM.Reset( bResetRAMCache);
	
	// Ref. Texture들의 모든 VRAM 할당 정보를 날린다.
	if( m_bLegacyMode && bByLostDevice)
	{
		for( i = 0; i < m_pVTex->getRefTextureCount(); i++)
		{
			I3VT_TEXINFO * pTexInfo = m_pVTex->getRefTexture( i);

			if( pTexInfo->m_pWeakTex != nullptr)
			{
				// Ref Texture 삭제.
				TrashTexture( pTexInfo);
			}
		}

		resetActiveRefTexture();
	}

	m_LRU.Reset();
}

bool i3VirtualTextureCache::_AddTransReq( I3VT_HRTILE idxRTile)
{

	if( m_TransQueue[ m_TQ_Head] != -1)
	{
		//I3ASSERT( m_TransQueue[ m_TQ_Head] == -1);
		return false;
	}

	m_TransQueue[ m_TQ_Head] = idxRTile;

	UINT32 idxNext;

	idxNext = m_TQ_Head + 1;

	if( idxNext >= MAX_TRANS_QUEUE)
		idxNext = 0;

	//if( idxNext == m_TQ_Tail)
	//if( ::InterlockedCompareExchange( (LONG*) &m_TQ_Tail, (LONG) m_TQ_Tail, (LONG) idxNext))
	{
		// Queue를 초과했다.
		//I3PRINTLOG(I3LOG_WARN,  "Requests of Trans. exceed the queue.");
		//I3ASSERT( idxNext != m_TQ_Tail);
	//	return false;
	}

	m_TQ_Head = idxNext;

	return true;
}

static INT32		s_Count = 0;

void i3VirtualTextureCache::ProcessTrans( REAL32 tmLimit)
{
	REAL32 tmElapsed;

	tmElapsed = 0.0f;

	tmLimit = 1.0f;

	m_Timer.GetDeltaSec();

	while( (tmElapsed < tmLimit) && m_TransQueue[ m_TQ_Tail] != -1)// && (m_TQ_Head != m_TQ_Tail))
	{
		// RAM->VRAM 전송
		INT32 idxRTile = m_TransQueue[ m_TQ_Tail];
		m_TransQueue[ m_TQ_Tail] = -1;

		if( idxRTile != -1)
		{
			I3VT_RTILE * pRTile = m_RAM.getTile( idxRTile);
			if( pRTile->m_idxTile != -1)
			{
				I3VT_TILE * pTile = m_pVTex->getTileBySerial( pRTile->m_idxTile);

				if( pTile->isLoaded_VRAM() == false)
				{
					if( m_bLegacyMode == false)
					{
						m_VRAM.Upload( pRTile->m_pBuff, pRTile->m_idxTile, pRTile->m_idxVTile);
					}
					else
					{
						INT16 x = (pRTile->m_idxVTile & 0xFFFF);
						INT16 y = (pRTile->m_idxVTile >> 16) & 0xFFFF;

					//	i3Texture * pTex = (i3Texture*)pRTile->m_pTex;
						I3VT_WEAK_TEX* pWeakTex = pRTile->m_pWeakTex;

					//	I3ASSERT( pWeakTex != nullptr);
						if (pWeakTex != nullptr)
						{
							// pRTile->m_pTex는 텍스쳐 대입전에 레퍼런스 증가해놓도록 수정했음 ..(2015.01.09.수빈)
							// 1인 경우에는 지워지는 과정 중으로 간주할수 있고, 텍스쳐 업로드를 하지 않도록 조절..
							// 2015.01.09.수빈 
							if (pWeakTex->m_pRealTex)				//  && pTex->GetRefCount() > 1) 
								m_VRAM.UploadToTex( pRTile->m_pBuff, pRTile->m_idxTile, pWeakTex->m_pRealTex, x, y);
						}

						// UploadToTex함수를 거치지 않으면 I3VT_TILE_STATE_LOADING이 해제되지 않는듯 하다..이것 명시적 제거..
						pTile->removeState(I3VT_TILE_STATE_LOADING);

						// VRAM에 올리고 RAM은 반환시켜주자.
						m_RAM.FreeBuffer( idxRTile);
						
					}
				}
			}
		}

		// Ring-buffer
		::InterlockedIncrement( (LONG*) &m_TQ_Tail);
		if( m_TQ_Tail >= MAX_TRANS_QUEUE)
			::InterlockedExchange( (LONG*) &m_TQ_Tail, 0);

		tmElapsed += m_Timer.GetDeltaSec();
	};

	if( m_bLegacyMode)
	{
		// 사용되지 않는 Texture들을 제거한다.
		TrashUnusedTexture();
	}

	m_VRAM.Update();

	m_timeLocal += m_timerIO.GetDeltaSec();

	if( m_timeLocal > 1.0f)
	{
		m_TPS = m_IOSize / m_timeLocal;
		m_TPS *= 1.0f / (1024.0f * 1024.0f);

		m_timeLocal = 0.0f;
		m_IOSize = 0;
	}
}

// i3VirtualTextureIO에서 Loading이 완료되었을 때, 호출하는 함수
//
// * 별도의 I/O Thread에서 호출된다.
//
bool i3VirtualTextureCache::CompleteLoad( char * pLoadBuf, INT32 idxRTile)
{
	I3VT_HTILE	idxTile;
	I3VT_RTILE * pRTile;

	if( m_TransQueue[ m_TQ_Head] != -1)
	{
		//I3ASSERT( m_TransQueue[ m_TQ_Head] == -1);
		return false;
	}

	pRTile = m_RAM.getTile( idxRTile);

	idxTile = pRTile->m_idxTile;

	{
		I3VT_TILE * pTile = m_pVTex->getTileBySerial( idxTile);

		//I3TRACE( "LOADED : %d\n", idxTile);

		pTile->setRTile( idxRTile);
	}
	

	m_TransQueue[ m_TQ_Head] = idxRTile;

	UINT32 idxNext;

	idxNext = m_TQ_Head + 1;

	if( idxNext >= MAX_TRANS_QUEUE)
		idxNext = 0;

	m_TQ_Head = idxNext;

	{
		m_IOSize += m_pCtx->m_DataSizePerTile;
	}

	return true;
}

bool i3VirtualTextureCache::Validate( VEC4D * pRect, VEC4D * pRange, bool bMustBeVRAM)
{

	INT32 x, y, ex, ey, sx, sy;
	INT32 cnt = 0;
	I3VT_TILE * pTile;
	REAL32 rx1, ry1, rx2, ry2;

	// 주어진 영역에 해당하는 Tile 영역의 결정.
	{
		rx1 = pRect->x * m_pVTex->getWidth();
		ry1 = pRect->y * m_pVTex->getHeight();
		rx2 = (pRect->x + pRect->z) * m_pVTex->getWidth() - 1;
		ry2 = (pRect->y + pRect->w) * m_pVTex->getHeight() - 1;

		sx = (INT32)(rx1 / I3VT_TILE_SIZE);
		sy = (INT32)(ry1 / I3VT_TILE_SIZE);
		ex = (INT32)(rx2 / I3VT_TILE_SIZE);
		ey = (INT32)(ry2 / I3VT_TILE_SIZE);
	}

	// 주어진 영역의 Tile들이 Cache에 Loading되어 있는가를 확인.
	for( y = sy; y <= ey; y++)
	{
		for( x = sx; x <= ex; x++)
		{
			pTile = vt_detail::getTileByIndex(m_pVTex->getData(), x, y);

			if( pTile->hasData() == false)
			{
				cnt++;
				INT32 px = (x * I3VT_TILE_SIZE);
				INT32 py = (y * I3VT_TILE_SIZE);

				INT32 idxRef = m_pVTex->FindRefTexture( px, py);

				if( idxRef != -1)
				{
					I3VT_TEXINFO * pRef = m_pVTex->getRefTexture( idxRef);

					I3TRACE( "Invalid Tile !!!! (%d, %d) -> (%d, %d) - %s\n", x, y, px, py, pRef->m_strName);
				}
				else
				{
					I3TRACE( "Invalid Tile !!!! (%d, %d) -> (%d, %d) - (Empty space)\n", x, y, px, py);
				}
				continue;
			}

			if( (pTile->isLoaded_VRAM() == false) && (pTile->isLoading() == false))
			{
				// VRAM에 없다.
				I3VT_HTILE idxTile = m_pVTex->getTileIndex( x, y);

				// RAM에서 확인
				if( pTile->isLoaded_RAM())
				{
					// RAM에 있다.
					if( bMustBeVRAM)
					{
						I3VT_RTILE * pRTile = m_RAM.getTile( pTile->getRTile());

						if( m_VRAM.Upload( pRTile->m_pBuff, idxTile) == false)
						{
							// VRAM Cache가 Overflow한다.
							return false;
						}
					}
					else
					{
						// bMustBeVRAM이 false인 경우는, Tile이 RAM에만 Loading되어 있어도 OK인 것으로
						// 간주한다.
						// 이것은 UI 관련 Texture가 Loading되었는지를 검사하는 단계에서만 사용된다.
						cnt++;
					}
				}
				else
				{
					// RAM에 없다!

					// Disk로부터 RAM으로의 읽어들이기를 요청해야 한다.
					m_RAM.LoadFromDisk( idxTile);

					// RAM으로의 Loading은 시간이 걸릴 수 있으며,
					// Background Thread에서 처리된다.
				}
			}
			else
			{
				// 이미 VRAM에 Loading되어 있거나, Loading 중이다.
				// 더 이상 처리할 필요 없이 Loading이 완려되기를 기다리면 된다.
				
				if( pTile->isLoaded_VRAM())
				{
					// 해당 Tile이 최근 사용됨을 기록해둔다.
					m_VRAM.UpdateLastUsedTile( pTile->getVTile());

					cnt++;
				}
			}
		}
	}

	return (cnt == ((ex - sx + 1) * (ey - sy + 1)));
}

namespace vt_detail
{
	void getIndex_ByPixel(INT32 px, INT32 py, INT32 * idxX, INT32 * idxY);
}

bool i3VirtualTextureCache::Validate_Legacy( I3VT_TEXINFO * pTex, VEC4D * pRect)
{

	INT32 x, y, ex, ey, sx, sy;
	I3VT_TILE * pTile;
	bool bAllLoaded = true;

	// 사용된 시간을 갱신
	pTex->m_LastUsed = m_pVTex->getFrameID();
	m_LRU.PushUsed( pTex, m_pVTex->getFrameID());
	
	if( pTex->m_bLoaded == 1)
	{
		// Loading이 완료된 Texture이다
		return true;
	}

	if( pTex->m_pWeakTex == nullptr)
	{
		// 실제 Texture 생성
		i3Texture * pNewTex = i3Texture::new_object();
		pNewTex->Create( pTex->m_Width, pTex->m_Height, 1, m_pVTex->getFormat(), 0);
		pNewTex->SetName( pTex->m_strName);
		if( i3::contain_string( pTex->m_strName, "blank") > -1)
			pTex->m_bBlankTexture = true;
		else
			pTex->m_bBlankTexture = false;

		pTex->m_pWeakTex = i3VTWeakTex_Create(pNewTex);
		pTex->m_bLoaded = 0;

		addActiveRefTexture( pNewTex);
	}

	// 주어진 영역에 해당하는 Tile 영역의 결정.
	{
		// 요구된 영역이 항상 Texture의 Left-Top이라는 법이 없지 않은가...
		// Texture의 Left-Top에서의 상대 위치를 계산.

		vt_detail::getIndex_ByPixel( pTex->m_X, pTex->m_Y, &sx, &sy);
		vt_detail::getIndex_ByPixel( pTex->m_X + pTex->m_Width - 1, pTex->m_Y + pTex->m_Height - 1, &ex, &ey);
	}

	// 주어진 영역의 Tile들이 Cache에 Loading되어 있는가를 확인.
	for( y = sy; y <= ey; y++)
	{
		for( x = sx; x <= ex; x++)
		{
			pTile = vt_detail::getTileByIndex(m_pVTex->getData(), x, y);

			if( pTile->hasData() == false)
				continue;

			if( pTile->isLoaded_VRAM() == false)
			{
				// VRAM에 없다.
				I3VT_HTILE idxTile = m_pVTex->getTileIndex( x, y);
				bool bNeedToLoad = false;
				
				if( pTile->isLoading())
				{
					// Loading 중에 있다.
					// 실제로 Loading중인지 체크 필요

				}
				else
				{
					if( pTile->isLoaded_RAM())
					{

						// RAM->VRAM 전송을 요청.
						I3VT_HRTILE hRTile = pTile->getRTile();

						if (hRTile == -1)
						{
							I3ASSERT(hRTile != -1);
							continue;
						}

						I3VT_RTILE * pRTile = m_RAM.getTile( hRTile);
						
						m_VRAM.UploadToTex( pRTile->m_pBuff, idxTile, (i3Texture *) pTex->m_pWeakTex->m_pRealTex, (x - sx), (y - sy));
						m_RAM.FreeBuffer( hRTile);
					}
					else
					{
						bNeedToLoad = true;
					}
				}

				if( bNeedToLoad)
				{
					// RAM에 없다!
					// Disk로부터 RAM으로의 읽어들이기를 요청해야 한다.
			//		i3Texture* pTex = (i3Texture *) pTex->m_pTex;
					m_RAM.LoadTexFromDisk( idxTile, pTex->m_pWeakTex, (x - sx), (y - sy));

					// RAM으로의 Loading은 시간이 걸릴 수 있으며,
					// Background Thread에서 처리된다.
				}

				bAllLoaded = false;
			}
		}
	}

	if( bAllLoaded)
	{
		pTex->m_bLoaded = 1;

		I3TRACE( "LEGACY : LOADED - %s\n", pTex->m_strName );

		/*
		if( bCheck)
		{
			i3Texture * pTexture = (i3Texture *) pTex->m_pTex;

			pTexture->Dump( "D:/Dump/UI_Source_1.tga");
		}
		*/
	}

	return true;
}

void i3VirtualTextureCache::TrashTexture( I3VT_TEXINFO * pTexInfo)
{

	//I3TRACE( "LEGACY : TRASH : %s\n", pTexInfo->m_szName);

	// Tile 및 RTile 정보 갱신
	{

		INT32 x, y, sx, sy, ex, ey;

		vt_detail::getIndex_ByPixel( pTexInfo->m_X, pTexInfo->m_Y, &sx, &sy);
		vt_detail::getIndex_ByPixel( pTexInfo->m_X + pTexInfo->m_Width - 1, pTexInfo->m_Y + pTexInfo->m_Height - 1, &ex, &ey);

//		for( y = sy; y <= ey; y++)
//		{
//			for( x = sx; x <= ex; x++)
//			{
//				I3VT_TILE * pTile = m_pVTex->getTileByIndex( x, y);
//
//				if ( pTile->isLoading() )			// 아직도 로딩단계에 남은 것들이 있다면 처리를 하지 않도록 조절...(이런 것들은 나중에 _Transfer함수 내부에서 삭제될것을 일단 기대 )
//				{
//					return;
//				}
//			}
//		}

		for( y = sy; y <= ey; y++)
		{
			for( x = sx; x <= ex; x++)
			{
				I3VT_TILE * pTile = vt_detail::getTileByIndex(m_pVTex->getData(), x, y);

				pTile->clearVTile();
				
				I3VT_HRTILE hrTile = pTile->getRTile();
				if (hrTile != -1)
				{
					I3VT_RTILE* rtile =	m_RAM.getTile(hrTile);
					
					i3VTWeakTex_WeakRelease(rtile->m_pWeakTex);

//					if ( rtile->m_pWeakTex )
//					{
//						i3Texture* realTex = static_cast<i3Texture*>(rtile->m_pTex);
//						I3_SAFE_RELEASE(realTex);
//						rtile->m_pTex = nullptr;
//					}
				}

				//레거시 모드일때 RAM을 Clear
				if (m_bLegacyMode)
					pTile->clearRTile();
			}
		}

	}

	// Texture 삭제
	{
	//	i3Texture * pTex = (i3Texture *) pTexInfo->m_pTex;
		I3VT_WEAK_TEX* weakTex = pTexInfo->m_pWeakTex;
		I3ASSERT( weakTex != nullptr);

		removeActiveRefTexture( weakTex->m_pRealTex);

		I3ASSERT( weakTex->m_pRealTex->GetRefCount() >= 1);				// 뽑기상점에서 보관되는 이미지 변수에 SetTexture를 통해 레퍼런스카운트가 증가되는 경우가 있었음..(일단 1이상인 것으로 ASSERT를 바꿈)
		i3VTWeakTex_StrongRelease(pTexInfo->m_pWeakTex);
		pTexInfo->m_bLoaded = 0;
	}
	
	// LRU 제거
	m_LRU.Remove( pTexInfo);
}

void i3VirtualTextureCache::TrashUnusedTexture(void)
{

	INT32 i, cnt;
	I3VT_TEXINFO * pArray[64], * pTexInfo;
	UINT32 diff, tmFrameID;

	cnt = m_LRU.GetUsedData( (void **) pArray, 64);
	tmFrameID = m_pVTex->getFrameID();

	for( i = 0; i < cnt; i++)
	{
		pTexInfo = pArray[i];

		if( pTexInfo->m_bLoaded )
		{
			diff = tmFrameID - pTexInfo->m_LastUsed;

			if( diff > 600)
			{
				// 시간 간격이 10초 이상 난다면, 사용되지 않는 Texture로 간주한다.
				if (!pTexInfo->m_bBlockAutoTrashTex)
					TrashTexture( pTexInfo);
			}
		}
	}
	//I3TRACE( "TrashUnused Texture %d\n", cnt);
}

void i3VirtualTextureCache::TrashAllTexture(void)
{

	INT32 i;

	for( i = 0; i < m_pVTex->getRefTextureCount(); i++)
	{
		I3VT_TEXINFO * pTex = m_pVTex->getRefTexture( i);

		if( pTex->m_pWeakTex != nullptr)
		{
			TrashTexture( pTex);
		}
	}
}

void i3VirtualTextureCache::RefreshTile( I3VT_HRTILE idxTile, char * pData)
{

	I3VT_TILE * pTile;

	pTile = m_pVTex->getTileBySerial( idxTile);

	if( pTile->isLoaded_RAM())
	{
		m_RAM.RefreshTile( pTile->getRTile(), pData);
	}

	if( pTile->isLoaded_VRAM())
	{
		m_VRAM.RefreshTile( idxTile, pData);
	}
}

bool i3VirtualTextureCache::Destroy( bool bLostDevice)
{

	m_VRAM.OnDestroy();

	// Lost Device 상태에서 I/O Thread 및 Queue 역시 초기화되기 때문에, 
	// 이전에 Request되었던 모든 Loading을 Clear 해주어야 한다.
	m_RAM.ClearAllIORequest();

	TrashAllTexture();

	i3mem::Fill( m_TransQueue, -1, sizeof( m_TransQueue));

	m_TQ_Head = 0;
	m_TQ_Tail = 0;

	return true;
}

bool i3VirtualTextureCache::isDestroyed( void)
{
	return m_VRAM.isDestroyed();
}

bool i3VirtualTextureCache::Revive( i3RenderContext * pCtx)
{
	//i3::vector<INT32> list;

	//m_VRAM.GetAllocatedTiles( list);

	m_VRAM.OnRevive();
	m_VRAM.Reset();

	//{
	//	
	//	INT32 idxTile;
	//	I3VT_TILE * pTile;

	//	// VRAM의 Cache Texture들이 모두 내용이 소실되었기 때문에
	//	// 다시 RAM/Disk에서 VRAM으로 전송해 주어야 한다.

	//	for(size_t i = 0; i < list.size(); i++)
	//	{
	//		idxTile = list[i];

	//		pTile = m_pVTex->getTileBySerial( idxTile);

	//		// VRAM에는 확실히 Pixel Data가 없기 때문에...

	//		// RAM에서 확인
	//		if( pTile->isLoaded_RAM())
	//		{
	//			// RAM에 있다.
	//			I3VT_RTILE * pRTile = m_RAM.getTile( pTile->getRTile());

	//			m_VRAM.Upload( pRTile->m_pBuff, idxTile, pRTile->m_idxVTile);
	//		}
	//		else
	//		{
	//			// RAM에 없다!

	//			// Disk로부터 RAM으로의 읽어들이기를 요청해야 한다.
	//			m_RAM.LoadFromDisk( idxTile);

	//			// RAM으로의 Loading은 시간이 걸릴 수 있으며,
	//			// Background Thread에서 처리된다.
	//		}
	//	}

	//	I3TRACE( "REVIVE -----End-----\n");
	//}

	// Trans Queue Reset
	m_TQ_Head = m_TQ_Tail = 0;

	return true;
}

void i3VirtualTextureCache::Dump(void)
{
	m_VRAM.Dump();
}

void i3VirtualTextureCache::SaveVRAMCache( const char * pszPath)
{
	m_VRAM.SaveCache( pszPath);
}

void i3VirtualTextureCache::GetProfileString( char * pszStr, INT32 len)
{
	char conv[256];
	
	i3::snprintf( conv, sizeof(conv), "-------------- Virtual Texture --------------\n");
	i3::generic_string_cat( pszStr, conv);

	 m_VRAM.GetProfileString( pszStr, len);
	 m_RAM.GetProfileString( pszStr, len);

	 // Dist I/O Rate
	 i3::snprintf( conv, sizeof(conv), "DISC - I/O Rate : %.1f MB/sec\n", m_TPS);
	 i3::generic_string_cat( pszStr, conv);

	 if( m_bLegacyMode)
	 {
		 char szSz[64];

		 i3::integral_to_comma_str( m_ActiveRefTexSz, szSz, _countof( szSz));
		 i3::snprintf( conv, _countof( conv), "# Legacy Tex : %d\nLegacy Tex Size : %s\n", m_ActiveRefTexCount, szSz);
		 i3::generic_string_cat( pszStr, conv);
	 }
}

void i3VirtualTextureCache::GetProfileString(i3::string& inout)
{
	char conv[256];

	i3::snprintf( conv, sizeof(conv), "-------------- Virtual Texture --------------\n");
	inout += conv;

	m_VRAM.GetProfileString( inout);
	m_RAM.GetProfileString( inout);

	// Dist I/O Rate
	i3::snprintf( conv, sizeof(conv), "DISC - I/O Rate : %.1f MB/sec\n", m_TPS);
	inout += conv;

	if( m_bLegacyMode)
	{
		char szSz[64];

		i3::integral_to_comma_str( m_ActiveRefTexSz, szSz, _countof( szSz));
		i3::snprintf( conv, _countof( conv), "# Legacy Tex : %d\nLegacy Tex Size : %s\n", m_ActiveRefTexCount, szSz);
		inout += conv;
	}


}


void i3VirtualTextureCache::resetActiveRefTexture(void)
{
	m_ActiveRefTexCount = 0;
	m_ActiveRefTexSz = 0;
}

void i3VirtualTextureCache::addActiveRefTexture( i3Texture * pTex)
{
	m_ActiveRefTexCount ++;
	m_ActiveRefTexSz += pTex->GetDataSize();
}

void i3VirtualTextureCache::removeActiveRefTexture( i3Texture * pTex)
{
	m_ActiveRefTexCount --;
	m_ActiveRefTexSz -= pTex->GetDataSize();
}
