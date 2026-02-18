#if !defined( __I3_FRESNEL_ATTR_H)
#define __I3_FRESNEL_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FresnelAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3FresnelAttr, i3RenderAttr);
protected:
	bool		m_bEnable = false;
	REAL32		m_IOR = 1.0f;
	COLORREAL	m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
	i3FresnelAttr(void);

	bool		isEnable(void)						{ return m_bEnable; }
	void		setEnable( bool bFlag)				{ m_bEnable = bFlag; }

	REAL32		getIOR(void)						{ return m_IOR; }
	void		setIOR( REAL32 val)					{ m_IOR = val; }

	COLORREAL *	getColor(void)						{ return &m_Color; }
	void		setColor( COLORREAL * pColor)		{ i3Color::Set( &m_Color, pColor); }
	void		setColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		m_Color.r = r;
		m_Color.g = g;
		m_Color.b = b;
		m_Color.a = a;
	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual bool	IsSame( i3RenderAttr * pAttr) override;
	virtual void	Apply( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump( void) override;
	#endif
};

#endif
