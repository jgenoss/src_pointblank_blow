#include "i3GfxType.h"
#include "i3RasterizeChainManager.h"
#include "i3RasterizeChain_ToneMapping.h"
#include "i3Shader.h"
#include "i3GfxGlobalVariable.h"
#include "i3RasterizeChainContext.h"

#define MAX_SAMPLES           16 

I3_CLASS_INSTANCE( i3RasterizeChain_ToneMapping);

i3RasterizeChain_ToneMapping::i3RasterizeChain_ToneMapping(void)
{		
	for( INT32 i=0; i<NUM_TONEMAP_TEXTURES; i++ )
	{
		m_pToneMapTexture[i] = nullptr;
		m_pToneMapDepthTexture[i] = nullptr;
	}
}

i3RasterizeChain_ToneMapping::~i3RasterizeChain_ToneMapping(void)
{
	I3_SAFE_RELEASE( m_pShaderSceneToSceneScale);
	I3_SAFE_RELEASE( m_pShaderMeasureLuminance);
	I3_SAFE_RELEASE( m_pShaderResampleAvgLum);
	I3_SAFE_RELEASE( m_pShaderResampleAvgLumExp);
	I3_SAFE_RELEASE( m_pShaderAdaptation);
	I3_SAFE_RELEASE( m_pShaderFinalScenePass);
	
	DestroyTexture();
}

INT32 i3RasterizeChain_ToneMapping::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->m_Format = I3G_IMAGE_FORMAT_ABGR_16F;
	return 1;
}

void i3RasterizeChain_ToneMapping::OnCreate( i3RasterizeChainManager * pManager)
{
	CreateTexture();

	RegisterTexture( pManager);

	ShaderCodeLoad();
}

void i3RasterizeChain_ToneMapping::CreateTexture(void)
{
	// Texture 생성
	{
		for( INT32 i=0; i<NUM_TONEMAP_TEXTURES; i++ )
		{
			INT32 iSampleLen = 1 << ( 2 * i );

			m_pToneMapTexture[i] = i3Texture::new_object();
#if defined( I3_DEBUG)
			char conv[256];
			i3::snprintf( conv, sizeof( conv), "ToneMapTexture(%d)", i);
			m_pToneMapTexture[i]->SetName( conv);
#endif
			m_pToneMapTexture[i]->Create( iSampleLen, iSampleLen, 1, I3G_IMAGE_FORMAT_R_32F, I3G_USAGE_RENDERTARGET);

			m_pToneMapDepthTexture[i] = i3Texture::new_object();
#if defined( I3_DEBUG)
			i3::snprintf( conv, sizeof( conv), "ToneMapTexture_Depth(%d)", i);
			m_pToneMapDepthTexture[i]->SetName( conv);
#endif
			m_pToneMapDepthTexture[i]->Create( iSampleLen, iSampleLen, 1, I3G_IMAGE_FORMAT_D24X8, I3G_USAGE_RENDERTARGET);
		}

		m_pLumCurTexture = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pLumCurTexture->SetName( "ToneMapping - Last Tone 0\n");
#endif
		m_pLumCurTexture->Create( 1, 1, 1, I3G_IMAGE_FORMAT_R_32F, I3G_USAGE_RENDERTARGET);

		m_pLumCurDepthTexture = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pLumCurDepthTexture->SetName( "ToneMapping - Last Tone 0 Depth\n");
#endif
		m_pLumCurDepthTexture->Create( 1, 1, 1, I3G_IMAGE_FORMAT_D24X8, I3G_USAGE_RENDERTARGET);

		m_pLumLastTexture = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pLumLastTexture->SetName( "ToneMapping - Last Tone 1\n");
#endif
		m_pLumLastTexture->Create( 1, 1, 1, I3G_IMAGE_FORMAT_R_32F, I3G_USAGE_RENDERTARGET);

		m_pLumLastDepthTexture = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pLumLastDepthTexture->SetName( "ToneMapping - Last Tone 1 Depth\n");
#endif
		m_pLumLastDepthTexture->Create( 1, 1, 1, I3G_IMAGE_FORMAT_D24X8, I3G_USAGE_RENDERTARGET);
	}
}

void i3RasterizeChain_ToneMapping::DestroyTexture(void)
{
	I3_SAFE_RELEASE( m_pLumCurDepthTexture);
	I3_SAFE_RELEASE( m_pLumCurDepthTexture);

	I3_SAFE_RELEASE( m_pLumLastDepthTexture);
	I3_SAFE_RELEASE( m_pLumLastDepthTexture);

	for( INT32 i=0; i<NUM_TONEMAP_TEXTURES; i++ )
	{
		I3_SAFE_RELEASE( m_pToneMapTexture[i]);
		I3_SAFE_RELEASE( m_pToneMapDepthTexture[i]);
	}
}

void i3RasterizeChain_ToneMapping::RegisterTexture( i3RasterizeChainManager * pManager)
{
	// 
	m_Imageformat = I3G_IMAGE_FORMAT_ABGR_16F;

	i3RenderTarget * pRT = g_pRenderContext->GetRenderTarget( 0);

	// Scaled Texture
	{
		UINT32 nCropWidth = pRT->GetWidth() - (pRT->GetWidth() % 8);
		UINT32 nCropHeight = pRT->GetHeight() - (pRT->GetHeight() % 8);
		UINT32 nWidth = (UINT32)(nCropWidth / 4);
		UINT32 nHeight = (UINT32)(nCropHeight / 4);

		m_pCtxSceneScaled = pManager->RegisterRT( nWidth, nHeight, I3G_IMAGE_FORMAT_ABGR_16F, true, "ToneMapping-DownScaledScreen");

		m_pCtxSceneScaled->SetSamplerUVWrapEnable( false);
		m_pCtxSceneScaled->setFilter( I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR);
	}

	// Tone Mapping Chain안에서만 사용되는...
	{
		m_ctxTex.m_MinFilter = I3G_TEXTURE_FILTER_LINEAR;
		m_ctxTex.m_MagFilter = I3G_TEXTURE_FILTER_LINEAR;
		m_ctxTex.m_HWrap	= I3G_TEXTURE_WRAP_CLAMP;
		m_ctxTex.m_VWrap	= I3G_TEXTURE_WRAP_CLAMP;
	}
}

void i3RasterizeChain_ToneMapping::ShaderCodeLoad(void)
{
	i3Shader* pShaderSceneToSceneScale = i3ShaderCache::LoadShader( "DownScale4x4PS.hlsl", I3G_SHADER_VERSION_2, I3G_SHADER_VERSION_2, I3G_GPU_VENDOR_NA, true);
	I3_REF_CHANGE( m_pShaderSceneToSceneScale, pShaderSceneToSceneScale);

	i3Shader* pShaderMeasureLuminance = i3ShaderCache::LoadShader( "i3SampleLumInitial.hlsl", I3G_SHADER_VERSION_2, I3G_SHADER_VERSION_2, I3G_GPU_VENDOR_NA, true);
	I3_REF_CHANGE( m_pShaderMeasureLuminance, pShaderMeasureLuminance);

	i3Shader* pShaderResampleAvgLum = i3ShaderCache::LoadShader( "i3SampleLumIterative.hlsl", I3G_SHADER_VERSION_2, I3G_SHADER_VERSION_2, I3G_GPU_VENDOR_NA, true);
	I3_REF_CHANGE( m_pShaderResampleAvgLum, pShaderResampleAvgLum);

	i3Shader* pShaderResampleAvgLumExp = i3ShaderCache::LoadShader( "i3SampleLumFinal.hlsl", I3G_SHADER_VERSION_2, I3G_SHADER_VERSION_2, I3G_GPU_VENDOR_NA, true);
	I3_REF_CHANGE( m_pShaderResampleAvgLumExp, pShaderResampleAvgLumExp);

	i3Shader* pShaderAdaptation = i3ShaderCache::LoadShader( "i3CalculateAdaptedLumPS.hlsl", I3G_SHADER_VERSION_2, I3G_SHADER_VERSION_2, I3G_GPU_VENDOR_NA, true);
	I3_REF_CHANGE( m_pShaderAdaptation, pShaderAdaptation);

	i3Shader* pShaderFinalScenePass = i3ShaderCache::LoadShader( "i3ToneMapFinalScenePassPS.hlsl", I3G_SHADER_VERSION_2, I3G_SHADER_VERSION_2, I3G_GPU_VENDOR_NA, true);
	I3_REF_CHANGE( m_pShaderFinalScenePass, pShaderFinalScenePass);
}

void i3RasterizeChain_ToneMapping::ApplyChain( I3RC_UPDATE * pInfo)
{
	// 1
	// 1/4 크기의 Downscale 이미지를 만든다.
	UpdateSceneScaled( pInfo);

	// 2
	// Luminance의 Log 평균을 구한다.
	UpdateMeasureLuminance( pInfo);

	// 3
	// 앞서 구해진 Log 평균의 산술 평균을 구한다.
	UpdateResampleAvgLuminance( pInfo);

	// 4
	// 최종 산술 평균의 exp 값을 계산.
	UpdateResampleAvgLuminanceEXP( pInfo);

	// 5
	UpdateAdaptationLuminance( pInfo);

	// 6
	UpdateFinalScenePass( pInfo);
}

void i3RasterizeChain_ToneMapping::_SetTonMapTexture( INT32 idx)
{
	I3ASSERT( idx >= 0 && idx < NUM_TONEMAP_TEXTURES);

#if defined( I3G_OGL)
	g_pRenderContext->enableFBOCompleteCheck( false);
#endif
	g_pRenderContext->SetColorRenderTarget( m_pToneMapTexture[m_nCurTonMapTexIdx]);
#if defined( I3G_OGL)
	g_pRenderContext->enableFBOCompleteCheck( true);
#endif
	g_pRenderContext->SetDepthRenderTarget( m_pToneMapDepthTexture[m_nCurTonMapTexIdx]);
}

void i3RasterizeChain_ToneMapping::_SetTonMapTexture( i3Texture * pColor, i3Texture * pDepth)
{
	I3ASSERT( pColor != nullptr && pDepth != nullptr);
#if defined( I3G_OGL)
	g_pRenderContext->enableFBOCompleteCheck( false);
#endif
	g_pRenderContext->SetColorRenderTarget( pColor);

#if defined( I3G_OGL)
	g_pRenderContext->enableFBOCompleteCheck( true);
#endif
	g_pRenderContext->SetDepthRenderTarget( pDepth);
}

void i3RasterizeChain_ToneMapping::UpdateSceneScaled( I3RC_UPDATE * pInfo)
{
	// Shader적용
	// DownScale4x4PS.hlsl
	g_pRenderContext->SetShader( m_pShaderSceneToSceneScale);		
		
	// Shader Parametter Update.
	{
		// Offset (원본 크기의 Render Target에서의 offset값을 구합니다)
		static VEC4D vecSampleOffsets[MAX_SAMPLES];

		i3RenderTarget * pRT = g_pRenderContext->GetRenderTarget( 0);

		GetSampleOffsets_DownScale4x4( pRT->GetWidth(), pRT->GetHeight(), vecSampleOffsets );

		m_pShaderSceneToSceneScale->SetParam( I3G_SHADER_TYPE_PIXEL, "vecSampleOffsets", vecSampleOffsets);
		
		// Texture Context
		i3ShaderParam * pParamTexScaled = m_pShaderSceneToSceneScale->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texInput");
		pInfo->m_pLastRT->UpdateShaderParam( pParamTexScaled);

		#if 0
		{
			// Save
			i3Surface * pSurface = pInfo->m_pLastRT->getColorTarget()->getSurface( 0);

			D3DXSaveSurfaceToFile( "D:\\skydump.pfm", D3DXIFF_PFM, pSurface->getSurface(), nullptr, nullptr);
		}
		#endif
	}
	
	// 설정된 렌더타겟에 텍스쳐를 그립니다.	
	m_pCtxSceneScaled->MakeCurrentTarget();
	Draw();
}

void i3RasterizeChain_ToneMapping::UpdateMeasureLuminance( I3RC_UPDATE * pInfo)
{
	m_nCurTonMapTexIdx = NUM_TONEMAP_TEXTURES - 1;

	// i3SampleLumInitial.hlsl
	g_pRenderContext->SetShader( m_pShaderMeasureLuminance);		

	// Shader Parametter Update.
	{
		// Offset
		{
			VEC4D vecSampleOffsets[MAX_SAMPLES];

			// Initialize the sample offsets for the initial luminance pass.
			float tU, tV;
			tU = 1.0f / ( 1.0f * m_pToneMapTexture[ m_nCurTonMapTexIdx]->GetWidth() );
			tV = 1.0f / ( 1.0f * m_pToneMapTexture[ m_nCurTonMapTexIdx]->GetHeight() );

			INT32 index = 0;
			for( INT32 x = -1; x <= 1; x++ )
			{
				for( INT32 y = -1; y <= 1; y++ )
				{
					vecSampleOffsets[index].x = x * tU;
					vecSampleOffsets[index].y = y * tV;
					index++;
				}
			}

			m_pShaderMeasureLuminance->SetParam( I3G_SHADER_TYPE_PIXEL, "g_vOffsets", vecSampleOffsets);
		}

		// Texture Context
		m_pShaderMeasureLuminance->SetParam( I3G_SHADER_TYPE_PIXEL, "g_texScaled", m_pCtxSceneScaled->getBindCtx());
		//pInfo->m_pLastRT->UpdateShaderParam( pParam);
	}

	// 설정된 렌더타겟에 텍스쳐를 그립니다.	
	_SetTonMapTexture( m_nCurTonMapTexIdx);
	Draw();
}

void i3RasterizeChain_ToneMapping::UpdateResampleAvgLuminance( I3RC_UPDATE * pInfo)
{
	VEC4D vecSampleOffsets[MAX_SAMPLES];	

	m_nCurTonMapTexIdx--;

	// i3SampleLumIterative.hlsl
	g_pRenderContext->SetShader( m_pShaderResampleAvgLum);				
	    
	// Initialize the sample offsets for the iterative luminance passes
	while( m_nCurTonMapTexIdx > 0 )
	{
		// Shader Parametter Update.
		{
			i3Texture * pTex = m_pToneMapTexture[ m_nCurTonMapTexIdx + 1];

			// Offset
			GetSampleOffsets_DownScale4x4( pTex->GetWidth(), pTex->GetHeight(), vecSampleOffsets );
			
			m_pShaderResampleAvgLum->SetParam( I3G_SHADER_TYPE_PIXEL, "vecSampleOffsets", vecSampleOffsets);

			// Texture Context
			m_ctxTex.m_pTexture = pTex;
			m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;
			
			m_pShaderResampleAvgLum->SetParam( I3G_SHADER_TYPE_PIXEL, "g_tex", &m_ctxTex);
		}

		// 설정된 렌더타겟에 텍스쳐를 그립니다.			
		_SetTonMapTexture( m_nCurTonMapTexIdx);
		Draw();

		m_nCurTonMapTexIdx--;
	}
}

void i3RasterizeChain_ToneMapping::UpdateResampleAvgLuminanceEXP( I3RC_UPDATE * pInfo)
{
	// i3SampleLumFinal.hlsl
	g_pRenderContext->SetShader( m_pShaderResampleAvgLumExp);

	// Shader Parametter Update.
	{
		i3Texture * pTex = m_pToneMapTexture[1];

		// Offset
		VEC4D vecSampleOffsets[MAX_SAMPLES];

		GetSampleOffsets_DownScale4x4( pTex->GetWidth(), pTex->GetHeight(), vecSampleOffsets );

		m_pShaderResampleAvgLumExp->SetParam( I3G_SHADER_TYPE_PIXEL, "vecSampleOffsets", vecSampleOffsets);

		// Texture Context
		m_ctxTex.m_pTexture = pTex;
		m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;

		m_pShaderResampleAvgLumExp->SetParam( I3G_SHADER_TYPE_PIXEL, "g_tex", &m_ctxTex);		
	}

	// 설정된 렌더타겟에 텍스쳐를 그립니다.	
	_SetTonMapTexture( 0);
	Draw();
}

void i3RasterizeChain_ToneMapping::UpdateAdaptationLuminance( I3RC_UPDATE * pInfo)
{
	REAL32 fDeltaSec = m_AdaptionTimer.GetDeltaSec();

	// i3CalculateAdaptedLumPS.hlsl
	g_pRenderContext->SetShader( m_pShaderAdaptation);	

	// Swap current & last luminance
	i3Texture * pTexSawp = m_pLumLastTexture;
	m_pLumLastTexture = m_pLumCurTexture;
	m_pLumCurTexture = pTexSawp;
		
	// Shader Parametter Update.
	{
		// Time(Elapsed..)
		m_pShaderAdaptation->SetParam( I3G_SHADER_TYPE_PIXEL, "fElapsedTime", fDeltaSec);

		// Texture Context(Adaption)
		m_ctxTex.m_pTexture = m_pLumLastTexture;
		m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;
		m_pShaderAdaptation->SetParam( I3G_SHADER_TYPE_PIXEL, "g_tex1", &m_ctxTex);

		// Texture Context(Last ToneMap Tex 1*1)
		m_ctxTex.m_pTexture = m_pToneMapTexture[0];
		m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;
		m_pShaderAdaptation->SetParam( I3G_SHADER_TYPE_PIXEL, "g_tex2", &m_ctxTex);
	}

	// 설정된 렌더타겟에 텍스쳐를 그립니다.	
	_SetTonMapTexture( m_pLumCurTexture, m_pLumCurDepthTexture);
	Draw();
}

void i3RasterizeChain_ToneMapping::UpdateFinalScenePass( I3RC_UPDATE * pInfo)
{
	i3ShaderParam * pParam;

	// i3ToneMapFinalScenePassPS.hlsl
	g_pRenderContext->SetShader( m_pShaderFinalScenePass);
	
	// Shader Parametter Update.
	{
		// Adjust Value
		{
			VEC2D vec;

			i3Vector::Set( &vec, m_fMiddleGray, m_SmallestLumToWhite * m_SmallestLumToWhite);

			m_pShaderFinalScenePass->SetParam( I3G_SHADER_TYPE_PIXEL, "g_ToneMapCoeff", &vec);
		}

		// Texture Context(Original First Scene-HDR)
		pParam = m_pShaderFinalScenePass->FindParam( I3G_SHADER_TYPE_PIXEL, "g_tex1");
		pInfo->m_pLastRT->UpdateShaderParam( pParam);

		// Texture Context(Final Result about Luminance Adaption)
		m_ctxTex.m_pTexture = m_pLumCurTexture;
		m_ctxTex.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;
		m_pShaderFinalScenePass->SetParam( I3G_SHADER_TYPE_PIXEL, "g_tex2", &m_ctxTex);
	}

	// 설정된 렌더타겟에 텍스쳐를 그립니다.	
	getManager()->SetRenderTarget( getTarget());
	Draw();
}


//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale4x4
// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
//       pixel shader.
//-----------------------------------------------------------------------------
bool i3RasterizeChain_ToneMapping::GetSampleOffsets_DownScale4x4( INT32 nWidth, INT32 nHeight, VEC4D* vecSampleOffsets)
{
    if( nullptr == vecSampleOffsets )
		return false;

    REAL32 tU = 1.0f / nWidth;
    REAL32 tV = 1.0f / nHeight;

    // Sample from the 16 surrounding points. Since the center point will be in
    // the exact center of 16 texels, a 0.5f offset is needed to specify a texel
    // center.
    INT32 index = 0;
    for( INT32 y = 0; y < 4; y++ )
    {
        for( INT32 x = 0; x < 4; x++ )
        {
            vecSampleOffsets[ index ].x = ( x - 0.0f ) * tU;
            vecSampleOffsets[ index ].y = ( y - 0.0f ) * tV;

            index++;
        }
    }

    return true;
}




//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale2x2
// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2
//       pixel shader.
//-----------------------------------------------------------------------------
bool i3RasterizeChain_ToneMapping::GetSampleOffsets_DownScale2x2( INT32 nWidth, INT32 nHeight, VEC4D* vecSampleOffsets)
{
    if( nullptr == vecSampleOffsets )
        return false;

    REAL32 tU = 1.0f / nWidth;
    REAL32 tV = 1.0f / nHeight;

    // Sample from the 4 surrounding points. Since the center point will be in
    // the exact center of 4 texels, a 0.5f offset is needed to specify a texel
    // center.
    INT32 index = 0;
    for( INT32 y = 0; y < 2; y++ )
    {
        for( INT32 x = 0; x < 2; x++ )
        {
            vecSampleOffsets[ index ].x = ( x - 0.5f ) * tU;
            vecSampleOffsets[ index ].y = ( y - 0.5f ) * tV;

            index++;
        }
    }

    return true;
}

bool i3RasterizeChain_ToneMapping::Destroy( bool bLostDevice)
{
	DestroyTexture();

	return i3RasterizeChainBase::Destroy( bLostDevice);
}

bool i3RasterizeChain_ToneMapping::Revive( i3RenderContext * pCtx)
{
	CreateTexture();

	return i3RasterizeChainBase::Revive(pCtx);
}
