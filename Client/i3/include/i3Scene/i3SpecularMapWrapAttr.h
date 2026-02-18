#if !defined( __I3_SPECULARMAP_WRAP_ATTR_H)
#define __I3_SPECULARMAP_WRAP_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureWrapAttr.h"

class I3_EXPORT_SCENE i3SpecularMapWrapAttr : public i3TextureWrapAttr
{
	I3_EXPORT_CLASS_DEFINE( i3SpecularMapWrapAttr, i3TextureWrapAttr);

public:
	i3SpecularMapWrapAttr(void);
};

#endif

