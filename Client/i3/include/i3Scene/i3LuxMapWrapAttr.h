#if !defined( __I3_LUXMAP_WRAP_ATTR_H)
#define __I3_LUXMAP_WRAP_ATTR_H

#include "i3RenderAttr.h"
#include "i3TextureWrapAttr.h"

class I3_EXPORT_SCENE i3LuxMapWrapAttr : public i3TextureWrapAttr
{
	I3_EXPORT_CLASS_DEFINE( i3LuxMapWrapAttr, i3TextureWrapAttr);

public:
	i3LuxMapWrapAttr(void);
};

#endif

