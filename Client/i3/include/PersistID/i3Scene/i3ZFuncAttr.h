#if !defined( __I3_Z_FUNC_ATTR_H)
#define __I3_Z_FUNC_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ZFuncAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3ZFuncAttr);
protected:
	I3G_COMP_FUNC	m_Function;

public:
	i3ZFuncAttr(void);
	virtual ~i3ZFuncAttr(void);

	void Set( I3G_COMP_FUNC func)
	{
		m_Function = func;
	}

	I3G_COMP_FUNC Get(void)
	{
		return m_Function;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif
};

#endif
