#include "i3GfxType.h"
#include "i3ShaderCache.h"
#include "i3PostProcessManager.h"
#include "i3PostProcessBloom.h"

I3_CLASS_INSTANCE(i3PostProcessBloom);

i3PostProcessBloom::~i3PostProcessBloom()
{
	I3_SAFE_RELEASE(m_pCombineShader);
	I3_SAFE_RELEASE(m_pBrightPassShader);
}

void i3PostProcessBloom::Create(i3PostProcessManager * pManager)
{
	//처음 생성 시 Shader 로드
	i3Shader* pCombineShader = i3ShaderCache::LoadShader("i3BloomCombine.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_REF_CHANGE(m_pCombineShader, pCombineShader);

	i3Shader* pBrightPassShader = i3ShaderCache::LoadShader("i3BrightPathExtract.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_REF_CHANGE(m_pBrightPassShader, pBrightPassShader);
}

void i3PostProcessBloom::ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput)
{
	// scene <- Manager로 부터 이전 까지의 최종 결과 화면을 받아온다.
	// temp1 <- Manager로부터 할당 받는다. (화면 크기 1/2)
	// temp2 <- Manager로부터 할당 받는다. (화면 크기 1/2)
	i3RenderTarget * pBloomRT;

	pBloomRT = pManager->RegisterRenderTarget(0.5f, I3G_IMAGE_FORMAT_NONE);

	////현재 뿌려지고 있는 RT를 바탕으로, 가장 밝은 부분을 골라서 첫 번째 Render Target에 그린다.
	_ApplyBrightPass( pBloomRT, pInput, pManager, pCtx);

	////밝은 부분만 골라진 temp1을 대상으로 Gaussian Blur를 적용시켜준다.
	pManager->ApplyGaussianBlur(pBloomRT, m_BlurIteration, m_BlurTheta);

	// temp1에 Bright Pass + Gaussian Blur가 먹힌 상태이며, temp2는 Gaussian Blur 작업을 위한 기본 상태이다.
	// target <- Manager로부터 적절한 결과물 RT를 얻어온다.
	{
		// Combine
		pCtx->SetShader(m_pCombineShader);

		m_Ctx2.setTexture(pInput->GetColorTexture());
		m_pCombineShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texOriginal", &m_Ctx2, 0, false);

		m_Ctx.setTexture(pBloomRT->GetColorTexture());
		m_pCombineShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texBloom", &m_Ctx, 0, false);

		VEC2D intensity = VEC2D( m_fBaseIntensity, m_fBloomIntensity);
		m_pCombineShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_Intensity", &intensity, false);

		pManager->DrawFullScreenQuad(pOutput, pCtx);
	}
}

void i3PostProcessBloom::_ApplyBrightPass(i3RenderTarget * pDest, i3RenderTarget * pSrc, i3PostProcessManager * pManager, i3RenderContext * pCtx)
{
	pCtx->SetShader(m_pBrightPassShader);
	
	m_pBrightPassShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_threshold", m_fBloomThreshold, false);
	
	m_Ctx.setTexture(pSrc->GetColorTexture());

	m_pBrightPassShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_Ctx, 0, false);

	pManager->DrawFullScreenQuad(pDest, pCtx);
}

bool i3PostProcessBloom::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3PostProcess::OnSaveXML(pFile, pXML);

	if(bResult == false)
		return false;

	pXML->addAttr("Bloom_Threshold", m_fBloomThreshold);
	pXML->addAttr("Bloom_Intensity", m_fBloomIntensity);
	pXML->addAttr("Base_Intensity", m_fBaseIntensity);
	pXML->addAttr("Blur_Interation", m_BlurIteration);
	pXML->addAttr("Blur_Theta", m_BlurTheta);

	return true;
}

bool i3PostProcessBloom::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3PostProcess::OnLoadXML(pFile, pXML);

	if(bResult == false)
		return false;

	pXML->getAttr("Bloom_Threshold", &m_fBloomThreshold);
	pXML->getAttr("Bloom_Intensity", &m_fBloomIntensity);
	pXML->getAttr("Base_Intensity", &m_fBaseIntensity);
	pXML->getAttr("Blur_Interation", &m_BlurIteration);
	pXML->getAttr("Blur_Theta", &m_BlurTheta);

	return true;
}




