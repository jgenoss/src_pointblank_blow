#if !defined( I3SG_TEXTURE_FUNCTION_ATTR_H)
#define I3SG_TEXTURE_FUNCTION_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureFunctionAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextureFunctionAttr, i3RenderAttr);
protected:
	I3G_TEXTURE_BIND		m_Bind = I3G_TEXTURE_BIND_DIFFUSE;
	I3G_TEXTURE_FUNCTION	m_Function = I3G_TEXTURE_FUNC_MODULATE;

public:
	i3TextureFunctionAttr(void);

	void					SetBindType( I3G_TEXTURE_BIND bind)		{ m_Bind = bind; }
	I3G_TEXTURE_BIND		GetBindType(void)						{ return m_Bind; }

	void					SetFunction( I3G_TEXTURE_FUNCTION func)	{ m_Function = func; }
	I3G_TEXTURE_FUNCTION	GetFunction(void)						{ return m_Function; }

	virtual void			CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual void			Apply( i3RenderContext * pContext) override;
	virtual bool			IsSame( i3RenderAttr * pAttr) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void			Dump(void) override;
	#endif
};

#endif
