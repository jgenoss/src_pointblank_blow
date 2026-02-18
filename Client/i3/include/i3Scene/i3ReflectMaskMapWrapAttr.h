#if !defined( __I3_REFLECTMASKMAP_WRAP_ATTR_H)
#define __I3_REFLECTMASKMAP_WRAP_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureWrapAttr.h"

class I3_EXPORT_SCENE i3ReflectMaskMapWrapAttr : public i3TextureWrapAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ReflectMaskMapWrapAttr, i3TextureWrapAttr);

public:
	i3ReflectMaskMapWrapAttr(void);
};

#endif

