#if !defined( __I3_VIRTUAL_TEXTURE_CACHE_H)
#define __I3_VIRTUAL_TEXTURE_CACHE_H

#include "i3VirtualTextureDefine.h"
#include "i3VirtualTextureCache_RAM.h"
#include "i3VirtualTextureCache_VRAM.h"
#include "i3VirtualTextureCache_Allocator.h"

class i3VirtualTexture;
class i3VirtualTextureIO;

#define		MAX_TRANS_QUEUE		4096

class I3_EXPORT_FRAMEWORK i3VirtualTextureCache : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3VirtualTextureCache, i3GfxResource);
protected:

	i3VirtualTextureCtx *	m_pCtx = nullptr;
	i3VirtualTexture *	m_pVTex = nullptr;

	//
	// Cached-Tile Table
	//
	// : Cache Texture에 배치된 Tile 정보를 담는 Array
	//
	// 
	i3VirtualTextureCache_VRAM		m_VRAM;

	// RAM Cache
	i3VirtualTextureCache_RAM		m_RAM;

	i3Timer							m_Timer;
	i3Timer							m_timerIO;

	REAL32							m_timeLocal = 0.0f;
	UINT32							m_IOSize = 0;
	REAL32							m_TPS = 0.0f;

	// Legacy용
	bool							m_bLegacyMode = false;
	i3LRUQueue						m_LRU;
	INT32							m_ActiveRefTexCount = 0;
	UINT32							m_ActiveRefTexSz = 0;

protected:
	I3VT_HRTILE 			m_TransQueue[ MAX_TRANS_QUEUE];
	UINT32					m_TQ_Head = 0;
	UINT32					m_TQ_Tail = 0;

	bool					_AddTransReq( I3VT_HRTILE idxRTile);
public:
	void					ProcessTrans( REAL32 tmLimit);

	void					resetActiveRefTexture(void);
	void					addActiveRefTexture( i3Texture * pTex);
	void					removeActiveRefTexture( i3Texture * pTex);


protected:
	// 아래의 함수들은 모두 i3VirtualTextureIO에서 호출하는 함수들
	friend i3VirtualTextureIO;

	// i3VirtualTextureIO에서 해당 Tile에 대한 Loading을 시작할 때 호출하는
	// 함수로, Loading할 Buffer를 요구한다.
	//
	// * 별도의 I/O Thread에서 호출된다.
	//
	// i3VirtualTextureIO에서 Loading이 완료되었을 때, 호출하는 함수
	//
	// * 별도의 I/O Thread에서 호출된다.
	//
	bool		CompleteLoad( char * pLoadBuf, INT32 key);

	void		TrashUnusedTexture(void);
	void		TrashTexture( I3VT_TEXINFO * pTexInfo);

public:
	i3VirtualTextureCache(void);
	virtual ~i3VirtualTextureCache(void);

	void		TrashAllTexture(void);

	i3Texture *			getCacheTexture(void)				{ return m_VRAM.getVRAMCacheTexture(); }
	
	i3Texture *			getRedirectTexture(void)			{ return m_VRAM.getRedirectTexture(); }

	INT32			getCacheTextureWidth(void)					{ return m_pCtx->m_CacheWidth; }
	INT32			getCacheTextureHeight(void)					{ return m_pCtx->m_CacheHeight; }

	VEC4D *			getRedirectShaderParam(void)			{ return m_VRAM.getRedirectShaderParam(); }
	VEC4D *			getCacheShaderParam(void)				{ return m_VRAM.getCacheShaderParam(); }

	I3VT_RTILE *	getRTile( I3VT_HRTILE idx)				{ return m_RAM.getTile( idx); }
	I3VT_VTILE *	getVTile( I3VT_HVTILE idx)				{ return m_VRAM.getTile( idx); }

	// Virtual Texture Cache를 생성한다.
	// 
	// 지정된 크기만큼의 Cache Texture를 만든다.
	// 
	bool		Create( INT32 cx, INT32 cy, INT32 cntRAMCache, i3VirtualTexture * pVTex, bool bLegacyMode);
	bool		Resize( INT32 cx, INT32 cy);

	void		ResetCache( bool bResetRAMCache = true, bool bByLostDevice = true);

	//
	// 지정 영역을 Validate 한다.
	//
	// pRect에 포함되는 Virtual Texture 영역의 Tile들은 Cache에 들어간다.
	// 즉, VRAM에 Upload된다.
	// 이 과정에서 만약 충분한 여분 공간이 없는 경우, Least Recently Used 방식에 따라
	// 이 전에 Upload된 Tile을 제거한다.
	//
	bool		Validate( VEC4D * pRect, VEC4D * pRange, bool bMustBeVRAM = true);
	bool		Validate_Legacy( I3VT_TEXINFO * pTex, VEC4D * pRect);

	void		UpdateIndirectTexture(void)							{ m_VRAM.UpdateIndirectTexture(); }

	char *				GetBufferOfRTile( I3VT_HRTILE idxRTile)		
	{ 
		I3VT_RTILE * pRTile = m_RAM.getTile( idxRTile);

		return pRTile->m_pBuff;
	}

	void				RefreshTile( I3VT_HRTILE idxTile, char * pData);

	virtual bool		Destroy( bool bLostDevice) override;
	virtual bool		isDestroyed( void) override;
	virtual bool		Revive( i3RenderContext * pCtx) override;

	// Debug
	void				Dump(void);
	void				SaveVRAMCache( const char * pszPath);
	I3VT_HRTILE			FindRTileByTile( I3VT_HTILE hTile)			{ return m_RAM.FindRTileByTile( hTile); }
	I3VT_HVTILE			FindVTileByTile( I3VT_HTILE hTile)			{ return m_VRAM.FindVTileByTile( hTile); }

	void				GetProfileString( char * pszStr, INT32 len);
	void				GetProfileString(i3::string& inout);
};

#endif
