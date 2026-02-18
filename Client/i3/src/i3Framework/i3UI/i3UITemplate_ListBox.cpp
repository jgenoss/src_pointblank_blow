#include "i3FrameworkPCH.h"
#include "i3UITemplate_ListBox.h"
#include "i3UIListBox.h"

const char * s_pszListBoxInfoString[I3UI_LISTBOX_SHAPE_COUNT] = 
{
	"왼쪽 상단 테두리 (크기 변화 없음)",
	"상단 테두리 (수평 크기 변화)",
	"오른쪽 상단 테두리 (크기 변화 없음)",

	"왼쪽 테두리 (수직 크기 변화)",
	"바탕 이미지 (크기 변화)",
	"오른쪽 테두리 (수직 크기 변화",

	"왼쪽 하단 테두리 (크기 변화 없음)",
	"하단 테두리 (수평 크기 변화)",
	"오른쪽 하단 테두리 (크기 변화 없음)",

	"사용 불가 상태의 바탕 이미지 (크기 변화)",

	"마우스가 올라와 있는 스트링 표시",
	"선택된 스트링 표시",
};


I3_CLASS_INSTANCE( i3UITemplate_ListBox);

i3UITemplate_ListBox::i3UITemplate_ListBox()
{
	m_pTScrollBar = i3UITemplate_VScrollBar::new_object();
}

i3UITemplate_ListBox::~i3UITemplate_ListBox()
{
	I3_SAFE_RELEASE( m_pTScrollBar);
}

i3UIShape* i3UITemplate_ListBox::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return & m_pShape[nIndex];
}


const char * i3UITemplate_ListBox::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return s_pszListBoxInfoString[nIndex];
}

INT32 i3UITemplate_ListBox::getSubTemplateCount(void)
{
	return I3UI_LISTBOX_SUBTEMPLATE_COUNT;
}

i3UITemplate* i3UITemplate_ListBox::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	return m_pTScrollBar;
}

const char * i3UITemplate_ListBox::getSubTemplateComment( INT32 idx)
{
	return "Scroll Bar";
}

void i3UITemplate_ListBox::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	if( pTemplate != nullptr)
	{
		I3ASSERT( i3::kind_of<i3UITemplate_VScrollBar*>(pTemplate));
	}

	I3_REF_CHANGE( m_pTScrollBar, (i3UITemplate_VScrollBar*) pTemplate);
}

const char * i3UITemplate_ListBox::getComment(void)
{
	return "List Box";
}

const char * i3UITemplate_ListBox::getTemplateName(void)
{
	return "List Box";
}

I3UI_CATEGORY i3UITemplate_ListBox::getTemplateCategory(void)
{
	return I3UI_CATEGORY_LISTBOX;
}

i3ClassMeta * i3UITemplate_ListBox::GetInstanceMeta()
{
	return i3UIListBox::static_meta();
}

void i3UITemplate_ListBox::InitializeLayout(void)
{
	AdjustLayout( 300.0f, 300.0f);
}

bool i3UITemplate_ListBox::GetControlRect( i3::pack::RECT * pRect)
{
	if( getShapeCount() <= 0)
		return false;

	INT32 i;
	for( i = 0;i < getShapeCount() - 2; ++i)
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

bool i3UITemplate_ListBox::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pTopLeft, *pTop, *pTopRight;
	i3::pack::RECT * pLeft, *pCenter, *pRight;
	i3::pack::RECT * pBottomLeft, *pBottom, *pBottomRight;
	i3::pack::RECT * pDisableCenter;

	pTopLeft		= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_TOPLEFT, pRects);
	pTop			= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_TOP, pRects);
	pTopRight		= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_TOPRIGHT, pRects);
	pLeft			= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_LEFT, pRects);
	pCenter			= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_CLIENTAREA, pRects);
	pRight			= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_RIGHT, pRects);

	pBottomLeft		= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_BOTTOMLEFT, pRects);
	pBottom			= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_BOTTOM, pRects);
	pBottomRight	= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_BOTTOMRIGHT, pRects);

	pDisableCenter	= _getTargetRect( (INT32)I3UI_LISTBOX_SHAPE_DISABLE_CLIENTAREA, pRects);

	//fixed width
	pLeft->right = pTopLeft->right;
	pBottomLeft->right = pTopLeft->right;

	pTopRight->right = pTopLeft->right;
	pRight->right = pTopLeft->right;
	pBottomRight->right = pTopLeft->right;

	//fixed height
	pTop->bottom = pTopLeft->bottom;
	pTopRight->bottom = pTopLeft->bottom;

	pBottomLeft->bottom = pTopLeft->bottom;
	pBottom->bottom = pTopLeft->bottom;
	pBottomRight->bottom = pTopLeft->bottom;

	//위치 x
	pTopRight->left = cx - pTopRight->right;
	pRight->left = pTopRight->left;
	pBottomRight->left = pTopRight->left;

	//위치 y
	pBottomLeft->top = cy - pBottomLeft->bottom;
	pBottom->top = pBottomLeft->top;
	pBottomRight->top = pBottomLeft->top;

	// Top (수평 크기만 조정)
	pTop->left = pTopLeft->left + pTopLeft->right;
	pTop->right = pTopRight->left - pTop->left;

	// Bottom
	pBottom->left = pBottomLeft->left + pBottomLeft->right;
	pBottom->right = pBottomRight->left - pBottom->left;

	// Left (수직 크기만 조정)
	pLeft->top = pTopLeft->top + pTopLeft->bottom;
	pLeft->bottom = pBottomLeft->top - pLeft->top;

	// Right (수직 크기만 조정)
	pRight->top = pTopRight->top + pTopRight->bottom;
	pRight->bottom = pBottomRight->top - pRight->top;

	// center
	pCenter->left = pTop->left;
	pCenter->top = pLeft->top;
	pCenter->right = pTop->right;
	pCenter->bottom = pLeft->bottom;

	*pDisableCenter = *pCenter;

	
	return true;
}

void i3UITemplate_ListBox::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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

bool i3UITemplate_ListBox::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_LISTBOX_SHAPE_CLIENTAREA];
	}
	else
	{
		pRect = getShape( I3UI_LISTBOX_SHAPE_CLIENTAREA)->getRect();
	}

	*pOut = *pRect;
			
	return true;
}