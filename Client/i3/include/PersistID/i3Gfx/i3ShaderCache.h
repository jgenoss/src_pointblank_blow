#if !defined( __I3_SHADER_CACHE_H)
#define __I3_SHADER_CACHE_H

#include "i3Shader.h"

class i3RenderContext;

class I3_EXPORT_GFX i3ShaderCache : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3ShaderCache);

protected:
	i3BinList			m_ShaderList;
	i3RenderContext *	m_pCtx;
	bool				m_bModified;
	UINT32				m_ISSVersion;

	char				m_szWorkDir[ MAX_PATH];

protected:
	void				_SaveToFile(void);
	UINT64				_getDowngradeShader( UINT64 code);
	i3Shader *			_genShader( i3ShaderCode * pCode);
	i3Shader *			_findDowngradeShader( i3ShaderCode * pCode, i3RenderContext * pCtx);

public:
	i3ShaderCache(void);
	virtual ~i3ShaderCache(void);

	void			bindRenderContext( i3RenderContext * pCtx)		{ m_pCtx = pCtx; }

	void			setWorkDir( char * pszPath)				{ i3String::Copy( m_szWorkDir, pszPath); }

	INT32			getShaderCount(void)					{ return m_ShaderList.GetCount(); }
	i3Shader *		getShader( INT32 idx)					{ return (i3Shader *) m_ShaderList.Items[idx]; }
	void			addShader( i3Shader * pShader);
	void			RemoveShader( i3Shader * pShader);
	void			RemoveAllShaders(void);
	INT32			FindShaderByName( char * pszName);
	INT32			FindShaderByCode( i3ShaderCode * pCode);

	UINT32			getISSVersion(void)						{ return m_ISSVersion; }
	void			setISSVersion( UINT32 version)			{ m_ISSVersion = version; }

	i3Shader *		Instanciate( i3ShaderCode * code);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	static	void	AddShaderSearchPath( const char * pszPath);
	static	void	ResetShaderSearchPath(void);
	static INT32	GetShaderSearchPathCount(void);
	static char *	GetShaderSearchPath( INT32 idx);

	static i3Shader *	LoadShader( char * pszPath, I3G_SHADER_VERSION versionVS = I3G_SHADER_VERSION_1, I3G_SHADER_VERSION versionPS = I3G_SHADER_VERSION_1);

	static i3ShaderCache *		LoadCacheFile( char * pszWorkDir);
	bool			SaveCacheFile(void);
	void			FlushCacheFile(void);
	
	static i3ShaderCache *		Load( const char * pszPath);
	bool						Save( const char * pszPath);

	static UINT32				GetRequestedISSVersion(void)	{ return 2; }
};

#endif
