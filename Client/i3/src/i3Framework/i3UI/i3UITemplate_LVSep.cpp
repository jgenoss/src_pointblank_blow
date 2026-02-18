#include "i3FrameworkPCH.h"
#include "i3UITemplate_LVSep.h"
#include "i3UIListView_Sep.h"

const char * s_pszLVSepInfoString[I3UI_LVSEP_SHAPE_COUNT] = 
{
	"Seperator Image",
};


I3_CLASS_INSTANCE( i3UITemplate_LVSep);


const char * i3UITemplate_LVSep::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_LVSEP_SHAPE_COUNT);
	return s_pszLVSepInfoString[nIndex];
}

i3ClassMeta * i3UITemplate_LVSep::GetInstanceMeta()
{
	return i3UIListView_Sep::static_meta();
}

void i3UITemplate_LVSep::InitializeLayout(void)
{
	AdjustLayout( 10.0f, 60.0f);
}

bool i3UITemplate_LVSep::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_LVSEP_SHAPE_SEPERATOR);

	*pRect = *(pShape->getRect());

	pRect->Normalize();

	return true;
}

bool i3UITemplate_LVSep::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pRect = _getTargetRect( I3UI_LVSEP_SHAPE_SEPERATOR, pRects);

	pRect->left		= 0.0f;
	pRect->top		= 0.0f;
	pRect->right	= cx;
	pRect->bottom	= cy;

	return true;
}

void i3UITemplate_LVSep::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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

	for( i = 0;i < getShapeCount(); ++i)
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
