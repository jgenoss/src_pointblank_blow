#if !defined( __I3_RASTERIZE_CHAIN_GAUSSIANBLUR_H)
#define __I3_RASTERIZE_CHAIN_GAUSSIANBLUR_H

#include "i3RasterizeChainBase.h"
//#include "i3RasterizeChainContext.h"

class i3Shader;

#define GAUSSIAN_SAMPLECOUNT 3

class I3_EXPORT_GFX i3RasterizeChain_GaussianBlur : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_GaussianBlur, i3RasterizeChainBase);

protected:	
	i3Shader *					m_pShader = nullptr;
	i3RasterizeChainContext *	m_pCtxBlurTarget = nullptr;

	VEC4D						m_vecSampleOffset[GAUSSIAN_SAMPLECOUNT];
	VEC4D						m_vecSampleWeight[GAUSSIAN_SAMPLECOUNT];
	REAL32						m_fGaussiantheta = 3.0f;

public:
	virtual ~i3RasterizeChain_GaussianBlur(void);

	virtual INT32				GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void				OnCreate( i3RasterizeChainManager * pManager) override;
	virtual void				ApplyChain( I3RC_UPDATE * pInfo) override;

	void						SetBlurParameter( REAL32 fX, REAL32 fY, INT32 nPixelCount);
	REAL32						ComputeGaussian(REAL32 n);

	REAL32						getGaussianTheta(void)			{ return m_fGaussiantheta;			}	
	void						setGaussianTheta( REAL32 fVal)	{ m_fGaussiantheta = fVal;			}	// Blur의 강도(퍼지는 정도)를 설정합니다.

	////////////////////////////////////////////////////////////////////////////////////////
	//	for Debug tool
public:
	virtual i3RasterizeChainContext *	GetResultChainCtx( void)		{ return m_pCtxBlurTarget; }
	
};

#endif	// __I3_RASTERIZE_CHAIN_GAUSSIANBLUR_H
