#include "i3GfxType.h"
#include "i3ShaderCache.h"
#include "i3PostProcessManager.h"
#include "i3PostProcessGamma.h"
#include "i3GfxGlobalVariableDX.h"

I3_CLASS_INSTANCE(i3PostProcessGamma);


i3PostProcessGamma::~i3PostProcessGamma()
{
	I3_SAFE_RELEASE(m_pShader);
}

void i3PostProcessGamma::Create(i3PostProcessManager * pManager)
{
	i3Shader* pNewShader = i3ShaderCache::LoadShader("i3Gamma.hlsl");
	I3_REF_CHANGE(m_pShader, pNewShader);
}

void i3PostProcessGamma::ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput)
{	
	// combine shader.
	pCtx->SetShader(m_pShader);

	// set g_fGammaRGB.
	VEC3D gamma = VEC3D(m_fGammaValue, m_fGammaValue, m_fGammaValue);
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_fGammaRGB", &gamma);

	// set g_texScene.
	m_pTexBindCtx.setTexture(pInput->GetColorTexture());
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texScene", &m_pTexBindCtx);

	// draw.
	pManager->DrawFullScreenQuad(pOutput, pCtx);	

}

void i3PostProcessGamma::SetGammaValue(REAL32 fGamma)
{
	m_fGammaValue = fGamma;

	// shader 내부에 m_fGammaValue로 나누는 연산 있어서 0은 배제.
	if (0.f == m_fGammaValue)
		m_fGammaValue += 0.1f;
}
