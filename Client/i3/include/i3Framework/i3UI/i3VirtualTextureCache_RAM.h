#if !defined( __I3_VIRTUAL_TEXTURE_RAM_CACHE_H)
#define __I3_VIRTUAL_TEXTURE_RAM_CACHE_H

#include "i3VirtualTextureDefine.h"

class i3VirtualTextureCache_VRAM;

enum I3VT_RAMCACHE_STATE
{
	I3VT_RAMCACHE_STATE_FREE		= 0,
	I3VT_RAMCACHE_STATE_LOADING,
	I3VT_RAMCACHE_STATE_COMPLETED
};

struct I3VT_RTILE
{
	char *				m_pBuff = nullptr;
	I3VT_HTILE			m_idxTile = -1;
	I3VT_HRTILE			m_idxNext = -1;

	INT32				m_idxVTile = -1;

	I3VT_WEAK_TEX*		m_pWeakTex = nullptr;
//	void *				m_pTex;

	I3VT_RTILE(void)
	{
		//m_State = I3VT_RAMCACHE_STATE_FREE;
		m_pBuff		= nullptr;
		m_idxTile	= -1;
		m_idxNext	= -1;
		m_idxVTile	= -1;
		m_pWeakTex	= nullptr;
	}
};

class i3VirtualTextureCache_RAM
{
protected:
	i3VirtualTextureCtx *		m_pCtx = nullptr;

	i3VirtualTextureCache *		m_pHostCache = nullptr;

	INT32					m_MaxTileCount = 0;

	I3VT_RTILE *		m_pInfo = nullptr;
	char *				m_pBuffer = nullptr;
	I3VT_HRTILE			m_idxFreeHead = -1;
	I3VT_HRTILE			m_idxUsedHead = -1;
	
	// LRU(Last Recently Used)¸¦ À§ÇÑ Ring-Buffer
	i3LRUQueue			m_LRU;

	i3::vector<char*>	m_FreeBufferList;	// for only legacy
	i3::vector<char*>	m_BuffList;

protected:
	void			_LoadFromDisk( I3VT_HTILE idxTile, I3VT_WEAK_TEX* pWeakTex, I3VT_HVTILE idxVTile);

public:
	i3VirtualTextureCache_RAM(void);
	~i3VirtualTextureCache_RAM(void);

	bool			Create( i3VirtualTextureCache * pCache, INT32 cntTile, i3VirtualTextureCtx * pCtx);
	void			Reset( bool bColdReset = true);

	I3VT_HRTILE		getUsedHead(void)						{ return m_idxUsedHead; }
	I3VT_RTILE *	getTile( I3VT_HRTILE idx)				{ return & m_pInfo[ idx]; }

	I3VT_HRTILE		Alloc(void);

	void			AllocBuffer( void);
	void			FreeBuffer( I3VT_HRTILE idx);

	void			LoadFromDisk( I3VT_HTILE idxTile, I3VT_HVTILE idxVTile = -1);
	void			LoadTexFromDisk( I3VT_HTILE idxTile, I3VT_WEAK_TEX * pWeakTex, INT32 xidx, INT32 yidx);
	void			ClearAllIORequest(void);

	void			RefreshTile( I3VT_HRTILE idxRTile, char * pData);

	// Debug
	I3VT_HRTILE		FindRTileByTile( I3VT_HTILE		hTile);

	void			GetProfileString( char * pszStr, INT32 len);
	void			GetProfileString(i3::string& inout);
};

#endif
