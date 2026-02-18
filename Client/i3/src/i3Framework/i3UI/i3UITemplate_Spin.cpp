#include "i3FrameworkPCH.h"
#include "i3UITemplate_Spin.h"
#include "i3UISpin.h"

//I3_CLASS_INSTANCE( i3UITemplate_Spin, i3UITemplate);
I3_CLASS_INSTANCE( i3UITemplate_Spin);

const char *	i3UITemplate_Spin::getShapeInfoString(INT32 nIndex)
{
	const char *	s_Template_Spin_Info[] = 
	{
		"¹ÙÅÁ¿¡ ±ò¸®´Â ÀÌ¹ÌÁö",	
	};

	I3_BOUNDCHK( nIndex, getShapeCount());

	return s_Template_Spin_Info[ nIndex];
}

i3UITemplate_Spin::i3UITemplate_Spin(void)
{
	m_pButtons[I3UI_SPIN_BTN_PREV] = i3UITemplate_Button::new_object();
	m_pButtons[I3UI_SPIN_BTN_PREV]->SetName("Prev");

	m_pButtons[I3UI_SPIN_BTN_NEXT] = i3UITemplate_Button::new_object();
	m_pButtons[I3UI_SPIN_BTN_NEXT]->SetName("Next");
}

i3UITemplate_Spin::~i3UITemplate_Spin(void)
{
	for( INT32 i = 0;i < getSubTemplateCount(); ++i)
	{
		I3_SAFE_RELEASE( m_pButtons[i]);
	}
}

I3UI_CATEGORY	i3UITemplate_Spin::getTemplateCategory(void)
{
	return I3UI_CATEGORY_SPIN;
}

INT32			i3UITemplate_Spin::getShapeCount(void) const
{
	return I3UI_SPIN_SHAPE_COUNT;
}

i3UIShape*		i3UITemplate_Spin::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return &m_Shapes[nIndex];
}

INT32			i3UITemplate_Spin::getSubTemplateCount(void)
{
	return I3UI_SPIN_BTN_COUNT;
}

i3UITemplate *	i3UITemplate_Spin::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	return (i3UITemplate *) m_pButtons[ idx];
}

void			i3UITemplate_Spin::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	#if defined( I3_DEBUG)
	if( pTemplate != nullptr)
	{
		I3ASSERT( i3::kind_of<i3UITemplate_Button*>(pTemplate));
	}
	#endif

	I3_REF_CHANGE( m_pButtons[idx], (i3UITemplate_Button *) pTemplate);
}

bool i3UITemplate_Spin::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_SPIN_SHAPE_BKGND);

	*pRect = *pShape->getRect();

	pRect->Normalize();

	return true;
}

i3ClassMeta * i3UITemplate_Spin::GetInstanceMeta(void)
{
	return i3UISpin::static_meta();
}

const char *  i3UITemplate_Spin::getComment(void)
{
	return "Spin(Comment)";
}

const char *  i3UITemplate_Spin::getTemplateName(void)
{
	return "Spin";
}

void i3UITemplate_Spin::InitializeLayout(void)
{
	AdjustLayout( 100.0f, 200.0f);
}

bool i3UITemplate_Spin::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pRect = _getTargetRect( 0, pRects);

	pRect->left		= 0.0f;
	pRect->top		= 0.0f;
	pRect->right	= cx;
	pRect->bottom	= cy;

	return true;
}

void i3UITemplate_Spin::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{

}
