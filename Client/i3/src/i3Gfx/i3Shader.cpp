#include "i3GfxType.h"
#include "i3Shader.h"
#include "i3ShaderGen.h"
#include "i3RenderContext.h"
#include "i3GfxGlobalVariable.h"

#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3Shader);

static char	s_szShaderWorkDir[ MAX_PATH] = { 0, };

i3Shader::i3Shader(void)
{
	for (INT32 i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		m_pData[i] = nullptr;
		m_DataSize[i] = 0;
		m_VersionTable[i] = 0;
	}
}

i3Shader::~i3Shader(void)
{
	INT32 i;

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		I3MEM_SAFE_FREE( m_pData[i]);
	}

	I3_SAFE_RELEASE( m_pCtx);
}

void i3Shader::SetShaderData( I3G_SHADER_TYPE type, char * pData, UINT32 sz, UINT16 version)
{
	I3MEM_SAFE_FREE( m_pData[ type]);

	m_pData[type] = (char *) i3MemAlloc( sz);
	I3ASSERT( m_pData[ type] != nullptr);

	m_DataSize[type] = sz;
	m_VersionTable[ type] = version;

	i3mem::Copy( m_pData[type], pData, sz);

	switch( type)
	{
		case I3G_SHADER_TYPE_VERTEX :			addState( I3G_SHADER_STATE_VS_0_COMPILED);	break;
		case I3G_SHADER_TYPE_BLENDED_VERTEX_1 :	addState( I3G_SHADER_STATE_VS_1_COMPILED);	break;
		case I3G_SHADER_TYPE_BLENDED_VERTEX_2 :	addState( I3G_SHADER_STATE_VS_2_COMPILED);	break;
		case I3G_SHADER_TYPE_BLENDED_VERTEX_3 :	addState( I3G_SHADER_STATE_VS_3_COMPILED);	break;
		case I3G_SHADER_TYPE_BLENDED_VERTEX_4 :	addState( I3G_SHADER_STATE_VS_4_COMPILED);	break;
		case I3G_SHADER_TYPE_PIXEL :			addState( I3G_SHADER_STATE_PS_COMPILED); break;
	}
}

bool i3Shader::Create( i3ShaderCode * pCode, const char * pszWorkDir, UINT32 style, I3G_SHADER_VERSION versionVS, I3G_SHADER_VERSION versionPS)
{
	// Code generation
	setCode( pCode->getCode());

	setStyle( style);
	setWorkDir( pszWorkDir);
	setRequestedVSVersion( versionVS);
	setRequestedPSVersion( versionPS);

	setISSVersion( i3ShaderCache::GetRequestedISSVersion());

	addState( I3G_SHADER_STATE_GENERATED );

	m_pCtx = i3ShaderContext::new_object();
	m_pCtx->Bind( this);

	if( m_pCtx->Compile( pszWorkDir, nullptr) == false)
	{
		return false;
	}

	return true;
}

bool i3Shader::Create( const char * pszWorkDir, const char * pszFileName, UINT32 style, I3G_SHADER_VERSION versionVS, I3G_SHADER_VERSION versionPS)
{
	I3ASSERT( pszFileName != nullptr);

	SetName( pszFileName);

	m_Code.SetName( pszFileName);
	setStyle( style);
	setWorkDir( pszWorkDir);
	setRequestedVSVersion( versionVS);
	setRequestedPSVersion( versionPS);

	setISSVersion( i3ShaderCache::GetRequestedISSVersion());

	addState( I3G_SHADER_STATE_GENERATED );

	m_pCtx = i3ShaderContext::new_object();
	m_pCtx->Bind( this);

	if( m_pCtx->Compile( pszWorkDir, pszFileName))
	{
	}

	return true;
}

bool i3Shader::_isNeedToRebuild(void)
{
	INT32 i;
	i3GfxCaps *		pCaps = i3RenderContext::GetCaps();
	INT32 vs0, vs1, cs0, cs1;
	INT32 ps0, ps1;

	vs0 = pCaps->getVertexShaderVersionMajor() + '0';
	vs1 = pCaps->getVertexShaderVersionMinor() + '0';
	ps0 = pCaps->getPixelShaderVersionMajor() + '0';
	ps1 = pCaps->getPixelShaderVersionMinor() + '0';

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		cs0 = (m_VersionTable[i] >> 8) & 0xFF;
		cs1 = m_VersionTable[i] & 0xFF;

		if( i != I3G_SHADER_TYPE_PIXEL)
		{
			if( cs0 > vs0)
				return true;
		}
		else
		{
			if( cs0 > ps0)
				return true;
		}
	}

	return false;
}

bool i3Shader::Validate( i3RenderContext * pRenderCtx)
{
	// Shader Context를 통해 Validate를 시도한다.
	if( m_pCtx == nullptr)
	{
		m_pCtx = i3ShaderContext::new_object();
		m_pCtx->Bind( this);
	}

	if( isAllValidated() == false)
	{
		if( _isNeedToRebuild())
		{
			Rebuild();
		}

		#if defined( I3_DEBUG)
		{
			char szGPU[64];

			switch( getTargetGPU())
			{
				case I3G_GPU_VENDOR_NVIDIA :	strncpy( szGPU, "NVIDIA", sizeof(szGPU)-1);	break;
				case I3G_GPU_VENDOR_ATI:		strncpy( szGPU, "ATI", sizeof(szGPU)-1);		break;
				default :						strncpy( szGPU, "N/A", sizeof(szGPU)-1);		break;
			}

			//I3TRACE( "[SHADER VALIDATE] %s [%s]\n", GetName(), szGPU);
		}
		#endif

		bool bRv = m_pCtx->BuildNativeShader( pRenderCtx);

		return bRv;
	}

	return true;
}

void i3Shader::QueryTargetGPU(void)
{
	INT32 i;

	m_TargetGPU = I3G_GPU_VENDOR_STANDARD;

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		UINT16 version = getShaderVersion( (I3G_SHADER_TYPE) i);

		if( ((version >> 8) & 0xFF) == '2')
		{
			UINT8 minor = (UINT8) (version & 0xFF);

			switch( minor)
			{
				case 'a' :		m_TargetGPU = I3G_GPU_VENDOR_NVIDIA;	break;
				case 'b' :		m_TargetGPU = I3G_GPU_VENDOR_ATI;		break;
			}
		}

	}
}

bool i3Shader::Rebuild(void)
{
	bool bResult;

	I3_SAFE_RELEASE( m_pCtx);

	m_State = 0;

	if( isCustomShader())
	{
		// GetName()으로 얻어지는 파일이 파일명이다.
		char szName[ MAX_PATH];

		// Create() 함수 내부에서 다시 SetName()을 하는 코드가 있다.
		// 이 코드에서 동일 포인터를 사용하는 심각한 문제가 발생하는 것을
		// 방지하기 위해 임시 버퍼에 복사해 사용한다.
		i3::safe_string_copy( szName, GetNameString(), MAX_PATH);
		bResult = Create( m_pszWorkDir, (char *) szName, getStyle(), getRequestedVSVersion(), getRequestedPSVersion());
	}
	else
	{
		bResult = Create( getShaderCode(), m_pszWorkDir, getStyle(), getRequestedVSVersion(), getRequestedPSVersion());
	}

	//Validate( g_pRenderContext);

	QueryTargetGPU();

	return bResult;
}


bool i3Shader::SetParam( I3G_SHADER_TYPE type, const char * pszName, REAL32 val, bool bReportError)
{
	i3ShaderParam * pParam;

	pParam = FindParam( type, pszName);

	if( pParam == nullptr)
	{
		if( bReportError)
		{
			I3PRINTLOG(I3LOG_WARN,  "[SHADER(%s)] : Could not found given shader parameter : %s !!!", GetName(), pszName);
		}

		return false;
	}

	pParam->Update( val);

	return true;
}

bool i3Shader::SetParam( I3G_SHADER_TYPE type, const char * pszName, VEC2D * pVec, bool bReportError)
{
	i3ShaderParam * pParam;

	pParam = FindParam( type, pszName);

	if( pParam == nullptr)
	{
		if( bReportError)
		{
			I3PRINTLOG(I3LOG_WARN,  "[SHADER(%s)] : Could not found given shader parameter : %s !!!", GetName(), pszName);
		}
		return false;
	}

	pParam->Update( pVec);

	return true;
}

bool i3Shader::SetParam( I3G_SHADER_TYPE type, const char * pszName, VEC3D * pVec, bool bReportError)
{
	i3ShaderParam * pParam;

	pParam = FindParam( type, pszName);

	if( pParam == nullptr)
	{
		if( bReportError)
		{
			I3PRINTLOG(I3LOG_WARN,  "[SHADER(%s)] : Could not found given shader parameter : %s !!!", GetName(), pszName);
		}
		return false;
	}

	pParam->Update( pVec);

	return true;
}

bool i3Shader::SetParam( I3G_SHADER_TYPE type, const char * pszName, VEC4D * pVec, bool bReportError)
{
	i3ShaderParam * pParam;

	pParam = FindParam( type, pszName);

	if( pParam == nullptr)
	{
		if( bReportError)
		{
			I3PRINTLOG(I3LOG_WARN,  "[SHADER(%s)] : Could not found given shader parameter : %s !!!", GetName(), pszName);
		}
		return false;
	}

	pParam->Update( pVec);

	return true;
}

bool i3Shader::SetParam( I3G_SHADER_TYPE type, const char * pszName, COLORREAL * pCol, bool bReportError)
{
	i3ShaderParam * pParam;

	pParam = FindParam( type, pszName);

	if( pParam == nullptr)
	{
		if( bReportError)
		{
			I3PRINTLOG(I3LOG_WARN,  "[SHADER(%s)] : Could not found given shader parameter : %s !!!", GetName(), pszName);
		}
		return false;
	}

	pParam->Update( pCol);

	return true;
}

bool i3Shader::SetParam( I3G_SHADER_TYPE type, const char * pszName, MATRIX * pMtx, bool bReportError)
{
	i3ShaderParam * pParam;

	pParam = FindParam( type, pszName);

	if( pParam == nullptr)
	{
		if( bReportError)
		{
			I3PRINTLOG(I3LOG_WARN,  "[SHADER(%s)] : Could not found given shader parameter : %s !!!", GetName(), pszName);
		}
		return false;
	}

	pParam->Update( pMtx);

	return true;
}

bool i3Shader::SetParam( I3G_SHADER_TYPE type, const char * pszName, I3G_TEXTURE_BIND_CONTEXT * pCtx, INT32 idx, bool bReportError)
{
	i3ShaderParam * pParam;

	pParam = FindParam( type, pszName);

	if( pParam == nullptr)
	{
		if( bReportError)
		{
			I3PRINTLOG(I3LOG_WARN,  "[SHADER(%s)] : Could not found given shader parameter : %s !!!", GetName(), pszName);
		}
		return false;
	}

	pParam->Update( pCtx);

	return true;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 SHADER
	{
		UINT8				m_ID[4] = { 'S', 'H', 'D', '1' };
		UINT64				m_Code = 0;
		UINT32				m_Style = 0;
		UINT32				m_DataSize[I3G_SHADER_TYPE_MAX] = { 0 };
		UINT16				m_VersionTable[I3G_SHADER_TYPE_MAX] = { 0 };
		UINT32				m_ISSVersion = 0;
		UINT16				m_Version[2] = { 0 };
		UINT32				pad[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Shader::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::SHADER data;
	INT32 i;

	result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_Code = getCode();
	data.m_Style = getStyle();
	data.m_ISSVersion = getISSVersion();
	data.m_Version[0]	= (UINT16) getRequestedVSVersion();
	data.m_Version[1]	= (UINT16) getRequestedPSVersion();

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		data.m_DataSize[i]		= getShaderDataSize( (I3G_SHADER_TYPE) i);
		data.m_VersionTable[i]	= getShaderVersion( (I3G_SHADER_TYPE) i);
	}

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		if( m_DataSize[i] > 0)
		{
			rc = pResFile->Write( m_pData[i], m_DataSize[i]);
			I3_CHKIO( rc);
			result += rc;
		}
	}

	return result;
}

UINT32 i3Shader::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::SHADER data;
	INT32 i;

	result = i3NamedElement::OnLoad( pResFile);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	m_Code.setCode(	data.m_Code);
	m_Style				= data.m_Style;
	m_ISSVersion		= data.m_ISSVersion;
	m_ReqVSVersion		= (I3G_SHADER_VERSION) data.m_Version[0];
	m_ReqPSVersion		= (I3G_SHADER_VERSION) data.m_Version[1];

	if( m_ReqVSVersion < I3G_SHADER_VERSION_1)
		m_ReqVSVersion = I3G_SHADER_VERSION_1;

	if( m_ReqPSVersion < I3G_SHADER_VERSION_1)
		m_ReqPSVersion = I3G_SHADER_VERSION_1;

	m_TargetGPU = I3G_GPU_VENDOR_STANDARD;

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		m_DataSize[i]		= data.m_DataSize[i];
		m_VersionTable[i]	= data.m_VersionTable[i];

		{
			UINT8 major, minor;

			major = (UINT8)(m_VersionTable[i] >> 8) & 0xFF;
			minor = (UINT8)(m_VersionTable[i] & 0xFF);

			if( major == '2')
			{
				if( minor == 'a')
					m_TargetGPU = I3G_GPU_VENDOR_NVIDIA;
				else if( minor == 'b')
					m_TargetGPU = I3G_GPU_VENDOR_ATI;
			}
		}

		if( m_DataSize[i] > 0)
		{
			m_pData[i] = (char *) i3MemAlloc( m_DataSize[i]);
			I3ASSERT( m_pData[i] != nullptr);

			rc = pResFile->Read( m_pData[i], m_DataSize[i]);
			I3_CHKIO( rc);
			result += rc;

			switch( i )
			{
				case 0 :	addState( I3G_SHADER_STATE_VS_0_COMPILED);	break;
				case 1 :	addState( I3G_SHADER_STATE_VS_1_COMPILED);	break;
				case 2 :	addState( I3G_SHADER_STATE_VS_2_COMPILED);	break;
				case 3 :	addState( I3G_SHADER_STATE_VS_3_COMPILED);	break;
				case 4 :	addState( I3G_SHADER_STATE_VS_4_COMPILED);	break;
				case 5 :	addState( I3G_SHADER_STATE_PS_COMPILED);	break;
			}
		}
	}

	return result;
}

bool i3Shader::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv;

	bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	{
		i3XMLElement * pCodeXML = new i3XMLElement( "Code");

		pCodeXML = (i3XMLElement*) pXML->LinkEndChild( pCodeXML);
		I3ASSERT( pCodeXML != nullptr);

		m_Code.SaveToXML( pFile, pCodeXML);
	}

	pXML->addAttr( "Style", getStyle());
	pXML->addAttr( "ISS", getISSVersion());
	pXML->addAttr( "RequestedVSVersion", getRequestedVSVersion());
	pXML->addAttr( "RequestedPSVersion", getRequestedPSVersion());
	pXML->addAttr( "TargetGPU", m_TargetGPU);

	// XML로 저장되는 것은 Rebuild를 전제로 하는 것이기 때문에
	// Compiled Data를 저장하지 않는다.

	return true;
}

bool i3Shader::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv;

	bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	{
		i3XMLElement * pCodeXML = (i3XMLElement *) pXML->FirstChild( "Code");
		I3ASSERT( pCodeXML != nullptr);

		if( pCodeXML != nullptr)
			m_Code.LoadFromXML( pFile, pCodeXML);
	}

	pXML->getAttr( "Style", & m_Style);
	pXML->getAttr( "ISS", & m_ISSVersion);
	pXML->getAttr( "RequestedVSVersion", (INT32 *) &m_ReqVSVersion);
	pXML->getAttr( "RequestedPSVersion", (INT32 *) &m_ReqPSVersion);
	pXML->getAttr( "TargetGPU", (INT32 *)  &m_TargetGPU);

	// XML로 저장되는 것은 Rebuild를 전제로 하는 것이기 때문에
	// Compiled Data를 저장하지 않는다.
	if( isCustomShader() == false)
	{
		char szName[256];

		m_Code.GenDefName( szName, sizeof(szName)-1);

		I3TRACE( "Shader Rename : %s -> %s\n", GetName(), szName);

		SetName( szName);
	}

	return true;
}

const char * i3Shader::GetWorkDir(void)
{
	return s_szShaderWorkDir; //(const char *) s_pShaderCache->s_szShaderWorkDir;
}

void i3Shader::SetWorkDir( const char * pszPath)
{
	i3::string_ncopy_nullpad( s_szShaderWorkDir, pszPath, _countof( s_szShaderWorkDir));

	i3::make_unix_path( s_szShaderWorkDir);
}