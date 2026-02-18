#include "i3FrameworkPCH.h"
#include "i3UIComboBox.h"

#include "i3UIEditBox.h"
#include "i3UIButton.h"
#include "i3UIListBox.h"

#include "../i3Framework.h"

I3_CLASS_INSTANCE( i3UIComboBox);

i3UIComboBox::i3UIComboBox()
{
	m_pEditBox = i3UIEditBox::new_object();
	m_DerivedTextCtrl = m_pEditBox;

	m_pButton = i3UIButton::new_object();
}

i3UIComboBox::~i3UIComboBox()
{
	I3_SAFE_RELEASE( m_pEditBox);
	I3_SAFE_RELEASE( m_pButton);
}


void i3UIComboBox::SetEditBoxReadOnly( bool bReadOnly)
{
	if( bReadOnly)
		addComboStyle( I3UI_COMBO_STYLE_EDITREADONLY);
	else
		removeComboStyle( I3UI_COMBO_STYLE_EDITREADONLY);

	m_pEditBox->SetReadOnly( bReadOnly);
}

bool	i3UIComboBox::_IsListBoxActionNeededWhenClick_DerivedCombo(i3UIControl* pClickedCtrl) const
{
	if (pClickedCtrl == m_pButton ) 
	{
		I3TRACE(" Combo Button From Button!\n");
		return true;
	}

	if (pClickedCtrl == m_pEditBox && m_pEditBox->isReadOnly() )
	{
		I3TRACE(" Combo Button From Editbox!\n");
		return true;
	}

	return false;
}

void	i3UIComboBox::_OnBindTemplate_DerivedCombo()
{
	
	//EditBox
	if( m_pEditBox->getParent() == nullptr)
	{
		AddControl( m_pEditBox);
		m_pEditBox->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pEditBox->addStyle( I3UI_STYLE_SLAVE);

		m_pEditBox->SetTextAlign( getTextAlignX(), getTextAlignY());
		m_pEditBox->SetTextColor( getTextColor());
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
	
	m_pEditBox->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_SUBTEMPLATE_EDITBOX));
	m_pButton->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_SUBTEMPLATE_BUTTON));
	m_pListBox->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_SUBTEMPLATE_LISTBOX));

}

void i3UIComboBox::_UpdateComponentSize_DerivedCombo( REAL32 cx, REAL32 cy)
{
	REAL32 offsetX = cx - m_fDefWidthButton;

	m_pEditBox->setPos( 0.0f, 0.0f, 0.0f);
	m_pEditBox->setSize( cx, m_fDefHeightButton);	//버튼이 에디트 바로 오른쪽에 놓이는 경우

	
	m_pButton->setPos( offsetX, 0.0f);
	m_pButton->setSize( m_fDefWidthButton, m_fDefHeightButton);

}

void	i3UIComboBox::_SetFocusEditBoxIfExists_DerivedCombo()
{
	if( m_pEditBox->isReadOnly() == false)
	{
		i3Framework::SetCurrentFocus( m_pEditBox);
	}
}




void i3UIComboBox::_OnEnabled_DerivedCombo(bool bEnable)
{
	m_pEditBox->EnableCtrl( bEnable);
	m_pButton->EnableCtrl( bEnable);
}

void	i3UIComboBox::_OnInitAfterLoad_DerivedCombo()
{
	m_pEditBox->SetReadOnly( isComboStyle(I3UI_COMBO_STYLE_EDITREADONLY));
}



#if defined( I3_DEBUG)	// Tool에서만 사용

void	i3UIComboBox::_GetTDKGridProperties_DerivedCombo(i3::vector<PROPERTYINFO*>& List)
{
	AddMFCProperty_UINT32BitFlag( List, &m_ComboStyle, I3UI_COMBO_STYLE_EDITREADONLY, "EditBox Read Only", "EditBox Read Only");
}

bool	i3UIComboBox::_OnTDKPropertyChanged_DerivedCombo(const char* szName)
{
	if( i3::generic_is_iequal( szName, "EditBox Read Only") )
	{
		// 변수는 자동갱신, 렌더링 반영 안된게 있다면...여기서 재구현..
		m_pEditBox->SetReadOnly(isComboStyle(I3UI_COMBO_STYLE_EDITREADONLY));
		return true;
	}

	return false;
}

#endif

void i3UIComboBox::CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIComboBase::CopyTo(pDest, method);

	I3ASSERT(i3::same_of< i3UIComboBox*>(pDest));

	i3UIEditBox* pDestEditBox = static_cast<i3UIComboBox*>(pDest)->m_pEditBox;
	this->m_pEditBox->CopyTo(pDestEditBox, method);
}
