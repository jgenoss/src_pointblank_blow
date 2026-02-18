#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChain_BloomCombine.h"


I3_CLASS_INSTANCE( i3RasterizeChain_BloomCombine);

i3RasterizeChain_BloomCombine::~i3RasterizeChain_BloomCombine(void)
{
	I3_SAFE_RELEASE( m_pShader);
}

INT32 i3RasterizeChain_BloomCombine::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->setToDefault();

	return 1;
}

void i3RasterizeChain_BloomCombine::OnCreate( i3RasterizeChainManager * pManager)
{
	// Chaing 초기화

	// Bright Path Texture
	{
		m_pCtxCombineTarget = getManager()->RegisterNamedRT( -1, -1, I3G_IMAGE_FORMAT_NONE, true, "BloomC");
	}
	
	// 2. Shader Code로드
	{
		i3Shader* pShader = i3ShaderCache::LoadShader( "i3BloomCombine.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
		I3_REF_CHANGE( m_pShader, pShader);
	}
}

void i3RasterizeChain_BloomCombine::ApplyChain( I3RC_UPDATE * pInfo)
{
	i3RenderContext * pGfx = pInfo->m_pGfx;
	//i3RasterizeChainContext * pCtxInput = nullptr;
	//i3RasterizeChainContext * pCtxBright = nullptr;

	i3RasterizeChainContext * pCtxOriginal = pInfo->m_pLastRT;
	if( pCtxOriginal == nullptr) return;

	/*
	if( getManager()->FindChainByName( i3RasterizeChain_Bloom::GetClassMeta()))
	{
		pCtxInput		= getManager()->FindNamedRT( "Bloom", false);
	}
	*/

	//if( pCtxInput != nullptr)
	//{
	//	pGfx->SetShader( m_pShader);

	//	// Shader Parameter Update!!!
	//	{
	//		// Texture Context
	//		i3ShaderParam * pParamTexBloom = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texBloom");
	//		pCtxInput->UpdateShaderParam( pParamTexBloom);

	//		// Texture Context
	//		i3ShaderParam * pParamTexOriginal = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texOriginal");
	//		pCtxOriginal->UpdateShaderParam( pParamTexOriginal);

	//		//
	//		m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_BloomIntensity", m_fBloomIntensity);
	//		m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_BaseIntensity", m_fBaseIntensity);
	//		m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_BloomSaturation", m_fBloomSat);
	//		m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_BaseSaturation", m_fBaseSat);
	//	}

	//}

	Draw();

	pGfx->SetShader( nullptr);
}

void i3RasterizeChain_BloomCombine::SetBloomSat(REAL32 fVal)
{
	m_fBloomSat = fVal;
	if( m_fBloomSat < 0.0f) m_fBloomSat = 0.0f;
}

void i3RasterizeChain_BloomCombine::SetBaseSat(REAL32 fVal)
{
	m_fBaseSat = fVal;
	if( m_fBaseSat < 0.0f) m_fBaseSat = 0.0f;
}

void i3RasterizeChain_BloomCombine::SetBloomIntensity(REAL32 fVal)
{
	m_fBloomIntensity = fVal;
	if( m_fBloomIntensity < 0.0f) m_fBloomIntensity = 0.0f;
}

void i3RasterizeChain_BloomCombine::SetBaseIntensity(REAL32 fVal)
{
	m_fBaseIntensity = fVal;
	if( m_fBaseIntensity < 0.0f) m_fBaseIntensity = 0.0f;
}
