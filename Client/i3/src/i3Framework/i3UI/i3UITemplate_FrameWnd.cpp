#include "i3FrameworkPCH.h"
#include "i3UITemplate_FrameWnd.h"
#include "i3UIFrameWnd.h"

//I3_CLASS_INSTANCE( i3UITemplate_FrameWnd, i3UITemplate);
I3_CLASS_INSTANCE( i3UITemplate_FrameWnd);

i3UITemplate_FrameWnd::i3UITemplate_FrameWnd(void)
{
	m_pTVScrollBar = i3UITemplate_VScrollBar::new_object();
	m_pTVScrollBar->SetName( "VScrollBar");

	m_pTHScrollBar = i3UITemplate_HScrollBar::new_object();
	m_pTHScrollBar->SetName( "HScrollBar");
}

i3UITemplate_FrameWnd::~i3UITemplate_FrameWnd(void)
{
	I3_SAFE_RELEASE( m_pTVScrollBar);
	I3_SAFE_RELEASE( m_pTHScrollBar);
}

INT32			i3UITemplate_FrameWnd::getShapeCount(void) const
{
	return I3UI_FRAMEWND_SHAPE_COUNT;
}

i3UIShape*		i3UITemplate_FrameWnd::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_FRAMEWND_SHAPE_COUNT);

	return & m_Shapes[ nIndex];
}

const char *	i3UITemplate_FrameWnd::getShapeInfoString(INT32 nIndex)
{
	const char *	s_Template_FrameWnd_Info[] = 
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
	};

	I3_BOUNDCHK( nIndex, I3UI_FRAMEWND_SHAPE_COUNT);

	return s_Template_FrameWnd_Info[ nIndex];
}

INT32 i3UITemplate_FrameWnd::getSubTemplateCount(void)
{
	return I3UI_FRAMEWND_SUBTEMPLATE_COUNT;
}

i3UITemplate* i3UITemplate_FrameWnd::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	i3UITemplate* pTemplate = nullptr;

	switch( idx) 
	{
	case I3UI_FRAMEWND_SUBTEMPLATE_VSCROLL:		pTemplate = m_pTVScrollBar;		break;
	case I3UI_FRAMEWND_SUBTEMPLATE_HSCROLL:		pTemplate = m_pTHScrollBar;		break;
	default:	break;
	}

	return pTemplate;
}

const char * i3UITemplate_FrameWnd::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	switch( idx) 
	{
	case I3UI_FRAMEWND_SUBTEMPLATE_VSCROLL:		return "Vertical Scroll Bar";
	case I3UI_FRAMEWND_SUBTEMPLATE_HSCROLL:		return "Horizontal Scroll Bar";
	default:									return "";
	}
}

void i3UITemplate_FrameWnd::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	if( pTemplate != nullptr)
	{
		switch( idx) 
		{
		case I3UI_FRAMEWND_SUBTEMPLATE_VSCROLL:		
			I3ASSERT( i3::kind_of<i3UITemplate_VScrollBar* >(pTemplate));
			I3_REF_CHANGE( m_pTVScrollBar, (i3UITemplate_VScrollBar*) pTemplate);
			break;
		case I3UI_FRAMEWND_SUBTEMPLATE_HSCROLL:		
			I3ASSERT( i3::kind_of<i3UITemplate_HScrollBar*>(pTemplate));
			I3_REF_CHANGE( m_pTHScrollBar, (i3UITemplate_HScrollBar*) pTemplate);
			break;
		default:
			break;
		}
	}
}

I3UI_CATEGORY i3UITemplate_FrameWnd::getTemplateCategory(void)
{
	return I3UI_CATEGORY_FRAMEWND;
}

i3ClassMeta * i3UITemplate_FrameWnd::GetInstanceMeta()
{
	return i3UIFrameWnd::static_meta();
}

void i3UITemplate_FrameWnd::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 50.0f);
}

bool i3UITemplate_FrameWnd::GetControlRect( i3::pack::RECT * pRect)
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

bool i3UITemplate_FrameWnd::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pTopLeft, *pTop, *pTopRight;
	i3::pack::RECT * pLeft, *pCenter, *pRight;
	i3::pack::RECT * pBottomLeft, *pBottom, *pBottomRight;
	i3::pack::RECT * pDisableCenter;

	pTopLeft		= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_TOPLEFT, pRects);
	pTop			= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_TOP, pRects);
	pTopRight		= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_TOPRIGHT, pRects);
	pLeft			= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_LEFT, pRects);
	pCenter			= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_CLIENT, pRects);
	pRight			= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_RIGHT, pRects);

	pBottomLeft		= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_BOTTOMLEFT, pRects);
	pBottom			= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_BOTTOM, pRects);
	pBottomRight	= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_BOTTOMRIGHT, pRects);

	pDisableCenter	= _getTargetRect( (INT32)I3UI_FRAMEWND_SHAPE_DISABLE_CLIENT, pRects);

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

	//
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

	/*
	i3UIShape * pTopLeft, *pTop, *pTopRight;
	i3UIShape * pLeft, *pCenter, *pRight;
	i3UIShape * pBottomLeft, *pBottom, *pBottomRight;
	i3UIShape * pDisableCenter;
	REAL32 x2, y2;

	x2 = cx;
	y2 = cy;

	pTopLeft		= getShape( I3UI_FRAMEWND_SHAPE_TOPLEFT);
	pTop			= getShape( I3UI_FRAMEWND_SHAPE_TOP);
	pTopRight		= getShape( I3UI_FRAMEWND_SHAPE_TOPRIGHT);

	pLeft			= getShape( I3UI_FRAMEWND_SHAPE_LEFT);
	pCenter			= getShape( I3UI_FRAMEWND_SHAPE_CLIENT);
	pRight			= getShape( I3UI_FRAMEWND_SHAPE_RIGHT);

	pBottomLeft		= getShape( I3UI_FRAMEWND_SHAPE_BOTTOMLEFT);
	pBottom			= getShape( I3UI_FRAMEWND_SHAPE_BOTTOM);
	pBottomRight	= getShape( I3UI_FRAMEWND_SHAPE_BOTTOMRIGHT);

	pDisableCenter	= getShape( I3UI_FRAMEWND_SHAPE_CLIENT);

	//fixed width
	pLeft->setWidth( pTopLeft->getWidth());
	pBottomLeft->setWidth( pTopLeft->getWidth());

	pTopRight->setWidth( pTopLeft->getWidth());
	pRight->setWidth( pTopLeft->getWidth());
	pBottomRight->setWidth( pTopLeft->getWidth());

	//fixed height
	pTop->setHeight( pTopLeft->getHeight());
	pTopRight->setHeight( pTopLeft->getHeight());

	pBottomLeft->setHeight( pTopLeft->getHeight());
	pBottom->setHeight( pTopLeft->getHeight());
	pBottomRight->setHeight( pTopLeft->getHeight());

	//위치
	pTopRight->setPosX(	cx - pTopRight->getWidth());
	pRight->setPosX( pTopRight->getPosX());
	pBottomRight->setPosX( pTopRight->getPosX());

	//
	pBottomLeft->setPosY( cy - pBottomLeft->getHeight());
	pBottom->setPosY( pBottomLeft->getPosY());
	pBottomRight->setPosY( pBottomLeft->getPosY());

	// Top (수평 크기만 조정)
	pTop->setPosX( pTopLeft->getPosX() + pTopLeft->getWidth());
	pTop->setWidth( pTopRight->getPosX() - pTop->getPosX());

	// Bottom
	pBottom->setPosX( pBottomLeft->getPosX() + pBottomLeft->getWidth());
	pBottom->setWidth( pBottomRight->getPosX() - pBottom->getPosX());

	// Left (수직 크기만 조정)
	pLeft->setPosY( pTopLeft->getPosY() + pTopLeft->getHeight());
	pLeft->setHeight( pBottomLeft->getPosY() - pLeft->getPosY());

	// Right (수직 크기만 조정)
	pRight->setPosY( pTopRight->getPosY() + pTopRight->getHeight());
	pRight->setHeight( pBottomRight->getPosY() - pRight->getPosY());

	// center
	pCenter->setPosX( pTop->getPosX());
	pCenter->setPosY( pLeft->getPosY());
	pCenter->setWidth( pTop->getWidth());
	pCenter->setHeight( pLeft->getHeight());

	pDisableCenter->setRect( pCenter->getRect());*/

	return true;
}

void i3UITemplate_FrameWnd::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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

bool i3UITemplate_FrameWnd::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_FRAMEWND_SHAPE_TOP];
	}
	else
	{
		pRect = getShape( I3UI_FRAMEWND_SHAPE_TOP)->getRect();
	}

	*pOut = *pRect;
		
	return true;
}