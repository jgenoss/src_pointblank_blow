#include "pch.h"

#if legacy_gui_scene
#include "UIPopupAbuse.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"

#include "../GameGUI.h"
#include "../GuiNotifyReceiver.h"

I3_CLASS_INSTANCE( UIPopupAbuse);//, UIPopupBase);

UIPopupAbuse::UIPopupAbuse():m_pAbuseWindow(nullptr), m_pAbuseEditbox(nullptr), m_pAbuseConfirm(nullptr)
{
}

UIPopupAbuse::~UIPopupAbuse()
{
}

/*virtual*/ void UIPopupAbuse::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Add Scene
}

/*virtual*/ void UIPopupAbuse::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate( rDeltaSeconds);

	if( m_pAbuseWindow != nullptr)
	{
		m_rAbusingTotalTime += rDeltaSeconds;
		UINT32 ForceTime = AbuseContext::i()->GetAbuseTime();
		UINT32 uiTemp = (UINT32)m_rAbusingTotalTime;

		if( uiTemp < ForceTime)
		{
			INT32 time = ForceTime - uiTemp;
			char temp[10],str[30];
			itoa(time,temp,10);
			sprintf_s(str,"%s",temp);//각 국가 번역 스트링으로
			INIT_CAPTION_BUTTON(m_pAbuseConfirm, str);/*확인*/
			//LinkControlEx(plist, "lbAbuseConfirm", m_pAbuseConfirm, GCI_LBP_A_BUTTON_ABUSING_CONFIRM);
			m_pAbuseConfirm->setInputDisable(true);
			m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 8);
			m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 8);
			m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 8);
			m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONENABLE, GUI_SHAPE_OFFSET + 8);
		}
		else
		{
			INIT_CAPTION_BUTTON(m_pAbuseConfirm, GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/
			m_pAbuseConfirm->setInputDisable(false);
			m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 9);
			m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 9);
			m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 8);
		}
	}
}

/*virtual*/ bool UIPopupAbuse::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( m_pAbuseWindow != nullptr)
	{
		m_pAbuseWindow->SetEnable(true);
		m_pAbuseEditbox->SetEnable(true);
		m_pAbuseConfirm->SetEnable(true);

		GetMainFrame()->SetEffectButtonStop(); //버튼깜박이 임시코드
		m_rAbusingTotalTime = 0.f;

		//서버에서 받은 어뷰징 레벨에 맞게 텍스트 설정 각 국가 번역 필요
		switch( AbuseContext::i()->GetAbuseLevel())
		{
		case ABUSING_LEVEL_NORMAL:
			m_pAbuseEditbox->SetText(GAME_RCSTRING("STR_ABUSING_LEVEL_NORMAL"));
			break;
		case ABUSING_LEVEL_STRONG:
			m_pAbuseEditbox->SetText(GAME_RCSTRING("STR_ABUSING_LEVEL_STRONG"));
			break;
		case ABUSING_LEVEL_STRONGEST:
			m_pAbuseEditbox->SetText(GAME_RCSTRING("STR_ABUSING_LEVEL_STRONGEST"));
			break;
		default :
			I3PRINTLOG(I3LOG_WARN,  "switch invalid call!!!");
			break;
		}
	}

	return true;
}

/*virtual*/ bool UIPopupAbuse::OnLinkControl( const i3::vector_set<STRCINFO*>* pList, i3GameNode * pControl)
{
	// 어뷰징 알림 팝업 : 20110331 하동익
	LinkControlEx( *pList, "lbAbuseWindow", m_pAbuseWindow, -1);
	LinkControlEx( *pList, "lbAbuseConfirm", m_pAbuseConfirm, GCI_LBP_A_BUTTON_ABUSING_CONFIRM);
	LinkControlEx( *pList, "lbAbuseEditbox", m_pAbuseEditbox, -1);

	return true;
}

/*virtual*/ void UIPopupAbuse::OnInitControl( void)
{
	// 어뷰징 알림 팝업 : 20110331 하동익
	if( m_pAbuseWindow != nullptr)
	{
		m_pAbuseWindow->SetEnable(false);
		m_pAbuseConfirm->SetEnable(false);
		m_pAbuseConfirm->setClickAction(false);
		m_pAbuseEditbox->SetEnable(false);
		
		m_rAbusingTotalTime = 0.f;
		m_pAbuseEditbox->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pAbuseEditbox->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));
		m_pAbuseEditbox->SetText("");
	}
}

/*virtual*/ void UIPopupAbuse::OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch( pNotify->m_nID)
	{
	case GCI_LBP_A_BUTTON_ABUSING_CONFIRM:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
			ConfirmOK();
		break;
	}
}

void UIPopupAbuse::ConfirmOK( void)
{
	if( m_pAbuseWindow != nullptr)
	{
		m_pAbuseWindow->SetEnable(false);

		UIPhaseLobby* pLobby = (UIPhaseLobby*) GetMainFrame()->GetCurrentPhase();

		if( pLobby != nullptr)
			pLobby->ConfirmAbuseOK();
	}
}
#endif