#if !defined( __I3_RASTERIZE_CHAIN_MEASURE_LUMINANCE_H)
#define __I3_RASTERIZE_CHAIN_MEASURE_LUMINANCE_H

#include "i3RasterizeChainBase.h"

class i3Shader;
class i3Texture;

#define NUM_TONEMAP_TEXTURES	4

class I3_EXPORT_GFX i3RasterizeChain_ToneMapping : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_ToneMapping, i3RasterizeChainBase);

protected:
	I3G_IMAGE_FORMAT			m_Imageformat = I3G_IMAGE_FORMAT_ABGR_16F;

	i3Shader *					m_pShaderSceneToSceneScale = nullptr;
	i3Shader *					m_pShaderMeasureLuminance = nullptr;
	i3Shader *					m_pShaderResampleAvgLum = nullptr;
	i3Shader *					m_pShaderResampleAvgLumExp = nullptr;
	i3Shader *					m_pShaderAdaptation = nullptr;
	i3Shader *					m_pShaderFinalScenePass = nullptr;

	i3RasterizeChainContext *	m_pCtxSceneScaled = nullptr;

	i3Texture *					m_pToneMapTexture[NUM_TONEMAP_TEXTURES];
	i3Texture *					m_pToneMapDepthTexture[NUM_TONEMAP_TEXTURES];

	i3Texture *					m_pLumCurTexture = nullptr;
	i3Texture *					m_pLumLastTexture = nullptr;
	i3Texture *					m_pLumCurDepthTexture = nullptr;
	i3Texture *					m_pLumLastDepthTexture = nullptr;

	I3G_TEXTURE_BIND_CONTEXT	m_ctxTex;

	REAL32						m_fMiddleGray = 0.3f;
	REAL32						m_SmallestLumToWhite = 1.2f;
	INT32						m_nCurTonMapTexIdx = NUM_TONEMAP_TEXTURES - 1;
	i3Timer						m_AdaptionTimer;

protected:
	void					UpdateSceneScaled( I3RC_UPDATE * pInfo);
	void					UpdateMeasureLuminance( I3RC_UPDATE * pInfo);
	void					UpdateResampleAvgLuminance( I3RC_UPDATE * pInfo);
	void					UpdateResampleAvgLuminanceEXP( I3RC_UPDATE * pInfo);
	void					UpdateAdaptationLuminance( I3RC_UPDATE * pInfo);
	void					UpdateFinalScenePass( I3RC_UPDATE * pInfo);

	void					RegisterTexture( i3RasterizeChainManager * pManager);
	void					ShaderCodeLoad(void);

	//==============================================================================================================
	// Util
	//==============================================================================================================
	bool						GetSampleOffsets_DownScale4x4( INT32 nWidth, INT32 nHeight, VEC4D* vecSampleOffsets);
	bool						GetSampleOffsets_DownScale2x2( INT32 nWidth, INT32 nHeight, VEC4D* vecSampleOffsets);

	void					_SetTonMapTexture( INT32 idx);
	void					_SetTonMapTexture( i3Texture * pColor, i3Texture * pDepth);

public:
	i3RasterizeChain_ToneMapping(void);
	virtual ~i3RasterizeChain_ToneMapping(void);

	virtual void			OnCreate( i3RasterizeChainManager * pManager) override;
	void					CreateTexture(void);
	void					DestroyTexture(void);	
	
	virtual INT32			GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void			ApplyChain( I3RC_UPDATE * pInfo) override;

	REAL32					getMiddleGray(void)			{ return m_fMiddleGray;	}
	void					setMiddleGray(REAL32 val)	{ m_fMiddleGray = val;	}

	REAL32					getSmallestLuminanceToWhite(void)		{ return m_SmallestLumToWhite; }
	void					setSmallestLuminanceToWhite( REAL32 v)	{ m_SmallestLumToWhite = v; }
	
	virtual bool			Destroy( bool bLostDevice) override;
	virtual bool			Revive( i3RenderContext * pCtx) override;


	////////////////////////////////////////////////////////////////////////////////////////
	//	for Debug tool
public:
	virtual i3RasterizeChainContext *	GetResultChainCtx( void)		{ return m_pCtxSceneScaled; }
};

#endif	// __I3_RASTERIZE_CHAIN_MEASURE_LUMINANCE_H
