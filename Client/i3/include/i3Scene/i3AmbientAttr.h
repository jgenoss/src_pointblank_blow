#if !defined( __I3_AMBIENT_ATTR_H)
#define __I3_AMBIENT_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3AmbientAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3AmbientAttr, i3RenderAttr);

protected:
	COLORREAL	m_Color = { 0.7843f, 0.7843f, 0.7843f, 1.0f };

//protected:
	//void	_Check(void);

public:
	i3AmbientAttr(void);

	void		Set( COLORREAL * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	void		Set( COLOR * pColor)
	{
		i3Color::Set( &m_Color, pColor);
	}

	COLORREAL *	Get(void)
	{
		return &m_Color;
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
