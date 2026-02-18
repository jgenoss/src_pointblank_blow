#if !defined( __I3_STENCIL_TEST_ENABLE_ATTR_H)
#define __I3_STENCIL_TEST_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3StencilTestEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3StencilTestEnableAttr);

public:
	i3StencilTestEnableAttr(void);
	virtual ~i3StencilTestEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext);
};

#endif
