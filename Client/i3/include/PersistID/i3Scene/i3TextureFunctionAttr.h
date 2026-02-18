#if !defined( I3SG_TEXTURE_FUNCTION_ATTR_H)
#define I3SG_TEXTURE_FUNCTION_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureFunctionAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextureFunctionAttr);
protected:
	I3G_TEXTURE_BIND		m_Bind;
	I3G_TEXTURE_FUNCTION	m_Function;

public:
	i3TextureFunctionAttr(void);
	virtual ~i3TextureFunctionAttr(void);

	void					SetBindType( I3G_TEXTURE_BIND bind)		{ m_Bind = bind; }
	I3G_TEXTURE_BIND		GetBindType(void)						{ return m_Bind; }

	void					SetFunction( I3G_TEXTURE_FUNCTION func)	{ m_Function = func; }
	I3G_TEXTURE_FUNCTION	GetFunction(void)						{ return m_Function; }

	virtual void			CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual void			Apply( i3RenderContext * pContext);
	virtual BOOL			IsSame( i3RenderAttr * pAttr);
	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void			Dump(void);
	#endif
};

#endif
