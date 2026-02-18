#if !defined( __I3_GIC_SHAPE_BEZIER_LINE_H__)
#define __I3_GIC_SHAPE_BEZIER_LINE_H__

#include "i3GICShapeLine.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeBezierLine : public i3GICShapeLine
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeBezierLine, i3GICShapeLine);

private:
	// Volatile member
	I3GIC_POINT		m_ControlP1;
	I3GIC_POINT		m_ControlP2;

public:
	i3GICShapeBezierLine();
	virtual ~i3GICShapeBezierLine() {}

	void			CalculateCurve( void);

	virtual	void	OnDraw( i3GICDiagramContext * pCtx);
};

#endif	// I3_DEBUG

#endif
