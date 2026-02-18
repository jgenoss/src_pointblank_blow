#if !defined( __I3_VERTEX_BLEND_ENABLE_ATTR_H)
#define __I3_VERTEX_BLEND_ENABLE_ATTR_H

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3VertexBlendEnableAttr : public i3AbstractBoolAttr
{
	I3_CLASS_DEFINE( i3VertexBlendEnableAttr);

public:
	i3VertexBlendEnableAttr(void);
	virtual ~i3VertexBlendEnableAttr(void);

	virtual void Apply( i3RenderContext * pContext);
};

#endif
