#if !defined ( __I3_ALPHA_TEST_ENABLE_ATTR_H)
#define __I3_ALPHA_TEST_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3AlphaTestEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3AlphaTestEnableAttr);

public:
	i3AlphaTestEnableAttr(void);
	virtual ~i3AlphaTestEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext);
};

#endif
