#include "i3SceneDef.h"
#include "i3SATGenerator.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3SATGenerator);

i3SATGenerator::i3SATGenerator(void)
{
	m_pUserAttr = i3UserAttr::new_object();
	m_pUserAttr->SetCallBack(RenderProc, this);
}

i3SATGenerator::~i3SATGenerator(void)
{
	I3_SAFE_RELEASE( m_pSwapChain[0]);
	I3_SAFE_RELEASE( m_pSwapChain[1]);
	I3_SAFE_RELEASE( m_pDepth);

	I3_SAFE_RELEASE( m_pSumShader);

	I3_SAFE_RELEASE( m_pUserAttr);
	I3_SAFE_RELEASE( m_pVA);
}

void i3SATGenerator::Create(void)
{
	m_pSumShader = i3ShaderCache::LoadShader( "i3SAT_Sum.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_MUST_ADDREF( m_pSumShader);

	{
		i3VertexFormat fmt;

		fmt.SetHasPosition( true);
		fmt.SetTextureCoordSetCount( 1);
		fmt.SetTextureCoordCount( 2);

		m_pVA = i3VertexArray::new_object();
		m_pVA->Create( &fmt, 6, 0);

		VEC3D pos[6];

		i3Vector::Set( &pos[0], -1.0f,	1.0f,	0.0f);
		i3Vector::Set( &pos[1], -1.0f,	-1.0f,	0.0f);
		i3Vector::Set( &pos[2],  1.0f,	1.0f,	0.0f);
		i3Vector::Set( &pos[3],  1.0f,	1.0f,	0.0f);
		i3Vector::Set( &pos[4], -1.0f,	-1.0f,	0.0f);
		i3Vector::Set( &pos[5],  1.0f,	-1.0f,	0.0f);

		VEC2D uv[6];
		i3Vector::Set( &uv[0],  0.0f,	0.0f);
		i3Vector::Set( &uv[1],  0.0f,	1.0f);
		i3Vector::Set( &uv[2],  1.0f,	0.0f);
		i3Vector::Set( &uv[3],  1.0f,	0.0f);
		i3Vector::Set( &uv[4],  0.0f,	1.0f);
		i3Vector::Set( &uv[5],  1.0f,	1.0f);

		m_pVA->Lock();
		m_pVA->SetPositions( 0, 6, pos);
		m_pVA->SetTextureCoords( 0, 0, 6, uv);
		m_pVA->Unlock();
	}

}

void i3SATGenerator::Bind( i3Texture * pSrcTex, i3Texture *pDestTex, i3Texture * pDepth)
{
	I3_REF_CHANGE( m_pSwapChain[0], pSrcTex);
	I3_REF_CHANGE( m_pSwapChain[1], pDestTex);
	I3_REF_CHANGE( m_pDepth, pDepth);
}

void i3SATGenerator::Process( i3SceneTracer * pTracer)
{
	pTracer->AddAttributeToOutput( m_pUserAttr);
}

#define SAT_UNIT		2

void i3SATGenerator::RenderProc( i3RenderContext * pCtx, void * pParam)
{
	i3SATGenerator * pSAT = (i3SATGenerator *) pParam;
	bool bZTest;
	bool bZWrite;
	bool bAlphaTest;
	bool bBlend;
	i3Texture * pTex[2];
	I3G_TEXTURE_BIND_CONTEXT diffuseCtx;	
	COLOR oldClearColor;

	pTex[0] = pSAT->m_pSwapChain[0];
	pTex[1] = pSAT->m_pSwapChain[1];

	{
		bZTest		= pCtx->GetZTestEnable();
		bZWrite		= pCtx->GetZWriteEnable();
		bAlphaTest	= pCtx->GetAlphaTestEnable();
		bBlend		= pCtx->GetBlendEnable();
		i3Color::Set( &oldClearColor, pCtx->GetClearColor());

		pCtx->SaveTextureBindContext( I3G_TEXTURE_BIND_DIFFUSE, &diffuseCtx);
	}

	{
		pCtx->SetDepthRenderTarget( pSAT->m_pDepth);
		pCtx->SetColorRenderTarget( pTex[1]);

		pCtx->SetZTestEnable( false);
		pCtx->SetZWriteEnable( false);
		pCtx->SetAlphaTestEnable( false);
		pCtx->SetBlendEnable( false);
		pCtx->SetVertexArray( pSAT->m_pVA);
		pCtx->SetFaceCullMode( I3G_FACE_CULL_NONE);
		pCtx->SetTextureWrap( I3G_TEXTURE_BIND_DIFFUSE, I3G_TEXTURE_WRAP_BORDER, I3G_TEXTURE_WRAP_BORDER);
		pCtx->SetTextureFilter( I3G_TEXTURE_BIND_DIFFUSE, I3G_TEXTURE_FILTER_NONE, I3G_TEXTURE_FILTER_NONE);
	}

	// SAT 생성
	INT32 h, v, i, idx = 0;
	REAL32 texelCX, texelCY;
	VEC2D texel;

	h = (INT32) i3Math::log( (REAL32) pSAT->m_pSwapChain[0]->GetWidth(), SAT_UNIT);
	v = (INT32) i3Math::log( (REAL32) pSAT->m_pSwapChain[0]->GetHeight(), SAT_UNIT);

	texelCX = 1.0f / pTex[0]->GetWidth();
	texelCY = 1.0f / pTex[0]->GetHeight();

	pSAT->m_pSumShader->Validate( pCtx);
	pCtx->SetShader( pSAT->m_pSumShader);

	i3ShaderParam * pParamStep = pSAT->m_pSumShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_Step");
	i3ShaderParam * pParamTexel = pSAT->m_pSumShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_Texel");

	if( pParamStep != nullptr && pParamTexel != nullptr)
	{
		// 가로 방향
		// 폭을 1/4씩 감소시키며 처리한다.
		texel.x = texelCX;
		texel.y = 0.0f;
		pParamTexel->Update( &texel);

		for( i = 0; i < h; i++)
		{
			// Texel Offset 지정. 현 단계에서는 가로 방향 Offset만 준다.
			pParamStep->Update( (REAL32) (1 << i));

			pCtx->SetTexture( I3G_TEXTURE_BIND_DIFFUSE, pTex[idx]);

			idx ^= 1;
			pCtx->SetColorRenderTarget( pTex[idx]);

			pCtx->DrawPrim( nullptr, I3G_PRIM_TRILIST, 0, 2);
		}

		// 세로 방향
		// 높이를 1/4씩 감소시키며 처리한다.
		texel.x = 0.0f;
		texel.y = texelCY;
		pParamTexel->Update( &texel);

		for( i = 0; i < v; i++)
		{
			// Texel Offset 지정. 현 단계에서는 가로 방향 Offset만 준다.
			pParamStep->Update( (REAL32) (1 << i));
			
			pCtx->SetTexture( I3G_TEXTURE_BIND_DIFFUSE, pTex[idx]);

			idx ^= 1;
			pCtx->SetColorRenderTarget( pTex[idx]);
			pCtx->DrawPrim( nullptr, I3G_PRIM_TRILIST, 0, 2);
		}
	}

	pCtx->RestoreTextureBindContext( I3G_TEXTURE_BIND_DIFFUSE, &diffuseCtx);
	pCtx->SetZTestEnable( bZTest);
	pCtx->SetZWriteEnable( bZWrite);
	pCtx->SetBlendEnable( bBlend);
	pCtx->SetAlphaTestEnable( bAlphaTest);
	pCtx->SetCurrentRenderTarget( nullptr);
	pCtx->SetClearColor( &oldClearColor);
	pCtx->SetShader( nullptr);
}
