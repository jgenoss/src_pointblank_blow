#include "i3GfxType.h"
#include "i3RasterizeChain_GaussianBlur.h"
#include "i3RasterizeChainManager.h"
#include "i3Shader.h"
#include "i3ShaderCache.h"
#include "i3RenderContext.h"
#include "i3RasterizeChainContext.h"

I3_CLASS_INSTANCE( i3RasterizeChain_GaussianBlur);

i3RasterizeChain_GaussianBlur::~i3RasterizeChain_GaussianBlur(void)
{
	I3_SAFE_RELEASE( m_pShader);
}

INT32 i3RasterizeChain_GaussianBlur::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->setToDefault();

	return 1;
}

void i3RasterizeChain_GaussianBlur::OnCreate( i3RasterizeChainManager * pManager)
{
	{
		m_pCtxBlurTarget = pManager->RegisterNamedRT( 0, 0, I3G_IMAGE_FORMAT_NONE, false, "Blur");
	}

	{
		i3Shader* pShader = i3ShaderCache::LoadShader( "i3GaussianBlur.hlsl");
		I3_REF_CHANGE( m_pShader, pShader);
		m_pShader->setRequestedPSVersion(I3G_SHADER_VERSION_2);
	}
}

void i3RasterizeChain_GaussianBlur::ApplyChain( I3RC_UPDATE * pInfo)
{
	i3RenderContext * pGfx = pInfo->m_pGfx;
	i3ShaderParam * pParam;

	pGfx->SetShader( m_pShader);	

	i3RasterizeChainContext * pCtxInput = pInfo->m_pLastRT;
	if( pCtxInput == nullptr) return;
	
	REAL32 sx = (REAL32)pCtxInput->getWidth();
	REAL32 sy = (REAL32)pCtxInput->getHeight();

	// 필요하다면, Multiple GaussianBlur로 합니다.

	// step 1..	
	{
		m_pCtxBlurTarget->MakeCurrentTarget();
		// X축
		SetBlurParameter( 0.0f, 1.0f / sy, 3);

		pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texInput");
		pCtxInput->UpdateShaderParam( pParam);
		Draw();

		// Y축
		pCtxInput->MakeCurrentTarget();
		
		SetBlurParameter( 1.0f / sx, 0.0f, 3);

		pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texInput");
		m_pCtxBlurTarget->UpdateShaderParam( pParam);
		Draw();
	}
}

void i3RasterizeChain_GaussianBlur::SetBlurParameter( REAL32 fX, REAL32 fY, INT32 nPixelCount)
{
	I3ASSERT( nPixelCount <= 3);

	i3ShaderParam * pParamCount = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "sampleCount");
	i3ShaderParam * pParamOffset = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "SampleOffsets");
	i3ShaderParam * pParamWeight = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "SampleWeights");
 
    // Maintain a sum of all the weighting values.
	REAL32 totalWeights = ComputeGaussian( 0);
	
	m_vecSampleWeight[0].x = totalWeights;

	i3Vector::Set( &m_vecSampleOffset[0], 0.0f, 0.0f, 0.0f, 0.0f);

    // Add pairs of additional sample taps, positioned
    // along a line in both directions from the center.
    for (INT32 i = 0; i < nPixelCount / 2; i++)
    {
        // Store weights for the positive and negative taps.
        REAL32 weight = ComputeGaussian((REAL32)(i + 1));

		i3Vector::SetX( &m_vecSampleWeight[i * 2 + 1], weight);
        i3Vector::SetX( &m_vecSampleWeight[i * 2 + 2], weight);

		totalWeights += weight * 2;

        // To get the maximum amount of blurring from a limited number of
        // pixel shader samples, we take advantage of the bilinear filtering
        // hardware inside the texture fetch unit. If we position our texture
        // coordinates exactly halfway between two texels, the filtering unit
        // will average them for us, giving two samples for the price of one.
        // This allows us to step in units of two texels per sample, rather
        // than just one at a time. The 1.5 offset kicks things off by
        // positioning us nicely in between two texels.
        REAL32 sampleOffset = i * 2 + 1.5f;

        //VEC2D delta = new Vector2(dx, dy) * sampleOffset;
		VEC2D delta;
		VEC2D delta2;
		i3Vector::Set( &delta, fX * sampleOffset, fY * sampleOffset);
		i3Vector::Set( &delta2, -fX * sampleOffset, -fY * sampleOffset);

        // Store texture coordinate offsets for the positive and negative taps.
		i3Vector::SetX( &m_vecSampleOffset[i * 2 + 1], i3Vector::GetX( &delta));
		i3Vector::SetY( &m_vecSampleOffset[i * 2 + 1], i3Vector::GetY( &delta));

		i3Vector::SetX( &m_vecSampleOffset[i * 2 + 2], i3Vector::GetX( &delta2));
		i3Vector::SetY( &m_vecSampleOffset[i * 2 + 2], i3Vector::GetY( &delta2));
    }

    // Normalize the list of sample weightings, so they will always sum to one.
    for (INT32 i = 0; i < nPixelCount; i++)
    {
        //m_SampleWeight[i] /= totalWeights;		
		i3Vector::SetX( &m_vecSampleWeight[i], i3Vector::GetX( &m_vecSampleWeight[i]) / totalWeights);
    }

    // Tell the effect about our new filter settings.
	if(pParamOffset)pParamOffset->Update( m_vecSampleOffset);
	if(pParamWeight)pParamWeight->Update( m_vecSampleWeight);
	if(pParamCount)pParamCount->Update( (REAL32)nPixelCount);
}

REAL32 i3RasterizeChain_GaussianBlur::ComputeGaussian(REAL32 n)
{	
	if( m_fGaussiantheta <= 0.1f) m_fGaussiantheta = 0.1f;
	else if( m_fGaussiantheta > 7.0f) m_fGaussiantheta = 7.0f;

	return (REAL32)((1.0 / i3Math::Sqrt(2 * I3_PI * m_fGaussiantheta)) * exp(-(n * n) / (2 * m_fGaussiantheta * m_fGaussiantheta)));
}

 