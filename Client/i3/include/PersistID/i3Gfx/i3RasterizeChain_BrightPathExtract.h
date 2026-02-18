#if !defined( __I3_RASTERIZE_CHAIN_BRIGHTPATHEXTRACT_H)
#define __I3_RASTERIZE_CHAIN_BRIGHTPATHEXTRACT_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_BrightPathExtract : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_BrightPathExtract);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;

	BOOL					m_bReUseTex;
	REAL32					m_fThreshold;

public:
	i3RasterizeChain_BrightPathExtract(void);
	virtual ~i3RasterizeChain_BrightPathExtract(void);

	void					Create( BOOL bReUseTex = FALSE);

	virtual void			InitChain(void);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	virtual void			SetEnable(BOOL bFlag);	

	void					Update(REAL32 fDeltaSec);

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);

	REAL32					getThreshold(void)				{ return m_fThreshold;		}	
	void					setThreshold(REAL32 fVal)		{ m_fThreshold = fVal;		}	// 밝은부분을 추출하는 한계값을 설정합니다.
};

#endif	// __I3_RASTERIZE_CHAIN_BRIGHTPATHEXTRACT_H
