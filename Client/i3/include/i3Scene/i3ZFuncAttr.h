#if !defined( __I3_Z_FUNC_ATTR_H)
#define __I3_Z_FUNC_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ZFuncAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ZFuncAttr, i3RenderAttr);
protected:
	I3G_COMP_FUNC	m_Function = I3G_COMP_LESS;

public:
	i3ZFuncAttr(void);

	void Set( I3G_COMP_FUNC func)
	{
		m_Function = func;
	}

	I3G_COMP_FUNC Get(void)
	{
		return m_Function;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif
