#include "i3FrameworkPCH.h"
#include "i3UITemplate_MessageBox.h"
#include "i3UIMessageBox.h"

//I3_CLASS_INSTANCE( i3UITemplate_MessageBox, i3UITemplate_FrameWnd);
I3_CLASS_INSTANCE( i3UITemplate_MessageBox);

i3UITemplate_MessageBox::i3UITemplate_MessageBox()
{
	m_pBtnTemplate	= i3UITemplate_Button::new_object();
	m_pStaticText	= i3UITemplate_StaticText::new_object();
}

i3UITemplate_MessageBox::~i3UITemplate_MessageBox()
{
	I3_SAFE_RELEASE( m_pBtnTemplate);
	I3_SAFE_RELEASE( m_pStaticText);
}

const char * i3UITemplate_MessageBox::getComment(void)
{
	return "MessageBox";
}

const char * i3UITemplate_MessageBox::getTemplateName(void)
{
	return "MessageBox";
}

i3ClassMeta * i3UITemplate_MessageBox::GetInstanceMeta()
{
	return i3UIMessageBox::static_meta();
}

INT32 i3UITemplate_MessageBox::getSubTemplateCount(void)
{
	return I3UI_MSGBOX_SUBTEMPLATE_COUNT;
}

i3UITemplate* i3UITemplate_MessageBox::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_MSGBOX_SUBTEMPLATE_COUNT);

	switch( idx)
	{
	case I3UI_MSGBOX_SUBTEMPLATE_BUTTON:	return m_pBtnTemplate;
	case I3UI_MSGBOX_SUBTEMPLATE_TEXT:		return m_pStaticText;
	default:								break;
	}

	return nullptr;
}

const char * i3UITemplate_MessageBox::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_MSGBOX_SUBTEMPLATE_COUNT);

	switch( idx)
	{
	case I3UI_MSGBOX_SUBTEMPLATE_BUTTON:	return "Button";
	case I3UI_MSGBOX_SUBTEMPLATE_TEXT:		return "Text";
	default:
		break;
	}

	return "Invalid id";
}

void i3UITemplate_MessageBox::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, I3UI_MSGBOX_SUBTEMPLATE_COUNT);

	I3ASSERT( pTemplate != nullptr);

	switch( idx)
	{
	case I3UI_MSGBOX_SUBTEMPLATE_BUTTON:
		{
			I3ASSERT( i3::same_of<i3UITemplate_Button* >(pTemplate));
			I3_REF_CHANGE( m_pBtnTemplate, (i3UITemplate_Button*)pTemplate);
		}break;
	case I3UI_MSGBOX_SUBTEMPLATE_TEXT:
		{
			I3ASSERT( i3::same_of<i3UITemplate_StaticText* >(pTemplate));
			I3_REF_CHANGE( m_pStaticText, (i3UITemplate_StaticText*)pTemplate);
		}break;
	default:
		break;
	}
}