#if !defined( __I3_FOG_COLOR_ATTR_H)
#define __I3_FOG_COLOR_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FogColorAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3FogColorAttr, i3RenderAttr);

protected:
	COLOR		m_Color = { 0, 0, 0, 255 };

public:
	i3FogColorAttr(void);

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

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
