#include "i3FrameworkPCH.h"
#include "i3UITemplate_ComboBox_NoEditBox.h"
#include "i3UITemplate_ButtonComposed3.h"
#include "i3UITemplate_ListBox.h"
#include "i3UIComboBox_NoEditBox.h"

const char * s_pszComboBoxNoEditBoxInfoString[I3UI_COMBOBOX_SHAPE_COUNT] = 
{
	"배경( 투명한걸로..?)",
};

I3_CLASS_INSTANCE( i3UITemplate_ComboBox_NoEditBox);

i3UITemplate_ComboBox_NoEditBox::i3UITemplate_ComboBox_NoEditBox()
{
	m_pTButton = i3UITemplate_ButtonComposed3::new_object();
	m_pTListBox = i3UITemplate_ListBox::new_object();
}

i3UITemplate_ComboBox_NoEditBox::~i3UITemplate_ComboBox_NoEditBox()
{
	I3_SAFE_RELEASE( m_pTButton);
	I3_SAFE_RELEASE( m_pTListBox);
}

i3UIShape* i3UITemplate_ComboBox_NoEditBox::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return & m_pShape[nIndex];
}

const char * i3UITemplate_ComboBox_NoEditBox::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return s_pszComboBoxNoEditBoxInfoString[nIndex];
}

INT32 i3UITemplate_ComboBox_NoEditBox::getSubTemplateCount(void)
{
	return I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_COUNT;
}

i3UITemplate* i3UITemplate_ComboBox_NoEditBox::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	i3UITemplate* pTemplate = nullptr;

	switch( idx) 
	{
	case I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_BUTTON:		pTemplate = m_pTButton;		break;
	case I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_LISTBOX:		pTemplate = m_pTListBox;	break;
	default:	break;
	}

	return pTemplate;
}

const char * i3UITemplate_ComboBox_NoEditBox::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	switch( idx) 
	{
	case I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_BUTTON:	return "Button";
	case I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_LISTBOX:	return "List Box";
	default:									return "";
	}
}

void i3UITemplate_ComboBox_NoEditBox::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	if( pTemplate != nullptr)
	{
		switch( idx) 
		{
		case I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_BUTTON:		
			I3ASSERT( i3::kind_of<i3UITemplate_ButtonComposed3* >(pTemplate));
			I3_REF_CHANGE( m_pTButton, (i3UITemplate_ButtonComposed3*) pTemplate);
			break;
		case I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_LISTBOX:	
			I3ASSERT( i3::kind_of<i3UITemplate_ListBox* >(pTemplate));
			I3_REF_CHANGE( m_pTListBox, (i3UITemplate_ListBox*) pTemplate);
			break;
		default:
			break;
		}
	}
}

I3UI_CATEGORY i3UITemplate_ComboBox_NoEditBox::getTemplateCategory(void)
{
	return I3UI_CATEGORY_COMBO_NOEDITBOX;
}

i3ClassMeta * i3UITemplate_ComboBox_NoEditBox::GetInstanceMeta()
{
	return i3UIComboBox_NoEditBox::static_meta();
}

void i3UITemplate_ComboBox_NoEditBox::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 300.0f);
}

bool i3UITemplate_ComboBox_NoEditBox::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_COMBOBOX_SHAPE_BKGND);

	*pRect = *(pShape->getRect());

	pRect->Normalize();

	return true;
}

bool i3UITemplate_ComboBox_NoEditBox::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT* pRect = _getTargetRect( I3UI_COMBOBOX_SHAPE_BKGND, pRects);

	pRect->left = 0.0f;
	pRect->top = 0.0f;
	pRect->right = cx;
	pRect->bottom = cy;

	return true;
}

void i3UITemplate_ComboBox_NoEditBox::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
}

