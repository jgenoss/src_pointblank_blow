#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChain_SimpleMotionBlur.h"
#include "i3RasterizeChainContext.h"

I3_CLASS_INSTANCE( i3RasterizeChain_SimpleMotionBlur);

#define		BLUR_DEFAULT_WIDTH		640
#define		BLUR_DEFAULT_HEIGHT		480

#define		BLUR_DEFAULT_ALPHA			0.5f
#define		BLUR_DEFAULT_ALPHA_MAX		0.99f
#define		BLUR_DEFAULT_ALPHA_MIN		0.1f

i3RasterizeChain_SimpleMotionBlur::i3RasterizeChain_SimpleMotionBlur(void) : m_nBlurHeight(BLUR_DEFAULT_HEIGHT), m_nBlurWidth(BLUR_DEFAULT_WIDTH),
	m_fAlphaBlur(BLUR_DEFAULT_ALPHA), m_fAlphaStart(BLUR_DEFAULT_ALPHA_MAX), m_fAlphaEnd(BLUR_DEFAULT_ALPHA_MIN)
{
}

i3RasterizeChain_SimpleMotionBlur::~i3RasterizeChain_SimpleMotionBlur(void)
{
	I3_SAFE_RELEASE( m_pShader_BitBlt);
	I3_SAFE_RELEASE( m_pShader_Blend);
}

INT32	i3RasterizeChain_SimpleMotionBlur::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->setToDefault();

	return 1;
}

void i3RasterizeChain_SimpleMotionBlur::OnCreate( i3RasterizeChainManager * pManager)
{	
	//	블러 해상도
	i3RenderTarget * pRT = g_pRenderContext->GetRenderTarget( 0);
	m_nBlurWidth	= pRT->GetWidth();
	m_nBlurHeight	= pRT->GetHeight();	

	// Texture State 등록 
	{
		// Tex1
		m_pCtxTexIn = pManager->RegisterNamedRT( -1, -1, I3G_IMAGE_FORMAT_NONE, false, "Tex1");
		
		// Tex2
		m_pCtxTexOut = pManager->RegisterNamedRT( -1, -1, I3G_IMAGE_FORMAT_NONE, false, "Tex2");
	}

	{
		i3Shader* pShader_BitBlt = i3ShaderCache::LoadShader( "i3BitBlit.hlsl");
		I3_REF_CHANGE( m_pShader_BitBlt, pShader_BitBlt);

		i3Shader* pShader_Blend = i3ShaderCache::LoadShader( "i3Blend.hlsl");
		I3_REF_CHANGE( m_pShader_Blend, pShader_Blend);
	}

	{
		m_ctxTex.m_MinFilter = I3G_TEXTURE_FILTER_POINT;
		m_ctxTex.m_MagFilter = I3G_TEXTURE_FILTER_POINT;
		m_ctxTex.m_HWrap = I3G_TEXTURE_WRAP_CLAMP;
		m_ctxTex.m_VWrap = I3G_TEXTURE_WRAP_CLAMP;
	}

	m_fBlurTime = 0.f;
	m_fProcessingTime = 0.f;
}

void i3RasterizeChain_SimpleMotionBlur::StartEffect(REAL32 fTime, REAL32 fStartBlurVal, REAL32 fEndBlurVal)
{	
	//	fTime이 0.0f이면 무한 반복
	//	fStartBlurVal과 fEndBlurVal 값이 같으면 알파 기복 없이 일정한 값으로 유지
	SetEnable(true);

	m_fAlphaStart = fStartBlurVal;
	m_fAlphaEnd = fEndBlurVal;

	SetBlurAlpha( fStartBlurVal);

	m_fBlurTime = fTime;
	m_fProcessingTime = 0.0f;
}

void i3RasterizeChain_SimpleMotionBlur::EndEffect(void)
{
	SetEnable(false);

	m_fBlurTime = m_fProcessingTime = 0;
}

void i3RasterizeChain_SimpleMotionBlur::SetEnable(bool bFlag)
{
	if( !getEnable())
	{
		m_bfirstEnable = true;
	}

	i3RasterizeChainBase::SetEnable( bFlag);
}

void i3RasterizeChain_SimpleMotionBlur::ApplyAccumBlur( I3RC_UPDATE * pInfo)
{
	//	누적될 블러의 색상조절
	m_BlurColor.a = GetBlurAlpha();

	if(m_bfirstEnable == true)
	{
		// 처음에는 단순히 복사만 한다.
		g_pRenderContext->SetShader( m_pShader_BitBlt);


		{
			i3RasterizeChainContext * pLastRT = pInfo->m_pLastRT;

			m_ctxTex.m_pTexture = pLastRT->getColorTarget();
			m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;

			m_pShader_BitBlt->SetParam( I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_ctxTex);
		}
	}
	else
	{
		g_pRenderContext->SetShader( m_pShader_Blend);

		{
			i3RasterizeChainContext * pLastRT = pInfo->m_pLastRT;

			m_ctxTex.m_pTexture = pLastRT->getColorTarget();
			m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;

			m_pShader_Blend->SetParam( I3G_SHADER_TYPE_PIXEL, "g_texInputBase", &m_ctxTex);

			m_ctxTex.m_pTexture = m_pCtxTexIn->getColorTarget();
			m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;
			m_pShader_Blend->SetParam( I3G_SHADER_TYPE_PIXEL, "g_texInputBlend", &m_ctxTex);

			m_pShader_Blend->SetParam( I3G_SHADER_TYPE_PIXEL, "g_blend", &m_BlurColor);
		}
	}

	m_bfirstEnable = false;

	getManager()->SetRenderTarget( m_pCtxTexOut);
	
	Draw();
}


void i3RasterizeChain_SimpleMotionBlur::ApplyBlur( I3RC_UPDATE * pInfo)
{
	getManager()->SetRenderTarget( getTarget());
	
	g_pRenderContext->SetShader( m_pShader_BitBlt);

	{
		m_ctxTex.m_pTexture = m_pCtxTexOut->getColorTarget();
		m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;

		m_pShader_BitBlt->SetParam( I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_ctxTex);
	}
	
	Draw();
}

void i3RasterizeChain_SimpleMotionBlur::ApplyChain( I3RC_UPDATE * pInfo)
{
	if( m_fBlurTime == 0.0f)
	{
		//	블러 알파 지정
		SetBlurAlpha( m_fAlphaStart);
	}
	else if( m_fProcessingTime < m_fBlurTime)
	{
		REAL32 gap = m_fAlphaEnd - m_fAlphaStart;
		REAL32 accum = m_fAlphaStart + ((m_fProcessingTime / m_fBlurTime) * gap);

		//	블러 알파 지정
		SetBlurAlpha( accum);

		m_fProcessingTime += pInfo->m_time;

		if(m_fProcessingTime >= m_fBlurTime)
		{
			EndEffect();	//	모션 블러 종료
		}
	}


	g_pRenderContext->SetShader( nullptr);

	//렌더링된 화면에다가 
	//전 프레임까지 누적된 블러 텍스쳐를 찍어 버린다. 
	ApplyAccumBlur( pInfo);	

	//화면에 복사한다.
	ApplyBlur( pInfo);

	{
		i3RasterizeChainContext * pTemp;

		// Input과 Output을 변경한다.
		pTemp = m_pCtxTexIn;
		m_pCtxTexIn = m_pCtxTexOut;
		m_pCtxTexOut = pTemp;
	}
}