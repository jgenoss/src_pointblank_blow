#if !defined( __I3_VIRTUAL_TEXTURE_CACHE_VRAM_H)
#define __I3_VIRTUAL_TEXTURE_CACHE_VRAM_H

#include "i3VirtualTextureDefine.h"

struct I3VT_VTILE
{
	INT32			m_idxTile = -1;

	INT32			m_linkNext = -1;				// 할당되지 않은 Cached-Tile들을 Single-LinkedList로 관리.

	I3VT_VTILE(void)
	{
		m_idxTile	= -1;
		m_linkNext	= -1;
	}
};

class i3VirtualTextureCache_VRAM
{
protected:
	i3VirtualTextureCtx *	m_pCtx = nullptr;

	i3Texture *			m_pCacheTex = nullptr;
	i3Texture *			m_pIndexTex = nullptr;

	INT32				m_cntTileX = 0;
	INT32				m_cntTileY = 0;
	INT32				m_cntTile = 0;

	// LRU(Last Recently Used)를 위한 Ring-Buffer
	i3::vector<INT32>	m_LRU;

	I3VT_VTILE *		m_pTable = nullptr;
	I3VT_HVTILE			m_idxFreeHead = -1;
	I3VT_HVTILE			m_idxUsedHead = -1;
	INT32				m_AllocCount = 0;
	INT32				m_TransferCount = 0;

	i3::vector<INT32>	m_UpdateList;

	// 데이터 전송용 Texture
	i3Texture *			m_pTempTex[2] = { nullptr, nullptr };
	INT32				m_idxTempTex = 0;

	VEC4D				m_RedirFactor;
	VEC4D				m_CacheFactor;

	// Profile 관련
	i3Timer				m_Timer;
	REAL32				m_timeLocal = 0.0f;

	UINT32				m_TransferSize = 0;
	REAL32				m_TPS = 0.0f;

protected:
	I3VT_HVTILE			AllocTile( bool bThresh);

	bool				_CreateTextures(void);
	void				_DestroyTextures(void);

	void				DumpLRU(void);
public:
	INT32				getCoordByPixelX( INT32 px)
	{
		return (px / m_pCtx->m_TileSize);
	}

	INT32				getCoordByPixelY( INT32 py)
	{
		return (py / m_pCtx->m_TileSize);
	}

	I3VT_HVTILE			getIndexByCoord( INT32	idxX, INT32 idxY)
	{
		return (idxY * m_cntTileX) + idxX;
	}

public:
	i3VirtualTextureCache_VRAM(void);
	~i3VirtualTextureCache_VRAM(void);

	I3VT_VTILE *		getTile( I3VT_HVTILE idx)				{ return & m_pTable[ idx]; }

	i3Texture *			getVRAMCacheTexture(void)			{ return m_pCacheTex; }
	i3Texture *			getRedirectTexture(void)			{ return m_pIndexTex; }

	VEC4D *				getRedirectShaderParam(void)		{ return & m_RedirFactor; }
	VEC4D *				getCacheShaderParam(void)			{ return & m_CacheFactor; }

	bool				Create( INT32 width, INT32 height, i3VirtualTextureCtx * pCtx);
	bool				Resize( INT32 width, INT32 height);
	void				Reset(void);

	bool				Upload( char * pSrcBuf, I3VT_HTILE idxTile, I3VT_HVTILE idxVTile = -1);
	bool				UploadToTex( char * pSrcBuf, I3VT_HTILE idxTile, i3Texture * pTex, INT32 xIdx, INT32 yIdx);
	void				UpdateIndirectTexture(void);

	void				Update(void);

	void				GetAllocatedTiles( i3::vector<INT32>& List);

	void				addLRU( I3VT_HVTILE idx);
	void				UpdateLastUsedTile( I3VT_HVTILE hVTile);

	void				RefreshTile( I3VT_HTILE idx, char * pData);

	void				OnDestroy(void);
	bool				isDestroyed(void)					{ return m_pCacheTex == nullptr; }
	void				OnRevive(void);

	// Debug
	void				Dump(void);
	void				SaveCache( const char * pszName);
	I3VT_HVTILE			FindVTileByTile( I3VT_HTILE hTile);

	void				GetProfileString( char * pszStr, INT32 len);
	void				GetProfileString(i3::string& inout);
};

#endif
