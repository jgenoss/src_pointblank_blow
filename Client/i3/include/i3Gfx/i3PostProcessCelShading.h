#if !defined (__I3_POSTPROCESS_CELSHADING_H)
#define __I3_POSTPROCESS_CELSHADING_H

#include "i3GfxResource.h"
#include "i3PostProcess.h"
#include "i3Shader.h"

class I3_EXPORT_GFX i3PostProcessCelShading : public i3PostProcess
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcessCelShading, i3PostProcess);

protected:
	i3Shader *					m_pEdgeShader = nullptr;
	i3Shader *					m_pCelShader = nullptr;
	i3Shader *					m_pShader_BitBlt = nullptr;

	I3G_TEXTURE_BIND_CONTEXT	m_Ctx;

public:
	~i3PostProcessCelShading();

	virtual bool IsResultOut() override { return true; }

	virtual void Create(i3PostProcessManager * pManager) override;
	virtual void ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput) override;
};

#endif