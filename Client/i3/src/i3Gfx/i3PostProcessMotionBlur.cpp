#include "i3GfxType.h"
#include "i3ShaderCache.h"
#include "i3PostProcessManager.h"
#include "i3PostProcessMotionBlur.h"

I3_CLASS_INSTANCE(i3PostProcessMotionBlur);

i3PostProcessMotionBlur::i3PostProcessMotionBlur()
{
	m_pCtx.setFilter(I3G_TEXTURE_FILTER_POINT, I3G_TEXTURE_FILTER_POINT);
}

i3PostProcessMotionBlur::~i3PostProcessMotionBlur()
{
	I3_SAFE_RELEASE(m_pShader_BitBlt);
	I3_SAFE_RELEASE(m_pShader_Blend);
}

void i3PostProcessMotionBlur::Create(i3PostProcessManager * pManager)
{
	i3Shader* pShader_BitBlt = i3ShaderCache::LoadShader( "i3BitBlit.hlsl");
	I3_REF_CHANGE( m_pShader_BitBlt, pShader_BitBlt);

	i3Shader* pShader_Blend = i3ShaderCache::LoadShader( "i3Blend.hlsl");
	I3_REF_CHANGE( m_pShader_Blend, pShader_Blend);
}

void i3PostProcessMotionBlur::StartMotionBlur(REAL32 time, REAL32 startAlpha, REAL32 endAlpha)
{
	SetEnable(true);

	m_fBlurAlpha = startAlpha;
	m_fStartAlpha = startAlpha;
	m_fEndAlpha = endAlpha;

	m_fBlurTime = time;
	m_fProcessingTime = 0.0f;
}

void i3PostProcessMotionBlur::EndMotionBlur()
{
	SetEnable(false);

	m_fProcessingTime = 0.0f;
	m_fBlurTime = 0.0f;
}

void i3PostProcessMotionBlur::AccumulateBlur(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput)
{
	//블러 누적.
	m_BlurColor.a = m_fBlurAlpha;

	if(m_bFirstEnable)
	{
		pCtx->SetShader(m_pShader_BitBlt);

		m_pCtx.setTexture(pInput->GetColorTexture());
		m_pShader_BitBlt->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_pCtx);
	}
	else
	{
		pCtx->SetShader(m_pShader_Blend);

		m_pCtx.setTexture(pInput->GetColorTexture());
		m_pShader_Blend->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInputBase", &m_pCtx);

		m_pCtx.setTexture(m_pPreviousRT->GetColorTexture());
		m_pShader_Blend->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInputBlend", &m_pCtx);

		m_pShader_Blend->SetParam(I3G_SHADER_TYPE_PIXEL, "g_blend", &m_BlurColor);
	}

	m_bFirstEnable = false;

	pManager->DrawFullScreenQuad(m_pPreviousRT2, pCtx, nullptr);
}

void i3PostProcessMotionBlur::ApplyBlur(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pOutput)
{
	pCtx->SetShader(m_pShader_BitBlt);

	m_pCtx.setTexture(m_pPreviousRT2->GetColorTexture());

	m_pShader_BitBlt->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_pCtx);

	pManager->DrawFullScreenQuad(pOutput, pCtx, nullptr);
}

void i3PostProcessMotionBlur::ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput)
{
	//블러 적용 시간에 따라 블러의 알파값을 조절한다.
	if(m_fBlurTime != 0.0f)
	{
		if(m_fProcessingTime < m_fBlurTime)
		{
			REAL32 gap = m_fEndAlpha - m_fStartAlpha;
			REAL32 accum = m_fStartAlpha + (m_fProcessingTime / m_fBlurTime) * gap;

			m_fBlurAlpha = accum;

			m_fProcessingTime += pManager->GetDeltaTime();

			if(m_fProcessingTime >= m_fBlurTime)
			{
				//effect 중지.
				EndMotionBlur();
			}
		}
	}

	//m_pPreviousRT		<- 이전 프레임
	//m_pPreviousRT2	<- 모션 블러가 적용될 임시 프레임
	m_pPreviousRT = pManager->RegisterRenderTarget(1.0f, I3G_IMAGE_FORMAT_NONE, "BlurInRT", false);
	m_pPreviousRT2 = pManager->RegisterRenderTarget(1.0f, I3G_IMAGE_FORMAT_NONE, "BlurOutRT", false);

	pCtx->SetShader(nullptr);

	//블러 결과를 누적시켜 m_pPreviousRT2에 그린다.
	AccumulateBlur(pManager, pCtx, pInput); 

	//그린 블러 결과를 인자로 전달된 pOutput에 그린다.
	ApplyBlur(pManager, pCtx, pOutput);

	//이 이후, 현재 그려진 결과를 다음 Apply시에 쓰기 위해 이름을 바꿔준다.
	//RegisterRenderTarget에서 이름으로 검색 후 렌더 타겟을 생성하기 때문에, 이름만 바꿔주면 알아서 검색해준다.
	m_pPreviousRT->SetName("BlurOutRT");
	m_pPreviousRT2->SetName("BlurInRT");
}

void i3PostProcessMotionBlur::SetEnable(bool isEnable)
{
	if(!isEnable)
	{
		m_bFirstEnable = true;	
	}

	i3PostProcess::SetEnable(isEnable);
}
