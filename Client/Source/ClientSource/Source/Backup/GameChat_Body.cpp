#include "pch.h"
#include "GameChat_Body.h"
#include "GlobalVariables.h"
#include "GuiNotifyReceiver.h"
#include "GameMaster.h"
#include "CommunityContext.h"

static void *		s_pPointer = NULL;
static INT32 g_WhisperLength = 0;

I3_CLASS_INSTANCE( CGameChatBody);//, i3GameObjBase);

CGameChatBody::CGameChatBody(void)
{
	m_pChatList = NULL;
	m_pChatCombo = NULL;
	m_pChatButton = NULL;
	m_pChatPopup = NULL;
	m_pChatBox = NULL;
	m_pChatLang = NULL;
	m_pTextBox = NULL;
	i3mem::FillZero(m_pChatChannelButton, sizeof(m_pChatChannelButton));

	m_nChatType = CHATTING_TYPE_ALL;
	m_nOldChatType = CHATTING_TYPE_ALL;
	m_nMaxChatLineCount = 0;
	m_ChatBodyState = CHAT_BODY_STATE_LOBBY;
	m_bOnceChatType = FALSE;
	m_bOnceChatTypeAll = FALSE;
	m_nCurrentChatBufferType = CHATTING_TYPE_ALL;
	m_nCurrentSelectedChatType = 0;
}

CGameChatBody::~CGameChatBody(void)
{

}

void  CGameChatBody::InitChatBody( CHAT_BODY_STATE state, INT32 nMaxLineCount)
{
	m_ChatBodyState = state;
	m_nMaxChatLineCount = nMaxLineCount;

	_InitChat( state);
}

void  CGameChatBody::OnUpdate( REAL32 rDeltaSeconds, void* pPointers)
{
#if ! defined(DEV_BUILD)
	// 이 비교문 때문에 Step과 Phase 왔다갔다 하면서 채팅에 문제가 발생합니다.
	// 해당 Step 또는 Phase에서 자신의 this 포인터를 넘겨주는데
	// Step이 사용되어야 하는데 Step -> Phase로 변경되기 덮어씌어져서
	// 문제가 발생합니다.
	// 그러나 이 코드가 왜 있는지 모르고 사이트 이펙트가 무섭기 때문에.
	// Dev 빌드에서 테스트 하고 지우도록 하겠습니다.
	//
	// 이 비교문 때문에 클랜전에서 매치메이킹 후 클랜 롭에서 채팅 안되는 문제가 발생합니다.
	//
	if( s_pPointer != pPointers)
		return;
#endif

	if( g_pFramework->getChatBox()->GetBackUpBufferCount() < m_nMaxChatLineCount)
	{
		m_pChatList->SetSliderPos(100.0f);
		m_pChatList->setInputDisable( TRUE);
	}
	else
	{
		m_pChatList->setInputDisable( FALSE);
	}

	// 매크로
	_UpdateMacroChat();

	// 입력 언어 변경
	_UpdateLang();

	// 몰라
	_UpdateParsingType();

	// 버퍼에 출력?
	_UpdateChatBuffer();

	// 채팅 채널 업데이트
	_UpdateChatChannelType();
}

void CGameChatBody::UpdateChatFocus(void)
{
	// Enter를 눌렀을 경우 채팅입력창이 Focus상태가 아니라면, Focus상태로 합니다.
	if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER))
	{
		if( !m_pChatBox->getIsFocus())
		{
			m_pChatBox->SetFocus( TRUE);
		}
	}
}

void CGameChatBody::UpdateChatType(void)
{
	if( g_pGUIRoot->getFocusControl() != m_pChatBox) return;

	// 채팅 상태설정.
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
		{
			if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
			{
				if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
				{
					m_pChatBox->SetIMEText("");
				}

				m_bOnceChatType = TRUE;
				m_pChatBox->SetFocus( TRUE);
			}
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F2 )
	{
		if( m_nChatType != CHATTING_TYPE_ALL || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetIMEText("");
			}

			m_bOnceChatType = FALSE;
			m_bOnceChatTypeAll = FALSE;
			m_nChatType = CHATTING_TYPE_ALL;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));
		}
		m_pChatBox->SetFocus( TRUE);
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F3 )
	{
		if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
		{
			if( m_nChatType != CHATTING_TYPE_TEAM || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
			{
				if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
				{
					m_pChatBox->SetIMEText("");
				}

				m_bOnceChatType = FALSE;
				m_bOnceChatTypeAll = FALSE;
				m_nChatType = CHATTING_TYPE_TEAM;
				g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"));
			}
			m_pChatBox->SetFocus( TRUE);
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F4 )
	{
		if( m_nChatType != CHATTING_TYPE_CLAN || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetIMEText("");
			}

			m_bOnceChatType = FALSE;
			m_bOnceChatTypeAll = FALSE;
			m_nChatType = CHATTING_TYPE_CLAN;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"));
		}
		m_pChatBox->SetFocus( TRUE);
	}
}

void CGameChatBody::SetEntranceStart(void * pPointer)
{
	s_pPointer = pPointer;

	m_pTextBox->SetText("");
	m_pChatBox->SetIMEText("");
	m_pChatBox->SetIMEMode( IME_MODE_NATIVE);
	m_pChatList->Clear();

	// 채팅 상자 초기화
	g_pFramework->getChatBox()->ResetChatBuffer();

	// Update Whisper Backup Buffer
	_UpdateWhisperBackupBuffer();

	// 채팅채널 초기화
	OnChatChannel(0);
}

void CGameChatBody::OnExitEnd(void)
{
	s_pPointer = NULL;

	m_nChatType = CHATTING_TYPE_ALL;

	// 채팅 상자 초기화
	g_pFramework->getChatBox()->ResetChatBuffer();
}

void CGameChatBody::OnWhisper( INT32 slotIdx)
{
	const char * pszNick = g_pGameContext->getNickForSlot(slotIdx);

	OnWhisper(pszNick);
}

void CGameChatBody::OnWhisper( const char * pszNick)
{
	char whisperCommand[MAX_STRING_COUNT] = "";

	// 채팅창에 귓속말 커멘드 입력
	i3String::Copy( whisperCommand, "[To:", MAX_STRING_COUNT );
	i3String::Concat( whisperCommand, pszNick);
	i3String::Concat( whisperCommand, "] ");

	m_pChatBox->SetIMEText(whisperCommand);
	m_pChatBox->SetFocus(TRUE);

	m_nChatType = CHATTING_TYPE_WHISPER;
	g_WhisperLength = i3String::Length( whisperCommand );
	g_pFramework->getChatBox()->SetChatTarget(pszNick);

	m_pChatBox->SetTextColor( GameGUI::GetColor(GCT_WISPER_CHAT) );
}

void CGameChatBody::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			m_pChatList->MovePrev();
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
			m_pChatList->MoveNext();
		}
	}

	g_pFramework->getChatBox()->CalcBackupBufferByRatio(m_pChatList->getCurValue()*0.01f, m_nMaxChatLineCount);
}

void CGameChatBody::OnDragChatList(void)
{
	g_pFramework->getChatBox()->CalcBackupBufferByRatio(m_pChatList->getCurValue()*0.01f, m_nMaxChatLineCount);
}

void CGameChatBody::OnChatPopup(UINT32 idxItem)
{
	if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_TEAM;
		if( idxItem == 2) m_nChatType = CHATTING_TYPE_CLAN;
	}
	else if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_CLAN;
	}
	else if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_CLAN;
	}
	else if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_CLAN;
	}

	if( m_nChatType == CHATTING_TYPE_ALL)
	{
		g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));
	}
	else if( m_nChatType == CHATTING_TYPE_TEAM)
	{
		g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"));
	}
	else if( m_nChatType == CHATTING_TYPE_CLAN)
	{
		g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"));
	}
}

void CGameChatBody::OnChatChannel(UINT32 idxItem)
{
	m_nCurrentSelectedChatType = idxItem;

	switch( idxItem)
	{
	case 0:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				m_nCurrentChatBufferType = CHATTING_TYPE_LOBBY;
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_ALL;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				m_nCurrentChatBufferType = CHATTING_TYPE_MATCH;
				break;
			case CHAT_BODY_STATE_CLAN_MEMBER:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN_MEMBER_PAGE;
				break;
			}
		}
		break;
	case 1:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_TEAM;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			case CHAT_BODY_STATE_CLAN_MEMBER:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			}
		}
		break;
	case 2:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			case CHAT_BODY_STATE_CLAN_MEMBER:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			}
		}
		break;
	case 3:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				break;
			}
		}
		break;
	default:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				break;
			case CHAT_BODY_STATE_READYROOM:
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				break;
			}
		}
		break;
	}
}

INT64 CGameChatBody::FindUserUIDByNick(char* nick)
{
	char szTemp[512];
	// 자신은 막는다.
	if ( g_pGameContext->IsMyNickName(nick) )
	{
		i3String::Copy( szTemp, GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"), 512 );//자신의 닉네임을 입력하셨습니다.
		g_pFramework->getChatBox()->PutSystemChatting(szTemp);
		return 0;
	}

	// 친구 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID 검출
			return node->_uid;
		}
	}

	// 클랜 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i);

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID 검출
			return node->_uid;
		}
	}

	return 0;
}


//Protect
void CGameChatBody::_InitChat(CHAT_BODY_STATE state)
{
	m_pChatList->CreateTextEx(m_nMaxChatLineCount,  GetDefaultFontName(), FONT_COUNT_256);
	m_pChatList->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pChatList->SetSelColor(0, 0, 0, 0);
	m_pChatList->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));

	for(INT32 i = 0; i < m_nMaxChatLineCount; i++)
	{
		m_pChatList->AddElement("");
	}
	m_pTextBox->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_2048, GAME_FONT_CHAT_SIZE, FALSE, NULL, FW_NORMAL, TRUE);	

	m_pTextBox->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));	
	m_pTextBox->SetTextAutoWrap( TRUE);
	m_pTextBox->SetTextAlign( ALIGN_LEFT, ALIGN_BOTTOM);
	m_pTextBox->SetWordLineFeed( FALSE);
	m_pTextBox->SetOutCharaProcess( TRUE);
#if defined(GUI_ADJUST_RUSSIA)
	m_pTextBox->SetOffsetPos(3, 0);
#endif

	REAL32 ry = i3GuiRoot::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;	
	m_pTextBox->SetTextSpace( 0, (INT32)(5 * ry));

	m_pChatLang->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_CHAT_SIZE);

	switch(GetCodePage())
	{
	case I3_LANG_CODE_KOREAN:
		m_pChatLang->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_KOREAN"));/*한*/
		break;
	default :
		m_pChatLang->SetCaption("");
		break;
	}	
	m_pChatLang->SetCaptionColor(GameGUI::GetColor(GCT_FOCUS));

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = ROOM_CHAT_SIZE;
	ime._StyleIME = 0;

	m_pChatBox->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_CHAT_SIZE, TRUE, &ime, FW_NORMAL, TRUE);

	m_pChatBox->SetTextAlign(ALIGN_LEFT, ALIGN_TOP);
	m_pChatBox->SetTextAutoWrap(FALSE);
	m_pChatBox->SetOffsetPos(0.0f, 2.0f);
	m_pChatBox->setCaretPosOffset(0.0f, 2.0f);	
	m_pChatBox->SetIMEMode( IME_MODE_NATIVE);
	m_pChatBox->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));

	g_pFramework->getChatBox()->ResetChatBuffer();
	m_pChatBox->SetIMEText("");

	m_pChatCombo->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pChatCombo->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));
	m_pChatCombo->SetOffsetPos(2, 0);

	INIT_COMBO_EX(m_pChatButton);	

	INIT_COMBO_EX( m_pChatChannelButton[0]);
	INIT_COMBO_EX( m_pChatChannelButton[1]);
	INIT_COMBO_EX( m_pChatChannelButton[2]);
	INIT_COMBO_EX( m_pChatChannelButton[3]);

	for(INT32 i=0; i<4; i++)
	{
		m_pChatChannelButton[i]->CreateCaption(  GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pChatChannelButton[i]->SetEnable( FALSE);
	}

	// 각 type마다 초기 설정치를 배정합니다.
	switch(state)
	{
	case CHAT_BODY_STATE_LOBBY:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHANNEL_WHISPER"));/*귓속말*/
		break;
	case CHAT_BODY_STATE_READYROOM:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_TEAM"));/*팀*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[3]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_TEAM"));/*팀*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[3]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_WHISPER_TEXT"));/*귓말*/
		break;
	case CHAT_BODY_STATE_CLAN_MATCH:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHANNEL_WHISPER"));/*귓속말*/
		break;
	case CHAT_BODY_STATE_CLAN_MEMBER:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*전체*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*클랜*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHANNEL_WHISPER"));/*귓속말*/
		break;
	}

	m_pChatPopup->SetSelColor(100, 100, 100, 100);
	m_pChatPopup->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));
	m_pChatPopup->SetExclusiveEnableControl(TRUE);
	m_pChatPopup->SetEnable(FALSE);
}

void CGameChatBody::_UpdateMacroChat(void)
{
	// Macro를 이용한 입력처리
	CHAT_INFO info;

	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_1 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[0]) > 0)
			{	
				if( g_pEnvSet->m_bMacroEnableFriend[0])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				// 욕설필터
				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[0], 1))
				{

					if( g_pEnvSet->m_bMacroEnableFriend[0])	info._Type = CHATTING_TYPE_TEAM;	// 팀
					else									info._Type = CHATTING_TYPE_ALL;		// 전체

					info.setString( g_pEnvSet->m_szMacro[0]);

					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_2 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[1]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[1])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[1], 1))
				{
					if( g_pEnvSet->m_bMacroEnableFriend[1])	info._Type = CHATTING_TYPE_TEAM;	// 팀
					else									info._Type = CHATTING_TYPE_ALL;		// 전체

					info.setString( g_pEnvSet->m_szMacro[1]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_3 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[2]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[2])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[2]), 1)
				{
					if( g_pEnvSet->m_bMacroEnableFriend[2])	info._Type = CHATTING_TYPE_TEAM;	// 팀
					else									info._Type = CHATTING_TYPE_ALL;		// 전체

					info.setString( g_pEnvSet->m_szMacro[2]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_4 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[3]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[3])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[3], 1))
				{
					if( g_pEnvSet->m_bMacroEnableFriend[3])	info._Type = CHATTING_TYPE_TEAM;	// 팀
					else									info._Type = CHATTING_TYPE_ALL;		// 전체

					info.setString( g_pEnvSet->m_szMacro[3]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_5 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[4]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[4])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[4], 1))
				{
					if( g_pEnvSet->m_bMacroEnableFriend[4])	info._Type = CHATTING_TYPE_TEAM;	// 팀
					else									info._Type = CHATTING_TYPE_ALL;		// 전체

					info.setString( g_pEnvSet->m_szMacro[4]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
	}
}

void CGameChatBody::_UpdateLang(void)
{
	// IME모드에 따라서 컨트롤을 변경합니다.
	const char * pszLang = "?";

	// IME모드에 따라서 컨트롤을 변경합니다.

	LANGID langId = m_pChatBox->getIME()->GetLangId();
	UINT32 nInputLang = PRIMARYLANGID( langId);
	UINT32 nInputSubLang = SUBLANGID( langId);

	// mode off is english
	if(m_pChatBox->getIME()->GetImeMode() != IME_MODE_NATIVE)
		nInputLang = LANG_ENGLISH;

	// EXPORT_AVALIABLE_CODE_VALUE
	//	Language mark
#if defined ( LOCALE_NORTHAMERICA)
	pszLang = "";	//	북미의 요청으로 안보이게 처리
#else
	switch(GetCodePage())
	{
	case I3_LANG_CODE_KOREAN:
		{
			switch( nInputLang)
			{
			case LANG_KOREAN:	pszLang = GAME_STRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_KOREAN");/*한*/	break;
			case LANG_ENGLISH:	pszLang = GAME_STRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_ENGLISH");/*영*/	break;
			}
		}
		break;
	case I3_LANG_CODE_THAI:
		{
			switch( nInputLang)
			{
			case LANG_THAI:		pszLang = "TH";		break;
			case LANG_ENGLISH:	pszLang = "EN";		break;
			}
		}
		break;
	case I3_LANG_CODE_JAPANESE:
		{
			switch( nInputLang)
			{
			case LANG_JAPANESE:	pszLang = "JP";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:
	case I3_LANG_CODE_CHINESE_TRADITIONAL:
		{
			switch( nInputLang)
			{
			case LANG_CHINESE:	pszLang = "CH";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_RUSSIA:
		{
			switch( nInputLang)
			{
			case LANG_RUSSIAN:	pszLang = "RU";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_TURKISH:
		{
			switch( nInputLang)
			{
			case LANG_TURKISH:	pszLang = "TR";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_WESTEUROPE:
		{
			switch( nInputLang)
			{
			case LANG_PORTUGUESE:	
				{
					switch( nInputSubLang)
					{
						case SUBLANG_PORTUGUESE:			pszLang = "PR";	break;
						case SUBLANG_PORTUGUESE_BRAZILIAN:	pszLang = "BR";	break;
					}
				}
				break;
			case LANG_SPANISH:	pszLang = "SP";	break;
			case LANG_ITALIAN:		pszLang = "IT";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_ARABIC:
		{
			switch( nInputLang)
			{
			case LANG_ARABIC:	pszLang = "AR";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	}
#endif

	m_pChatLang->SetCaption( pszLang);
}

void CGameChatBody::_UpdateParsingType(void)
{
	COLOR col;
	CHAT_SYS_TYPE ChatType = CHAT_SYS_TYPE_NONE;

	char szTarget[256];
	char szHistory[256];
	szTarget[0] = 0;
	szHistory[0] = 0;

	INT32 nWhsiperTextCount = 0;
	static BOOL	bUseHistory = FALSE;

	if( m_bOnceChatTypeAll)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
		m_pChatCombo->SetTextColor( &col);
		m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));
		i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
	}
	else if( m_bOnceChatType)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
		m_pChatCombo->SetTextColor( &col);
		m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));
		i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
	}
	else
	{
		switch( m_nChatType)
		{
		case CHATTING_TYPE_ALL:			// 전체
		case CHATTING_TYPE_MATCH:
		case CHATTING_TYPE_CLAN_MEMBER_PAGE:
			i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
			i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
			m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));
			break;
		case CHATTING_TYPE_TEAM:		// 팀
			i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
			i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
			m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));
			break;
		case CHATTING_TYPE_CLAN:		// 클랜
			i3Color::Set(&col, GameGUI::GetColor(GCT_CLAN_CHAT));
			i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
			m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_CLAN"));
			break;
		case CHATTING_TYPE_WHISPER:		// 귓말
			i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
			break;
		case CHATTING_TYPE_REPLY:		// 답변
			i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
			break;
		default: break;
		}
	}

	szTarget[i3String::Length( szTarget)] = 0;

	g_pFramework->getChatBox()->SetEnable( TRUE);
	m_pChatBox->SetTextColor( &col);


	if( m_nChatType != CHATTING_TYPE_WHISPER && m_nChatType != CHATTING_TYPE_REPLY)
	{
		ChatType = g_pFramework->getChatBox()->ParsingText( m_pChatBox->getIMETextBuf());
		{
			switch( ChatType)
			{
			case CHAT_SYS_TYPE_WHISPER:
				{
					char szWhisper[128];
					if( !bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_WHISPER;
					i3String::Copy( szWhisper, "[To:", 128 );
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatTarget());
					i3String::Concat( szWhisper, "] ");
					szWhisper[i3String::Length( szWhisper)] = 0;
					g_WhisperLength = i3String::Length( szWhisper);
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetHistoryText());
					m_pChatBox->SetIMEText( szWhisper);
				}
				break;
			case CHAT_SYS_TYPE_REPLY:
				{
					char szWhisper[128];
					if( !bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_REPLY;
					if( i3String::Length(g_pFramework->getChatBox()->GetChatReplyTarget()) > 0)
					{
						i3String::Copy( szWhisper, "[To:", 128 );
						i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatReplyTarget());
						i3String::Concat( szWhisper, "] ");
						szWhisper[i3String::Length( szWhisper)] = 0;
						g_WhisperLength = i3String::Length( szWhisper);
						i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetHistoryText());
						m_pChatBox->SetIMEText( szWhisper);
					}
					else
					{
						m_nChatType = m_nOldChatType;
						m_pChatBox->SetIMEText("");
					}
				}
				break;
			case CHAT_SYS_TYPE_ONETIME_TEAM:	// 일회성 팀 챗팅!!
				if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
				{
					m_bOnceChatType = TRUE;
					m_pChatBox->SetIMEText("");
				}
				break;
			}
		}
	}

	if( m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
	{
		// 귓속말 상태에서 아무런 텍스트가 입력되지 않은상태에서 글자가 지워지면 초기화합니다.
		if( m_nChatType == CHATTING_TYPE_WHISPER)
		{
			char szWhisper[128];
			i3String::Copy( szWhisper, "[To:", 128 );
			i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatTarget());
			i3String::Concat( szWhisper, "] ");

			const char* szContents = m_pChatBox->getIMETextBuf();

			if( i3String::Contain( szContents, szWhisper) == -1)
			{
				m_pChatBox->SetIMEText("");
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;
			}
		}

		if( m_nChatType == CHATTING_TYPE_REPLY)
		{
			char szWhisper[128];
			i3String::Copy( szWhisper, "[To:", 128 );
			i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatReplyTarget());
			i3String::Concat( szWhisper, "] ");

			const char* szContents = m_pChatBox->getIMETextBuf();

			if( i3String::Contain( szContents, szWhisper) == -1)
			{
				m_pChatBox->SetIMEText("");
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;
			}
		}
	}

	// Chat History기능
	if( g_pFramework->getChatBox()->GetHistoryBufferCount() > 0)
	{
		i3InputKeyboard* pKeyboard = g_pFramework->getKeyboard();

		char szText[256];
		szText[0]=0;
		INT32 nChatMode = CHATTING_TYPE_ALL;

		if (pKeyboard->GetStrokeState(I3I_KEY_UP))
		{
			m_pChatBox->SetFocus( TRUE);

			bUseHistory = TRUE;

			if( g_pFramework->getChatBox()->GetHistoryBufferOffset() == 0)
			{
				m_nOldChatType = m_nChatType;
			}

			g_pFramework->getChatBox()->GetHistoryBufferText(1, szText, 256, &nChatMode);

			if(szText[0] != NULL)
			{
				m_pChatBox->SetIMEText( szText);
				m_nChatType = nChatMode;
			}
			else
			{
				m_pChatBox->SetIMEText( "");
			}
		}
		else if (pKeyboard->GetStrokeState(I3I_KEY_DOWN))
		{
			m_pChatBox->SetFocus( TRUE);

			bUseHistory = TRUE;

			g_pFramework->getChatBox()->GetHistoryBufferText(-1, szText, 256, &nChatMode);

			if(szText[0] != NULL)
			{
				m_pChatBox->SetIMEText( szText);
				m_nChatType = nChatMode;
			}
			else
			{
				bUseHistory = FALSE;
				m_pChatBox->SetIMEText( "");
				m_nChatType = m_nOldChatType;
			}
		}
	}


	char szTemp[256] = {};
	char szTemp2[256] = {};

	BOOL bWhisper = FALSE;
	BOOL bReply = FALSE;
	INT32 nReplyTextCount = 0;




	// IME 에디트 입력받는 부분은 받드시 상태를 체크해서 처리해야 됩니다. komet 09.06
	// IME를 처리하는 Windows API Proc은 메시지 큐잉 방식으로 처리하기 때문입니다.
	if( ! GUI_IME_CONFIRM(m_pChatBox))
		return;

	switch( ChatType)
	{
	case CHAT_SYS_TYPE_CLEAR:
		g_pFramework->getChatBox()->ResetChatBuffer();
		m_pChatBox->SetIMEText("");
		break;
	case CHAT_SYS_TYPE_HELP:
		g_pFramework->getChatBox()->SetHelpText();
		m_pChatBox->SetIMEText("");
		break;
	}
	const char * ChatText = m_pChatBox->getIMETextBuf();

	// GM용 방 명령어 체크.
	if ( m_ChatBodyState == CHAT_BODY_STATE_READYROOM  )
	{
		if ( SetGMCommand(ChatText) )
			m_pChatBox->SetIMEText("");
	}

	// GM용 로비 명령어 체크.
	if ( m_ChatBodyState == CHAT_BODY_STATE_LOBBY )
	{
		if ( SetGMLobbyCommand(ChatText) )
			m_pChatBox->SetIMEText("");
	}
 
	// GM 명령어는 채팅메시지로 뿌리지 않습니다.
	if ( g_pGameContext->IsGameMaster() == TRUE && GM_COMMAND_UNKNOWN != GetCommandType(ChatText) )
		m_pChatBox->SetIMEText("");


	if( i3String::Length(ChatText) <= 0 || i3String::ContainNoCase(ChatText, "{col:") >= 0)
	{
		m_pChatBox->InputDone();
		return;
	}

	if(g_pFramework->AddFilterMessage(m_pChatBox->getIMETextBuf(), 1))
	{
		// 필터링 단어 *로 변경
		g_pFramework->IsFilterChange(ChatText);

		// 네트워크에 메시지를 보냅니다.
		CHAT_INFO info;
		info._Type = (UINT16)m_nChatType;

		if( m_bOnceChatTypeAll)
		{
			i3String::Copy( szTemp, ChatText, 256 );
		}
		else if( m_bOnceChatType)
		{
			i3String::Copy( szTemp, ChatText, 256 );
			info._Type = CHATTING_TYPE_TEAM;
		}
		else
		{

			switch( m_nChatType)
			{
			case CHATTING_TYPE_ALL:	// 전체
				i3String::Copy( szTemp, ChatText, 256 );
				break;
			case CHATTING_TYPE_TEAM:// 팀
				i3String::Concat( szTemp, ChatText );
				break;
			case CHATTING_TYPE_CLAN:// 클랜
				i3String::Copy( szTemp, ChatText, 256 );
				break;
			case CHATTING_TYPE_WHISPER: // 귓말
				i3String::Copy( szTemp2, "{col:237, 86, 250, 255}", 256 );
				i3String::Concat( szTemp2, ChatText);
				i3String::Concat( szTemp2, "{/col}");

				i3String::Copy( szTemp, ChatText + g_WhisperLength, 256 );

				nWhsiperTextCount = i3String::Length( ChatText + g_WhisperLength);
				bWhisper = TRUE;
				m_nChatType = m_nOldChatType;
				break;
			case CHATTING_TYPE_REPLY:// 답변
				i3String::Copy( szTemp2, "{col:237, 86, 250, 255}", 256 );
				i3String::Concat( szTemp2, ChatText);
				i3String::Concat( szTemp2, "{/col}");

				i3String::Copy( szTemp, ChatText + g_WhisperLength, 256 );

				nReplyTextCount = i3String::Length( ChatText + g_WhisperLength);
				bReply = TRUE;
				m_nChatType = m_nOldChatType;
				break;
			default: break;
			}
		}

		szTemp[i3String::Length( szTemp)] = 0;
		szTemp2[i3String::Length( szTemp2)] = 0;

		// 귓말의 경우 다른 방식으로 처리해야 합니다.
		if( bWhisper)
		{
			if( nWhsiperTextCount > 0)
			{
				g_pFramework->getChatBox()->PutSystemChatting(szTemp2);

				// 친구 목록에 있는 닉네임인 경우 해당 UID를 찾아서 UID로 보낸다.
				INT64 ReceiverUID = FindUserUIDByNick(g_pFramework->getChatBox()->GetChatTarget());
				if (0 < ReceiverUID)
				{
					// TODO:채팅 입력
					// Send Event
					g_pGameContext->SetEvent( EVENT_SEND_WHISPER_FIND_UID, g_pFramework->getChatBox()->GetChatTarget(), szTemp, &ReceiverUID);

					i3String::Copy( szHistory, "/w ", 256 );
					strcat( szHistory, g_pFramework->getChatBox()->GetChatTarget());
					strcat( szHistory, " ");
					strcat( szHistory, (ChatText + g_WhisperLength));
					szHistory[i3String::Length( szHistory)] = 0;
					g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
				}
				else
				{
					// TODO:채팅 입력
					// Send Event
					g_pGameContext->SetEvent( EVENT_SEND_WHISPER, g_pFramework->getChatBox()->GetChatTarget(), szTemp);

					i3String::Copy( szHistory, "/w ", 256 );
					strcat( szHistory, g_pFramework->getChatBox()->GetChatTarget());
					strcat( szHistory, " ");
					strcat( szHistory, (ChatText + g_WhisperLength));
					szHistory[i3String::Length( szHistory)] = 0;
					g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
				}
			}
		}
		else if( bReply)
		{
			if( nReplyTextCount > 0)
			{
				// 친구 목록에 있는 닉네임인 경우 해당 UID를 찾아서 UID로 보낸다.


				// TODO:채팅 입력
				// Send Event
				g_pFramework->getChatBox()->PutSystemChatting(szTemp2);
				g_pGameContext->SetEvent( EVENT_SEND_WHISPER, g_pFramework->getChatBox()->GetChatReplyTarget(), szTemp);

				i3String::Copy( szHistory, "/r ", 256 );
				strcat( szHistory, " ");
				strcat( szHistory, ChatText + g_WhisperLength);
				szHistory[i3String::Length( szHistory)] = 0;
				g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
			}
		}
		else
		{
			info.setString( szTemp);

			if( info._Type == CHATTING_TYPE_CLAN)	// 클랜 채팅이라면..
			{
				// 클랜 미가입 상태입니다.
				USER_INFO_BASIC		MyUserInfoBasic;
				g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
				if(MyUserInfoBasic._clanidx == 0 || MyUserInfoBasic._clanstate == 0)
				{
					g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_CHAT_INGAME_NOTICE_NO_JOIN_CLAN"));/*<클랜에 미가입한 상태입니다.>*/
				}
				else
				{
					// TODO:채팅 입력
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info );
					g_pFramework->getChatBox()->PutHistory(ChatText, m_nChatType);
				}
			}
			else
			{
				if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH)	//	클랜전 로비에서 예외
					info._Type = CHATTING_TYPE_MATCH;

				if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)	//	클랜 가입자 페이지에서 예외
					info._Type = CHATTING_TYPE_CLAN_MEMBER_PAGE;

				// 커뮤니티 옵션에서의 채팅 제한이 걸려있다면, 그것에 대한 처리를 합니다.
				if( g_pEnvSet->m_nChatType != 0 && (m_nChatType != CHATTING_TYPE_TEAM && m_nChatType != CHATTING_TYPE_CLAN) )
				{
					i3String::Copy( szTemp2, "[", 256 );
					i3String::Concat( szTemp2, g_pGameContext->GetMyNickName());
					i3String::Concat( szTemp2, "] ");
					i3String::Concat( szTemp2, szTemp);

					g_pFramework->getChatBox()->PutSystemChatting(szTemp2);
					g_pFramework->getChatBox()->PutHistory(ChatText, m_nChatType);
				}
				else
				{
					// TODO:채팅 입력
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info );
					g_pFramework->getChatBox()->PutHistory(ChatText, m_nChatType);
				}
			}
		}

		g_WhisperLength = 0;
	}

	g_pFramework->getChatBox()->ResetHistoryBufferIndex();

	m_pChatBox->SetIMEText("");

	m_bOnceChatType = FALSE;
	m_bOnceChatTypeAll = FALSE;

	if( bUseHistory) m_nChatType = m_nOldChatType;
	bUseHistory = FALSE;

	m_pChatBox->InputDone();

}

void CGameChatBody::_UpdateChatBuffer(void)
{
	// dummy chat text 추가
	UINT32 nListCount = m_pChatList->GetElementCount();
	CGameChatBox * pFrameWorkChatBox = g_pFramework->getChatBox();
	UINT32 nBufferCount = pFrameWorkChatBox->GetBufferCount();

	for(UINT32 j = nListCount; j < nBufferCount; j++)
	{
		m_pChatList->AddElement("");
	}


	INT32 BufferIdx= 0;
	if( pFrameWorkChatBox->GetBackUpBufferIndex() == 0)
	{
		BufferIdx = MAX_CHAT_BUFFER-1;
	}
	else
	{
		BufferIdx = pFrameWorkChatBox->GetBackUpBufferIndex() - 1;
	}


	// Buffer를 돌면서 출력할 글자를 담습니다.
	INT32 nAccumCount = 0;
	INT32 nLoopCount = 0;
	char szTotalText[2048] = {0,};
	char szChatBoxText[2048] = {0,};

	for( INT32 i = BufferIdx; nLoopCount < m_nMaxChatLineCount; i--)
	{
		nAccumCount++;

		INT32	ChatBoxBufCount = pFrameWorkChatBox->GetBufferCount();
		if( nLoopCount > ChatBoxBufCount || ChatBoxBufCount <= 0 || nAccumCount > ChatBoxBufCount)
		{
			break;
		}

		if( i<0)
		{
			i = MAX_CHAT_BUFFER-1;
		}

		if( m_nCurrentChatBufferType == CHATTING_TYPE_ALL || m_nCurrentChatBufferType == CHATTING_TYPE_LOBBY ||  m_nCurrentChatBufferType == CHATTING_TYPE_MATCH ||  m_nCurrentChatBufferType == CHATTING_TYPE_CLAN_MEMBER_PAGE)
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
		else if( m_nCurrentChatBufferType == CHATTING_TYPE_TEAM && ( pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_TEAM || pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_CLAN) )
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
		else if( m_nCurrentChatBufferType == CHATTING_TYPE_WHISPER && ( pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_WHISPER || pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_REPLY) )
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
		else if( m_nCurrentChatBufferType == pFrameWorkChatBox->getBufferType(i) || pFrameWorkChatBox->getBufferType(i) == 0)
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
	}

	INT32 nLen = i3String::Length( szTotalText);
	if( nLen > 0) szTotalText[nLen-1] = 0;

	// chat list를 출력합니다.
	m_pTextBox->SetText( szTotalText);
}

void CGameChatBody::_UpdateWhisperBackupBuffer(void)
{
	for( INT32  i = 0 ; i < g_pFramework->getChatBox()->GetWhisperBufferCount() ; i++)
	{
		INT32 index = (g_pFramework->getChatBox()->GetCurrentWhisperBufferIndex() + i) % 5;
		g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatBox()->GetWhisperBufferText(index), CHATTING_TYPE_WHISPER);
	}

	g_pFramework->getChatBox()->ResetWhisperBuffer();
}

void CGameChatBody::_UpdateChatChannelType(void)
{
	m_pChatChannelButton[m_nCurrentSelectedChatType]->SetShapeEnable( 1, TRUE);
}

//20110425 하동익 중국 필터링 관련
/*
void CGameChatBody::ReConstructText(char *src, int AbuseWordCount)
{
	int j,k,S_Length,A_Length,count,Buf_Count,Buf[100];
	char *AbuseWord;
	for(int i=0; i<AbuseWordCount; i++)
	{
		AbuseWord = g_pGameContext->GetAbuseWord(i);
		S_Length = strlen(src);
		A_Length = strlen(AbuseWord);
		j = 0;
		k = 0;
		count = 0;
		Buf_Count = 0;
		while(1)
		{
			 //대문자일 경우 소문자로 변환 :: AbusingWord는 Lower 케이스.
			 //차후 기능 확장가능 다른 나라인 경우....
			char T_Char = src[j];
			if(T_Char >= 65 && T_Char <=90)
			{
				T_Char += 32;
			}


			if(T_Char == AbuseWord[k])
			{
				Buf[Buf_Count++] = j;
				count++;
				k++;
				j++;
			}
			else if(T_Char == ' ')
			{
				//공백일 경우 그냥 포인터 이동
				j++;
			}
			else
			{
				//일치하지 않을 경우 버프 카운터 초기화
				Buf_Count = 0;
				j++;
			}

			//루프 아웃 관련
			if(count == A_Length && j < S_Length - 1)
			{
				// 저장된 Word값 *로 변환(늦게할 경우 무한루프)
				for(int u = 0; u < Buf_Count; u++)
				{
					src[Buf[u]] = '*';
				}
				count = Buf_Count = k = 0;
			}
			else if(count == A_Length && j == S_Length)
			{
				// AbusingWord존재하고, 문자열도 탐색이 완료된 경우
				// 저장된 Word값 *로 변환(늦게할 경우 무한루프)
				for(int u = 0; u < Buf_Count; u++)
				{
					src[Buf[u]] = '*';
				}
				break;
			}
			else if(count < A_Length && j == S_Length)
			{
				break;
			}
		}
	}
	g_pGameContext->ClearAbuseWordInfo();
}
*/