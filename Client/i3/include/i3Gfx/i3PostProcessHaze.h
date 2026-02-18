#if !defined(__I3_POSTPROCESS_HAZE_H)
#define __I3_POSTPROCESS_HAZE_H

#include "i3PostProcess.h"

#define NAME_SIZE 128


class I3_EXPORT_GFX i3PostProcessHaze : public i3PostProcess
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcessHaze, i3PostProcess);
private:
	VEC2D		m_UVPos;

	enum FADEINOUT
	{
		enum_Fade_None = -1,
		enum_Fade_Out,
		enum_Fade_In,
	};

protected:
	i3Shader *					m_pShader = nullptr;

	I3G_TEXTURE_BIND_CONTEXT	m_pTexBindCtx;
	i3VertexArray*				m_pVA = nullptr;
	i3Texture*					m_pHazeTex = nullptr;
	REAL32						m_fWaveVolume = 0.0f;
	REAL32						m_fSpeed = 2.5f;
	char						m_TextureName[NAME_SIZE] = { 0 };
	FADEINOUT					m_FadeState = FADEINOUT::enum_Fade_None;

public:
	i3PostProcessHaze();
	virtual ~i3PostProcessHaze();

	virtual void			Create(i3PostProcessManager * pManager) override;
	virtual void			ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput) override;
	virtual bool			IsResultOut() override { return true; }

	void					StartEffect();
	void					EndEffect();
	void					InitEffect();

	void					UpdateFadeInOut(REAL32 deltatime);

	void					SetWave(REAL32 wave) { m_fWaveVolume = wave; }
	REAL32					GetWave() { return m_fWaveVolume; }

	void					SetSpeed(REAL32 speed) { m_fSpeed = speed; }
	void					SetHazePath(const char* chPath);
};

#endif