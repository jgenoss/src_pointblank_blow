#if !defined( __I3_SHADER_H)
#define __I3_SHADER_H

#include "i3GfxResource.h"
#include "i3ShaderDefine.h"
#include "i3ShaderContext.h"
#include "i3ShaderCode.h"

class i3RenderContext;

#define		I3G_SHADER_STYLE_BUILTIN			0x80000000

#define		I3G_SHADER_STATE_GENERATED			0x40000000
#define		I3G_SHADER_STATE_LOADED				0x20000000

#define		I3G_SHADER_STATE_VS_0_COMPILED		0x00000001
#define		I3G_SHADER_STATE_VS_1_COMPILED		0x00000002
#define		I3G_SHADER_STATE_VS_2_COMPILED		0x00000004
#define		I3G_SHADER_STATE_VS_3_COMPILED		0x00000008
#define		I3G_SHADER_STATE_VS_4_COMPILED		0x00000010
#define		I3G_SHADER_STATE_PS_COMPILED		0x00000020

#define		I3G_SHADER_STATE_VS_0_VALIDATED		0x00000100
#define		I3G_SHADER_STATE_VS_1_VALIDATED		0x00000200
#define		I3G_SHADER_STATE_VS_2_VALIDATED		0x00000400
#define		I3G_SHADER_STATE_VS_3_VALIDATED		0x00000800
#define		I3G_SHADER_STATE_VS_4_VALIDATED		0x00001000
#define		I3G_SHADER_STATE_PS_VALIDATED		0x00002000

#define		I3G_SHADER_STATE_ALL_COMPILED		(I3G_SHADER_STATE_VS_0_COMPILED | I3G_SHADER_STATE_VS_1_COMPILED | I3G_SHADER_STATE_VS_2_COMPILED | \
												I3G_SHADER_STATE_VS_3_COMPILED | I3G_SHADER_STATE_VS_4_COMPILED | I3G_SHADER_STATE_PS_COMPILED)

#define		I3G_SHADER_STATE_ALL_VALIDATED		(I3G_SHADER_STATE_VS_0_VALIDATED | I3G_SHADER_STATE_VS_1_VALIDATED | I3G_SHADER_STATE_VS_2_VALIDATED | \
												I3G_SHADER_STATE_VS_3_VALIDATED | I3G_SHADER_STATE_VS_4_VALIDATED | I3G_SHADER_STATE_PS_VALIDATED)

class I3_EXPORT_GFX i3Shader : public i3NamedElement
{
	I3_CLASS_DEFINE( i3Shader);
protected:
	// Persistent members
	i3ShaderCode		m_Code;
	UINT32				m_ISSVersion;
	I3G_SHADER_VERSION	m_ReqVSVersion;
	I3G_SHADER_VERSION	m_ReqPSVersion;

	char *				m_pData[I3G_SHADER_TYPE_MAX];
	UINT32				m_DataSize[I3G_SHADER_TYPE_MAX];
	UINT16				m_VersionTable[ I3G_SHADER_TYPE_MAX];

	UINT32				m_Style;

	// Volatile members
	UINT32				m_State;
	i3ShaderContext *	m_pCtx;
	char *				m_pszWorkDir;

	i3Shader *			m_pDowngradeShader;

public:
	i3Shader(void);
	virtual ~i3Shader(void);

	UINT32				getState(void)						{ return m_State; }
	void				setState( UINT32 state)				{ m_State = state; }
	void				addState( UINT32 mask)				{ m_State |= mask; }
	void				removeState( UINT32 mask)			{ m_State &= ~mask; }
	bool				isGenerated(void)					{ return (m_State & I3G_SHADER_STATE_GENERATED) != 0; }
	bool				isAllCompiled(void)					{ return (m_State & I3G_SHADER_STATE_ALL_COMPILED) == I3G_SHADER_STATE_ALL_COMPILED; }
	bool				isAllValidated(void)				{ return (m_State & I3G_SHADER_STATE_ALL_VALIDATED) == I3G_SHADER_STATE_ALL_VALIDATED; }
	bool				isCompiled( I3G_SHADER_TYPE type)	{ return ((m_State >> type) & 1) != 0; }
	bool				isValidated( I3G_SHADER_TYPE type)	{ return ((m_State >> (type + 8)) & 1) != 0; }
	bool				_isNeedToRebuild(void);

	// Shader Model Version과는 다르게 i3Engine에서 관리용으로 사용하는 Shader System Version이다.
	UINT32				getISSVersion(void)					{ return m_ISSVersion; }
	void				setISSVersion( UINT32 version)		{ m_ISSVersion = version; }

	I3G_SHADER_VERSION	getRequestedVSVersion(void)			{ return m_ReqVSVersion; }
	void				setRequestedVSVersion( I3G_SHADER_VERSION ver)
	{
		m_ReqVSVersion = ver;
	}

	I3G_SHADER_VERSION	getRequestedPSVersion(void)			{ return m_ReqPSVersion; }
	void				setRequestedPSVersion( I3G_SHADER_VERSION ver)
	{
		m_ReqPSVersion = ver;
	}


	UINT32				getStyle(void)						{ return m_Style; }
	void				setStyle( UINT32 mask)				{ m_Style = mask; }
	void				addStyle( UINT32 mask)				{ m_Style |= mask; }
	void				removeStyle( UINT32 mask)			{ m_Style &= ~mask; }
	bool				isCustomShader(void)				{ return (m_Style & I3G_SHADER_STYLE_BUILTIN) == 0; }

	UINT64				getCode(void)						{ return m_Code.getCode(); }
	void				setCode( UINT64 code)				{ m_Code.setCode( code); }

	i3ShaderCode *		getShaderCode(void)					{ return & m_Code; }
	void				setShaderCode( i3ShaderCode code)	{ m_Code = code; }

	i3ShaderContext *	getContext(void)					{ return m_pCtx; }

	i3ShaderParam *		FindParam( I3G_SHADER_TYPE type, const char * pszName)
	{
		I3ASSERT( m_pCtx != NULL);

		return m_pCtx->FindParam( type, pszName);
	}

	char *				getWorkDir(void)					{ return m_pszWorkDir; }
	void				setWorkDir( char * pszPath)			{ m_pszWorkDir = pszPath; }

	char *				getShaderData( I3G_SHADER_TYPE type)		{ return m_pData[ type]; }
	INT32				getShaderDataSize( I3G_SHADER_TYPE type)	{ return m_DataSize[ type]; }
	UINT16				getShaderVersion( I3G_SHADER_TYPE type)		{ return m_VersionTable[ type]; }

	void				SetShaderData( I3G_SHADER_TYPE type, char * pData, UINT32 sz, UINT16 version);

	bool				Create( i3ShaderCode * pCode, char * pszWorkDir, UINT32 style = I3G_SHADER_STYLE_BUILTIN, I3G_SHADER_VERSION versionVS = I3G_SHADER_VERSION_1, I3G_SHADER_VERSION versionPS = I3G_SHADER_VERSION_1);
	bool				Create( char * pszWorkDir, char * pszFileName, UINT32 style = 0, I3G_SHADER_VERSION versionVS = I3G_SHADER_VERSION_1, I3G_SHADER_VERSION versionPS = I3G_SHADER_VERSION_1);
	bool				Validate( i3RenderContext * pCtx);
	bool				Rebuild(void);

	i3Shader *			getDowngradeShader(void)			{ return m_pDowngradeShader; }
	void				setDowngradeShader( i3Shader * pShader)
	{
		m_pDowngradeShader = pShader;
	}

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
