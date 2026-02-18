#include "i3GfxType.h"
#include "i3RenderContext.h"
#include "i3VertexArray.h"

#include "i3Base/string/ext/resolve_path_from_environment_variable.h"

i3GfxCaps * i3RenderContext::m_pCaps = nullptr;

void i3RenderContext::SetDefaultRenderState(void)
{
	UINT32 i;

#ifdef I3_DEBUG
	removeRenderFlag(I3RENDER_FLAG_SHOW_FPS);
#endif

	m_RenderFlag &= ~I3RENDER_FLAG_SKIP_BLENDMODE;

#ifdef I3G_DX
	SetShader( nullptr);
	SelectShader( nullptr);	
#endif

	SetAlphaTestEnable(false);
	SetAlphaRefValue( 0);
	SetAlphaFunction( I3G_COMP_GREATER);

	SetBlendEnable(false);

	SetZTestEnable(true);
	SetZWriteEnable(true);
	SetZFunction( I3G_COMP_LESS);

	SetRenderMode( I3G_RENDER_POLYGON);

	SetShadeMode( I3G_SHADE_PHONG);

	SetFaceCullMode( I3G_FACE_CULL_BACK);

#ifdef I3G_PSP
 	m_SrcBlend = I3G_BLEND_ONE;
	m_DestBlend = I3G_BLEND_ZERO;
	m_BlendOp = I3G_BLEND_OP_ADD;
#endif

	SetSrcBlend( I3G_BLEND_ONE );
	SetDestBlend( I3G_BLEND_ZERO );
	SetBlendOperation( I3G_BLEND_OP_ADD);


	SetFogEnable(false);
	{
		COLOR col;

		i3Color::Set( &col, (UINT8) 0, 0, 0, 255);

		SetFogColor( &col);
	}
	SetFogMode( I3G_FOG_NONE);
	SetFogTableEnable(false);

	#if defined( I3_FIXED_POINT)
		SetFogNearFar( i3FXD::FromReal(1.0f), i3FXD::FromReal( 100.0f));
		SetFogDensity( i3FXD::FromReal( 1.0f));
	#else
		SetFogNearFar( 1.0f, 100.0f);
		SetFogDensity( 1.0f);
	#endif

	SetEdgeAntiAliasEnable(false);

	SetStencilEnable(false);
	SetStencilOperation( I3G_STENCIL_OP_KEEP, I3G_STENCIL_OP_KEEP, I3G_STENCIL_OP_KEEP);
	SetStencilFunction( I3G_COMP_ALWAYS);
	SetStencilRefValue( 0);
	SetStencilReadMask( 0xFF);
	SetStencilWriteMask( 0xFF);
	SetClippingEnable(true);
	SetLightingEnable(false);
	SetVertexColorEnable(true);
	SetNormalizeNormalEnable(true);

	{
		COLORREAL col;

		i3Color::Set( &col, 0.3f, 0.3f, 0.3f, 1.0f);

		SetMaterial(  &col, &col, &col, 128.0f);
	}

	{
		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);

		SetColor( &col);
	}

	SetFresnelEnable( false);
	SetFresnelIndexOfReflect( 1.0f);
	SetFresnelColor( 1.0f, 1.0f, 1.0f, 1.0f);
	setInstancingEnable( false);

	SetVertexBlendEnable(false);
	SetSoftwareVertexBlendEnable(false);

	for( i = 0; i < I3G_TEXTURE_BIND_MAX; i++)
	{
		SetTextureEnable( (I3G_TEXTURE_BIND) i, false);
		SetTextureWrap( (I3G_TEXTURE_BIND) i, I3G_TEXTURE_WRAP_WRAP, I3G_TEXTURE_WRAP_WRAP);
		SetTextureFilter( (I3G_TEXTURE_BIND) i, I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);
	}

	SetTextureWrap( I3G_TEXTURE_BIND_SHADOW, I3G_TEXTURE_WRAP_CLAMP, I3G_TEXTURE_WRAP_CLAMP);
	SetTextureFilter( I3G_TEXTURE_BIND_SHADOW, I3G_TEXTURE_FILTER_POINT, I3G_TEXTURE_FILTER_POINT);

	for( i = 1; i < 4; i++)
	{
		_SetTextureFilter( i, I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);
		_SetTextureWrap( i, I3G_TEXTURE_WRAP_WRAP, I3G_TEXTURE_WRAP_WRAP);
		_SetMaxAnisotropy( i, m_pCaps->getMaxAnisotropy());
	}

#if defined( I3_DEBUG)
	m_DrawPrimitiveList.reserve( 512);
	m_bDumpDrawPrimitive = false;
#endif
}

i3VertexArray * i3RenderContext::CreateVertexArray(void)
{
	return i3VertexArray::new_object();
}

bool i3RenderContext::CreateDefaultShaderContext(void)
{	
	/*
	#if defined( I3G_DX)
		return _i3GfxCreateDefaultShaders( this);
	#else
		return false;
	#endif
	*/

	return true;
}

void i3RenderContext::_FindShaderWorkDir( char * pszPath)
{
	INT32 i;
//	char szFull[MAX_PATH];
//	char szPath[ MAX_PATH];
	i3::stack_string strFull;
	i3::stack_string strPath;

	for( i = 0; i < i3ShaderCache::GetShaderSearchPathCount(); i++)
	{
//		i3System::ResolvePath( i3ShaderCache::GetShaderSearchPath(i), szPath, MAX_PATH);
		i3::resolve_path_from_environment_variable(i3ShaderCache::GetShaderSearchPath(i), strPath);

		#if defined( I3_DEBUG)
			i3::snprintf( strFull, MAX_PATH, "%s/i3SuperShader.hlsl", strPath);
			//sprintf( szFull, "%s/Cache.i3GL", pszPath);
		#else
			i3::snprintf( strFull, MAX_PATH, "%s/DefaultSHGL", strPath);
		#endif

		if( i3FileStream::isFile( strFull.c_str() ))
		{
			// Find!
			i3::safe_string_copy( pszPath, strPath, MAX_PATH);
			return;
		}		
	}

	// Debug의 경우 I3_ROOT가 설정되야 합니다.
	//I3PRINTLOG(I3LOG_FATAL,  "not found shader folder!!!!");
}

bool i3RenderContext::_LoadShaderCache( const char * pszPath)
{
	I3_SAFE_RELEASE( m_pShaderCache);

	m_pShaderCache = i3ShaderCache::LoadCacheFile( (char *) pszPath);

	m_pShaderCache->bindRenderContext( this);

	return true;
}

bool i3RenderContext::_SaveShaderCache( const char * pszPath)
{

	return true;
}


void i3RenderContext::FlushVideoMemory(void)
{
	m_pD3DDevice->EvictManagedResources();
}

#if defined( I3_DEBUG)
void i3RenderContext::AddDrawPrimitiveList( const char * pszname, INT32 start, INT32 cnt)
{
	tagPRIMITIVEINFO * pInfo = new tagPRIMITIVEINFO( pszname, start, cnt);

	NetworkDump_Malloc( pInfo, sizeof( tagPRIMITIVEINFO), __FILE__, __LINE__);

	m_DrawPrimitiveList.push_back( pInfo);
}
#endif
