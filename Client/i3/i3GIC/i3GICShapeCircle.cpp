#include "stdafx.h"
#include "i3GICShapeCircle.h"
#include "i3GICDiagramContext.h"
#include "i3GICControlPoint.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeCircle);

i3GICShapeCircle::i3GICShapeCircle()
{
	setStyle( I3GIC_STYLE_CIRCLE);

	setSize( 100.0f, 100.0f);
}


/*virtual*/ void i3GICShapeCircle::OnDraw( i3GICDiagramContext * pCtx)
{
	RECT rect;
	POINT pt;

	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		pt.x = (INT32) getPosX();
		pt.y = (INT32) getPosY();

		pCtx->GetDrawPos( &pt);

		rect.left = pt.x;
		rect.top = pt.y;
		rect.right = pt.x + (INT32) getWidth();
		rect.bottom = pt.y + (INT32) getHeight();

		pCtx->toDP( &rect);

		pCtx->SetModeToDevice();

		_SelectPenAndBrush( pCtx);
		pCtx->DP_Ellipse( rect.left, rect.top, rect.right, rect.bottom);

		pCtx->SetModeToDraw();
	}

	OnAfterDraw( pCtx);

	i3GICShapeFigure::OnDraw( pCtx);
}

#endif	// I3_DEBUG

