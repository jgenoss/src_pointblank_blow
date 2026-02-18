#include "i3FrameworkPCH.h"
#include "i3UITemplate_ToolTip.h"
#include "I3UIToolTip.h"

static const char* s_pszToolTipInfoString[ I3UI_TOOLTIP_SHAPE_COUNT] = 
{
	"테두리: 왼쪽 위",			
	"테두리: 위",				
	"테두리: 오른쪽 위",		

	"테두리: 왼쪽",			
	"바탕",						
	"테두리: 오른쪽",			

	"테두리: 왼쪽 아래",		
	"테두리: 아래",			
	"테두리: 오른쪽 아래",	

	//"꼬리",						
};


I3_CLASS_INSTANCE( i3UITemplate_ToolTip);

i3UIShape* i3UITemplate_ToolTip::getShape(INT32 nIndex)	
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return &m_pShape[nIndex];
}

const char * i3UITemplate_ToolTip::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return s_pszToolTipInfoString[nIndex];
}

const char * i3UITemplate_ToolTip::getComment(void)
{
	return "Tool Tip(Comment)";
}

const char * i3UITemplate_ToolTip::getTemplateName(void)
{
	return "Tool Tip";
}

I3UI_CATEGORY  i3UITemplate_ToolTip::getTemplateCategory(void)
{
	return I3UI_CATEGORY_TOOLTIP;	// ???
}

bool i3UITemplate_ToolTip::GetControlRect( i3::pack::RECT * pRect)
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

void i3UITemplate_ToolTip::InitializeLayout(void)
{
	AdjustLayout( 300.0f, 200.0f);
}

bool i3UITemplate_ToolTip::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	//_SetTailPos( cx, cy);
	
	i3::pack::RECT* pTL = _getTargetRect( I3UI_TOOLTIP_SHAPE_TOPLEFT, pRects);
	i3::pack::RECT* pT = _getTargetRect( I3UI_TOOLTIP_SHAPE_TOP, pRects);
	i3::pack::RECT* pTR = _getTargetRect( I3UI_TOOLTIP_SHAPE_TOPRIGHT, pRects);

	i3::pack::RECT* pL = _getTargetRect( I3UI_TOOLTIP_SHAPE_LEFT, pRects);
	i3::pack::RECT* pC = _getTargetRect( I3UI_TOOLTIP_SHAPE_CENTER, pRects);
	i3::pack::RECT* pR = _getTargetRect( I3UI_TOOLTIP_SHAPE_RIGHT, pRects);
	
	i3::pack::RECT* pBL = _getTargetRect( I3UI_TOOLTIP_SHAPE_BOTTOMLEFT, pRects);
	i3::pack::RECT* pB = _getTargetRect( I3UI_TOOLTIP_SHAPE_BOTTOM, pRects);
	i3::pack::RECT* pBR = _getTargetRect( I3UI_TOOLTIP_SHAPE_BOTTOMRIGHT, pRects);
	
	//i3::pack::RECT* pTail = _getTargetRect( I3UI_TOOLTIP_SHAPE_TAIL, pRects);	// 꼬리

	REAL32 lw = pTL->right;		//테두리의 가로 두께
	REAL32 lh = pTL->bottom;	//테두리의 세로 두께

	if( cx <= lw * 2 )
		cx = lw * 2;

	if( cy <= lh * 2)
		cy = lh * 2;

	//REAL32 defWTail = 0.2f * cx;
	//REAL32 defHTail = 0.2f * cy;

	//크기(right == width, bottom == height)
	pL->right = pBL->right = lw;						//왼쪽 모서리 두께
	pT->right = pC->right = pB->right = cx - 2 * lw;	//가로 폭
	pTR->right = pR->right = pBR->right = lw;			//오른쪽 모서리 두게

	pT->bottom = pTR->bottom = lh;						//위쪽 모서리 두께
	pL->bottom = pC->bottom = pR->bottom = cy - 2 * lh;	//세로 폭
	pBL->bottom = pB->bottom = pBR->bottom = lh;	//아래쪽 모서리 두께
	

	//위치(left, top)
	pL->left = pBL->left = pTL->left;
	pT->left = pC->left = pB->left = pTL->left + lw;
	pTR->left = pR->left = pBR->left = pTL->left + cx - lw;
	
	pT->top = pTR->top = pTL->top;
	pL->top = pC->top = pR->top = pTL->top + lh;
	pBL->top = pB->top = pBR->top = pTL->top + cy - lh;
	
	//꼬리
	//pTail->left = pB->left;
	//pTail->top = pB->top;
	//pTail->right = defWTail;	//temp
	//pTail->bottom = defHTail;	//temp

	//pB->left = pTail->left + pTail->right;
	//pB->right = cx - 2 * lw - pTail->right;
	
	return true;
}

//void i3UITemplate_ToolTip::_SetTailPos( REAL32 cx, REAL32 cy )
//{
//	REAL32 sx = cx * m_fTailPosX;
//	REAL32 sy = cy * m_fTailPosY;
//	
//	i3::pack::RECT* pTail = _getTargetRect( I3UI_TOOLTIP_SHAPE_TAIL, pRects);	// 꼬리
//
//	pTail->right = sx;
//	pTail->bottom = sy;
//	
//}

i3ClassMeta	* i3UITemplate_ToolTip::GetInstanceMeta()
{
	return i3UIToolTip::static_meta();
}

void i3UITemplate_ToolTip::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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

bool i3UITemplate_ToolTip::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_TOOLTIP_SHAPE_CENTER];
	}
	else
	{
		pRect = getShape( I3UI_TOOLTIP_SHAPE_CENTER)->getRect();
	}

	*pOut = *pRect;
			
	return true;
}
	