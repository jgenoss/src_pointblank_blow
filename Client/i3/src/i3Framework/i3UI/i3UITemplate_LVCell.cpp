#include "i3FrameworkPCH.h"
#include "i3UITemplate_LVCell.h"
#include "i3UIListView_Cell.h"

const char * s_pszLVCellInfoString[I3UI_LVCELL_SHAPE_COUNT] = 
{
	"Cell Image",
};

I3_CLASS_INSTANCE( i3UITemplate_LVCell);

const char * i3UITemplate_LVCell::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_LVCELL_SHAPE_COUNT);
	return s_pszLVCellInfoString[nIndex];
}

i3ClassMeta * i3UITemplate_LVCell::GetInstanceMeta()
{
	return i3UIListView_Cell::static_meta();
}

void i3UITemplate_LVCell::InitializeLayout(void)
{
	AdjustLayout( 100.0f, 30.0f);
}

bool i3UITemplate_LVCell::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_LVCELL_SHAPE_CELL);

	*pRect = *(pShape->getRect());

	pRect->Normalize();

	return true;
}

bool i3UITemplate_LVCell::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pRect = _getTargetRect( I3UI_LVCELL_SHAPE_CELL, pRects);

	pRect->left		= 0.0f;
	pRect->top		= 0.0f;
	pRect->right	= cx;
	pRect->bottom	= cy;

	return true;
}

void i3UITemplate_LVCell::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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


bool i3UITemplate_LVCell::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_LVCELL_SHAPE_CELL];
	}
	else
	{
		pRect = getShape( I3UI_LVCELL_SHAPE_CELL)->getRect();
	}

	*pOut = *pRect;
		
	return true;
}