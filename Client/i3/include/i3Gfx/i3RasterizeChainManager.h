#if !defined( __I3_RASTERIZE_CHAIN_MANAGER_H)
#define __I3_RASTERIZE_CHAIN_MANAGER_H

#include "i3RasterizeChainBase.h"
//#include "i3RasterizeChainContext.h"
#include "i3GfxResource.h"

class i3RenderContext;

class I3_EXPORT_GFX i3RasterizeChainManager : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChainManager, i3GfxResource);

protected:
	i3RenderContext *			m_pGfx = nullptr;

	bool						m_bInitialized = false;
	i3::vector<i3RasterizeChainContext*>		m_TempRTList;					// Render Target이 담기는 리스트입니다.(재사용가능)
	i3::vector<i3RasterizeChainContext*>		m_NamedRTList;					// Render Target이 담기는 리스트입니다.(이름으로 등록되는 리스트-재사용 불가)
	i3::vector<i3RasterizeChainBase*>			m_ChainList;					// 각각의 Chain이 담기는 리스트입니다.


	INT32						m_nInputRTIdx = 0;
	INT32						m_nLastRTIdx = 0;

	bool						m_bLock = false;
	bool						m_bUseNamedContext = false;

	INT32						m_RTWidth = 0;
	INT32						m_RTHeight = 0;
	I3G_IMAGE_FORMAT			m_RTImageFormat = I3G_IMAGE_FORMAT_BGRA_8888;
	INT32						m_DSWidth = 0;
	INT32						m_DSHeight = 0;
	I3G_IMAGE_FORMAT			m_DSImageFormat = I3G_IMAGE_FORMAT_D24X8;

	i3RasterizeChainContext *	m_pLastRT = nullptr;
	i3RasterizeChainContext *	m_pFirstRT = nullptr;

	i3VertexArray *				m_pDefVA = nullptr;

protected:
	i3RasterizeChainBase *		_FindNextColorOut( INT32 idx);
	i3Texture				*	_FindAdjustDepth(i3RasterizeChainContext * pCtxExcept);

	i3RasterizeChainContext *	_RegisterRT( UINT32 w, UINT32 h, UINT32 style, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName = nullptr, bool bNamedRT = false);
	i3RasterizeChainContext *	_AddRT( UINT32 w, UINT32 h, UINT32 style, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName = nullptr, bool bNamedRT = false);

	void						_CreateDefVertexArray(void);
	void						_ResizeTarget(void);
	void						_UpdateChainTarget(void);

public:
	i3RasterizeChainManager(void);
	virtual ~i3RasterizeChainManager(void);

	void						Create( i3RenderContext * pGfx);
	//void						CreateSourceRT(void);				// 이함수는 원래의 Render Target과 같은 크기로 생성합니다. 이때 이미지 포멧은 원본Source와 다를 수 있습니다.
	//void						CreateSourceDepthSurface(void);		// 이함수는 원래의 Depth Surface와 같은 값으로 생성합니다.
	void						DestroyRenderTarget(void);
	//void						CreateDefaultRenderTarget(void);

	i3VertexArray *				getDefaultVertexArray(void)			{ return m_pDefVA; }

	I3G_IMAGE_FORMAT			GetOriginalRenderTargetImageFormat(void);

	INT32						getTempRTCount(void)				{ return (INT32)m_TempRTList.size();															}
	i3RasterizeChainContext *	getTempRT( INT32 idx)				{ return m_TempRTList[idx]; }

	INT32						getNamedRTCount(void)				{ return (INT32)m_NamedRTList.size();														}
	i3RasterizeChainContext *	getNamedRT( INT32 idx)				{ return m_NamedRTList[idx]; }
	

	i3RasterizeChainContext*	RegisterRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName = nullptr);	
	i3RasterizeChainContext*	RegisterNamedRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName = nullptr);

	i3RasterizeChainContext*	RegisterRT( char* pszName = nullptr, bool Scaled = false, INT32 nDownScale = 1.0f, bool bDepth = false);
	i3RasterizeChainContext*	RegisterNamedRT( char* pszName = nullptr, bool Scaled = false, INT32 nDownScale = 1.0f, bool bDepth = false);

	void						UnregisterRT( i3RasterizeChainContext * pCtx);

	i3RasterizeChainContext *	FindTempRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, i3RasterizeChainContext * pExcept = nullptr);
	i3RasterizeChainContext*	FindNamedRT( const char* pszName, bool bMustExist = true);
	i3RasterizeChainContext *	GetTempRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, i3RasterizeChainContext * pExcept = nullptr);

	i3RasterizeChainContext *	getFirstRenderTarget(void)			{ return m_pFirstRT; }

	void						SetRenderTarget( i3RasterizeChainContext * pCtx);

	void						AddChain( i3RasterizeChainBase* pChain, bool bAutoAdjust = true);	
	void						RemoveChain( i3RasterizeChainBase * pChain, bool bAutoAdjust = true);
	void						RemoveAllChain( bool bAutoAdjust = true);
	i3RasterizeChainBase *		FindChainByName( i3ClassMeta * pMeta);
	i3RasterizeChainBase*		AddChainByName( const char* chainname);
	void						RemoveChainByIndex( INT32 nIndex);
	void						SwapChainByIndex( INT32 nIndex1, INT32 nIndex2);

	bool						isEnableChainRasterize(void)		{ return (m_ChainList.size() >= 1);																	}
	INT32						getChainCount(void)					{ return (INT32)m_ChainList.size();																		}
	i3RasterizeChainBase *		getChain( INT32 idx)				{ return m_ChainList[idx]; }

	void						SetColorRenderTarget(i3Texture* pTexture, bool bNamedContext = false);
	i3RasterizeChainContext*	SwapTarget(void);

	void						Apply( REAL32 tm, i3RenderContext * pGfx);

	static	i3RasterizeChainManager *		getInstance(void);

	virtual bool				Destroy( bool bLostDevice) override;
	virtual bool				isDestroyed( void) override;
	virtual bool				Revive( i3RenderContext * pCtx) override;

	void						OnEnableChain( i3RasterizeChainBase * pChain);
	void						OnSize(void);

};

#endif	// __I3_RASTERIZE_CHAIN_MANAGER_H
