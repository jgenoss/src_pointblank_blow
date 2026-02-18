#if !defined( __I3_SPECULARMAP_ENABLE_ATTR_H)
#define __I3_SPECULARMAP_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3SpecularMapEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3SpecularMapEnableAttr);

public:
	i3SpecularMapEnableAttr(void);
	virtual ~i3SpecularMapEnableAttr(void);

	virtual void	Apply( i3RenderContext * pContext);
};


#endif
