#include "i3SceneDef.h"
#include "i3VSM_BlurFilter.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3VSM_BlurFilter);

i3VSM_BlurFilter::~i3VSM_BlurFilter(void)
{
	I3_SAFE_RELEASE( m_pSrcTex);
	I3_SAFE_RELEASE( m_pDestTex);
	I3_SAFE_RELEASE( m_pDepth);

	I3_SAFE_RELEASE( m_pShader);

	I3_SAFE_RELEASE( m_pUserAttr);
	I3_SAFE_RELEASE( m_pVA);

	I3MEM_SAFE_FREE( m_pOffset);
	I3MEM_SAFE_FREE( m_pParams);
}

void i3VSM_BlurFilter::Create( INT32 kernelSize)
{
	m_pShader = i3ShaderCache::LoadShader( "i3VSM_BlurFilter.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_MUST_ADDREF( m_pShader);

	m_pUserAttr = i3UserAttr::new_object();
	m_pUserAttr->SetCallBack( RenderProc, this);

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

	m_KernelSize = kernelSize;
	if( (m_KernelSize & 1) == 0)
	{
		// 짝수개의 Kernel Size는 중점을 잡기가 좀 애매하기때문에...
		m_KernelSize ++;
	}

	_BuildParameters();
}

inline REAL32 _computeGaussian( REAL32 n, REAL32 gaussTheta)
{	
	if( gaussTheta <= 0.1f) gaussTheta = 0.1f;
	else if( gaussTheta > 7.0f) gaussTheta = 7.0f;

	return (REAL32)((1.0 / i3Math::Sqrt(2 * I3_PI * gaussTheta)) * exp(-(n * n) / (2 * gaussTheta * gaussTheta)));
}

void i3VSM_BlurFilter::_BuildParameters(void)
{
	INT32 i, j;
	REAL32 sum;

	I3MEM_SAFE_FREE( m_pOffset);
	I3MEM_SAFE_FREE( m_pParams);

	m_pOffset = (VEC2D *) i3MemAlloc( sizeof(VEC2D) * m_KernelSize);
	I3ASSERT( m_pOffset != nullptr);

	m_pParams = (VEC4D *) i3MemAlloc( sizeof(VEC4D) * m_KernelSize);
	I3ASSERT( m_pParams != nullptr);

	i3mem::FillZero( m_pParams, sizeof(VEC4D) * m_KernelSize);

	I3ASSERT( m_KernelSize > 0);

	i3Vector::Set( &m_pOffset[0], 0.0f, 0.0f);
	
	sum = m_pParams[0].z = _computeGaussian( 0, 5.0f);		// Weight

	INT32 kernel = (INT32) sqrt( (REAL32)m_KernelSize);
	INT32 idx = 0;
	INT32 half = (kernel >> 1);

	for( i = 0; i < kernel; i ++)
	{
		for( j = 0; j < kernel; j++)
		{
			REAL32 offx, offy;
		
			offx = (REAL32)j - half;
			offy = (REAL32)i - half;

			i3Vector::Set( &m_pOffset[idx], offx, offy);
		
			// Store weights for the positive and negative taps.
			REAL32 w = _computeGaussian((REAL32)((m_KernelSize >> 1) - idx), 5.0f);

			m_pParams[idx].z = w;

			//I3TRACE( "OFFSET[%d] : %f, %f * %f\n", idx, offx, offy, w);
				
			sum += w;

			idx ++;
		}
	}

	// Normalize weights
	for( i = 0; i < m_KernelSize; i++)
	{
		m_pParams[i].z = m_pParams[i].z / sum;
	}
}

void i3VSM_BlurFilter::_UpdateOffsetTable(void)
{
	INT32 i;
	REAL32 px, py;

	if( m_pSrcTex == nullptr)
		return;

	px = 1.0f / m_pSrcTex->GetWidth();
	py = 1.0f / m_pSrcTex->GetHeight();

	// FillZero에 의해 이미 0으로 초기화되어 있기 때문에 처리하지 않음.
	//m_pParams[0].x = m_pParams[0].y = 0.0f;

	for( i = 0; i < m_KernelSize; i ++)
	{
		m_pParams[i].x = px * m_pOffset[i].x;
		m_pParams[i].y = py * m_pOffset[i].y;
	}
}

void i3VSM_BlurFilter::Bind( i3Texture * pSrcTex, i3Texture *pDestTex, i3Texture * pDepth)
{
	I3_REF_CHANGE( m_pSrcTex, pSrcTex);
	I3_REF_CHANGE( m_pDestTex, pDestTex);
	I3_REF_CHANGE( m_pDepth, pDepth);

	if( m_pSrcTex != nullptr)
	{
		_UpdateOffsetTable();
	}
}

void i3VSM_BlurFilter::Process( i3SceneTracer * pTracer)
{
	pTracer->AddAttributeToOutput( m_pUserAttr);
}   

#define SAT_UNIT		2

void i3VSM_BlurFilter::RenderProc( i3RenderContext * pCtx, void * pParam)
{
	i3VSM_BlurFilter * pFilter = (i3VSM_BlurFilter *) pParam;
	bool bZTest;
	bool bZWrite;
	bool bAlphaTest;
	bool bBlend;
	i3Texture * pSrcTex, * pDestTex;
	I3G_TEXTURE_BIND_CONTEXT diffuseCtx;	
	COLOR oldClearColor;

	pSrcTex = pFilter->m_pSrcTex;
	pDestTex = pFilter->m_pDestTex;

	{
		bZTest		= pCtx->GetZTestEnable();
		bZWrite		= pCtx->GetZWriteEnable();
		bAlphaTest	= pCtx->GetAlphaTestEnable();
		bBlend		= pCtx->GetBlendEnable();
		i3Color::Set( &oldClearColor, pCtx->GetClearColor());

		pCtx->SaveTextureBindContext( I3G_TEXTURE_BIND_DIFFUSE, &diffuseCtx);
	}

	{
		pCtx->SetDepthRenderTarget( pFilter->m_pDepth);
		pCtx->SetColorRenderTarget( pDestTex);

		pCtx->SetZTestEnable(false);
		pCtx->SetZWriteEnable(false);
		pCtx->SetAlphaTestEnable(false);
		pCtx->SetBlendEnable(false);
		pCtx->SetVertexArray( pFilter->m_pVA);
		pCtx->SetFaceCullMode( I3G_FACE_CULL_NONE);
		pCtx->SetTextureWrap( I3G_TEXTURE_BIND_DIFFUSE, I3G_TEXTURE_WRAP_BORDER, I3G_TEXTURE_WRAP_BORDER);
		pCtx->SetTextureFilter( I3G_TEXTURE_BIND_DIFFUSE, I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_ANISOTROPIC);

		COLOR border;

		i3Color::Set( &border, (UINT8) 255, 255, 255, 255);
		pCtx->SetTextureBorderColor( I3G_TEXTURE_BIND_DIFFUSE, &border);
	}

	pFilter->m_pShader->Validate( pCtx);
	pCtx->SetShader( pFilter->m_pShader);

	i3ShaderParam * pGaussianParam = pFilter->m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_Params");
	if( pGaussianParam != nullptr)
		pGaussianParam->Update( pFilter->m_pParams);

	{
		pCtx->SetTexture( I3G_TEXTURE_BIND_DIFFUSE, pSrcTex);

		pCtx->DrawPrim( nullptr, I3G_PRIM_TRILIST, 0, 2);
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
