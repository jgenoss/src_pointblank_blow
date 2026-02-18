#include "i3FrameworkPCH.h"
#include "i3UITemplate_ImageBoxEx.h"
#include "i3UIImageBoxEx.h"

const char * s_pszImageBoxExInfoString[I3UI_IMAGEBOXEX_SHAPE_COUNT] = 
{
	"바탕 이미지 (크기 변화)",
};


I3_CLASS_INSTANCE( i3UITemplate_ImageBoxEx);

i3UIShape* i3UITemplate_ImageBoxEx::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return & m_pShape[nIndex];
}

const char * i3UITemplate_ImageBoxEx::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return s_pszImageBoxExInfoString[nIndex];
}

I3UI_CATEGORY i3UITemplate_ImageBoxEx::getTemplateCategory(void)
{
	return I3UI_CATEGORY_IMAGEBOXEX;
}

i3ClassMeta * i3UITemplate_ImageBoxEx::GetInstanceMeta()
{
	return i3UIImageBoxEx::static_meta();
}

void i3UITemplate_ImageBoxEx::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 200.0f);
}

bool i3UITemplate_ImageBoxEx::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_IMAGEBOXEX_SHAPE_IMAGE);

	*pRect = *(pShape->getRect());

	pRect->Normalize();

	return true;
}

bool i3UITemplate_ImageBoxEx::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pRect = _getTargetRect( I3UI_IMAGEBOXEX_SHAPE_IMAGE, pRects);

	pRect->left		= 0.0f;
	pRect->top		= 0.0f;
	pRect->right	= cx;
	pRect->bottom	= cy;

	return true;
}

void i3UITemplate_ImageBoxEx::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
	//i3::pack::RECT * pOldRect = nullptr;	// C4189

	*pCX = 0.0f;
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
	}
}
