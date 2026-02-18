#if !defined( __I3_ALPHA_FUNC_ATTR_H)
#define __I3_ALPHA_FUNC_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3AlphaFuncAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3AlphaFuncAttr, i3RenderAttr);

protected:
	UINT8			m_AlphaRef = 0;
	I3G_COMP_FUNC	m_AlphaFunc = I3G_COMP_GREATER;

public:
	i3AlphaFuncAttr(void);

	void			SetRefValue( UINT8 val)			{ m_AlphaRef = val; }
	UINT8			GetRefValue(void)				{ return m_AlphaRef; }

	void			SetFunction( I3G_COMP_FUNC func)
	{
		m_AlphaFunc = func;
	}

	I3G_COMP_FUNC	GetFunction(void)
	{
		return m_AlphaFunc;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32 OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif
