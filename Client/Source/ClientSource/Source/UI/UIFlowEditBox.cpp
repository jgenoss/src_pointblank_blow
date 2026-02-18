#include "pch.h"
#include "UIFlowEditBox.h"

#include "i3Base/itl/map.h"

namespace 
{
	typedef i3::map<I3UI_EDITBOX_STYLE, i3UIEditBox::UI_BOX_PROPERTY> edit_flow_map;

	static edit_flow_map s_edit_flow_tbl;

	void create_flow_table(void)
	{
		if(s_edit_flow_tbl.size() == 0)
		{
			s_edit_flow_tbl.insert( edit_flow_map::value_type(I3UI_EDITBOX_STYLE_SCROLL_LEFT,	i3UIEditBox::UIBOX_SCROLLING_LEFT) );
			s_edit_flow_tbl.insert( edit_flow_map::value_type(I3UI_EDITBOX_STYLE_SCROLL_RIGHT,	i3UIEditBox::UIBOX_SCROLLING_RIGHT) );
			s_edit_flow_tbl.insert( edit_flow_map::value_type(I3UI_EDITBOX_STYLE_SCROLL_UP,		i3UIEditBox::UIBOX_SCROLLING_UP) );
			s_edit_flow_tbl.insert( edit_flow_map::value_type(I3UI_EDITBOX_STYLE_SCROLL_DOWN,	i3UIEditBox::UIBOX_SCROLLING_DOWN) );
		}
	}
}

UIFlowEditBox::UIFlowEditBox()
{
	m_pEditBox = nullptr;
	
	m_rElapsedTime = 0.0f;
	m_rWaitTime = 0.0f;

	m_bStartFlag = false;
	m_bContinue = false;

	create_flow_table();
}

UIFlowEditBox::~UIFlowEditBox()
{
	Reset();
}

void UIFlowEditBox::_SetFlowDirection(const I3UI_EDITBOX_STYLE style)
{
	I3ASSERT(m_pEditBox != nullptr);
	if( style & I3UI_EDITBOX_STYLE_SCROLL )
	{
		edit_flow_map::iterator iter = s_edit_flow_tbl.find(style);

		m_pEditBox->removeEditBoxStyle(I3UI_EDITBOX_STYLE_SCROLL);
		m_pEditBox->addEditBoxStyle(style);
		m_pEditBox->setBoxProperty(iter->second);
	}
}

void UIFlowEditBox::_ResetFlowDirection(void)
{
	I3ASSERT(m_pEditBox != nullptr);
	m_pEditBox->removeEditBoxStyle(I3UI_EDITBOX_STYLE_SCROLL);
	m_pEditBox->setBoxProperty(i3UIEditBox::UIBOX_NORMAL);
}

void UIFlowEditBox::SetFlowEditBox(i3UIEditBox * pEdit, REAL32 rWaitTime/* = 0.0f*/,REAL32 rFlowSpeed/* = 1.0f*/)
{
	I3ASSERT(pEdit != nullptr);

	m_pEditBox = pEdit;
	m_pEditBox->addEditBoxStyle( I3UI_EDITBOX_STYLE_ANNOUNCE);
	m_pEditBox->setScrollMoveSpeed(rFlowSpeed);
	m_rWaitTime = rWaitTime;
}

void UIFlowEditBox::SetFlowTextData(const i3::wstring& wstr)
{
	I3ASSERT(m_pEditBox != nullptr);
	m_pEditBox->SetText(wstr);
}

void UIFlowEditBox::SetFlowTextData(const i3::rc_wstring& wstr)
{
	I3ASSERT(m_pEditBox != nullptr);
	m_pEditBox->SetText(wstr);
}

/*** Scroll Text 설정 시, Text 시작 위치가 EditBox 영역 밖으로 나가는것을 막기위한 함수. ***/ 
void UIFlowEditBox::SetFlowTextData(const i3::wstring& wstr, const I3UI_EDITBOX_STYLE style)
{
	I3ASSERT(m_pEditBox != nullptr);

	_ResetFlowDirection();		// Scroll Style 제거
	m_pEditBox->SetText(wstr);	// Text 설정
	_SetFlowDirection(style);	// Scroll Style 추가
}

void UIFlowEditBox::SetFlowTextData(const i3::rc_wstring& wstr, const I3UI_EDITBOX_STYLE style)
{
	I3ASSERT(m_pEditBox != nullptr);

	_ResetFlowDirection();		// Scroll Style 제거
	m_pEditBox->SetText(wstr);	// Text 설정
	_SetFlowDirection(style);	// Scroll Style 추가
}

void UIFlowEditBox::Reset(void)
{
	m_pEditBox = nullptr;

	m_rElapsedTime = 0.0f;
	m_rWaitTime = 0.0f;

	m_bStartFlag = false;
	m_bContinue = false;
}

void UIFlowEditBox::PlayEditTextFlow(TEXT_FLOW_DIRECTION eFlowDir, bool bContinue/* = false*/, bool bForceScroll/* = false*/)
{
	const WCHAR16 * pwstr = m_pEditBox->getText();

	// 설정된 Text가 없다면, 수행하지 않습니다.
	if( pwstr == nullptr || i3::generic_string_size(pwstr) == 0)
		return;

	INT32 i32TextArea = 0;
	INT32 i32EditArea = 0;

	switch(eFlowDir)
	{
	case TFD_HORIZONTAL:
		{	
			i32TextArea = m_pEditBox->getUIText()->getExtentWidth();
			i32EditArea = (INT32)m_pEditBox->getWidth();
		}
		break;
	case TFD_VERTICAL:
		{	
			i32TextArea = m_pEditBox->getUIText()->getExtentHeight();
			i32EditArea = (INT32)m_pEditBox->getHeight();
		}
		break;
	default	:		return;
	}

	m_pEditBox->setScrollState( i3UIEditBox::UBSS_START);
	m_pEditBox->setTextStartPosX(0);
	m_pEditBox->setTextStartPosY(0);

	m_bStartFlag = bForceScroll;	// true : text 길이에 상관 없이 항상 Flow 처리
	m_bContinue = bContinue;		// true : 현재 text 위치에서 시작 , false : edit 창 밖에서 시작
	m_rElapsedTime = 0.0f;

	if(i32TextArea >= i32EditArea)
		m_bStartFlag = true;
}

void UIFlowEditBox::StopEditTextFlow(void)
{
	m_rElapsedTime = 0.0f;

	m_bStartFlag = false;

	m_pEditBox->setScrollState( i3UIEditBox::UBSS_START);
	m_pEditBox->setTextStartPosX(0);
	m_pEditBox->setTextStartPosY(0);
	//m_pEditBox->setModifyState(true);
}

void UIFlowEditBox::OnUpdate(REAL32 rDeltaSeconds)
{
	if(m_bStartFlag == false)	 
		return;
	
	m_rElapsedTime += rDeltaSeconds;

	if(m_rElapsedTime < m_rWaitTime)
		return;

	if( isReadyToFlow() )
	{
		m_pEditBox->setContinueScrollPos(m_bContinue);
		m_pEditBox->setScrollState( i3UIEditBox::UBSS_INIT);
		m_bContinue = false;
	}
}

bool UIFlowEditBox::isReadyToFlow(void) const
{
	switch(m_pEditBox->getScrollState())
	{
	case i3UIEditBox::UBSS_START	:	return true;
	case i3UIEditBox::UBSS_END		:	return true;
	default							:	break;
	}

	return false;
}

bool UIFlowEditBox::isSameText(const i3::wliteral_range& wstrRng)
{
	return i3::generic_is_equal( m_pEditBox->getTextRange(), wstrRng);
}