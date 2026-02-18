#include "i3FrameworkPCH.h"
#include "i3UITemplate_LVBox.h"
#include "i3UIListView_Box.h"

#include "i3UITemplate_VScrollBar.h"
#include "i3UITemplate_LVHeader.h"

static const char * s_pszLVBoxInfoString[I3UI_LVBOX_SHAPE_COUNT] = 
{
	"Left Top",	"Top", "Right Top",
	"Left",	"Center", "Right",
	"Left Bottom",	"Bottom", "Right Bottom"
};

I3_CLASS_INSTANCE( i3UITemplate_LVBox);

i3UITemplate_LVBox::i3UITemplate_LVBox()
{
	m_pTScrollBar = i3UITemplate_VScrollBar::new_object();
}

i3UITemplate_LVBox::~i3UITemplate_LVBox()
{
	I3_SAFE_RELEASE( m_pTScrollBar);
}

INT32 i3UITemplate_LVBox::getSubTemplateCount(void)
{
	return I3UI_LVBOX_SUBTEMPLATE_COUNT;
}

i3UITemplate* i3UITemplate_LVBox::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_LVBOX_SUBTEMPLATE_COUNT);

	if( idx == I3UI_LVBOX_SUBTEMPLATE_VSCROLL)		return m_pTScrollBar;
	//else if( idx == I3UI_LVBOX_SUBTEMPLATE_HEADER)	return m_pTHeader;

	return nullptr;
}

const char * i3UITemplate_LVBox::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_LVBOX_SUBTEMPLATE_COUNT);

	if( idx == I3UI_LVBOX_SUBTEMPLATE_VSCROLL)		return "Scroll Bar";
	//else if( idx == I3UI_LVBOX_SUBTEMPLATE_HEADER)	return "Header";

	return "";
}

void i3UITemplate_LVBox::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, I3UI_LVBOX_SUBTEMPLATE_COUNT);

	if( pTemplate != nullptr)
	{
		switch( idx) 
		{
		case I3UI_LVBOX_SUBTEMPLATE_VSCROLL:
			I3ASSERT( i3::same_of<i3UITemplate_VScrollBar*>(pTemplate));
			I3_REF_CHANGE( m_pTScrollBar, (i3UITemplate_VScrollBar*) pTemplate);
			break;
		default:
			break;
		}
	}
}

const char * i3UITemplate_LVBox::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_LVBOX_SHAPE_COUNT);	
	return s_pszLVBoxInfoString[nIndex];
}

i3ClassMeta	* i3UITemplate_LVBox::GetInstanceMeta()
{
	return i3UIListView_Box::static_meta();
}

bool i3UITemplate_LVBox::GetControlRect( i3::pack::RECT * pRect)
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

void i3UITemplate_LVBox::InitializeLayout(void)
{
	AdjustLayout( 300.0f, 300.0f);
}

bool i3UITemplate_LVBox::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pTopLeft, *pTop, *pTopRight;
	i3::pack::RECT * pLeft, *pCenter, *pRight;
	i3::pack::RECT * pBottomLeft, *pBottom, *pBottomRight;

	pTopLeft		= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_LEFTTOP, pRects);
	pTop			= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_TOP, pRects);
	pTopRight		= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_RIGHTTOP, pRects);

	pLeft			= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_LEFT, pRects);
	pCenter			= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_CENTER, pRects);
	pRight			= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_RIGHT, pRects);

	pBottomLeft		= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_LEFTBOTTOM, pRects);
	pBottom			= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_BOTTOM, pRects);
	pBottomRight	= _getTargetRect( (INT32)I3UI_LVBOX_SHAPE_RIGHTBOTTOM, pRects);


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

	//위치
	pTopRight->left = cx - pTopRight->right;
	pRight->left = pTopRight->left;
	pBottomRight->left = pTopRight->left;

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

	return true;
}

void i3UITemplate_LVBox::SpreadLayout( REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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

bool i3UITemplate_LVBox::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
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