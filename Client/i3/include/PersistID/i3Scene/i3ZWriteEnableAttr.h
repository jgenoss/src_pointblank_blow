#if !defined( __I3_Z_WRITE_ENABLE_ATTR_H)
#define __I3_Z_WRITE_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3ZWriteEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3ZWriteEnableAttr);

public:
	i3ZWriteEnableAttr(void);
	virtual ~i3ZWriteEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext);
};

#endif
