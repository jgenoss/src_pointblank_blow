#if !defined( __I3_VERTEX_SHADER_ATTR_H)
#define __I3_VERTEX_SHADER_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ShaderAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderAttr, i3RenderAttr);
protected:
	i3Shader *			m_pShader = nullptr;

	i3::rc_string		m_strPath;
	bool				m_bAddCache = true;
	I3G_SHADER_VERSION	m_vsVersion = I3G_SHADER_VERSION_1;
	I3G_SHADER_VERSION	m_psVersion = I3G_SHADER_VERSION_1;

public:
	i3ShaderAttr(void);
	virtual ~i3ShaderAttr(void);

	i3Shader *			getShader(void)						{ return m_pShader; }
	void				setShader( i3Shader * pShader);

	const char *			getSource(void) const				{ return m_strPath.c_str(); }
	const i3::rc_string&	getSourceString() const				{ return m_strPath; }
	void				setSource( const char * pszPath);

	I3G_SHADER_VERSION	getTargetVertexShaderVersion(void)		{ return m_vsVersion; }
	void				setTargetVertexShaderVersion( I3G_SHADER_VERSION ver)	{	m_vsVersion = ver; }

	I3G_SHADER_VERSION	getTargetPixelShaderVersion(void)		{ return m_psVersion; }
	void				setTargetPixelShaderVersion( I3G_SHADER_VERSION ver)	{ m_psVersion = ver; }

	void				LoadShader(void);

	bool				getAddCacheState(void)					{ return m_bAddCache; }
	void				setAddCacheState( bool bState);

	virtual void		Apply( i3RenderContext * pContext) override;
	virtual bool		IsSame( i3RenderAttr * pAttr1) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

#if defined( I3_DEBUG)
	virtual void		Dump(void) override;
#endif
};

#endif
