#if !defined( __I3_GIC_EXTERNAL_H)
#define __I3_GIC_EXTERNAL_H

#include "i3GICDefine.h"
#include "i3GICObject.h"
#include "i3GICShape.h"
#include "i3GICShapeFigure.h"
#include "i3GICShapeRect.h"
#include "i3GICShapeRectBar.h"
#include "i3GICShapeTextRect.h"
#include "i3GICShapeRenderTexture.h"
#include "i3GICShapeLine.h"
#include "i3GICShapeBezierLine.h"
#include "i3GICShapeCircle.h"
#include "i3GICShapeCircleBar.h"
#include "i3GICShapeTriangle.h"
#include "i3GICControlPoint.h"
#include "i3GICLinkPoint.h"
#include "i3GICDiagram.h"
#include "i3GICDiagramView.h"
#include "i3GICDiagramContext.h"
#include "i3GICShapeTemplate.h"
#include "i3GICDeviceContext.h"

I3_EXPORT_GIC
void i3GICRegisterMetas(void);

#endif
