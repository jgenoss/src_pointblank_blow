#if !defined( __I3_RASTERIZE_CHAIN_LUMINANCE_ADAPTATION_H)
#define __I3_RASTERIZE_CHAIN_LUMINANCE_ADAPTATION_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_Luminance_Adaptation : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_Luminance_Adaptation);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;

	i3Texture *				m_pCurTexture;
	i3Texture *				m_pPrevTexture;

	BOOL					m_bReUseTex;

public:
	i3RasterizeChain_Luminance_Adaptation(void);
	virtual ~i3RasterizeChain_Luminance_Adaptation(void);

	void					Create( BOOL bReUseTex = FALSE, I3G_IMAGE_FORMAT Imageformat = I3G_IMAGE_FORMAT_RGBA_8888);

	virtual void			InitChain(void);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	virtual void			SetEnable(BOOL bFlag);	

	void					Update(REAL32 fDeltaSec);

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_LUMINANCE_ADAPTATION_H
