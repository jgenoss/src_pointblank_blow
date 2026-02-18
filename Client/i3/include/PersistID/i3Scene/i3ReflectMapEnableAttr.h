#if !defined( __I3_REFLECTMAP_ENABLE_ATTR_H)
#define __I3_REFLECTMAP_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3ReflectMapEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3ReflectMapEnableAttr);

public:
	i3ReflectMapEnableAttr(void);
	virtual ~i3ReflectMapEnableAttr(void);

	virtual void	Apply( i3RenderContext * pContext);
};


#endif
