#include "pch.h"
#include "UIPopupCommunity.h"

I3_CLASS_INSTANCE( UIPopupCommunity, UIPopupBase);

BOOL UIPopupCommunity::m_bWarningNoteCount = FALSE;

UIPopupCommunity::UIPopupCommunity()
{
}

UIPopupCommunity::~UIPopupCommunity()
{
}

/*virtual*/ void UIPopupCommunity::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
}

void UIPopupCommunity::_UpdateAlarmNote(REAL32 rDeltaSeconds)
{
	if( m_bAlarmCommunity)
	{
		m_rAlarmTerm += rDeltaSeconds;

		if (0.25f < m_rAlarmTerm)
		{
			m_rAlarmTerm = 0.0f;
			m_bAlarmFlag = !m_bAlarmFlag;

			/*if (m_bAlarmFlag)
			{
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
			}
			else
			{
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
			}*/
		}
	}
}

void UIPopupCommunity::StartAlarmNote( void)
{
	m_bAlarmCommunity = true;

	// 쪽지 경고창 플래그 활성화
	m_bWarningNoteCount = false;
}

void UIPopupCommunity::EndAlarmNote(void)
{
	m_bAlarmCommunity = false;

	// 원상복귀
	/*m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);*/
}

void UIPopupCommunity::ReceiveAllNote( INT32 offset /*= 0*/)
{
}

void UIPopupCommunity::SetNoteCheck( INT32 idx)
{
}

void UIPopupCommunity::SetNoteType( NOTE_TYPE type)
{
}

void UIPopupCommunity::SetSelectedNote( INT32 idx)
{
}

void UIPopupCommunity::ResetNote(void)
{
}

