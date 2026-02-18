#if !defined( __I3_SHADOWMAP_ENABLE_ATTR_H)
#define __I3_SHADOWMAP_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3ShadowMapEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3ShadowMapEnableAttr);

public:
	i3ShadowMapEnableAttr(void);
	virtual ~i3ShadowMapEnableAttr(void);

	virtual void	Apply( i3RenderContext * pContext);
};


#endif
