#include "i3FrameworkPCH.h"
#include "i3UITemplate_ScrollBar.h"
#include "i3UIScrollBar.h"

//I3_ABSTRACT_CLASS_INSTANCE( i3UITemplate_ScrollBar, i3UITemplate);
I3_CLASS_INSTANCE( i3UITemplate_ScrollBar);

const char *	i3UITemplate_ScrollBar::getShapeInfoString(INT32 nIndex)
{
	const char *	s_Template_ScrollBar_Info[] = 
	{
		"바탕에 깔리는 이미지",

		"이전 버튼의 Normal 상태",
		"이전 버튼의 Push 상태",
		"이전 버튼에 Mouse Cursor가 들어간 상태",
		"이전 버튼의 Disable 상태",

		"다음 버튼의 Normal 상태",
		"다음 버튼의 Push 상태",
		"다음 버튼에 Mouse Cursor가 들어간 상태",
		"다음 버튼의 Disable 상태",

		"Track 버튼의 Normal 상태",
		"Track 버튼의 Push 상태",
		"Track 버튼에 Mouse Cursor가 들어간 상태",
		"Track 버튼의 Disable 상태",
	};

	I3_BOUNDCHK( nIndex, getShapeCount());

	return s_Template_ScrollBar_Info[ nIndex];
}

i3UITemplate_ScrollBar::i3UITemplate_ScrollBar(void)
{	
	m_pButtons[I3UI_SCROLLBAR_BTN_PREV] = i3UITemplate_Button::new_object();
	m_pButtons[I3UI_SCROLLBAR_BTN_PREV]->SetName("Prev");

	m_pButtons[I3UI_SCROLLBAR_BTN_NEXT] = i3UITemplate_Button::new_object();
	m_pButtons[I3UI_SCROLLBAR_BTN_NEXT]->SetName("Next");

	m_pButtons[I3UI_SCROLLBAR_BTN_TRACK] = i3UITemplate_Button::new_object();
	m_pButtons[I3UI_SCROLLBAR_BTN_TRACK]->SetName("Track");
}

i3UITemplate_ScrollBar::~i3UITemplate_ScrollBar(void)
{
	INT32 i;
	for( i = 0;i < getSubTemplateCount(); ++i)
	{
		I3_SAFE_RELEASE( m_pButtons[i]);
	}
}

I3UI_CATEGORY	i3UITemplate_ScrollBar::getTemplateCategory(void)
{
	return I3UI_CATEGORY_SCROLL;
}

INT32			i3UITemplate_ScrollBar::getShapeCount(void) const
{
	return I3UI_SCROLLBAR_SHAPE_COUNT;
}

i3UIShape*		i3UITemplate_ScrollBar::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return &m_Shapes[nIndex];
}

INT32			i3UITemplate_ScrollBar::getSubTemplateCount(void)
{
	return I3UI_SCROLLBAR_BTN_COUNT;
}

i3UITemplate *	i3UITemplate_ScrollBar::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_SCROLLBAR_BTN_COUNT);

	return (i3UITemplate *) m_pButtons[ idx];
}

void			i3UITemplate_ScrollBar::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, I3UI_SCROLLBAR_BTN_COUNT);

	#if defined( I3_DEBUG)
	if( pTemplate != nullptr)
	{
		I3ASSERT( i3::kind_of<i3UITemplate_Button*>(pTemplate));
	}
	#endif

	I3_REF_CHANGE( m_pButtons[idx], (i3UITemplate_Button *) pTemplate);
}

bool i3UITemplate_ScrollBar::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( 0);

	*pRect = *pShape->getRect();

	pRect->Normalize();

	return true;
}

i3ClassMeta * i3UITemplate_ScrollBar::GetInstanceMeta(void)
{
	return i3UIScrollBar::static_meta();
}

bool i3UITemplate_ScrollBar::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pRect = _getTargetRect( 0, pRects);

	pRect->left		= 0.0f;
	pRect->top		= 0.0f;
	pRect->right	= cx;
	pRect->bottom	= cy;

	return true;
}

void i3UITemplate_ScrollBar::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{

}
