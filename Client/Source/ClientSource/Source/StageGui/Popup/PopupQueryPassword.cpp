#include "pch.h"
#include "PopupQueryPassword.h"



QueryPasswordParam::QueryPasswordParam()
{
	pMsg			= NULL;
	OnlyNumber		= FALSE;
	ShowPassword	= FALSE;
	LimitCount		= 0;
	Callback		= NULL;
	pUserContext	= NULL;
}


//////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(CPopupQueryPassword, CPopupBase);


CPopupQueryPassword::CPopupQueryPassword( void )
{
	m_pJoinWin		= NULL;

	m_pJoinCaption	= NULL;
	m_pJoinMessage	= NULL;
	m_pJoinInput	= NULL;
	m_pJoinConfirm	= NULL;
	m_pJoinCancel	= NULL;

	m_pCallback		= NULL;
	m_pUserContext	= NULL;
}


void CPopupQueryPassword::OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify )
{
	switch(pNotify->m_nID)
	{
	case GCI_LBP_B_JOIN_CONFIRM:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
			OnClosePopup(reinterpret_cast<void*>(TRUE));
		break;

	case GCI_LBP_B_JOIN_CANCEL:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
			OnClosePopup(reinterpret_cast<void*>(FALSE));
		break;
	}
}

BOOL CPopupQueryPassword::OnLinkControl( i3BinList * plist, i3GameNode * pControl )
{
	LinkControlEx(plist, "lbJoinWin", m_pJoinWin, -1);	

	LinkControlEx(plist, "lbJoinCaption", m_pJoinCaption, -1);
	LinkControlEx(plist, "lbJoinMessage", m_pJoinMessage, -1);
	LinkControlEx(plist, "lbJoinInput", m_pJoinInput, -1);
	LinkControlEx(plist, "lbJoinConfirm", m_pJoinConfirm, GCI_LBP_B_JOIN_CONFIRM);
	LinkControlEx(plist, "lbJoinCancel", m_pJoinCancel, GCI_LBP_B_JOIN_CANCEL);

	return FALSE;
}

void CPopupQueryPassword::OnInitControl( void )
{
	m_pJoinWin->SetEnable(FALSE);
	INIT_WIN_CAPTION(m_pJoinCaption, GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"));/*알림*/

	m_pJoinMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pJoinMessage->SetTextColor(GetColor(GCT_DEFAULT));
	m_pJoinMessage->SetText("");



	INIT_CAPTION_BUTTON(m_pJoinConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/	
	INIT_CAPTION_BUTTON(m_pJoinCancel, GAME_STRING("STR_TBL_GUI_LOBBY_CANCEL_TEXT"));/*취소*/

	CPopupBase::OnInitControl();
}

void CPopupQueryPassword::OnExitKey( void )
{
	CPopupQueryPassword::OnClosePopup(reinterpret_cast<void*>(FALSE));
}

void CPopupQueryPassword::OnOpenPopup( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/ )
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);
	m_pJoinWin->SetEnable(TRUE);

	const QueryPasswordParam *	pParam	= (const QueryPasswordParam *)pArg1;

	m_pCallback		= pParam->Callback;
	m_pUserContext	= pParam->pUserContext;

	m_pJoinMessage->SetText(pParam->pMsg);


	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = pParam->LimitCount;
	ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;

	if ( pParam->OnlyNumber )
		ime._StyleIME |= IME_STYLE_NUMERIC_ONLY;

	m_pJoinInput->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pJoinInput->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pJoinInput->SetTextColor(GetColor(GCT_DEFAULT));
	m_pJoinInput->SetAlphaNumericOnly( true);
	m_pJoinInput->setPasswardEnable( ! pParam->ShowPassword);

	m_pJoinInput->SetFocus(TRUE);
	m_pJoinInput->SetIMEText("");
	m_pJoinInput->SetText("");

}

void CPopupQueryPassword::OnClosePopup( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/ )
{
	CPopupBase::OnClosePopup();
	m_pJoinWin->SetEnable(FALSE);

	if ( m_pCallback )
	{
		string	password = m_pJoinInput->getIMETextBuf();
		m_pCallback(reinterpret_cast<BOOL>(pArg1), password, m_pUserContext);
	}
}

BOOL CPopupQueryPassword::OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code )
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			if( InputKeyboardEnter())
			{
				OnClosePopup(reinterpret_cast<void*>(TRUE));
			}
		}
	}

	return CPopupBase::OnEvent(event, pObj, param2, code);
}