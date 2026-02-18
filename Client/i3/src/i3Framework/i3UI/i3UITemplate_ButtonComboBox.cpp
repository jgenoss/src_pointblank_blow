#include "i3FrameworkPCH.h"
#include "i3UITemplate_ButtonComboBox.h"
#include "i3UIButtonComboBox.h"

const char * s_pszBtnComboBoxInfoString[I3UI_COMBOBOX_SHAPE_COUNT] = 
{
	"배경( 투명한걸로..?)",
};

I3_CLASS_INSTANCE( i3UITemplate_ButtonComboBox);

i3UITemplate_ButtonComboBox::~i3UITemplate_ButtonComboBox()
{
	I3_SAFE_RELEASE( m_pTEditBox);
	I3_SAFE_RELEASE( m_pTButton);
	I3_SAFE_RELEASE( m_pTListBox);
}

i3UIShape* i3UITemplate_ButtonComboBox::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return & m_pShape[nIndex];
}

const char * i3UITemplate_ButtonComboBox::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return s_pszBtnComboBoxInfoString[nIndex];
}

INT32 i3UITemplate_ButtonComboBox::getSubTemplateCount(void)
{
	return I3UI_COMBOBOX_SUBTEMPLATE_COUNT;
}

i3UITemplate* i3UITemplate_ButtonComboBox::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	i3UITemplate* pTemplate = nullptr;

	switch( idx) 
	{
	case I3UI_COMBOBOX_SUBTEMPLATE_EDITBOX:		pTemplate = m_pTEditBox;	break;
	case I3UI_COMBOBOX_SUBTEMPLATE_BUTTON:		pTemplate = m_pTButton;		break;
	case I3UI_COMBOBOX_SUBTEMPLATE_LISTBOX:		pTemplate = m_pTListBox;	break;
	default:	break;
	}

	return pTemplate;
}

const char * i3UITemplate_ButtonComboBox::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	switch( idx) 
	{
	case I3UI_COMBOBOX_SUBTEMPLATE_EDITBOX:		return "Button";
	case I3UI_COMBOBOX_SUBTEMPLATE_BUTTON:		return "Button";
	case I3UI_COMBOBOX_SUBTEMPLATE_LISTBOX:		return "List Box";
	default:									return "";
	}
}

void i3UITemplate_ButtonComboBox::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	if( pTemplate != nullptr)
	{
		switch( idx) 
		{
		case I3UI_COMBOBOX_SUBTEMPLATE_EDITBOX:		
			
			if( i3::same_of<i3UITemplate_ButtonComposed3* >(pTemplate) == false)
			{
				//I3_SAFE_RELEASE( pTemplate);
				I3_REF_CHANGE( m_pTEditBox, i3UITemplate_ButtonComposed3::new_object());
			}
			else
			{
				I3ASSERT( i3::same_of<i3UITemplate_ButtonComposed3* >(pTemplate));
				I3_REF_CHANGE( m_pTEditBox, (i3UITemplate_ButtonComposed3*) pTemplate);
			}
			break;
		case I3UI_COMBOBOX_SUBTEMPLATE_BUTTON:		
			I3ASSERT( i3::same_of<i3UITemplate_Button* >(pTemplate));
			I3_REF_CHANGE( m_pTButton, (i3UITemplate_Button*) pTemplate);
			break;
		case I3UI_COMBOBOX_SUBTEMPLATE_LISTBOX:	
			I3ASSERT( i3::same_of<i3UITemplate_ListBox* >(pTemplate));
			I3_REF_CHANGE( m_pTListBox, (i3UITemplate_ListBox*) pTemplate);
			break;
		default:
			break;
		}
	}
}

I3UI_CATEGORY i3UITemplate_ButtonComboBox::getTemplateCategory(void)
{
	return I3UI_CATEGORY_BUTTONCOMBO;
}

i3ClassMeta * i3UITemplate_ButtonComboBox::GetInstanceMeta()
{
	return i3UIButtonComboBox::static_meta();
}

void i3UITemplate_ButtonComboBox::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 300.0f);
}

bool i3UITemplate_ButtonComboBox::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_COMBOBOX_SHAPE_BKGND);

	*pRect = *(pShape->getRect());

	pRect->Normalize();
	
	return true;
}

bool i3UITemplate_ButtonComboBox::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT* pRect = _getTargetRect( I3UI_COMBOBOX_SHAPE_BKGND, pRects);

	pRect->left = 0.0f;
	pRect->top = 0.0f;
	pRect->right = cx;
	pRect->bottom = cy;

	return true;
}

void i3UITemplate_ButtonComboBox::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
}

