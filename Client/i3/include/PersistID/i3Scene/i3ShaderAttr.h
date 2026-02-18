#if !defined( __I3_VERTEX_SHADER_ATTR_H)
#define __I3_VERTEX_SHADER_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ShaderAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3ShaderAttr);
protected:
	i3Shader *			m_pShader;

public:
	i3ShaderAttr(void);
	virtual ~i3ShaderAttr(void);

	i3Shader *			getShader(void)						{ return m_pShader; }
	void				setShader( i3Shader * pShader);

	virtual void		Apply( i3RenderContext * pContext);
	virtual BOOL		IsSame( i3RenderAttr * pAttr1);

	virtual	void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

#if defined( I3_DEBUG)
	virtual void		Dump(void);
#endif
};

#endif
