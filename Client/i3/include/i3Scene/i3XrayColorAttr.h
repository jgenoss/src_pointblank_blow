#if !defined( __I3_XRAY_COLOR_ATTR_H)
#define __I3_XRAY_COLOR_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3XrayColorAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3XrayColorAttr, i3RenderAttr);

protected:
	COLORREAL		m_Color;

public:
	i3XrayColorAttr(void);

	void			Set( COLORREAL * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	void			Set( COLOR * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	COLORREAL *		Get(void)
	{
		return &m_Color; 
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

};

#endif
