#if !defined( __I3_LUXMAP_ENABLE_ATTR_H)
#define __I3_LUXMAP_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3LuxMapEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3LuxMapEnableAttr);

public:
	i3LuxMapEnableAttr(void);
	virtual ~i3LuxMapEnableAttr(void);

	virtual void	Apply( i3RenderContext * pContext);
};


#endif
