#include "i3FrameworkPCH.h"
#include "i3UITemplate_3DView.h"
#include "i3UI3DViewBox.h"

I3_CLASS_INSTANCE( i3UITemplate_3DView);

I3UI_CATEGORY i3UITemplate_3DView::getTemplateCategory(void)
{
	return I3UI_CATEGORY_STATIC_IMAGE;
}

i3ClassMeta * i3UITemplate_3DView::GetInstanceMeta()
{
	return i3UI3DViewBox::static_meta();
}

void i3UITemplate_3DView::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 200.0f);
}

bool i3UITemplate_3DView::GetControlRect( i3::pack::RECT * pRect)
{
	//i3UIShape * pShape = getShape( I3UI_IMAGEBOX_SHAPE_IMAGE);

	//*pRect = *(pShape->getRect());

	pRect->right = 200.f;
	pRect->bottom = 200.f;

	return true;
}

bool i3UITemplate_3DView::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{	
	/*i3::pack::RECT * pRect = _getTargetRect( I3UI_IMAGEBOX_SHAPE_IMAGE, pRects);

	pRect->left		= 0.0f;
	pRect->top		= 0.0f;
	pRect->right	= cx;
	pRect->bottom	= cy;*/

	return true;
}

void i3UITemplate_3DView::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
	//i3::pack::RECT * pOldRect = nullptr;	// C4189

	/**pCX = 0.0f;
	*pCY = 0.0f;

	REAL32 left = 0.0f;
	REAL32 top = 0.0f;

	INT32 i;
	INT32 totalCount = getShapeCount();
	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( i = 0;i < totalCount; ++i)
	{
		i3::pack::RECT * pRect = _getTargetRect( i, pRects);

		pRect->left = left;
		pRect->top	= top;

		left += pRect->right + I3UI_SPREAD_MARGIN;

		*pCX = i3Math::Max( left, *pCX);
		
		if( i%3 == 2)
		{
			left = 0.0f;
			top += pRect->bottom + I3UI_SPREAD_MARGIN;
		}
		
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}*/
}
