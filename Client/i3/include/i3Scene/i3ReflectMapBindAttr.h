#if !defined( __I3_REFLECTMAP_BIND_ATTR_H)
#define __I3_REFLECTMAP_BIND_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureBindAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3ReflectMapBindAttr : public i3TextureBindAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ReflectMapBindAttr, i3TextureBindAttr);
protected:
	bool		m_bFresnel = true;

public:
	i3ReflectMapBindAttr(void);

	bool				getFresnelTermEnable(void)				{ return m_bFresnel; }
	void				setFresnelTermEnable( bool bFlag)		{ m_bFresnel = bFlag; }

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump( void) override;
	#endif
};

#endif
