#if !defined( __I3_VERTEX_SHADER_ATTR_H)
#define __I3_VERTEX_SHADER_ATTR_H

#include "i3RenderAttr.h"

class i3VertexShaderAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3VertexShaderAttr);
protected:
	i3VertexShader *	m_pShader;

public:
	i3VertexShaderAttr(void);
	virtual ~i3VertexShaderAttr(void);

	i3VertexShader *	getShader(void)				{ return m_pShader; }
	void				SetShader( i3VertexShader * pShader);

	virtual void		Apply( i3RenderContext * pContext);

	virtual	void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
