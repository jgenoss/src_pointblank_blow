#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChainContext.h"

I3_CLASS_INSTANCE( i3RasterizeChainContext);

i3RasterizeChainContext::i3RasterizeChainContext(void)
{
	m_BindCtx.m_HWrap = I3G_TEXTURE_WRAP_WRAP;
	m_BindCtx.m_VWrap = I3G_TEXTURE_WRAP_WRAP;	
	m_BindCtx.m_ModifyFlag = 0xFFFFFFFF;
}

i3RasterizeChainContext::~i3RasterizeChainContext(void)
{
	I3_SAFE_RELEASE( m_pColorTarget);
	I3_SAFE_RELEASE( m_pDepthTarget);
}


bool i3RasterizeChainContext::Create( UINT32 width, UINT32 height, UINT32 levels, I3G_IMAGE_FORMAT format, bool bDepth)
{
	Destroy();

	m_pColorTarget = i3Texture::new_object();
#if defined( I3_DEBUG)
	m_pColorTarget->SetName( "RasterizeColorTarget");
#endif
	m_pColorTarget->Create( width, height, levels, format, I3G_USAGE_RENDERTARGET);

	I3ASSERT( m_pColorTarget != nullptr);	

	m_bOwnDepth = bDepth;

	if( m_bOwnDepth)
	{
		m_pDepthTarget = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pDepthTarget->SetName( "RasterizeDepthTarget");
#endif
		m_pDepthTarget->Create( width, height, levels, I3G_IMAGE_FORMAT_D24X8, I3G_USAGE_RENDERTARGET);
	}

	return true;
}

// pDepth가 있으면 ref하고 없으면 새로 만들어준다.
void i3RasterizeChainContext::MakeDepthTargetIfNULL( i3Texture * pDepth)
{
	I3ASSERT( m_pColorTarget != nullptr);

	I3_REF_CHANGE( m_pDepthTarget, pDepth);

	if( m_pDepthTarget == nullptr)
	{
		m_pDepthTarget = i3Texture::new_object();
#if defined( I3_DEBUG)
		m_pDepthTarget->SetName( "RasterizeDepthTarget");
#endif
		m_pDepthTarget->Create( m_pColorTarget->GetWidth(), m_pColorTarget->GetHeight(), m_pColorTarget->GetLevelCount(), I3G_IMAGE_FORMAT_D24X8, I3G_USAGE_RENDERTARGET);
	}
}

void i3RasterizeChainContext::Destroy(void)
{
	I3_SAFE_RELEASE( m_pColorTarget);
	I3_SAFE_RELEASE( m_pDepthTarget);
}

void	i3RasterizeChainContext::MakeMultiSampledTarget( bool bForceDepth)
{
	
}

void i3RasterizeChainContext::SetSamplerUVWrapEnable( bool bEnable)
{
	if( bEnable )
	{
		m_BindCtx.m_HWrap = m_BindCtx.m_VWrap = I3G_TEXTURE_WRAP_WRAP;
	}
	else
	{
		m_BindCtx.m_HWrap = m_BindCtx.m_VWrap = I3G_TEXTURE_WRAP_CLAMP;
	}
}

void i3RasterizeChainContext::setFilter( I3G_TEXTURE_FILTER minFilter, I3G_TEXTURE_FILTER magFilter)
{
	m_BindCtx.m_MinFilter = minFilter;
	m_BindCtx.m_MagFilter = magFilter;
}


void i3RasterizeChainContext::UpdateShaderParam(i3ShaderParam * pParam)
{
	if(pParam)
	{
		m_BindCtx.m_pTexture = m_pColorTarget;

		// 이부분은 추후 필요에 따라 추가되거나 수정될 필요가 있습니다. - 송명일
		m_BindCtx.m_ModifyFlag = I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER;		

		pParam->Update( &m_BindCtx);
	}
}

void i3RasterizeChainContext::Draw(i3VertexArray* pArray)
{	
	if( pArray == nullptr)
	{
		i3RasterizeChainManager * pManager = i3RasterizeChainManager::getInstance();

		pArray = pManager->getDefaultVertexArray();
	}

	g_pRenderContext->SetVertexArray( pArray);	
	g_pRenderContext->DrawPrim( nullptr, I3G_PRIM_TRISTRIP, 0, 2);
}

void i3RasterizeChainContext::MakeCurrentTarget( INT32 idx)
{
	i3RasterizeChainManager::getInstance()->SetRenderTarget( this);	
}
