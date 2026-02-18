#if !defined( __I3_RASTERIZE_CHAIN_BLOOMCOMBINE_H)
#define __I3_RASTERIZE_CHAIN_BLOOMCOMBINE_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_BloomCombine : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_BloomCombine, i3RasterizeChainBase);

protected:	
	i3Shader *					m_pShader = nullptr;
	i3RasterizeChainContext *	m_pCtxCombineTarget = nullptr;

	REAL32						m_fBloomSat = 0.3f;
	REAL32						m_fBaseSat = 1.0f;
	REAL32						m_fBloomIntensity = 1.5f;
	REAL32						m_fBaseIntensity = 1.0f;

public:
	virtual ~i3RasterizeChain_BloomCombine(void);

	REAL32						getBloomSat(void)					{ return m_fBloomSat;			}
	REAL32						getBaseSat(void)					{ return m_fBaseSat;			}
	REAL32						getBloomIntensity(void)				{ return m_fBloomIntensity;		}
	REAL32						getBaseIntensity(void)				{ return m_fBaseIntensity;		}

	void						SetBloomSat(REAL32 fVal);			// Bloom된것의 채도(Saturation)을 변경합니다
	void						SetBaseSat(REAL32 fVal);			// 원본의 채도(Saturation)을 변경합니다.
	void						SetBloomIntensity(REAL32 fVal);		// Bloom된것의 강도를 변경합니다.
	void						SetBaseIntensity(REAL32 fVal);		// 원본의 강도를 변경합니다.

	virtual INT32				GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void				ApplyChain( I3RC_UPDATE * pInfo) override;
	virtual void				OnCreate( i3RasterizeChainManager * pManager) override;
};

#endif	// __I3_RASTERIZE_CHAIN_BLOOMCOMBINE_H
