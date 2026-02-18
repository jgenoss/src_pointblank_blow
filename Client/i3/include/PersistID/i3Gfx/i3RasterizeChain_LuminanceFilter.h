#if !defined( __I3_RASTERIZE_CHAIN_LUMINANCEFILTER_H)
#define __I3_RASTERIZE_CHAIN_LUMINANCEFILTER_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_LuminanceFilter : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_LuminanceFilter);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;
	i3List					m_FilterTextureList;
	i3List					m_VertexArrayList;

	BOOL					m_bReUseTex;

public:
	i3RasterizeChain_LuminanceFilter(void);
	virtual ~i3RasterizeChain_LuminanceFilter(void);

	void					Create( BOOL bReUseTex = FALSE, I3G_IMAGE_FORMAT Imageformat = I3G_IMAGE_FORMAT_RGBA_8888);

	virtual void			InitChain(void);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	virtual void			SetEnable(BOOL bFlag);	

	void					Update(REAL32 fDeltaSec);

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_LUMINANCEFILTER_H
