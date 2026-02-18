#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChain_BrightPathExtract.h"
#include "i3RasterizeChainContext.h"

I3_CLASS_INSTANCE( i3RasterizeChain_BrightPathExtract);

i3RasterizeChain_BrightPathExtract::~i3RasterizeChain_BrightPathExtract(void)
{
	I3_SAFE_RELEASE( m_pShader);
}

INT32 i3RasterizeChain_BrightPathExtract::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->setToDefault();

	return 1;
}

void i3RasterizeChain_BrightPathExtract::OnCreate( i3RasterizeChainManager * pManager)
{
	m_pCtxBrightPath = pManager->RegisterNamedRT( "BrightPass");

	{
		i3Shader* pShader = i3ShaderCache::LoadShader( "i3BrightPathExtract.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
		I3_REF_CHANGE( m_pShader, pShader);
	}
}

void i3RasterizeChain_BrightPathExtract::ApplyChain( I3RC_UPDATE * pInfo)
{
	i3RenderContext * pGfx = pInfo->m_pGfx;

	pGfx->SetShader( m_pShader);	

	i3RasterizeChainContext * pCTXInput = pInfo->m_pLastRT;

	// Shader Parameter Update
	{
		// 밝은부분을 결정하는 변수
		m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_threshold", m_fThreshold);

		// Texture Context
		i3ShaderParam * pParamTexInput = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texInput");
		pCTXInput->UpdateShaderParam( pParamTexInput);
	}
	
	// 설정된 렌더타겟에 텍스쳐를 그립니다.	
	Draw();
}
