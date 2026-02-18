#include "i3FrameworkPCH.h"
#include "i3UIButtonComboBox.h"

#include "i3UIButtonComposed3.h"
#include "i3UIButton.h"
#include "i3UIListBox.h"

I3_CLASS_INSTANCE( i3UIButtonComboBox);

i3UIButtonComboBox::i3UIButtonComboBox()
{
	m_pEditBox = i3UIButtonComposed3::new_object();
	m_DerivedTextCtrl = m_pEditBox;

	m_pButton = i3UIButton::new_object();
}

i3UIButtonComboBox::~i3UIButtonComboBox()
{
	I3_SAFE_RELEASE( m_pButton);
	I3_SAFE_RELEASE( m_pEditBox);
}


void i3UIButtonComboBox::_OnBindTemplate_DerivedCombo(void)
{
	//EditBox
	if( m_pEditBox->getParent() == nullptr)
	{
		AddControl( m_pEditBox);
		m_pEditBox->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pEditBox->addStyle( I3UI_STYLE_SLAVE);

		m_pEditBox->SetTextAlign( getTextAlignX(), getTextAlignY());
		m_pEditBox->SetButtonTextColorAll( getTextColor());
		m_pEditBox->SetTextShadowColor( this->getTextShadowColor());
		m_pEditBox->SetTextShadowType( this->getTextShadowType());
		m_pEditBox->setTextStyle( this->getTextStyle());

		m_pEditBox->SetTextSpace( m_SpaceX, m_SpaceY);
	}

	//Button
	if( m_pButton->getParent() == nullptr)
	{
		AddControl( m_pButton);
		m_pButton->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pButton->addStyle( I3UI_STYLE_SLAVE);
	}


	//Set Template
	m_pEditBox->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_SUBTEMPLATE_EDITBOX));
	m_pButton->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_SUBTEMPLATE_BUTTON));
	m_pListBox->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_SUBTEMPLATE_LISTBOX));
}

void i3UIButtonComboBox::_UpdateComponentSize_DerivedCombo( REAL32 cx, REAL32 cy)
{
	REAL32 offsetX = cx - m_fDefWidthButton;

	m_pEditBox->setPos( 0.0f, 0.0f, 0.0f);
	m_pEditBox->setSize( cx, m_fDefHeightButton);	//버튼이 에디트 바로 오른쪽에 놓이는 경우

	m_pButton->setPos( offsetX, 0.0f);
	m_pButton->setSize( m_fDefWidthButton, m_fDefHeightButton);

}

bool	i3UIButtonComboBox::_IsListBoxActionNeededWhenClick_DerivedCombo(i3UIControl* pClickedCtrl) const
{
	if (pClickedCtrl == m_pButton)
	{
		I3TRACE(" Combo Button From Button!\n");
		return true;
	}
	if( pClickedCtrl == m_pEditBox)
	{
		I3TRACE(" Combo Button From Editbox!\n");
		return true;
	}

	return false;
}

#if defined( I3_DEBUG)	// Tool에서만 사용

void	i3UIButtonComboBox::_GetTDKGridProperties_DerivedCombo(i3::vector<PROPERTYINFO*>& List)
{
	// No Imp.
}

bool	i3UIButtonComboBox::_OnTDKPropertyChanged_DerivedCombo(const char* szName)
{
	return false;
}

void i3UIButtonComboBox::_OnTDKPropertyChanged_DerivedColor(COLOR pColor)
{
	m_pEditBox->SetButtonTextColorAll(pColor);
}

#endif


void	i3UIButtonComboBox::_OnEnabled_DerivedCombo(bool bEnable)
{
	m_pEditBox->EnableCtrl(bEnable);
	m_pButton->EnableCtrl( bEnable);
}

void	i3UIButtonComboBox::CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIComboBase::CopyTo(pDest, method);

	I3ASSERT(i3::same_of< i3UIButtonComboBox*>(pDest));

	i3UIButtonComposed3* pDestEditBox = static_cast<i3UIButtonComboBox*>(pDest)->m_pEditBox;
	this->m_pEditBox->CopyTo(pDestEditBox, method);
}


