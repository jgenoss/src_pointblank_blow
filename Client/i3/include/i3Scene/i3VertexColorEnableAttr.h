#if !defined( __I3_VERTEX_COLOR_ENABLE_ATTR_H)
#define __I3_VERTEX_COLOR_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3VertexColorEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3VertexColorEnableAttr);

public:
	i3VertexColorEnableAttr(void);
	virtual ~i3VertexColorEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext);
};

#endif
