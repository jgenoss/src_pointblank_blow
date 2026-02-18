#if !defined( __I3_RASTERIZE_CHAIN_TONEMAP_H)
#define __I3_RASTERIZE_CHAIN_TONEMAP_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_ToneMap : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_ToneMap);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;

	BOOL					m_bReUseTex;

public:
	i3RasterizeChain_ToneMap(void);
	virtual ~i3RasterizeChain_ToneMap(void);

	void					Create( BOOL bReUseTex = FALSE, I3G_IMAGE_FORMAT Imageformat = I3G_IMAGE_FORMAT_RGBA_8888);

	virtual void			InitChain(void);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	virtual void			SetEnable(BOOL bFlag);	

	void					Update(REAL32 fDeltaSec);

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_TONEMAP_H
