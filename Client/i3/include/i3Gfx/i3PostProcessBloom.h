#if !defined( __I3_POSTPROCESS_BLOOM_H)
#define __I3_POSTPROCESS_BLOOM_H

#include "i3GfxResource.h"
#include "i3PostProcess.h"
#include "i3Shader.h"

class I3_EXPORT_GFX i3PostProcessBloom : public i3PostProcess
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcessBloom, i3PostProcess);
	
protected:
	i3Shader *					m_pCombineShader = nullptr;
	i3Shader *					m_pBrightPassShader = nullptr;

	I3G_TEXTURE_BIND_CONTEXT	m_Ctx;
	I3G_TEXTURE_BIND_CONTEXT	m_Ctx2;

	REAL32						m_fBloomThreshold = 0.75f;
	REAL32						m_fBloomIntensity = 0.4f;
	REAL32						m_fBaseIntensity = 1.0f;
	INT32						m_BlurIteration = 2;
	REAL32						m_BlurTheta = 4.0f;

private:
	void				_ApplyBrightPass(i3RenderTarget * pDest, i3RenderTarget * pSrc, i3PostProcessManager * pManager, i3RenderContext * pCtx);

public:
	virtual ~i3PostProcessBloom();

	void							SetBloomIntensity(REAL32 intensity)		{ m_fBloomIntensity = intensity; }
	REAL32							GetBloomIntensity()	{ return m_fBloomIntensity; }

	void							SetBloomThreshold(REAL32 threshold)		{ m_fBloomThreshold = threshold; }
	REAL32							GetBloomThreshold()						{ return m_fBloomThreshold; }

	void							SetBlurIteration(INT32 iterCount)		{ m_BlurIteration = iterCount; }
	INT32							GetBlurIteration()						{ return m_BlurIteration; }

	void							SetBlurTheta(REAL32 theta)				{ m_BlurTheta = theta; }
	REAL32							GetBlurTheta()							{ return m_BlurTheta; }

	virtual bool					IsResultOut() override { return true; }
	virtual void					Create(i3PostProcessManager * pManager) override;
	virtual void					ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput) override;

	virtual bool					OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool					OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif