#if !defined( __I3_RASTERIZE_CHAIN_SAMPLE_H)
#define __I3_RASTERIZE_CHAIN_SAMPLE_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_Sample : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_Sample, i3RasterizeChainBase);
	
public:
	virtual INT32				GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void				ApplyChain(I3RC_UPDATE * pInfo) override {}
	virtual void				OnCreate(i3RasterizeChainManager * pManager) override {}
};

#endif	// __I3_RASTERIZE_CHAIN_SAMPLE_H
