#if !defined( __I3_RASTERIZE_CHAIN_H)
#define __I3_RASTERIZE_CHAIN_H

#if defined( I3G_DX)
#include "i3GfxResource.h"
#include "i3TextureDX.h"
#endif

#define DEBUG_TEXTURE_COUNT	12

typedef struct {
	REAL32       p[4];
    REAL32       tu, tv;
} TVERTEX;

class I3_EXPORT_GFX i3RasterizeChainBase : public i3GfxResource
{
	I3_CLASS_DEFINE( i3RasterizeChainBase);

protected:
	BOOL			m_bEnable;
	BOOL			m_bDebug;
	INT32			m_nIdxRT;
	
	INT32			m_nDebugViewTextureCount;
	i3TextureDX *	m_pDebugViewTexture[DEBUG_TEXTURE_COUNT];	

	static i3List s_InputTextureList;
	static i3List s_OutputTextureList;

	static i3Texture *	s_pCurTexture;
	static BOOL		s_bLastRasterizeChain;

public:
	i3RasterizeChainBase(void);
	virtual ~i3RasterizeChainBase(void);

	void						SetRenderContextAttrs(void);
	void						ChangeRenderTarget(INT32 index);
	void						ChangeRenderTarget(i3Texture* pTex);
	void						FinalRenderTarget(void);
	IDirect3DTexture9 *			GetDXTexture( INT32 index);
	i3Texture *					GetTexture( INT32 index);

	// Texture°ü¸®

	static void					SetLastRTChain(BOOL bFlag)				{ s_bLastRasterizeChain = bFlag;							}
	static BOOL					IsLastRTChain(void)						{ return s_bLastRasterizeChain;								}

	static void					SetCurTexture( i3Texture* pTex);
	static i3Texture*			GetCurTexture(void);

	static void					SetInputTexture( i3Texture* pTex);
	static void					SetOutputTexture( i3Texture* pTex);	
	static void					DestroyInputOutputTexture(void);
	static INT32				GetInputTextureCount(void)				{ return s_InputTextureList.GetCount();						}
	static INT32				GetOutputTextureCount(void)				{ return s_OutputTextureList.GetCount();					}
	static i3Texture *			GetInputTexture(INT32 i)				{ return (i3Texture*)s_InputTextureList.GetItem(i);			}
	static i3Texture *			GetOutputTexture(INT32 i)				{ return (i3Texture*)s_OutputTextureList.GetItem(i);		}

	void						UpdateDebugView(void);
	void						SetDebugViewTexture( INT32 nIdx, i3TextureDX * pTex);
	void						setDebugViewEnable( BOOL bFlag)			{ m_bDebug = bFlag;			}
	
	INT32						GetCurrentRenderTargetIdx( void)		{ return m_nIdxRT;			}

	virtual void				InitChain(void);
	virtual void				ApplyChain(REAL32 fDeltaSec);
	virtual void				SetEnable(BOOL bFlag);	
	BOOL						getEnable(void)							{ return m_bEnable;			}

	virtual BOOL				Destroy( bool bLostDevice);
	virtual BOOL				Revive( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_H
