#if !defined( __I3_RASTERIZE_CHAIN_BLOOMCOMBINE_H)
#define __I3_RASTERIZE_CHAIN_BLOOMCOMBINE_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_BloomCombine : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_BloomCombine);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;

	BOOL					m_bReUseTex;

	REAL32					m_fBloomSat;
	REAL32					m_fBaseSat;
	REAL32					m_fBloomIntensity;
	REAL32					m_fBaseIntensity;

public:
	i3RasterizeChain_BloomCombine(void);
	virtual ~i3RasterizeChain_BloomCombine(void);

	void					Create( BOOL bReUseTex = FALSE);

	virtual void			InitChain(void);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	virtual void			SetEnable(BOOL bFlag);	

	void					Update(REAL32 fDeltaSec);

	REAL32					getBloomSat(void)					{ return m_fBloomSat;			}
	REAL32					getBaseSat(void)					{ return m_fBaseSat;			}
	REAL32					getBloomIntensity(void)				{ return m_fBloomIntensity;		}
	REAL32					getBaseIntensity(void)				{ return m_fBaseIntensity;		}

	void					SetBloomSat(REAL32 fVal);			// Bloom된것의 채도(Saturation)을 변경합니다
	void					SetBaseSat(REAL32 fVal);			// 원본의 채도(Saturation)을 변경합니다.
	void					SetBloomIntensity(REAL32 fVal);		// Bloom된것의 강도를 변경합니다.
	void					SetBaseIntensity(REAL32 fVal);		// 원본의 강도를 변경합니다.

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_BLOOMCOMBINE_H
