#if !defined( __I3_NORMALMAP_WRAP_ATTR_H)
#define __I3_NORMALMAP_WRAP_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureWrapAttr.h"

class I3_EXPORT_SCENE i3NormalMapWrapAttr : public i3TextureWrapAttr
{
	I3_EXPORT_CLASS_DEFINE( i3NormalMapWrapAttr, i3TextureWrapAttr);

public:
	i3NormalMapWrapAttr(void);
};

#endif

