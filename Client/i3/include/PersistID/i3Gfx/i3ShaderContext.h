
#if !defined( __I3_SHADER_CONTEXT_H)
#define __I3_SHADER_CONTEXT_H

#include "i3ShaderParam.h"
#include "i3Light.h"
#include "i3ShaderCode.h"
#include "i3ShaderParam.h"
#include "i3GfxResource.h"

class i3RenderContext;

#define I3G_SHADER_CTX_STATE_COMPILED			0x00000001
#define	I3G_SHADER_CTX_STATE_ERROR				0x00000002
#define	I3G_SHADER_CTX_STATE_SKINNING			0x00000004
#define	I3G_SHADER_CTX_STATE_LOSTDEVICE			0x80000000

// Shader Version
enum I3G_SHADER_VERSION
{	
	I3G_SHADER_VERSION_1 = 1,
	I3G_SHADER_VERSION_2,
	I3G_SHADER_VERSION_3,

	I3G_SHADER_VERSION_COUNT,
};

class i3Shader;

#define		MAX_LPS				2

class I3_EXPORT_GFX i3ShaderContext : public i3GfxResource
{
	I3_CLASS_DEFINE( i3ShaderContext);
protected:
	// Volatile members
	i3Shader *			m_pShader;

	i3List *			m_ParamListArray[ I3G_SHADER_TYPE_MAX];

	UINT32				m_State;
	UINT32				m_ModifiedFlag;
	char *				m_pErrorMsg;

	void *				m_pShaderBin[ I3G_SHADER_TYPE_MAX];

	i3Light *			m_pLights[MAX_LPS];
	INT32				m_LightCount;

protected:
	void				_ReleaseShaders(void);
	
#if defined( I3G_DX)
	bool						_SyncParams( I3G_SHADER_TYPE type, DWORD * pShaderData, ID3DXConstantTable * pTable);
	bool						_Compile( I3G_SHADER_TYPE type, char * pszWorkDir, char * pszFileName, INT32 numBones);
	bool						_CompileMem( I3G_SHADER_TYPE type, char * pszHLSL, UINT32 sz, INT32 numBones);
	IDirect3DVertexShader9 *	_CreateVertexShader( I3G_SHADER_TYPE type, i3RenderContext * pContext);
	IDirect3DPixelShader9 *		_CreatePixelShader( i3RenderContext * pContext);
	INT32 						_MakeVSProfile( I3G_SHADER_TYPE type, INT32 * pStartIndex);
	INT32						_MakePSProfile( INT32 * pStartIndex);
#endif

public:
	i3ShaderContext(void);
	virtual ~i3ShaderContext(void);

	UINT32			getState(void)						{ return m_State; }
	void			setState( UINT32 state)				{ m_State = state; }
	void			addState( UINT32 state)				{ m_State |= state; }
	void			removeState( UINT32 state)			{ m_State &= ~state; }
	bool			isState( UINT32 state)				{ return (m_State & state) != 0; }
	bool			isCompiled(void)					{ return isState( I3G_SHADER_CTX_STATE_COMPILED); }
	bool			isError(void)						{ return isState( I3G_SHADER_CTX_STATE_ERROR); }

	void			setLight( i3Light * pLight1, i3Light * i3Light2);
	void			resetLight(void)					{ m_LightCount = 0; }

	UINT32			getModifiedFlag(void)				{ return m_ModifiedFlag; }
	void			setModifiedFlag( UINT32 flag)		{ m_ModifiedFlag = flag; }
	void			addModifiedFlag( UINT32 flag)		{ m_ModifiedFlag |= flag; }
	void			removeModifiedFlag( UINT32 flag)	{ m_ModifiedFlag &= ~flag; }

	void			AddParam( I3G_SHADER_TYPE type, i3ShaderParam * pParam);
	void			RemoveParam( I3G_SHADER_TYPE type, i3ShaderParam * pParam);
	void			RemoveAllParams(void);
	UINT32			getParamCount( I3G_SHADER_TYPE type);
	i3ShaderParam *	getParam( I3G_SHADER_TYPE type, INT32 idx);
	i3ShaderParam *	FindParam( I3G_SHADER_TYPE type, const char * pszName);

	void *			getShader( I3G_SHADER_TYPE type)	{ return m_pShaderBin[ type]; }

	char *			getErrorMsg(void)					{ return m_pErrorMsg; }
	void			SetErrorMsg( char * pszMsg, INT32 len);

	void			ReportErrorMsg(void);

	bool			IsShader( I3G_SHADER_TYPE type);

	void			Bind( i3Shader * pShader);
	bool			Compile( char * pszWorkPath, char * pszFileName = NULL);
	bool			BuildNativeShader( i3RenderContext * pContext);
	bool			Apply( i3RenderContext * pContext);
	void			UpdateParams( I3G_SHADER_TYPE type, i3RenderContext * pCtx, bool bDump = false);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	Revive( i3RenderContext * pCtx);
};

#endif
