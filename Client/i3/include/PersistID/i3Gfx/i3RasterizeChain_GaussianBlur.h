#if !defined( __I3_RASTERIZE_CHAIN_GAUSSIANBLUR_H)
#define __I3_RASTERIZE_CHAIN_GAUSSIANBLUR_H

#include "i3RasterizeChainBase.h"

#define GAUSSIAN_SAMPLECOUNT 5

class I3_EXPORT_GFX i3RasterizeChain_GaussianBlur : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_GaussianBlur);

protected:
	i3VertexArray *			m_pVA;	
	i3Shader *				m_pShader;

	BOOL					m_bReUseTex;

	VEC4D					m_vecSampleOffset[GAUSSIAN_SAMPLECOUNT];
	VEC4D					m_vecSampleWeight[GAUSSIAN_SAMPLECOUNT];
	REAL32					m_fGaussiantheta;

public:
	i3RasterizeChain_GaussianBlur(void);
	virtual ~i3RasterizeChain_GaussianBlur(void);

	void					Create( BOOL bReUseTex = FALSE);

	virtual void			InitChain(void);
	virtual void			ApplyChain(REAL32 fDeltaSec);
	virtual void			SetEnable(BOOL bFlag);	

	void					Update(REAL32 fDeltaSec);
	void					ApplyBlur(void);
	void					SetBlurParameter( REAL32 fX, REAL32 fY, INT32 nPixelCount);
	REAL32					ComputeGaussian(REAL32 n);

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);

	REAL32					getGaussianTheta(void)			{ return m_fGaussiantheta;			}	
	void					setGaussianTheta( REAL32 fVal)	{ m_fGaussiantheta = fVal;			}	// Blur의 강도(퍼지는 정도)를 설정합니다.
};

#endif	// __I3_RASTERIZE_CHAIN_GAUSSIANBLUR_H
