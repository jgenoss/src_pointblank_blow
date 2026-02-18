#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChain_DownScale.h"
#include "i3RasterizeChainContext.h"

I3_CLASS_INSTANCE( i3RasterizeChain_DownScale);


i3RasterizeChain_DownScale::~i3RasterizeChain_DownScale(void)
{
	I3_SAFE_RELEASE( m_pShader);
}

INT32 i3RasterizeChain_DownScale::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->setToDefault();

	return 1;
}


void i3RasterizeChain_DownScale::OnCreate( i3RasterizeChainManager * pManager)
{	
	// Scaled Texture
	{
		m_pCtxDownScale = pManager->RegisterNamedRT( "DownScale", true, 2, true);
	}

	// Shader
	{
		i3Shader* pShader = i3ShaderCache::LoadShader( "i3DownScale.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
		I3_REF_CHANGE( m_pShader, pShader);
	}
}

void i3RasterizeChain_DownScale::ApplyChain( I3RC_UPDATE * pInfo)
{
	m_pCtxDownScale->MakeCurrentTarget();	

	// Shader Parameter Update
	{
		// Texture Context		
		i3ShaderParam * pParamTexInput = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texInput");

		pInfo->m_pLastRT->UpdateShaderParam( pParamTexInput);
	}

	// Shader적용
	g_pRenderContext->SetShader( m_pShader);
	
	// 설정된 렌더타겟에 텍스쳐를 그립니다.	
	Draw();
}
