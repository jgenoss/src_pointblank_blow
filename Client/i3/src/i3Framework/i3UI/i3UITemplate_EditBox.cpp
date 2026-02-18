#include "i3FrameworkPCH.h"
#include "i3UITemplate_EditBox.h"
#include "i3UIEditBox.h"

const char * s_pszEditBoxInfoString[I3UI_EDITBOX_SHAPE_COUNT] = 
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

	"사용 불가 상태의 바탕 이미지 (크기 변화)"
};

I3_CLASS_INSTANCE( i3UITemplate_EditBox);


i3UITemplate_EditBox::i3UITemplate_EditBox()
{
	m_pTScrollBar = i3UITemplate_VScrollBar::new_object();
}

i3UITemplate_EditBox::~i3UITemplate_EditBox()
{
	I3_SAFE_RELEASE( m_pTScrollBar);
}

const char * i3UITemplate_EditBox::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_EDITBOX_SHAPE_COUNT);

	return s_pszEditBoxInfoString[nIndex];
}

I3UI_CATEGORY i3UITemplate_EditBox::getTemplateCategory(void)
{
	return I3UI_CATEGORY_EDITBOX;
}

// ScrollBar 추가용
INT32 i3UITemplate_EditBox::getSubTemplateCount(void)
{
	return I3UI_EDITBOX_SUBTEMPLATE_COUNT;
}

i3UITemplate* i3UITemplate_EditBox::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	return m_pTScrollBar;
}

const char * i3UITemplate_EditBox::getSubTemplateComment( INT32 idx)
{
	return "Scroll Bar";
}

void i3UITemplate_EditBox::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	if( pTemplate != nullptr)
	{
		I3ASSERT( i3::kind_of<i3UITemplate_VScrollBar*>(pTemplate));
	}

	I3_REF_CHANGE( m_pTScrollBar, (i3UITemplate_VScrollBar*) pTemplate);
}
/////////

i3ClassMeta * i3UITemplate_EditBox::GetInstanceMeta()
{
	return i3UIEditBox::static_meta();
}

void i3UITemplate_EditBox::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 30.0f);
}

bool i3UITemplate_EditBox::GetControlRect( i3::pack::RECT * pRect)
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

bool i3UITemplate_EditBox::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pTopLeft, *pTop, *pTopRight;
	i3::pack::RECT * pLeft, *pCenter, *pRight;
	i3::pack::RECT * pBottomLeft, *pBottom, *pBottomRight;
	i3::pack::RECT * pDisableCenter;

	pTopLeft		= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_TOPLEFT, pRects);
	pTop			= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_TOP, pRects);
	pTopRight		= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_TOPRIGHT, pRects);
	pLeft			= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_LEFT, pRects);
	pCenter			= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_CLIENTAREA, pRects);
	pRight			= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_RIGHT, pRects);

	pBottomLeft		= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_BOTTOMLEFT, pRects);
	pBottom			= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_BOTTOM, pRects);
	pBottomRight	= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_BOTTOMRIGHT, pRects);

	pDisableCenter	= _getTargetRect( (INT32)I3UI_EDITBOX_SHAPE_DISABLE_CLIENTAREA, pRects);

	//fixed width
	pLeft->right = pTopLeft->right;
	pBottomLeft->right = pTopLeft->right;

	//pLeft->setWidth( pTopLeft->getWidth());
	//pBottomLeft->setWidth( pTopLeft->getWidth());

	pTopRight->right = pTopLeft->right;
	pRight->right = pTopLeft->right;
	pBottomRight->right = pTopLeft->right;

	//pTopRight->setWidth( pTopLeft->getWidth());
	//pRight->setWidth( pTopLeft->getWidth());
	//pBottomRight->setWidth( pTopLeft->getWidth());

	//fixed height
	pTop->bottom = pTopLeft->bottom;
	pTopRight->bottom = pTopLeft->bottom;
	//pTop->setHeight( pTopLeft->getHeight());
	//pTopRight->setHeight( pTopLeft->getHeight());

	pBottomLeft->bottom = pTopLeft->bottom;
	pBottom->bottom = pTopLeft->bottom;
	pBottomRight->bottom = pTopLeft->bottom;

	//pBottomLeft->setHeight( pTopLeft->getHeight());
	//pBottom->setHeight( pTopLeft->getHeight());
	//pBottomRight->setHeight( pTopLeft->getHeight());

	//위치
	pTopRight->left = cx - pTopRight->right;
	pRight->left = pTopRight->left;
	pBottomRight->left = pTopRight->left;

	//pTopRight->setPosX(	cx - pTopRight->getWidth());
	//pRight->setPosX( pTopRight->getPosX());
	//pBottomRight->setPosX( pTopRight->getPosX());

	//
	pBottomLeft->top = cy - pBottomLeft->bottom;
	pBottom->top = pBottomLeft->top;
	pBottomRight->top = pBottomLeft->top;
	//pBottomLeft->setPosY( cy - pBottomLeft->getHeight());
	//pBottom->setPosY( pBottomLeft->getPosY());
	//pBottomRight->setPosY( pBottomLeft->getPosY());

	// Top (수평 크기만 조정)
	pTop->left = pTopLeft->left + pTopLeft->right;
	pTop->right = pTopRight->left - pTop->left;

	//pTop->setPosX( pTopLeft->getPosX() + pTopLeft->getWidth());
	//pTop->setWidth( pTopRight->getPosX() - pTop->getPosX());

	// Bottom
	pBottom->left = pBottomLeft->left + pBottomLeft->right;
	pBottom->right = pBottomRight->left - pBottom->left;

	//pBottom->setPosX( pBottomLeft->getPosX() + pBottomLeft->getWidth());
	//pBottom->setWidth( pBottomRight->getPosX() - pBottom->getPosX());

	// Left (수직 크기만 조정)
	pLeft->top = pTopLeft->top + pTopLeft->bottom;
	pLeft->bottom = pBottomLeft->top - pLeft->top;

	//pLeft->setPosY( pTopLeft->getPosY() + pTopLeft->getHeight());
	//pLeft->setHeight( pBottomLeft->getPosY() - pLeft->getPosY());

	// Right (수직 크기만 조정)
	pRight->top = pTopRight->top + pTopRight->bottom;
	pRight->bottom = pBottomRight->top - pRight->top;

	//pRight->setPosY( pTopRight->getPosY() + pTopRight->getHeight());
	//pRight->setHeight( pBottomRight->getPosY() - pRight->getPosY());

	// center
	pCenter->left = pTop->left;
	pCenter->top = pLeft->top;
	pCenter->right = pTop->right;
	pCenter->bottom = pLeft->bottom;

	//pCenter->setPosX( pTop->getPosX());
	//pCenter->setPosY( pLeft->getPosY());
	//pCenter->setWidth( pTop->getWidth());
	//pCenter->setHeight( pLeft->getHeight());

	//pDisableCenter->setRect( pCenter->getRect());
	*pDisableCenter = *pCenter;

	return true;
}

void i3UITemplate_EditBox::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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
		
		if( i % 3 == 2)
		{
			left = 0.0f;
			top += pRect->bottom + I3UI_SPREAD_MARGIN;
		}
		
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}
}

bool i3UITemplate_EditBox::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
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
	