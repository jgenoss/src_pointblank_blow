#if !defined( __I3_GIC_SHAPE_CIRCLE_H__)
#define __I3_GIC_SHAPE_CIRCLE_H__

#include "i3GICShapeRect.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeCircle : public i3GICShapeRect
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeCircle, i3GICShapeRect);

private:	

public:
	i3GICShapeCircle();
	virtual ~i3GICShapeCircle() {}

	virtual void	OnAfterDraw( i3GICDiagramContext * pCtx) {}

	virtual void	OnDraw( i3GICDiagramContext * pCtx);
};

#endif I3_DEBUG

#endif
