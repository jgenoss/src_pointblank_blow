#include "pch.h"
#include "UIPopupGMOption.h"
#include "UIMainFrame.h"
#include "GMChatBuffer.h"
#include "UserInfoContext.h"
#include "UIUtil.h"
#include "../GMAuthorityContext.h"
#include "../CommunityContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "GlobalInfoDef.h"
#include <fstream>

extern "C" {
	int UIPopupGMOption_CopyData( LuaState * L)				// 데이터 복사
	{
		i3UIEditBox * pEdit = reinterpret_cast<i3UIEditBox*>(i3Lua::GetUserDataArg(L , 1));
		
		UIPopupGMOption * p = static_cast<UIPopupGMOption*>( GameUI::GetPopup(UPW_GM_OPTION) );
		if( p != nullptr)
			p->CopyData(pEdit);
		return 0;
	}

	int UIPopupGMOption_PenaltyIndex( LuaState * L)
	{
		INT32 _PenaltyIndex = i3Lua::GetIntegerArg(L, 1);

		UIPopupGMOption * p = static_cast<UIPopupGMOption*>( GameUI::GetPopup(UPW_GM_OPTION) );
		if( p != nullptr)
			p->UserPenaltyApply(_PenaltyIndex);
		return 0;
	}
};

LuaFuncReg UIPopupGMOption_Glue[] = {
	{"CopyData",					UIPopupGMOption_CopyData},
	{"PenaltyIndex",				UIPopupGMOption_PenaltyIndex},
	{nullptr,							nullptr}
};


I3_CLASS_INSTANCE( UIPopupGMOption);//, UIPopupBase);

UIPopupGMOption::UIPopupGMOption() : m_pChatLogBox(nullptr),
m_pChatEdit(nullptr)
{
	m_pChatBuffer = new CGMChatBuffer();
	MEMDUMP_NEW( m_pChatBuffer, sizeof(CGMChatBuffer) );

	RegisterGameEventFunc( EVENT_GM_AUTHORITY_POPUP_OPEN,	&UIPopupGMOption::ReceiveGameEvent_GMPopup_Open);
	RegisterGameEventFunc( EVENT_GM_AUTHORITY_CHAT_RECEIVE, &UIPopupGMOption::ReceiveGameEvent_Chat_Message_Receive);
}

UIPopupGMOption::~UIPopupGMOption()
{
	I3_SAFE_DELETE( m_pChatBuffer);
}

void UIPopupGMOption::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPopupGMOption::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIPopupGMOption::__CreateChatLogFile(void)
{
	wchar_t szCurDir[ MAX_PATH ] = L"";

	SHGetSpecialFolderPathW(nullptr, szCurDir, CSIDL_PERSONAL, false);

	i3::safe_string_copy(m_strChatFilePath, szCurDir, MAX_PATH);

	i3::generic_string_cat(m_strChatFilePath, L"\\");
	i3::generic_string_cat(m_strChatFilePath, GAME_TITLE_NAME_W);
	FileUtil::create_directory( m_strChatFilePath );

	i3::generic_string_cat(m_strChatFilePath, L"\\GM_chat_log");
	FileUtil::create_directory( m_strChatFilePath );

	// 날짜
	i3::generic_string_cat(m_strChatFilePath, L"\\");
	i3::generic_string_cat(m_strChatFilePath, FileUtil::wstring_by_date(L"_") );

	// 시간
	i3::generic_string_cat(m_strChatFilePath, L"_");
	i3::generic_string_cat(m_strChatFilePath, FileUtil::wstring_by_time(L"_") );

	// 유저 닉네임
	i3::generic_string_cat(m_strChatFilePath, L"_");
	i3::generic_string_cat(m_strChatFilePath, m_wstrUserNick );

	// 확장자
	i3::generic_string_cat(m_strChatFilePath, L".txt");

	std::wofstream ofs(m_strChatFilePath.c_str());
	ofs.close();
}

void UIPopupGMOption::__WriteChatLogFile(const i3::wstring& wstr)
{
	// 첫 생성
	if( m_pChatBuffer->isFirstChat() )
		__CreateChatLogFile();

	// 근데 경로가 없으면 생성을 못합니다.
	if(m_strChatFilePath.length() == 0)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, L"Failed - Create Chatting Log File");
		return;
	}

	// 경로는 있는데 파일이 없으면 생성
	if( !FileUtil::is_exist_file(m_strChatFilePath) )
	{
		__CreateChatLogFile();

		// 그래도 없으면 저장 못합니다.
		if( !FileUtil::is_exist_file(m_strChatFilePath) )
		{
			i3::rc_wstring wstrError; 
			i3::sprintf(wstrError, L"Could Not Find File [%s]", m_strChatFilePath );
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrError);
			return;
		}
	}

	std::wofstream ofs(m_strChatFilePath.c_str(), (std::ios_base::out | std::ios_base::app) );
	ofs.write( wstr.c_str(), wstr.size() );
	ofs.write(L"\n", 1);
	ofs.close();	
}

void UIPopupGMOption::_SetUserInfo(void)
{
	const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();

	// 정보가 없으면 리턴합니다.
	if(pInfo->_UserInfo.m_i64FindUID <= 0)
		return;

	UINT32 rank = pInfo->_UserInfo.m_rank;
	m_wstrUserNick = GMAuthorityContext::i()->GetUserNickName();


	char szUID[MAX_STRING_COUNT] = {0};
	INT64 _UID = GMAuthorityContext::i()->GetUserUID();
	_i64toa(_UID, szUID, 10);
	
	LuaState * L = _CallLuaFunction("SetUserInfo");

	if(L != nullptr)
	{
		i3Lua::PushInteger(L, rank);
		i3Lua::PushStringUTF16To8(L, m_wstrUserNick);
		i3Lua::PushString(L, szUID);					// TODO : 일단 UTF-8 호환성이 있다고 간주해준다. (2014.07.23.수빈)

		_EndLuaFunction(L , 3);
	}
}


void UIPopupGMOption::_SendChatMessage(void)
{
	const i3::wstring& wstrImeBuf = m_pChatEdit->getIMETextBufString();

	i3::wstring wstr_chatMsg;

	if(wstrImeBuf.length() > 0)
	{
		wstr_chatMsg = L"[";
		i3::generic_string_cat( wstr_chatMsg, UserInfoContext::i()->GetMyNickName());
		wstr_chatMsg += L"] ";
		i3::generic_string_cat( wstr_chatMsg, wstrImeBuf);

		// 채팅 내용 파일로 기록
		__WriteChatLogFile(wstr_chatMsg);

		// 채팅 버퍼에 기록
		m_pChatBuffer->PutChatting(wstr_chatMsg);

		T_UID _UID = GMAuthorityContext::i()->GetUserUID();

		UINT16 _Size = (UINT16)wstr_chatMsg.size();

		// 채팅 메세지 전송
		GameEventSender::i()->SetEvent(EVENT_GM_AUTHORITY_CHAT_SEND, &_UID, &_Size, wstr_chatMsg.c_str() );

		// EditBox 초기화
		m_pChatEdit->SetText( L"");
		m_pChatEdit->GetIME()->SetInputText( L"", 0);

		_UpdateChatLogBox();
	}
}

void UIPopupGMOption::_UpdateChatLogBox(void)
{
	if(m_pChatLogBox == nullptr)
		return;

	i3::wstring wstrChatLog;

	m_pChatBuffer->CopyChatBuffer(wstrChatLog);
	m_pChatLogBox->SetText(wstrChatLog);
}


/*virtual*/ void UIPopupGMOption::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pChatLogBox = i3::same_cast<i3UIEditBox*>(pScene->FindChildByName( "Edit_ChatLog"));
	I3ASSERT( m_pChatLogBox != nullptr);

	m_pChatEdit = i3::same_cast<i3UIEditBox*>(pScene->FindChildByName( "Edit_Chat"));
	I3ASSERT( m_pChatEdit != nullptr);
}

/*virtual*/ void UIPopupGMOption::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	m_pChatLogBox = nullptr;
	m_pChatEdit = nullptr;
	m_pChatBuffer->ResetBuffer();
	m_wstrUserNick.clear();
	m_strChatFilePath.clear(); 
}


/*virtual*/ void UIPopupGMOption::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	
	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_GMOption.i3UIs", UIPopupGMOption_Glue);
}

/*virtual*/ bool UIPopupGMOption::OnEntranceStart(void * pArg1 , void * pArg2 )
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	GetSlide()->ForceEnd();
	return true;
}

/*virtual*/ void UIPopupGMOption::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();
}

/*virtual*/ void UIPopupGMOption::OnExitEnd(void)
{
	UIPopupBase::OnExitEnd();

	T_UID i64UserUID = GMAuthorityContext::i()->GetUserUID();

	GameEventSender::i()->SetEvent(EVENT_GM_AUTHORITY_POPUP_CLOSE, &i64UserUID);
}

/*virtual*/ bool UIPopupGMOption::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	bool bRv = UIPopupBase::OnEvent( event, pObj, param2, code);

	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_CLEAR					:
		case EDIT_STATE_CHANGE_COMPOSITION_CHAR :
		case EDIT_STATE_CHANGE_INPUT_CHAR		:	break;
		case EDIT_STATE_RETURN					:	
			{
				if(m_pChatEdit->isFocused())	
					_SendChatMessage();	
			}break;
		default:	break;
		}
	}

	return bRv;
}

/*virtual*/ bool UIPopupGMOption::OnKey_Enter(void)
{
	if( !m_pChatEdit->isFocused())
	{
		g_pFramework->SetFocus( m_pChatEdit);
		return true;
	}

	return false;
}

void UIPopupGMOption::CopyData(i3UIEditBox * pEdit)
{
	I3ASSERT(pEdit != nullptr);

	HGLOBAL hmem;
	WCHAR16* ptr;

	size_t len = i3::generic_string_size(pEdit->getText());

	if( len > 0)
	{
		hmem = GlobalAlloc( GHND, (len+1)*sizeof(WCHAR));
		ptr = (WCHAR16*)GlobalLock( hmem);
		wmemcpy( ptr, pEdit->getText(), len);

		ptr[len] = 0;
		GlobalUnlock( hmem);

		if( OpenClipboard( g_hWnd))
		{
			EmptyClipboard();
			SetClipboardData( CF_UNICODETEXT, hmem);
			//SetClipboardData( CF_TEXT, hmem);
			CloseClipboard();
		}
		
		GlobalFree( hmem);
		
	}
}

void UIPopupGMOption::UserPenaltyApply(INT32 idx)
{
	I3TRACE("check idx : %d\n", idx);

	UINT8 penaltyType = GM_OPTION_NONE;
	INT32 penaltyAmount = GM_OPTION_NONE;

	// Index 범위가 채팅금지일 경우 
	if ( idx > NONE_PENALTY && idx < ACCOUNT_BLOCK_ONCE)
	{
		penaltyType = GM_OPTION_REESRTICT_CHAT;

		switch(idx)
		{
		case CHATTING_RESTRICTION_5MIN:
			penaltyAmount = 300;
			break;
		case CHATTING_RESTRICTION_10MIN:
			penaltyAmount = 600;
			break;
		case CHATTING_RESTRICTION_30MIN:
			penaltyAmount = 1800;
			break;
		case CHATTING_RESTRICTION_60MIN:
			penaltyAmount = 3600;
			break;
		case CHATTING_RESTRICTION_RELEASE:
			penaltyAmount = 0;
			break;
//		default:
//			I3TRACE("Penalty number error\n");
//			break;
		}
	}
	// Index 범위가 접속제한일 경우
	else if ( idx > CHATTING_RESTRICTION_RELEASE && idx < PENALTY_MAX)
	{
		penaltyType = GM_OPTION_BAN_PLAYER;

		switch(idx)
		{
		case ACCOUNT_BLOCK_ONCE:
			penaltyAmount = 0;
			break;
		case ACCOUNT_BLOCK_1DAY:
			penaltyAmount = 1;
			break;
		case ACCOUNT_BLOCK_3DAY:
			penaltyAmount = 3;
			break;
		case ACCOUNT_BLOCK_7DAY:
			penaltyAmount = 7;
			break;
		case ACCOUNT_BLOCK_ETERNAL:
			penaltyAmount = PERMANENT_BLOCK;
			break;
//		default:
//			I3TRACE("Penalty number error\n");
//			break;
		}
	}

	// 제재 내용을 선택하지 않으면 패널티 타입이 없기 때문에 보내지 않음
	if (penaltyType != GM_OPTION_NONE)
	{
		T_UID i64UserUID = GMAuthorityContext::i()->GetUserUID();

		GameEventSender::i()->SetEvent(EVENT_GM_AUTHORITY_USER_PENALTY_APPLY, &i64UserUID, &penaltyType, &penaltyAmount);
	}
}

void UIPopupGMOption::ReceiveGameEvent_GMPopup_Open( INT32 arg, const i3::user_data&)
{
	if ( EV_SUCCESSED(arg))
	{
		_SetUserInfo();
	}
}

void UIPopupGMOption::ReceiveGameEvent_Chat_Message_Receive( INT32 arg, const i3::user_data&)
{
	if ( EV_SUCCESSED(arg))
	{
		const i3::rc_wstring& wstrChat = GMAuthorityContext::i()->GetChatMessage();

		m_pChatBuffer->PutChatting(wstrChat);

		_UpdateChatLogBox();
	}
}