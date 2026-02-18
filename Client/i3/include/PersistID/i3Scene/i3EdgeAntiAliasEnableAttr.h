#if !defined( __I3_EDGE_ANTIALIAS_ENABLE_ATTR_H)
#define __I3_EDGE_ANTIALIAS_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3EdgeAntiAliasEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3EdgeAntiAliasEnableAttr);

public:
	i3EdgeAntiAliasEnableAttr(void);
	virtual ~i3EdgeAntiAliasEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext);
};

#endif
