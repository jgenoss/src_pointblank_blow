#include "i3FrameworkPCH.h"
#include "i3UITemplate_HBar.h"
#include "I3UIHBar.h"

static const char* s_pszHBarInfoString[ I3UI_HBAR_SHAPE_COUNT] = 
{
	"왼쪽경계",
	"가운데바탕",
	"오른쪽경계",
};

I3_CLASS_INSTANCE( i3UITemplate_HBar);

i3UIShape* i3UITemplate_HBar::getShape(INT32 nIndex)	
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return &m_pShape[nIndex];
}

const char * i3UITemplate_HBar::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return s_pszHBarInfoString[nIndex];
}

I3UI_CATEGORY  i3UITemplate_HBar::getTemplateCategory(void)
{
	return I3UI_CATEGORY_HBAR;
}

bool i3UITemplate_HBar::GetControlRect( i3::pack::RECT * pRect)
{
	for( INT32 i=0; i < getShapeCount(); i++)
	{
		i3UIShape * pShape = getShape( i);
		
		if( i == 0)
		{
			*pRect = *pShape->getRect();
			pRect->right += pShape->getPosX();
			pRect->bottom += pShape->getPosY();
		}
		else
		{
			pRect->left		= i3Math::Min( pRect->left, pShape->getPosX());
			pRect->top		= i3Math::Min( pRect->top, pShape->getPosY());
			pRect->right	= i3Math::Max( pRect->right, pShape->getPosX() + pShape->getWidth());
			pRect->bottom	= i3Math::Max( pRect->bottom, pShape->getPosY() + pShape->getHeight());
		}
	}

	pRect->Normalize();

	return true;
}

void i3UITemplate_HBar::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 50.0f);
}

bool i3UITemplate_HBar::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT* pLeft		= _getTargetRect( I3UI_HBAR_SHAPE_LEFT, pRects);
	i3::pack::RECT* pCenter		= _getTargetRect( I3UI_HBAR_SHAPE_CENTER, pRects);
	i3::pack::RECT* pRight		= _getTargetRect( I3UI_HBAR_SHAPE_RIGHT, pRects);

	pLeft->bottom = cy;
	REAL32 w = pLeft->right;
	
	pRight->top = pLeft->top;
	pRight->left = cx - pRight->right;
	pRight->bottom = pLeft->bottom;

	pCenter->top = pLeft->top;
	pCenter->left = pLeft->top + w;
	pCenter->right = pRight->left - w;
	pCenter->bottom = pLeft->bottom;

	return true;
}

i3ClassMeta	* i3UITemplate_HBar::GetInstanceMeta()
{
	return i3UIHBar::static_meta();
}

void i3UITemplate_HBar::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}
}
