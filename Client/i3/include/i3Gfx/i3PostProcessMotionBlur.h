#if !defined( __I3_POSTPROCESS_MOTIONBLUR_H)
#define __I3_POSTPROCESS_MOTIONBLUR_H

#include "i3PostProcess.h"

class I3_EXPORT_GFX i3PostProcessMotionBlur : public i3PostProcess
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcessMotionBlur, i3PostProcess);
	
protected:

	bool						m_bFirstEnable = true;

	i3Shader *					m_pShader_BitBlt = nullptr;
	i3Shader *					m_pShader_Blend = nullptr;

	I3G_TEXTURE_BIND_CONTEXT	m_pCtx;

	i3RenderTarget *			m_pPreviousRT = nullptr;
	i3RenderTarget *			m_pPreviousRT2 = nullptr;

	COLORREAL					m_BlurColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	REAL32						m_fBlurAlpha = 0.5f;
	REAL32						m_fStartAlpha = 0.99f;
	REAL32						m_fEndAlpha = 0.1f;

	REAL32						m_fProcessingTime = 0.0f;
	REAL32						m_fBlurTime = 0.0f;

public:
	i3PostProcessMotionBlur();
	~i3PostProcessMotionBlur();

	void					AccumulateBlur(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput);
	void					ApplyBlur(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pOutput);

	void					StartMotionBlur(REAL32 time, REAL32 startAlpha, REAL32 endAlpha);
	void					EndMotionBlur();

	REAL32					GetBlurTime()			{ return m_fBlurTime; }
	REAL32					GetProcessingTime()		{ return m_fProcessingTime; }

	virtual void			Create(i3PostProcessManager * pManager) override;
	virtual void			ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput) override;

	virtual void			SetEnable(bool isEnable) override;
	virtual bool			IsResultOut() override { return true; }
};

#endif