#include "i3FrameworkPCH.h"
#include "i3UITemplate_Tab.h"
#include "I3UITab.h"

static const char* s_pszTabInfoString[ I3UI_TAB_SHAPE_COUNT] = 
{
	"테두리: 왼쪽 위",		
	"테두리: 위",			
	"테두리: 오른쪽 위",	
	"테두리: 왼쪽",		
	"바탕",					
	"테두리: 오른쪽",		

	"테두리: 왼쪽 위(마우스온)",		
	"테두리: 위(마우스온)",			
	"테두리: 오른쪽 위(마우스온)",	
	"테두리: 왼쪽(마우스온)",		
	"바탕(마우스온)",					
	"테두리: 오른쪽(마우스온)",		

	"테두리: 왼쪽 위(선택)",		
	"테두리: 위(선택)",			
	"테두리: 오른쪽 위(선택)",	
	"테두리: 왼쪽(선택)",		
	"바탕(선택)",					
	"테두리: 오른쪽(선택)",		

	"테두리: 왼쪽 위(비활성)",		
	"테두리: 위(비활성)",			
	"테두리: 오른쪽 위(비활성)",	
	"테두리: 왼쪽(비활성)",		
	"바탕(비활성)",					
	"테두리: 오른쪽(비활성)"		
};

I3_CLASS_INSTANCE( i3UITemplate_Tab);

i3UIShape* i3UITemplate_Tab::getShape(INT32 nIndex)	
{
	I3_BOUNDCHK( nIndex, getShapeCount() );
	return &m_pShape[nIndex];
}

const char * i3UITemplate_Tab::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount() );
	return s_pszTabInfoString[nIndex];
}

const char * i3UITemplate_Tab::getComment(void)
{
	return "Tab(Comment)";
}

const char * i3UITemplate_Tab::getTemplateName(void)
{
	return "Tab";
}

I3UI_CATEGORY  i3UITemplate_Tab::getTemplateCategory(void)
{
	return I3UI_CATEGORY_TAB;
}

bool i3UITemplate_Tab::GetControlRect( i3::pack::RECT * pRect)
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

void i3UITemplate_Tab::InitializeLayout(void)
{
	AdjustLayout( 100.0f, 70.0f);
}

bool i3UITemplate_Tab::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	INT32 loop = I3UI_TAB_SHAPE_COUNT / I3UI_TAB_SHAPE_COUNT_PER_SET;

	//기준
	i3::pack::RECT* pTopLeft = _getTargetRect( I3UI_TAB_SHAPE_TOPLEFT, pRects);

	for( INT32 i=0; i<loop; i++)
	{
		INT32 offset = I3UI_TAB_SHAPE_COUNT_PER_SET * i;

		i3::pack::RECT* pTL = _getTargetRect( I3UI_TAB_SHAPE_TOPLEFT + offset, pRects);
		i3::pack::RECT* pT = _getTargetRect( I3UI_TAB_SHAPE_TOP + offset, pRects);
		i3::pack::RECT* pTR = _getTargetRect( I3UI_TAB_SHAPE_TOPRIGHT + offset, pRects);
		i3::pack::RECT* pL = _getTargetRect( I3UI_TAB_SHAPE_LEFT + offset, pRects);
		i3::pack::RECT* pC = _getTargetRect( I3UI_TAB_SHAPE_CENTER + offset, pRects);
		i3::pack::RECT* pR = _getTargetRect( I3UI_TAB_SHAPE_RIGHT + offset, pRects);
		
		//height
		pTL->bottom = pT->bottom = pTR->bottom = pTopLeft->bottom;
		pL->bottom = pC->bottom = pR->bottom = cy - pTopLeft->bottom;

		//width
		pTL->right = pL->right = pR->right = pTR->right = pTopLeft->right;
		pT->right = pC->right = cx - pTopLeft->right * 2;

		//top
		pTL->top = pT->top = pTR->top = pTopLeft->top;
		pL->top = pC->top = pR->top = pTopLeft->top + pTopLeft->bottom;

		//left
		pTL->left = pL->left = pTopLeft->left;
		pT->left = pC->left = pTopLeft->left + pTopLeft->right;
		pTR->left = pR->left = pT->left + pT->right;

	}

	return true;
}

i3ClassMeta	* i3UITemplate_Tab::GetInstanceMeta()
{
	return i3UITab::static_meta();
}

void i3UITemplate_Tab::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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


bool i3UITemplate_Tab::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_TAB_SHAPE_CENTER];
	}
	else
	{
		pRect = getShape( I3UI_TAB_SHAPE_CENTER)->getRect();
	}

	*pOut = *pRect;
			
	return true;
}
	