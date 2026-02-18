#if !defined( __I3_RASTERIZE_CHAIN_NIGHTVISION_H__)
#define __I3_RASTERIZE_CHAIN_NIGHTVISION_H__

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_NightVision : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_NightVision);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;

	BOOL					m_bReUseTex;
	REAL32					m_fNVBrightness;

public:
	i3RasterizeChain_NightVision(void);
	virtual ~i3RasterizeChain_NightVision(void);

	void					Create( BOOL bReUseTex = FALSE, I3G_IMAGE_FORMAT Imageformat = I3G_IMAGE_FORMAT_RGBA_8888);

	//
	void					Update(REAL32 fDeltaSec);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	
	virtual BOOL			Revive( i3RenderContext * pCtx);
};

#endif	//#define __I3_RASTERIZE_CHAIN_NIGHTVISION_H__