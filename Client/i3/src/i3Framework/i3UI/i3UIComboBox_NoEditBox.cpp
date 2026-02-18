#include "i3FrameworkPCH.h"
#include "i3UIComboBox_NoEditBox.h"

#include "i3UIButtonComposed3.h"
#include "i3UIListBox.h"

I3_CLASS_INSTANCE( i3UIComboBox_NoEditBox);

i3UIComboBox_NoEditBox::i3UIComboBox_NoEditBox()
{
	m_pButton = i3UIButtonComposed3::new_object();
	m_DerivedTextCtrl = m_pButton;
}

i3UIComboBox_NoEditBox::~i3UIComboBox_NoEditBox()
{
	I3_SAFE_RELEASE( m_pButton);
}

void i3UIComboBox_NoEditBox::_OnBindTemplate_DerivedCombo(void)
{
	
	//  Caption plus Arrow Button
	if( m_pButton->getParent() == nullptr)
	{
		AddControl( m_pButton);
		m_pButton->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pButton->addStyle( I3UI_STYLE_SLAVE);
		
		m_pButton->SetTextAlign( getTextAlignX(), getTextAlignY());

		m_pButton->SetTextShadowColor( this->getTextShadowColor());
		m_pButton->SetTextShadowType( this->getTextShadowType());
		m_pButton->setTextStyle( this->getTextStyle());
		m_pButton->SetTextSpace( m_SpaceX, m_SpaceY);

		m_pButton->SkipUnselect(true);		// 생성할때부터 셀렉션 고정으로 일단 놓음...
	}

	//Set Template
	m_pButton->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_BUTTON));		// enum타입 변경이 요구됨..

	m_pListBox->SetTemplate( m_pTemplate->getSubTemplate( I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_LISTBOX));


}



void i3UIComboBox_NoEditBox::_UpdateComponentSize_DerivedCombo( REAL32 cx, REAL32 cy)
{
	m_pButton->setPos( 0.0f, 0.0f);
	m_pButton->setSize( cx, m_fDefHeightButton);
}


bool	i3UIComboBox_NoEditBox::_IsListBoxActionNeededWhenClick_DerivedCombo(i3UIControl* pClickedCtrl) const
{
	if (pClickedCtrl == m_pButton)
	{
		I3TRACE(" Combo Button From Button!\n");
		return true;
	}
	return false;
}

#if defined( I3_DEBUG)	// Tool에서만 사용

void	i3UIComboBox_NoEditBox::_GetTDKGridProperties_DerivedCombo(i3::vector<PROPERTYINFO*>& List)
{
	AddMFCProperty_BeginGroup(List, "Child ComboButton");
	
	AddMFCProperty_Color(List, &m_pButton->m_ButtonTextColor[i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE],
		"ButtonTextColor_Normal", "버튼 텍스트 노멀색상");
	AddMFCProperty_Color(List, &m_pButton->m_ButtonTextColor[i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE],
		"ButtonTextColor_MouseOver", "버튼 텍스트 마우스오버 색상");
	AddMFCProperty_Color(List, &m_pButton->m_ButtonTextColor[i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED],
		"ButtonTextColor_Selected", "버튼 텍스트 셀렉션/눌림 색상");
	AddMFCProperty_Color(List, &m_pButton->m_ButtonTextColor[i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_DISABLE],
		"ButtonTextColor_Disabled", "버튼 텍스트 불활성 색상");

	AddMFCProperty_EndGroup(List);


}

bool	i3UIComboBox_NoEditBox::_OnTDKPropertyChanged_DerivedCombo(const char* szName)
{
	if (i3::generic_is_equal(szName, "ButtonTextColor_Normal") ||
		i3::generic_is_equal(szName, "ButtonTextColor_MouseOver") || 
		i3::generic_is_equal(szName, "ButtonTextColor_Selected") || 
		i3::generic_is_equal(szName, "ButtonTextColor_Disabled") )
	{
		m_pButton->addState( I3UI_STATE_NEED_UPDATE);

		return true;
	}

	return false;
}

void i3UIComboBox_NoEditBox::_OnTDKPropertyChanged_DerivedColor(COLOR pColor)
{
	m_pButton->SetButtonTextColorAll(pColor);
}

#endif

void	i3UIComboBox_NoEditBox::_OnEnabled_DerivedCombo(bool bEnable)
{
	m_pButton->EnableCtrl( bEnable);
}

void	i3UIComboBox_NoEditBox::_RemoveListBox_DerivedCombo()
{
	m_pButton->removeState(I3UI_STATE_SELECTED);
	m_pButton->addState(I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

// for i3UIComboBox_NoEditBox ..  이것말고 버튼 콤보박스는 일단 기존대로 처리함..
namespace pack
{
	struct ALIGN4 UICOMBOBOX_NOEDITBOX
	{
		UINT8		m_ID[4] = { 'U', 'C', 'N', '1' };
		UINT32		m_clrMainText_Normal = 0;
		UINT32		m_clrMainText_MouseOver = 0;
		UINT32		m_clrMainText_Selected = 0;
		UINT32		m_clrMainText_Disable = 0;	// 이 구조체에서는 더 이상 패딩을 두지 않음..버전별로 새로 만들것..
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

namespace
{
	UINT32& FileIDToUINT(UINT8 (&strID)[4] ) {  return reinterpret_cast<UINT32&>(strID); }
	const UINT32& FileIDToUINT(const char (&strID)[5] ) {  return reinterpret_cast<const UINT32&>(strID); }

	const UINT32& UCN1 = FileIDToUINT("UCN1");

	const UINT32& UCM4 = FileIDToUINT("UCM4");	//	--> ComboBox_NoEditBox 의 별도 읽기/저장을 위한 구별 ID
}

UINT32	i3UIComboBox_NoEditBox::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Result = i3UIComboBase::OnSave( pResFile);
	I3_CHKIO( Result);
	
	pack::UICOMBOBOX_NOEDITBOX data;
	
	i3UIButtonComposed3* mainButton = m_pButton;
	data.m_clrMainText_Normal = mainButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE);
	data.m_clrMainText_MouseOver = mainButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE);
	data.m_clrMainText_Selected = mainButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED);
	data.m_clrMainText_Disable  = mainButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_DISABLE);

	Rc = pResFile->Write( &data, sizeof( data));

	I3_CHKIO( Rc);
	Result += Rc;
	
	return Result;
}


UINT32	i3UIComboBox_NoEditBox::_OnLoadMore( i3ResourceFile * pResFile, UINT32 parentFileID)
{
	
	UINT32 Rc, Result = 0;

	if ( parentFileID != UCM4)
	{
		m_pButton->SetButtonTextColorAll( this->getTextColor() );
		return Result;
	}

	pack::UICOMBOBOX_NOEDITBOX data;

	Rc =  pResFile->Read( &data, sizeof( data));\
	I3_CHKIO( Rc);
	Result += Rc;

	i3UIButtonComposed3* mainButton = m_pButton;

	mainButton->SetButtonTextColor(	i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE, data.m_clrMainText_Normal);
	mainButton->SetButtonTextColor(	i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE, data.m_clrMainText_MouseOver);
	mainButton->SetButtonTextColor(	i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED, data.m_clrMainText_Selected);
	mainButton->SetButtonTextColor(	i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_DISABLE, data.m_clrMainText_Disable);

	return Result;
		
}

bool	i3UIComboBox_NoEditBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UIComboBase::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	UINT32 uiClr = m_pButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE);
	I3COLOR clr;	clr.SetRGBA(uiClr);

	pXML->addAttr("ComboButtonText_Normal_ColorR", (INT32)i3Color::GetR(&clr));
	pXML->addAttr("ComboButtonText_Normal_ColorG", (INT32)i3Color::GetG(&clr));
	pXML->addAttr("ComboButtonText_Normal_ColorB", (INT32)i3Color::GetB(&clr));
	pXML->addAttr("ComboButtonText_Normal_ColorA", (INT32)i3Color::GetA(&clr));

	uiClr = m_pButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE);
	clr.SetRGBA(uiClr);

	pXML->addAttr("ComboButtonText_MouseOver_ColorR", (INT32)i3Color::GetR(&clr));
	pXML->addAttr("ComboButtonText_MouseOver_ColorG", (INT32)i3Color::GetG(&clr));
	pXML->addAttr("ComboButtonText_MouseOver_ColorB", (INT32)i3Color::GetB(&clr));
	pXML->addAttr("ComboButtonText_MouseOver_ColorA", (INT32)i3Color::GetA(&clr));

	uiClr = m_pButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED);
	clr.SetRGBA(uiClr);

	pXML->addAttr("ComboButtonText_Selected_ColorR", (INT32)i3Color::GetR(&clr));
	pXML->addAttr("ComboButtonText_Selected_ColorG", (INT32)i3Color::GetG(&clr));
	pXML->addAttr("ComboButtonText_Selected_ColorB", (INT32)i3Color::GetB(&clr));
	pXML->addAttr("ComboButtonText_Selected_ColorA", (INT32)i3Color::GetA(&clr));
	
	uiClr = m_pButton->GetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_DISABLE);
	clr.SetRGBA(uiClr);
	
	pXML->addAttr("ComboButtonText_Disabled_ColorR", (INT32)i3Color::GetR(&clr));
	pXML->addAttr("ComboButtonText_Disabled_ColorG", (INT32)i3Color::GetG(&clr));
	pXML->addAttr("ComboButtonText_Disabled_ColorB", (INT32)i3Color::GetB(&clr));
	pXML->addAttr("ComboButtonText_Disabled_ColorA", (INT32)i3Color::GetA(&clr));
	
	return bResult;
}

bool	i3UIComboBox_NoEditBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UIComboBase::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;
	
	INT32 r = 255, g = 255, b = 255, a = 255;

	if (pXML->getAttr("ComboButtonText_Normal_ColorR", &r) )
	{
		pXML->getAttr("ComboButtonText_Normal_ColorG", &g);
		pXML->getAttr("ComboButtonText_Normal_ColorB", &b);
		pXML->getAttr("ComboButtonText_Normal_ColorA", &a);

		I3COLOR color;
		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);

		m_pButton->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE, color);

		pXML->getAttr("ComboButtonText_MouseOver_ColorR", &r);
		pXML->getAttr("ComboButtonText_MouseOver_ColorG", &g);
		pXML->getAttr("ComboButtonText_MouseOver_ColorB", &b);
		pXML->getAttr("ComboButtonText_MouseOver_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_pButton->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE, color);

		pXML->getAttr("ComboButtonText_Selected_ColorR", &r);
		pXML->getAttr("ComboButtonText_Selected_ColorG", &g);
		pXML->getAttr("ComboButtonText_Selected_ColorB", &b);
		pXML->getAttr("ComboButtonText_Selected_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_pButton->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED, color);

		pXML->getAttr("ComboButtonText_Disabled_ColorR", &r);
		pXML->getAttr("ComboButtonText_Disabled_ColorG", &g);
		pXML->getAttr("ComboButtonText_Disabled_ColorB", &b);
		pXML->getAttr("ComboButtonText_Disabled_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_pButton->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_DISABLE, color);

	}
	else
	{
		m_pButton->SetButtonTextColorAll( this->getTextColor() );
	}

	return bResult;
}

void	i3UIComboBox_NoEditBox::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method )
{
	i3UIComboBase::CopyTo(pDest, method);

	I3ASSERT( i3::same_of< i3UIComboBox_NoEditBox*>(pDest)); 

	i3UIButtonComposed3* pDestButton = static_cast<i3UIComboBox_NoEditBox*>(pDest)->m_pButton;
	this->m_pButton->CopyTo(pDestButton, method);
}
