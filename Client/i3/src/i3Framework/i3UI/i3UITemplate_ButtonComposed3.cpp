#include "i3FrameworkPCH.h"
#include "i3UITemplate_ButtonComposed3.h"
#include "i3UIButtonComposed3.h"

static const char * s_pszButtonInfoString[I3UI_BUTTON_COMP3_SHAPE_COUNT] = 
{
	"0: 선택되지 않은 상태(왼쪽)",
	"1: 선택되지 않은 상태(가운데)",
	"2: 선택되지 않은 상태(오른쪽)",

	"3: 선택된 상태(왼쪽)",
	"4: 선택된 상태(가운데)",
	"5: 선택된 상태(오른쪽)",
	
	"6: OnMouse 상태(왼쪽)",
	"7: OnMouse 상태(가운데)",
	"8: OnMouse 상태(오른쪽)",

	"9: Disable 상태(왼쪽)",
	"10: Disable 상태(가운데)",
	"11: Disable 상태(오른쪽)",
};

I3_CLASS_INSTANCE( i3UITemplate_ButtonComposed3);


const char * i3UITemplate_ButtonComposed3::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_BUTTON_COMP3_SHAPE_COUNT);

	return s_pszButtonInfoString[nIndex];
}

I3UI_CATEGORY i3UITemplate_ButtonComposed3::getTemplateCategory(void)
{
	return I3UI_CATEGORY_BUTTON;
}

i3ClassMeta * i3UITemplate_ButtonComposed3::GetInstanceMeta()
{
	return i3UIButtonComposed3::static_meta();
}

bool i3UITemplate_ButtonComposed3::GetControlRect( i3::pack::RECT * pRect)
{
	if( getShapeCount() <= 0)
		return false;

	INT32 i;
	for( i = 0;i < getShapeCount(); ++i)
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

void i3UITemplate_ButtonComposed3::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 50.0f);
}

bool i3UITemplate_ButtonComposed3::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT* pLeft		= _getTargetRect( 0, pRects);
	i3::pack::RECT* pCenter		= _getTargetRect( 1, pRects);
	i3::pack::RECT* pRight		= _getTargetRect( 2, pRects);

	pLeft->bottom = cy;
	REAL32 w = pLeft->right;
	
	pRight->top = pLeft->top;
	pRight->left = cx - pRight->right;
	pRight->bottom = cy;

	pCenter->top = pLeft->top;
	pCenter->left = pLeft->left + w;
	pCenter->right = cx - pRight->right - w;
	pCenter->bottom = pLeft->bottom;

	INT32 i;
	for( i = 1; i < 4; ++i)
	{
		i3::pack::RECT* pLefti		= _getTargetRect( i* 3, pRects);
		i3::pack::RECT* pCenteri	= _getTargetRect( i* 3 + 1, pRects);
		i3::pack::RECT* pRighti		= _getTargetRect( i* 3 + 2, pRects);

		*pLefti		= *pLeft;
		*pCenteri	= *pCenter;
		*pRighti	= *pRight;
	}

	return true;
}

void i3UITemplate_ButtonComposed3::SpreadLayout( REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
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

bool i3UITemplate_ButtonComposed3::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		INT32 i;
		for( i = 0;i < getShapeCount(); ++i)
		{
			pRect = &pIn[i];
			if( i == 0)
			{
				*pOut = *pRect;
				pOut->right += pRect->left;
				pOut->bottom += pRect->top;
			}
			else
			{
				pOut->left		= i3Math::Min( pOut->left, pRect->left);
				pOut->top		= i3Math::Min( pOut->top, pRect->top);
				pOut->right	= i3Math::Max( pOut->right, pRect->left + pRect->right);
				pOut->bottom	= i3Math::Max( pOut->bottom, pRect->top + pRect->bottom);
			}
		}
	}
	else
	{
		GetControlRect(pOut);
	}

	return true;
}

bool i3UITemplate_ButtonComposed3::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRet = i3UITemplate::OnLoadXML( pFile, pXML);
	return bRet;
}