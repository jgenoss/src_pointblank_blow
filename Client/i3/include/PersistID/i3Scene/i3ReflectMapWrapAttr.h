#if !defined( __I3_REFLECTMAP_WRAP_ATTR_H)
#define __I3_REFLECTMAP_WRAP_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureWrapAttr.h"

class I3_EXPORT_SCENE i3ReflectMapWrapAttr : public i3TextureWrapAttr
{
	I3_CLASS_DEFINE( i3ReflectMapWrapAttr);

public:
	i3ReflectMapWrapAttr(void);
	virtual ~i3ReflectMapWrapAttr(void);
};

#endif

