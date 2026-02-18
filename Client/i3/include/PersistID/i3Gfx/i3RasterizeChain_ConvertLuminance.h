#if !defined( __I3_RASTERIZE_CHAIN_CONVERTLUMINANCE_H)
#define __I3_RASTERIZE_CHAIN_CONVERTLUMINANCE_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_ConvertLuminance : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_ConvertLuminance);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;
	i3Texture *				m_pLuminanceTexture;

	BOOL					m_bReUseTex;

public:
	i3RasterizeChain_ConvertLuminance(void);
	virtual ~i3RasterizeChain_ConvertLuminance(void);

	void					Create( BOOL bReUseTex = FALSE, I3G_IMAGE_FORMAT Imageformat = I3G_IMAGE_FORMAT_RGBA_8888);

	virtual void			InitChain(void);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	virtual void			SetEnable(BOOL bFlag);	

	void					Update(REAL32 fDeltaSec);

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_CONVERTLUMINANCE_H
