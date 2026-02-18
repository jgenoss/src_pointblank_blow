#include "pch.h"
#include "PopupClanSendNote.h"
#include "GlobalVariables.h"

#define CLAN_SEND_NOTE_TYPE		3

I3_CLASS_INSTANCE(CPopupClanSendNote, CPopupBase);

CPopupClanSendNote::CPopupClanSendNote()
{
	m_pGuiRoot = NULL;
	m_pRootCaption = NULL;
	m_pButtonSending = NULL;
	m_pButtonClear = NULL;
	m_pButtonClose = NULL;

	m_pSendNoteEdit = NULL;
	m_pSendTypeStatic = NULL;
	m_pSendTypeCombo = NULL;
	m_pSendTypeButton = NULL;
	m_pSendTypePopup = NULL;

	m_nTypeComboIdx = 0;
}

CPopupClanSendNote::~CPopupClanSendNote()
{
}

void CPopupClanSendNote::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
		case GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_SENDING:
			if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSending();
			break;
		case GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_CLEAR:
			if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnClear();
			break;
		case GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_CLOSE:
			if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnExitKey();
			break;
		case GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_TYPE:
			if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTypeComboButton();
			break;
		case GCI_CLAN_P_POPUP_MANAGEMENT_SEND_NOTE_POPUP:
			if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTypeComboPopup(pNotify->m_nPriParam);
			break;
	}
}

void CPopupClanSendNote::OnGameEvent( INT32 event, INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	switch( event )
	{
	case EVENT_CLAN_NOTE:
		{
			if (EV_SUCCESSED(arg))
			{
				//	정상 처리시 성공 메시지없이 팝업을 닫는다.
				OnExitKey();
			}
			else
			{
				//	에러시 
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_FAILED_SEND_NOTE"));/*쪽지 보내기에 실패했습니다.\n 다시 시도해 주세요.*/
			}
		}
		break;
	}
}

BOOL CPopupClanSendNote::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "clanManagementSendNotePopup", m_pGuiRoot, -1);
	LinkControlEx(plist, "clanManagementSendNoteCaption", m_pRootCaption, -1);
	LinkControlEx(plist, "clanManagementSendNoteSending", m_pButtonSending, GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_SENDING);
	LinkControlEx(plist, "clanManagementSendNoteClear", m_pButtonClear, GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_CLEAR);
	LinkControlEx(plist, "clanManagementSendNoteClose", m_pButtonClose, GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_CLOSE);

	LinkControlEx(plist, "clanManagementSendNoteMemoEdit", m_pSendNoteEdit, -1);
	LinkControlEx(plist, "clanManagementSendNoteTypeStatic", m_pSendTypeStatic, -1);
	LinkControlEx(plist, "clanManagementSendNoteTypeCombo", m_pSendTypeCombo, -1);
	LinkControlEx(plist, "clanManagementSendNoteTypeButton", m_pSendTypeButton, GCI_CLAN_B_POPUP_MANAGEMENT_SEND_NOTE_TYPE);
	LinkControlEx(plist, "clanManagementSendNoteTypePopup", m_pSendTypePopup, GCI_CLAN_P_POPUP_MANAGEMENT_SEND_NOTE_POPUP);

	return FALSE;
}

void CPopupClanSendNote::OnInitControl(void)
{
	m_pGuiRoot->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pRootCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_SEND_NOTE"));/*쪽지 보내기*/

	INIT_CAPTION_BUTTON(m_pButtonSending, GAME_STRING("STBL_IDX_CLAN_SEND"));/*보내기*/
	INIT_CAPTION_BUTTON(m_pButtonClear, GAME_STRING("STBL_IDX_CLAN_REWRITE"));/*다시쓰기*/
	INIT_CAPTION_BUTTON(m_pButtonClose, GAME_STRING("STBL_IDX_CLAN_CLOSE"));/*닫기*/

	//	메모창
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = 120;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER | IME_STYLE_MULTILINE;
	m_pSendNoteEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pSendNoteEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pSendNoteEdit->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSendNoteEdit->SetText("");		//	"여기에 입력하세요"
	m_pSendNoteEdit->SetTextAutoWrap( TRUE);
	m_pSendNoteEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pSendNoteEdit->SetFocus(TRUE);

	INIT_STATIC_CAPTION(m_pSendTypeStatic, GAME_STRING("STBL_IDX_CLAN_SEND_TO"));/*받는사람*/
	INIT_WIN_CAPTION(m_pSendTypeCombo, GAME_STRING("STBL_IDX_CLAN_ALL_MEMBER"));/*전체*/
	INIT_COMBO_EX(m_pSendTypeButton);

	//	팀 구성 팝업
	m_pSendTypePopup->CreateTextEx(CLAN_SEND_NOTE_TYPE,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pSendTypePopup->SetSelColor(100, 100, 100, 100);
	m_pSendTypePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSendTypePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSendTypePopup->SetExclusiveEnableControl(TRUE);

	for(INT32 i=0; i<CLAN_SEND_NOTE_TYPE; i++)
	{
		switch(i)
		{
			case 0: m_pSendTypePopup->AddElement(GAME_STRING("STBL_IDX_CLAN_ALL_MEMBER"));/*전체*/		break;
			case 1: m_pSendTypePopup->AddElement(GAME_STRING("STBL_IDX_CLAN_STAFF_MEMBER"));/*스태프*/		break;
			case 2: m_pSendTypePopup->AddElement(GAME_STRING("STBL_IDX_CLAN_REGULAR_MEMBER"));/*일반회원*/	break;
		}
	}

	m_pSendTypePopup->SetSelectedElement(m_nTypeComboIdx, FALSE);
	m_pSendTypePopup->SetEnable(FALSE);

	CPopupBase::OnInitControl();
}

void CPopupClanSendNote::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	m_pGuiRoot->SetEnable(TRUE);

	m_pSendNoteEdit->SetIMEText("");
	m_pSendNoteEdit->SetFocus(TRUE);

	m_nTypeComboIdx = 0;
	char * pszSelect = (char*) m_pSendTypePopup->GetText(0);

	m_pSendTypeCombo->SetText( pszSelect);
}

void CPopupClanSendNote::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pGuiRoot->SetEnable(FALSE);
}

void CPopupClanSendNote::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);
}

void CPopupClanSendNote::OnPreGuiNotify(void)
{
}

BOOL CPopupClanSendNote::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		i3InputDeviceManager * pMng = (i3InputDeviceManager*) pObj;
		i3InputKeyboard * pKeyboard = pMng->GetKeyboard();

		if( m_pSendNoteEdit->isEnable())
		{
			if(pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				FocusToEdit();

				return TRUE;
			}
		}
	}

	return CPopupBase::OnEvent(event, pObj, param2, code);
}

void CPopupClanSendNote::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupClanSendNote::OnWheelUpdate(INT32 scrollby)
{
}

void CPopupClanSendNote::OnTypeComboButton(void)
{
	ExclusionPopup(m_pSendTypePopup);
}

void CPopupClanSendNote::OnTypeComboPopup(INT32 idx)
{
	m_nTypeComboIdx = idx; 

	char * pszSelect = (char*) m_pSendTypePopup->GetText(idx);

	m_pSendTypeCombo->SetText( pszSelect);
}

void CPopupClanSendNote::OnSending(void)
{
	char temp[256] = "";

	const char * pszMessage = m_pSendNoteEdit->GetText();	
	if( i3String::Length( pszMessage) <= 0)
	{
		STRING_POPUP_C( GAME_STRING("STBL_IDX_CLAN_PLEASE_INPUT"), MAKE_CALLBACK(CbNoMessage), this);/*내용을 입력하세요.*/
		return;
	}

#if !defined(LOCALE_CHINA)
	i3String::Copy( temp, (char*) m_pSendTypePopup->GetText(m_nTypeComboIdx), 256 );
	i3String::Concat(temp, GAME_STRING("STBL_IDX_CLAN_Q_SEND_TO"));/*에게 보내시겠습니까?*/
#else
	i3String::Copy(temp, GAME_STRING("STBL_IDX_CLAN_Q_SEND_TO"), 256);
	i3String::Concat(temp,(char*) m_pSendTypePopup->GetText(m_nTypeComboIdx));/*보내시겠습니까 에게 */
#endif

	STRING_POPUP_Q(temp, MAKE_CALLBACK(CbNoteSending), this);
}

void CPopupClanSendNote::OnClear(void)
{
	m_pSendNoteEdit->SetIMEText("");
	
	FocusToEdit();
}

BOOL CPopupClanSendNote::SendNoteToServer(void)
{	
	const char * pszNote = m_pSendNoteEdit->GetText();

	if( pszNote == NULL || i3String::Length(pszNote) <= 0)	return FALSE;

	I3_BOUNDCHK( m_nTypeComboIdx, (INT32)CLAN_NOTE_REGULAR+1);
	CLAN_NOTE_CLASS nType = (CLAN_NOTE_CLASS) m_nTypeComboIdx;

	//	서버로 메시지 쪽지 보냄
	g_pFramework->GetClanContext()->SendClanNote((UINT8) nType, pszNote);

	return TRUE;
}

void CPopupClanSendNote::CbNoMessage(void * pThis, INT32 nParam)
{
	((CPopupClanSendNote*)pThis)->FocusToEdit();
}

void CPopupClanSendNote::CbNoteSending(void * pThis, INT32 nParam)
{
	if( nParam == MB_OK)
	{
		//	메시지를 서버로 보내고 팝업 종료
		((CPopupClanSendNote*)pThis)->SendNoteToServer();
	}
	else
	{
		((CPopupClanSendNote*)pThis)->FocusToEdit();
	}
}
