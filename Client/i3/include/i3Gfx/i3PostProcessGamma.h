#ifndef __I3_POSTPROCESS_GAMMA_H
#define __I3_POSTPROCESS_GAMMA_H

#include "i3PostProcess.h"

class I3_EXPORT_GFX i3PostProcessGamma : public i3PostProcess
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcessGamma, i3PostProcess);

public:
	virtual ~i3PostProcessGamma();

private:
	i3Shader*					m_pShader = nullptr;
	I3G_TEXTURE_BIND_CONTEXT	m_pTexBindCtx;

	REAL32						m_fGammaValue = 0.0f;

public:
	virtual void Create(i3PostProcessManager* pManager) override;
	virtual void ApplyPostProcess(i3PostProcessManager* pManager, i3RenderContext* pCtx
									, i3RenderTarget* pInput, i3RenderTarget* pOutput) override;
	virtual bool IsResultOut() override { return true; }

	void SetGammaValue(REAL32 fGamma);
};

#endif // __I3_POSTPROCESS_GAMMA_H