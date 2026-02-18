#include "i3FrameworkPCH.h"
#include "i3VirtualTextureCache_RAM.h"
#include "i3VirtualTexture.h"

#define BUFF_RESERVE_SIZE		128

i3VirtualTextureCache_RAM::i3VirtualTextureCache_RAM(void)
{
	m_FreeBufferList.reserve( BUFF_RESERVE_SIZE);
}

i3VirtualTextureCache_RAM::~i3VirtualTextureCache_RAM(void)
{
	// m_pCtx는 직접 할당한 버퍼가 아니다. 해제 불가.
	for (INT32 i =0 ; i < m_MaxTileCount; ++i)
	{
		i3VTWeakTex_WeakRelease(m_pInfo[i].m_pWeakTex);

//		if (m_pInfo[i].m_pWeakTex != nullptr)
//		{
//			i3Texture* pRealTex = static_cast<i3Texture*>(m_pInfo[i].m_pTex);
//			pRealTex->Release();
//		}
	}

	I3_SAFE_DELETE_ARRAY( m_pInfo);			// 배열이므로 그냥 DELETE일리 없다.(2015.01.09.수빈)

	I3MEM_SAFE_FREE( m_pBuffer);

	for( UINT32 i = 0; i < m_BuffList.size(); i++)
	{
		char * pBuff = m_BuffList.at( i);
		I3MEM_SAFE_FREE( pBuff);
	}
}

bool i3VirtualTextureCache_RAM::Create( i3VirtualTextureCache * pCache, INT32 cntTile, i3VirtualTextureCtx * pCtx)
{
	INT32 i;

	I3ASSERT( pCtx != nullptr);
	I3ASSERT( cntTile > 0);

	m_pCtx = pCtx;
	m_pHostCache = pCache;

	m_MaxTileCount = cntTile;

	// Constructor를 호출하기 위해 new operator를 사용
	m_pInfo = new I3VT_RTILE[ m_MaxTileCount];
	if( m_pInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not allocate a memory buffer for the Virtual texture RAM cache.");
		return false;
	}

	NetworkDump_Malloc( m_pInfo, sizeof( I3VT_RTILE) * m_MaxTileCount, __FILE__, __LINE__);

	if( pCtx->m_bLegacyMode == false)
	{
		UINT32 sz;

		sz = m_pCtx->m_DataSizePerTile * m_MaxTileCount;

		I3TRACE( "[VTEXSYS] - RAM Cache : %d bytes are used\n", sz);

		m_pBuffer = (char *) i3MemAlloc( sz);

		// 할당된 Buffer Pointer를 각 Tile에 맞게 설정.
		for( i = 0; i < m_MaxTileCount; i++)
		{
			m_pInfo[i].m_pBuff	=	m_pBuffer + (i * m_pCtx->m_DataSizePerTile);
		}
	}
	else
	{
		AllocBuffer();
	}

	// LRU Buffer
	{
		m_LRU.Create( m_MaxTileCount);
	}

	Reset();

	return true;
}

void i3VirtualTextureCache_RAM::AllocBuffer( void)
{
	char * pBuff = (char *) i3MemAlloc( m_pCtx->m_DataSizePerTile * BUFF_RESERVE_SIZE);
	m_BuffList.push_back( pBuff);

	for( UINT32 i = 0; i < BUFF_RESERVE_SIZE; i++)
	{
		m_FreeBufferList.push_back( pBuff + (m_pCtx->m_DataSizePerTile * i) );
	}
}

void i3VirtualTextureCache_RAM::Reset( bool bColdReset)
{
	INT32 i;
	i3VirtualTexture * pVTex = m_pCtx->m_pVTex;

	if( bColdReset )
	{
		// 모든 RTile들을 Free 상태로 돌린다.
		for( i = 0; i < m_MaxTileCount; i++)
		{
			if( m_pInfo[i].m_idxTile != -1)
			{
				// 기존에 RAM에 Loading되어 있던 Tile에 대해서 초기화
				I3VT_TILE * pTile = pVTex->getTileBySerial( m_pInfo[i].m_idxTile);

				pTile->clearRTile();
			}

			m_pInfo[i].m_idxNext	=	i + 1;

			i3VTWeakTex_WeakRelease(m_pInfo[i].m_pWeakTex);
//			if (m_pInfo[i].m_pTex != nullptr)
//			{
//				static_cast<i3Texture*>(m_pInfo[i].m_pTex)->Release();
//				m_pInfo[i].m_pTex = nullptr;
//			}
		}

		m_pInfo[ m_MaxTileCount - 1].m_idxNext = -1;

		m_idxFreeHead = 0;
		m_idxUsedHead = -1;

		// LRU 초기화
		m_LRU.Reset();
	}
	else
	{
		// 단지 VRAM에 할당된 영역만 Clear 한다.
		for( i = 0; i < m_MaxTileCount; i++)
		{
			m_pInfo[i].m_idxVTile = -1;

			i3VTWeakTex_WeakRelease(m_pInfo[i].m_pWeakTex);
//			if (m_pInfo[i].m_pTex != nullptr)
//			{
//				static_cast<i3Texture*>(m_pInfo[i].m_pTex)->Release();
//				m_pInfo[i].m_pTex = nullptr;
//			}
		}
	}
}

I3VT_HRTILE i3VirtualTextureCache_RAM::Alloc(void)
{
	I3VT_HRTILE		idx = -1;

	// Free List 확인
	if( m_idxFreeHead != -1)
	{
		// 빈 tile이 있다.
		idx = m_idxFreeHead;

		m_idxFreeHead = m_pInfo[idx].m_idxNext;
	}
	else
	{
		// 기존에 사용되던 Tile 중, 가장 나중의 것을 선택
		idx = (INT32) m_LRU.PopOldest();

		// 기존 Tile이 사라지기 때문에 Tile 정보를 갱신해 주어야 한다.
		FreeBuffer( idx);
	}

	m_pInfo[idx].m_idxNext = m_idxUsedHead;
	m_pInfo[idx].m_idxTile = -1;
	m_pInfo[idx].m_idxVTile = -1;
	I3ASSERT(m_pInfo[idx].m_pWeakTex == nullptr);

	if( m_pCtx->m_bLegacyMode)
	{
		if( m_FreeBufferList.size() == 0)
			AllocBuffer();

		m_pInfo[idx].m_pBuff = m_FreeBufferList.back();
		m_FreeBufferList.erase( m_FreeBufferList.end() - 1);
	}

	m_idxUsedHead = idx;

	m_LRU.PushUsed( (void *) idx, m_pCtx->m_pVTex->getFrameID());

	return idx;
}

void i3VirtualTextureCache_RAM::FreeBuffer( I3VT_HRTILE idx)
{
	i3VTWeakTex_WeakRelease(m_pInfo[idx].m_pWeakTex);

//	if (m_pInfo[idx].m_pTex != nullptr)
//	{
//		i3Texture* pRealTex = static_cast<i3Texture*>(m_pInfo[idx].m_pTex);
//		pRealTex->Release();
//		m_pInfo[idx].m_pTex = nullptr;
//	}

	if( m_pInfo[idx].m_pBuff != nullptr)
	{
		m_FreeBufferList.push_back( m_pInfo[idx].m_pBuff);
		m_pInfo[idx].m_pBuff = nullptr;
	}

	if( m_pInfo[idx].m_idxTile != -1)
	{// 로딩이 안되어 있을 수도 있다.
		i3VirtualTexture * pVTex = m_pCtx->m_pVTex;
		I3VT_TILE * pTile = pVTex->getTileBySerial( m_pInfo[idx].m_idxTile);
		pTile->clearRTile();
	}
}

void i3VirtualTextureCache_RAM::_LoadFromDisk( I3VT_HTILE idxTile, I3VT_WEAK_TEX* pWeakTex, I3VT_HVTILE idxVTile)
{
	I3VT_TILE * pTile;
	I3VT_HRTILE		idxRTile;
	i3VirtualTexture * pVTex;

	I3ASSERT( m_pCtx != nullptr);
	I3ASSERT( m_pCtx->m_pVTex != nullptr);

	pVTex = m_pCtx->m_pVTex;
	pTile = pVTex->getTileBySerial( idxTile);

	pTile->addState( I3VT_TILE_STATE_LOADING);

	// RAM Tile 할당
	idxRTile = Alloc();
	I3ASSERT( idxRTile != -1);
	
	I3VT_RTILE * pRTile = getTile( idxRTile);
	
	I3ASSERT(pRTile->m_pWeakTex == nullptr);

	pRTile->m_idxTile = idxTile;
	pRTile->m_idxVTile = idxVTile;
	pRTile->m_pWeakTex = pWeakTex;

	i3VTWeakTex_AddRef(pWeakTex);	// 이제부터 레퍼런스카운팅을 강제로 늘린다! (2015.01.09.수빈)
									// 실제로 램에서 VRAM으로 전송되기 전에 count == 1이면 처리 스킵하고 반납..(2이상 요구)
//	if (pTex)
//		pTex->AddRef();				

	pVTex->ReqLoad( idxTile, idxRTile);

	// 아직 Loading이 완료되지 않았기 때문에 Tile에는 RTile 정보를 갱신해 두지 않는다.
	// Tile 정보는 Loading이 완료됐을 때 호출되는 Completion Procedure에서 설정한다.
}

void i3VirtualTextureCache_RAM::LoadFromDisk( I3VT_HTILE idxTile, I3VT_HVTILE idxVTile)
{
	_LoadFromDisk( idxTile, nullptr, idxVTile);
}

void i3VirtualTextureCache_RAM::LoadTexFromDisk( I3VT_HTILE idxTile, I3VT_WEAK_TEX * pWeakTex, INT32 xidx, INT32 yidx)
{
	// Legacy Mode에서 VTile은 사용되지 않고, 직접 Texture에 Loading을 하기 때문에
	// Loading할 좌표를 VTile(32bits)에 16 bits 씩 넣어둔다.
	INT32 idxVTile = ((yidx << 16) & 0xFFFF0000) | (xidx & 0xFFFF);

	_LoadFromDisk( idxTile, pWeakTex, idxVTile );
}

I3VT_HRTILE	 i3VirtualTextureCache_RAM::FindRTileByTile( I3VT_HTILE		hTile)
{
	INT32 i;

	for( i = 0; i < m_MaxTileCount; i++)
	{
		if( m_pInfo[i].m_idxTile == hTile)
			return i;
	}

	return -1;
}

void i3VirtualTextureCache_RAM::RefreshTile( I3VT_HRTILE idxRTile, char * pData)
{
	I3VT_RTILE * pRTile = getTile( idxRTile);

	if( pRTile->m_pBuff != nullptr)
	{
		i3mem::Copy( pRTile->m_pBuff, pData, m_pCtx->m_DataSizePerTile);
	}
}

void i3VirtualTextureCache_RAM::ClearAllIORequest(void)
{
	INT32 i;
	i3VirtualTexture * pVTex = m_pCtx->m_pVTex;

	// 모든 RTile들 중, Loading 중인 모든 RAM Tile을 Clear 해둔다.
	for( i = 0; i < m_MaxTileCount; i++)
	{
		if( m_pInfo[i].m_idxTile != -1)
		{
			// 기존에 RAM에 Loading되어 있던 Tile에 대해서 초기화
			I3VT_TILE * pTile = pVTex->getTileBySerial( m_pInfo[i].m_idxTile);

			pTile->removeState( I3VT_TILE_STATE_LOADING);
		}

		m_pInfo[i].m_idxTile = -1;
		m_pInfo[i].m_idxVTile = -1;

		i3VTWeakTex_WeakRelease(m_pInfo[i].m_pWeakTex);
//		if (m_pInfo[i].m_pTex != nullptr)
//		{
//			static_cast<i3Texture*>(m_pInfo[i].m_pTex)->Release();
//			m_pInfo[i].m_pTex = nullptr;
//		}
	}
}

void i3VirtualTextureCache_RAM::GetProfileString( char * pszStr, INT32 len)
{
}


void	i3VirtualTextureCache_RAM::GetProfileString(i3::string& inout)
{

}
