#include "i3GfxType.h"
#include "i3ShaderCache.h"
#include "i3PostProcessManager.h"
#include "i3PostProcessCelShading.h"

I3_CLASS_INSTANCE(i3PostProcessCelShading);

i3PostProcessCelShading::~i3PostProcessCelShading()
{
	//할당한 셰이더는 파괴자에서 해제해줍니다.
	I3_SAFE_RELEASE(m_pEdgeShader);
	I3_SAFE_RELEASE(m_pCelShader);
	I3_SAFE_RELEASE(m_pShader_BitBlt);
}

void i3PostProcessCelShading::Create(i3PostProcessManager * pManager)
{
	//Create() 함수에서 셰이더 로드.
	//추가로 초기화해야 할 코드가 있다면 이 곳에서 초기화해줍니다.
	//i3PostProcessManager::AddPostProcess()를 호출할 시 같이 불립니다.

	//셀 셰이딩을 위해 제작된 2개의 셰이더 및 복사용 셰이더, 총 3개의 셰이더를 로드합니다.
	i3Shader* pEdgeShader = i3ShaderCache::LoadShader("i3EdgeDetection.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_REF_CHANGE(m_pEdgeShader, pEdgeShader);

	i3Shader* pCelShader = i3ShaderCache::LoadShader("i3CelShading.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_REF_CHANGE(m_pCelShader, pCelShader);

	i3Shader* pShader_BitBlt = i3ShaderCache::LoadShader("i3BitBlit.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_REF_CHANGE(m_pShader_BitBlt, pShader_BitBlt);
}

void i3PostProcessCelShading::ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput)
{
	//Post Process 진행 과정 중 사용해야 할 i3RenderTarget을
	//RegisterRenderTarget을 통해 할당받습니다.
	i3RenderTarget * pTemp = pManager->RegisterRenderTarget(2.0f, I3G_IMAGE_FORMAT_NONE);

	//i3RenderContext에 Shader 세팅.
	//이 셰이더는 렌더 타겟을 복사하는 셰이더입니다.
	pCtx->SetShader(m_pShader_BitBlt);

	//그려야 할 i3RenderTarget의 텍스처를 I3G_TEXTURE_BIND_CONTEXT에 세팅해줍니다.
	//셰이더에 인자를 전달하기 위해 멤버 변수로 I3G_TEXTURE_BIND_CONTEXT 클래스를 가지고 있습니다.
	//옵션이 달라질 수 있으므로 주의해야 합니다.
	m_Ctx.setTexture(pInput->GetColorTexture());

	m_pShader_BitBlt->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_Ctx);

	//할당받은 i3RenderTarget에 세팅된 셰이더를 사용하여 그립니다(pTemp는 Back buffer인 i3RenderTarget이 아니므로, Off-screen 렌더입니다).
	pManager->DrawFullScreenQuad(pTemp, pCtx);

	//바로 셀 셰이딩으로 넘어가면 결과에 노이즈가 심합니다.
	//i3PostProcessManager를 사용하여 low-pass filter인 가우시안 블러를 적용해줍니다. 역시 pTemp에 그려집니다.
	pManager->ApplyGaussianBlur(pTemp, 1, 0.5f);

	//본격적인 셀 셰이딩을 위해 셰이더를 세팅합니다.
	pCtx->SetShader(m_pCelShader);
	
	//Off-screen에 그려진 결과를 사용하기 위해 pTemp의 텍스처를 가져와 셰이더에 인자로 세팅해줍니다.
	m_Ctx.setTexture(pTemp->GetColorTexture());
	m_pCelShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_Ctx, 0, false);

	//셀 셰이딩의 결과를 다른 곳에 그리기 위해, 새로운 i3RenderTarget을 할당받습니다.
	i3RenderTarget * pTemp2 = pManager->RegisterRenderTarget(2.0f, I3G_IMAGE_FORMAT_NONE);

	//할당받은 i3RenderTarget에 그립니다.
	pManager->DrawFullScreenQuad(pTemp2, pCtx);

	//셀 셰이딩의 모습을 강조하기 위해 Edge detection으로 테두리를 그려줍니다.
	pCtx->SetShader(m_pEdgeShader);
	
	m_Ctx.setTexture(pTemp2->GetColorTexture());
	m_pEdgeShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_Ctx, 0, false);

	m_pEdgeShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_thickness", 3.5f, false);
	m_pEdgeShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_threshold", 0.7f, false);
 
	//Edge에서도 발생한 노이즈를 제거하기 위해 low-pass filter를 한 번 더 적용합니다. 새로운 i3RenderTarget을 한 번 더 할당받습니다.
	i3RenderTarget * pTemp3 = pManager->RegisterRenderTarget(2.0f, I3G_IMAGE_FORMAT_NONE);
	pManager->DrawFullScreenQuad(pTemp3, pCtx);

	//Edge detection된 결과에 low-pass filter 적용.
	pManager->ApplyGaussianBlur(pTemp3, 1, 0.5f);

	pCtx->SetShader(m_pShader_BitBlt);

	m_Ctx.setTexture(pTemp3->GetColorTexture());
	m_pShader_BitBlt->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_Ctx);

	//만약 이 i3PostProcess가 마지막으로 나오는 결과라면, pOutput은 Back buffer가 넘어오기 때문에 바로 화면에 그려집니다.
	//마지막이 아니라면, 해당 결과는 다음 i3PostProcess에 pInput으로 넘어가게 됩니다.
	pManager->DrawFullScreenQuad(pOutput, pCtx);
}