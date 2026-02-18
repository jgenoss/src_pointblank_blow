#if !defined( __I3_SHADER_CACHE_H)
#define __I3_SHADER_CACHE_H

#include "i3Shader.h"
#include "i3Base/itl/vector_map.h"

class i3RenderContext;

#define SPLIT_ACTIVE_SHADER

struct ShaderComp
{
	bool operator()(const i3Shader* p1, const i3Shader* p2) const {  return p1->getCode() < p2->getCode(); }
	bool operator()(const i3Shader* p1, const i3ShaderCode* c2) const { return p1->getCode() < c2->getCode();  }
	bool operator()(const i3ShaderCode* c1, const i3Shader* p2) const { return c1->getCode() < p2->getCode();  }
	bool operator()(const i3Shader* p1, const UINT64& c2) const {  return p1->getCode() < c2;  }
	bool operator()(const UINT64& c1, const i3Shader* p2) const {  return c1 < p2->getCode(); }
};


class I3_EXPORT_GFX i3ShaderCache : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderCache, i3PersistantElement);

protected:
	i3::vector_multiset<i3Shader*, ShaderComp> m_ShaderList;				// 이건 동일값 반복이 존재할수 있음..
	i3RenderContext *	m_pCtx = nullptr;
	bool				m_bModified = false;
	UINT32				m_ISSVersion;									// [initialize at constructor]

	char				m_szWorkDir[MAX_PATH] = { 0 };

	#if defined( SPLIT_ACTIVE_SHADER)
	i3::vector_set<i3Shader*, ShaderComp> m_ActiveShaderList;			// 이건 유일..
	#endif

protected:
	UINT64				_getDowngradeShader( UINT64 code);
	i3Shader *			_genShader( i3ShaderCode * pCode);
	i3Shader *			_findDowngradeShader( i3ShaderCode * pCode, i3RenderContext * pCtx);

public:
	i3ShaderCache(void);
	virtual ~i3ShaderCache(void);

	void			bindRenderContext( i3RenderContext * pCtx)		{ m_pCtx = pCtx; }

	void			setWorkDir( char * pszPath)				{ i3::safe_string_copy( m_szWorkDir, pszPath, MAX_PATH); }

	INT32			getShaderCount(void)					{ return (INT32)m_ShaderList.size(); }
	i3Shader *		getShader( INT32 idx)					{ return m_ShaderList.get_vector()[idx]; }
	void			addShader( i3Shader * pShader);
	void			RemoveShader( i3Shader * pShader);
	void			RemoveAllShaders(void);
	i3Shader*		FindShaderByName( const char * pszName) const;
	i3Shader*		FindShaderByCode( i3ShaderCode * pCode) const;

	#if defined( SPLIT_ACTIVE_SHADER)
	INT32			getActiveShaderCount(void)				{ return (INT32)m_ActiveShaderList.size(); }
	i3Shader *		getActiveShader( INT32 idx)				{ return m_ActiveShaderList.get_vector()[idx]; }

	i3Shader*		FindActiveShaderByCode( i3ShaderCode * pCode);
	#endif

	UINT32			getISSVersion(void)						{ return m_ISSVersion; }
	void			setISSVersion( UINT32 version)			{ m_ISSVersion = version; }

	i3Shader *		Instanciate( i3ShaderCode * code);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	static	void	AddShaderSearchPath( const char * pszPath);
	static	void	ResetShaderSearchPath(void);
	static INT32	GetShaderSearchPathCount(void);
	static const char *	GetShaderSearchPath( INT32 idx);

	static i3Shader *	LoadShader( const char * pszPath, 
							I3G_SHADER_VERSION versionVS = I3G_SHADER_VERSION_1, 
							I3G_SHADER_VERSION versionPS = I3G_SHADER_VERSION_1,
							I3G_GPU_VENDOR target = I3G_GPU_VENDOR_NA,
							bool bAddCache = true);

	static i3Shader *	CloneShader( i3Shader * pShader, I3G_GPU_VENDOR target);

	static i3ShaderCache *		LoadCacheFile( char * pszWorkDir);
	bool			SaveCacheFile(void);
	void			FlushCacheFile(void);
	void			Sort(void);
	
	static i3ShaderCache *		Load( const char * pszPath);
	bool						Save( const char * pszPath);

	void						Dump(void);

	static UINT32				GetRequestedISSVersion(void);
	static void					SetRequestedISSVersion(UINT32 ver);
};

#endif
