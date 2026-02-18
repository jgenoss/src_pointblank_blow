#if !defined( __I3_REFLECTMAP_BIND_ATTR_H)
#define __I3_REFLECTMAP_BIND_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureBindAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3ReflectMapBindAttr : public i3TextureBindAttr
{
	I3_CLASS_DEFINE( i3ReflectMapBindAttr);
protected:
	bool		m_bFresnel;
	REAL32		m_FresnelFactor;
	REAL32		m_ReflectFactor;

public:
	i3ReflectMapBindAttr(void);
	virtual ~i3ReflectMapBindAttr(void);

	bool				getFresnelTermEnable(void)				{ return m_bFresnel; }
	void				setFresnelTermEnable( bool bFlag)		{ m_bFresnel = bFlag; }

	REAL32				getReflectFactor(void)					{ return m_ReflectFactor; }
	void				setReflectFactor( REAL32 factor)		{ m_ReflectFactor = factor; }

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump( void);
	#endif
};

#endif
