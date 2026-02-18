#if !defined( __I3_REFLECTMASKMAP_BIND_ATTR_H)
#define __I3_REFLECTMASKMAP_BIND_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureBindAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3ReflectMaskMapBindAttr : public i3TextureBindAttr
{
	I3_CLASS_DEFINE( i3ReflectMaskMapBindAttr);

public:
	i3ReflectMaskMapBindAttr(void);
	virtual ~i3ReflectMaskMapBindAttr(void);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump( void);
	#endif
};

#endif
