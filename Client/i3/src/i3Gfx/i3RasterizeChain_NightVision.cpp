#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChain_NightVision.h"
#include "i3RasterizeChainContext.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3RasterizeChain_NightVision);

i3RasterizeChain_NightVision::~i3RasterizeChain_NightVision(void)
{
	I3_SAFE_RELEASE( m_pShader);

	I3_SAFE_RELEASE( m_pVA);
	for( UINT32 i = 0; i < m_vMaskingTextureList.size(); i++)
	{
		MaskingTextureInfo * pInfo = m_vMaskingTextureList.at( i);
		I3_SAFE_RELEASE( pInfo->_pTexture);
		I3_MUST_DELETE( pInfo);
	}

	m_vMaskingTextureList.clear();
}

INT32 i3RasterizeChain_NightVision::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->setToDefault();

	return 1;
}

void i3RasterizeChain_NightVision::AddMaskingTexture( const char * pszFileName)
{
	MaskingTextureInfo * pNewInfo = new MaskingTextureInfo;
	NetworkDump_Malloc( pNewInfo, sizeof( MaskingTextureInfo), __FILE__, __LINE__);
	pNewInfo->_strFileName = pszFileName;

	m_vMaskingTextureList.push_back( pNewInfo);
}


void i3RasterizeChain_NightVision::OnCreate( i3RasterizeChainManager * pManager)
{
	CreateVolatileResource();

	{
		i3Shader* pShader = i3ShaderCache::LoadShader( "i3NightVision.hlsl");
		I3_REF_CHANGE( m_pShader, pShader);
	}

	{
		m_TexCtx.m_bEnable = false;
		m_TexCtx.m_MinFilter = I3G_TEXTURE_FILTER_LINEAR;
		m_TexCtx.m_MagFilter = I3G_TEXTURE_FILTER_LINEAR;
		m_TexCtx.m_HWrap = I3G_TEXTURE_WRAP_WRAP;
		m_TexCtx.m_VWrap = I3G_TEXTURE_WRAP_WRAP;
		i3Matrix::Identity( &m_TexCtx.m_Matrix);
		i3Vector::Set( &m_TexCtx.m_Scale, 0.0f, 0.0f, 0.0f);
		i3Vector::Set( &m_TexCtx.m_Offset, 0.0f, 0.0f, 0.0f);
	}
}


void i3RasterizeChain_NightVision::CreateVolatileResource(void)
{
	
	// 외부에서 불러들일 텍스쳐 생성
	for( UINT32 i = 0; i < m_vMaskingTextureList.size(); i++)
	{
		MaskingTextureInfo * pInfo = m_vMaskingTextureList.at(i);
		
		if( i3::generic_string_size( pInfo->_strFileName) != 0)
		{
			pInfo->_pTexture = i3Texture::new_object();
			if( pInfo->_pTexture->Create( pInfo->_strFileName.c_str()) == false)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Create Texture fail. %s", pInfo->_strFileName);
			}
		}
	}

	// Vertex buffer사용
	{
		i3RenderTarget * pRT = g_pRenderContext->GetRenderTarget( 0);
		REAL32 sx = (REAL32) pRT->GetWidth();
		REAL32 sy = (REAL32) pRT->GetHeight();
		REAL32 divide = sy / 32.0f;
		
		VEC3D _pos[4];
		VEC2D _tex[4];
		VEC2D _tex2[4];
		i3VertexFormat format;

		// Vertex Create
		if( m_pVA == nullptr)
		{
			m_pVA = i3VertexArray::new_object();		
			I3ASSERT( m_pVA != nullptr);

			format.SetHasPosition( true);
			format.SetHasColor( false);
			format.SetTextureCoordSetCount( 2);
			format.SetTextureCoordCount( 2);	

			m_pVA->setRestoreType( I3G_RESTORE_MEM);
			m_pVA->Create( &format, 4, 0);
			
			{
				//	Position
				i3Vector::Set( &_pos[0], -1.0f, 1.0f, 0.0f);
				i3Vector::Set( &_pos[1], -1.0f, -1.0f, 0.0f);
				i3Vector::Set( &_pos[2], 1.0f, 1.0f, 0.0f);
				i3Vector::Set( &_pos[3], 1.0f, -1.0f, 0.0f);

				//	Texture
				sx = 1.0f / sx * 0.5f;
				sy = 1.0f / sy * 0.5f;
				i3Vector::Set( &_tex[0], 0.0f+sx, 0.0f+sy);
				i3Vector::Set( &_tex[1], 0.0f+sx, 1.0f+sy);
				i3Vector::Set( &_tex[2], 1.0f+sx, 0.0f+sy);			
				i3Vector::Set( &_tex[3], 1.0f+sx, 1.0f+sy);

				// Texture2
				INT32 i;

				for( i = 0; i < 4; i++)
				{
					i3Vector::Scale( &_tex2[i], &_tex[i], divide);
				}
			}

			m_pVA->Lock( 0);
			m_pVA->SetPositions( 0, 4, _pos);
			m_pVA->SetTextureCoords( 0, 0, 4, _tex);
			m_pVA->SetTextureCoords( 1, 0, 4, _tex2);
			m_pVA->Unlock();
		}
	}
}

void i3RasterizeChain_NightVision::DestroyVolatileResource(void)
{
	I3_SAFE_RELEASE( m_pVA);

	for( UINT32 i = 0; i < m_vMaskingTextureList.size(); i++)
	{
		MaskingTextureInfo * pInfo = m_vMaskingTextureList.at( i);
		I3_SAFE_RELEASE( pInfo->_pTexture);
	}
}


void i3RasterizeChain_NightVision::ApplyChain( I3RC_UPDATE * pInfo)
{
	i3RenderContext * pGfx = pInfo->m_pGfx;
	i3RasterizeChainContext * pCtxInput = pInfo->m_pLastRT;

	pGfx->SetShader( m_pShader);	

	m_TexCtx.m_pTexture = m_vMaskingTextureList.at(m_nMaskType)->_pTexture;
	m_TexCtx.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;
	
		
	m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_texMask", &m_TexCtx);
	m_pShader->SetParam( I3G_SHADER_TYPE_PIXEL, "g_fNVMixRate", m_fNVMixRate);

	// Texture Context		
	i3ShaderParam * pParam = m_pShader->FindParam( I3G_SHADER_TYPE_PIXEL, "g_texInput");
	pCtxInput->UpdateShaderParam( pParam);
	
	Draw( m_pVA);

	pGfx->SetShader( nullptr);
}
