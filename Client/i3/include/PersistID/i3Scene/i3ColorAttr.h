#if !defined( __I3_COLOR_ATTR_H)
#define __I3_COLOR_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ColorAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3ColorAttr);

protected:
	COLOR	m_Color;

protected:
	void	_Check(void);

public:
	i3ColorAttr(void);
	virtual ~i3ColorAttr(void);

	void		Set( COLORREAL * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	void		Set( COLOR * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	COLOR *	Get(void)
	{
		return &m_Color;
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
