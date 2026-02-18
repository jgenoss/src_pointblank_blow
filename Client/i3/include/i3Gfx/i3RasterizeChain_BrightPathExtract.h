#if !defined( __I3_RASTERIZE_CHAIN_BRIGHTPATHEXTRACT_H)
#define __I3_RASTERIZE_CHAIN_BRIGHTPATHEXTRACT_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_BrightPathExtract : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_BrightPathExtract, i3RasterizeChainBase);

protected:	
	i3Shader *					m_pShader = nullptr;
	i3RasterizeChainContext *	m_pCtxBrightPath = nullptr;
	REAL32						m_fThreshold = 0.5f;

public:
	virtual ~i3RasterizeChain_BrightPathExtract(void);

	virtual INT32				GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void				ApplyChain( I3RC_UPDATE * pInfo) override;
	virtual void				OnCreate( i3RasterizeChainManager * pManager) override;

	REAL32						getThreshold(void)				{ return m_fThreshold;		}	
	void						setThreshold(REAL32 fVal)		{ m_fThreshold = fVal;		}	// 밝은부분을 추출하는 한계값을 설정합니다.
};

#endif	// __I3_RASTERIZE_CHAIN_BRIGHTPATHEXTRACT_H
