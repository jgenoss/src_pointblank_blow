#if !defined( __I3_ALPHA_FUNC_ATTR_H)
#define __I3_ALPHA_FUNC_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3AlphaFuncAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3AlphaFuncAttr);

protected:
	UINT8			m_AlphaRef;
	I3G_COMP_FUNC	m_AlphaFunc;

public:
	i3AlphaFuncAttr(void);
	virtual ~i3AlphaFuncAttr(void);

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

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif
};

#endif
