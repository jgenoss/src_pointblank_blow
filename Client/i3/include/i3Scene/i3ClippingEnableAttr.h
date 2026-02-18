#if !defined( __I3_CLIPPING_ENABLE_ATTR_H)
#define __I3_CLIPPING_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3ClippingEnableAttr : public i3AbstractBoolAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ClippingEnableAttr, i3AbstractBoolAttr);

public:
	i3ClippingEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext) override;
};

#endif
