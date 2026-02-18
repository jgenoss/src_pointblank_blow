#include "i3SceneDef.h"
#include "i3ShaderAttr.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ShaderAttr);

i3ShaderAttr::i3ShaderAttr(void)
{
	SetID( I3_ATTRID_SHADER);
		
#if defined( I3_DEBUG)
	m_bAddCache = false;
#endif
}

i3ShaderAttr::~i3ShaderAttr(void)
{
	I3_SAFE_RELEASE( m_pShader);
}

void i3ShaderAttr::setShader( i3Shader * pShader)
{
	I3_REF_CHANGE( m_pShader, pShader);

	setSource( pShader->GetName());
}

void i3ShaderAttr::setSource( const char * pszPath)
{
	i3::make_relative_path( i3Shader::GetWorkDir(), pszPath, m_strPath);

	if( (m_pShader != nullptr) || (m_strPath[0] != 0))
	{
		LoadShader();
	}
}

void i3ShaderAttr::setAddCacheState( bool bState)
{
	m_bAddCache = bState;
}

void i3ShaderAttr::LoadShader(void)
{
	I3_SAFE_RELEASE( m_pShader);

	m_pShader = i3ShaderCache::LoadShader( m_strPath.c_str(), m_vsVersion, m_psVersion, I3G_GPU_VENDOR_NA, m_bAddCache);

	I3_SAFE_ADDREF( m_pShader);
}

void i3ShaderAttr::Apply( i3RenderContext * pContext)
{
	if( (m_pShader == nullptr) && (m_strPath[0] != 0))
	{
		LoadShader();
	}

	pContext->SetShader( m_pShader);
}

bool	i3ShaderAttr::IsSame( i3RenderAttr * pAttr1)
{
	i3ShaderAttr * pOther = (i3ShaderAttr *) pAttr1;

	if( pOther->getShader() == getShader())
		return true;

	if (i3::generic_is_iequal( m_strPath, pOther->getSourceString() ) )
		return true;

	return false;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 SHADER_ATTR
	{
		INT8		m_ID[4] = { 'S', 'H', 'A', '1' };
		char		m_szPath[256] = { 0 };
		INT32		m_bAddCache = 0;
		INT32		m_vsVersion = 0;
		INT32		m_psVersion = 0;
		UINT32		pad[6] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ShaderAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::SHADER_ATTR data;

	i3::string_ncopy_nullpad( data.m_szPath, getSourceString(), _countof( data.m_szPath));
	data.m_bAddCache	= (INT32) (m_bAddCache == true);
	data.m_vsVersion	= (INT32) getTargetVertexShaderVersion();
	data.m_psVersion	= (INT32) getTargetPixelShaderVersion();

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Rc;
}

UINT32 i3ShaderAttr::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::SHADER_ATTR data;

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_bAddCache	 = data.m_bAddCache != 0;

	setTargetVertexShaderVersion( (I3G_SHADER_VERSION) data.m_vsVersion);
	setTargetPixelShaderVersion( (I3G_SHADER_VERSION) data.m_psVersion);

	setSource( data.m_szPath);

	return Result;
}

#if defined( I3_DEBUG)
void i3ShaderAttr::Dump(void)
{
	if( m_pShader != nullptr)
	{
		/*
		I3TRACE( "i3ShaderAttr : VS:%s  SkinVS:%s PS:%s\n", 
			m_pShader->getVertexShaderName(),
			m_pShader->getSkinVertexShaderName(),
			m_pShader->getPixelShaderName());
			*/
	}
	else
		I3TRACE( "i3ShaderAttr : nullptr\n");
}

#endif
