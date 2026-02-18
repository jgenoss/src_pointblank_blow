#if !defined(__I3_POSTPROCESS_NIGHTVISION_H)
#define __I3_POSTPROCESS_NIGHTVISION_H

#include "i3PostProcess.h"

class I3_EXPORT_GFX i3PostProcessNightVision : public i3PostProcess
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcessNightVision, i3PostProcess);

protected:

	i3Shader *					m_pShader = nullptr;
	i3VertexArray *				m_pVA = nullptr;
	I3G_TEXTURE_BIND_CONTEXT	m_pCtx;
	REAL32						m_fMixRate = 0.0f;
	UINT32						m_nMaskingType = 0;

	i3::vector<i3Texture*>		m_maskTextureList;

public:
	i3PostProcessNightVision();
	virtual ~i3PostProcessNightVision();

	void					AddMaskingTexture(const char * filename);
	void					SetMixRate(REAL32 rate)						{ m_fMixRate = MINMAX(0.0f, rate, 1.0f); }
	void					SetMaskingType(UINT32 maskingType)			{ m_nMaskingType = maskingType; }

	virtual void			Create(i3PostProcessManager * pManager) override;
	virtual void			ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput) override;

	virtual bool			IsResultOut() override { return true; }
};

#endif