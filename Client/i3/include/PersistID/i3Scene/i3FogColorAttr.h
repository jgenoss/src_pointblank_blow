#if !defined( __I3_FOG_COLOR_ATTR_H)
#define __I3_FOG_COLOR_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FogColorAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3FogColorAttr);

protected:
	COLOR		m_Color;

public:
	i3FogColorAttr(void);
	virtual ~i3FogColorAttr(void);

	void			Set( COLORREAL * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	void			Set( COLOR * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	COLOR *		Get(void)
	{
		return &m_Color; 
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
