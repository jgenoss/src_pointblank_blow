#if !defined( __I3_NORMALMAP_ENABLE_ATTR_H)
#define __I3_NORMALMAP_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3NormalMapEnableAttr : public i3AbstractBoolAttr
{
	I3_EXPORT_CLASS_DEFINE( i3NormalMapEnableAttr, i3AbstractBoolAttr);

public:
	i3NormalMapEnableAttr(void);

	virtual void	Apply( i3RenderContext * pContext) override;
};


#endif
