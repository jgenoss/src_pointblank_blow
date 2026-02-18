#if !defined( __I3_LIGHTING_ENABLE_ATTR_H)
#define __I3_LIGHTING_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3LightingEnableAttr : public i3AbstractBoolAttr
{
	I3_EXPORT_CLASS_DEFINE( i3LightingEnableAttr, i3AbstractBoolAttr);

public:
	i3LightingEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext) override;
};

#endif
