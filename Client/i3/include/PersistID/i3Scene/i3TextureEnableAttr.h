#if !defined( __I3_TEXTURE_ENABLE_ATTR_H)
#define __I3_TEXTURE_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3TextureEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3TextureEnableAttr);

public:
	i3TextureEnableAttr(void);
	virtual ~i3TextureEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext);
};

#endif
