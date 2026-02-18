#if !defined( __I3_RASTERIZE_CHAIN_DOWNSCALE_H)
#define __I3_RASTERIZE_CHAIN_DOWNSCALE_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_DownScale : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_DownScale, i3RasterizeChainBase);

protected:	
	i3Shader *					m_pShader = nullptr;
	i3RasterizeChainContext *	m_pCtxDownScale = nullptr;

	REAL32						m_fRateWidth = 2.0f;
	REAL32						m_fRateHeight = 2.0f;

public:
	virtual ~i3RasterizeChain_DownScale(void);

	virtual INT32				GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void				ApplyChain( I3RC_UPDATE * pInfo) override;
	virtual void				OnCreate( i3RasterizeChainManager * pManager) override;

};

#endif	// __I3_RASTERIZE_CHAIN_DOWNSCALE_H
