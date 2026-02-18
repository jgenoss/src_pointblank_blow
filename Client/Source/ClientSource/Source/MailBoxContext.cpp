#include "pch.h"
#include "MailBoxContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "i3Base/string/ext/lrtrim.h"
#include "i3Base/string/ext/atoi.h"

//

bool _Note::comp_idx( const _Note& lhs, const _Note& rhs )
{
	return rhs.pNote->header._id < lhs.pNote->header._id;
}

bool _Note::comp_sender( const _Note& lhs, const _Note& rhs )
{
	return 0 > i3::generic_icompare( lhs.pNote->body._sender, rhs.pNote->body._sender );
}

bool _Note::comp_sender_rev( const _Note& lhs, const _Note& rhs )
{
	return 0 < i3::generic_icompare( lhs.pNote->body._sender, rhs.pNote->body._sender );
}

bool _Note::comp_period( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_ui32Expire > pRightGift->_ui32Expire;
}

bool _Note::comp_period_rev( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_ui32Expire < pRightGift->_ui32Expire;
}

bool _Note::comp_gift_state( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_eDeliver > pRightGift->_eDeliver;
}

bool _Note::comp_gift_state_rev( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_eDeliver < pRightGift->_eDeliver;
}


MailBoxContext::~MailBoxContext()
{
	i3::cu::for_each_SafeFree_clear(m_NoteReceiveList); 
	i3::cu::for_each_SafeFree_clear(m_NoteGiftList);

	m_GiftReqList.clear();
}


bool MailBoxContext::AddNote( const NOTE_RECEIVE * pNew, bool bAlram)
{
	NOTE_RECEIVE * pNote = (NOTE_RECEIVE*)i3MemAlloc( sizeof(NOTE_RECEIVE));
	i3mem::Copy( pNote, pNew, sizeof( NOTE_RECEIVE));

	AddReceiveNote( pNote);
	this->SetNoteNewFlag();

	if(bAlram)
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RECEIVE_NOTE);

	return true;
}

bool MailBoxContext::DeleteNote( INT32 noteId)
{
	for( UINT32 x = 0; x < (UINT32)GetNoteReceiveCount(); x++)
	{
		NOTE_RECEIVE* pNote = (NOTE_RECEIVE*)GetReceiveNote(x);
		if( pNote->header._id == noteId)
		{
			I3MEM_SAFE_FREE( pNote);
			i3::vu::erase_index(m_NoteReceiveList, x);
			return true;
		}
	}

	return false;
}

//bool MailBoxContext::DeleteAllGift( INT32 noteId)
//{
//	for( UINT32 x = 0; x < (UINT32)GetAllNoteReceiveCount(); x++)
//	{
//		NOTE_RECEIVE* pNote = (NOTE_RECEIVE*)GetReceiveAllNote(x);
//		if( pNote->header._id == noteId)
//		{
//			I3MEM_SAFE_FREE( pNote);
//			i3::vu::erase_index(m_AllNoteReceiveList, x);
//			return true;
//		}
//	}
//
//	return false;
//
//	INT32	nCount	= GetNoteGiftCount();
//
//	for( INT32 i = 0; i < nCount; i++)
//	{
//		NOTE_GIFT* pNoteGift	= (NOTE_GIFT*)GetNoteGift( i);
//		if( pNoteGift->gift._i32GiftWareDBIdx == GiftDBIdx )
//		{
//			I3MEM_SAFE_FREE( pNoteGift);
//			i3::vu::erase_index(m_NoteGiftList, i);
//			return true;
//		}
//	}
//
//	return false;
//}

// 이것 한번만 필요한 것으로 보이는데....
// 여러번 호출되지 않게 생성자에 넣을 필요가 있음...일단은 방치...
// 생성자수준에서는 아직 필요한 스트링값을 얻을수 없고, 파일 로딩뒤에나 가능한듯 하다..
// 

void MailBoxContext::CreateClanMessageHandle()
{
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_ACCEPT ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_DENIAL ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_ACCEPT ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_DENIAL ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_SECESSION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_SECESSION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_DEPORTATION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_MASTER ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_MASTER");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_STAFF ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_STAFF");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_REGULAR ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_REGULAR");
}



void MailBoxContext::UpdateClanMessage()
{
	for( UINT32 x = 0; x < (UINT32)GetNoteReceiveCount(); x++)
	{
		UpdateClanMessage((NOTE_RECEIVE*)GetReceiveNote(x));
	}
}

void MailBoxContext::UpdateClanMessage( NOTE_RECEIVE* pNote )
{
	if( nullptr == pNote )
		return ;

	i3::stack_wstring wstrMessage;

	if( 0 >= pNote->body._messageSize )
	{
		UINT8* pcSize		= (UINT8*)pNote->body._message;
		UINT8* pcMessazeType = (UINT8*)&pNote->body._message[1];

		switch( *pcMessazeType )
		{
		case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:	// "축하합니다!\n클랜 가입이 승인되었습니다.\n즐거운 클랜 활동을 즐기세요."
		case NOTE_MESSAGE_CLAN_MASTER:			// "클랜 마스터 권한이 위임되었습니다.\n클랜 마스터가 되신 것을 축하 합니다."
		case NOTE_MESSAGE_CLAN_STAFF:			// "축하합니다.\n클랜 마스터가 스태프로 임명하였습니다."
		case NOTE_MESSAGE_CLAN_REGULAR:			// "클랜 마스터가 일반회원으로 임명하였습니다."
			wstrMessage = GAME_RCSTRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]);
			break;
		case NOTE_MESSAGE_CLAN_INVITE:			// "[%s] 클랜가입 권유\n클랜으로부터 가입을 초대받았습니다.\n가입에 동의하시겠습니까?"
		case NOTE_MESSAGE_CLAN_INVITE_DENIAL:	// "[%s]\n클랜으로부터 가입 신청이\n거부 되었습니다."
		case NOTE_MESSAGE_CLAN_DEPORTATION:		// "[%s]\n클랜에서 제명되었습니다."
			{
				i3::sprintf( wstrMessage, GAME_RCSTRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), pNote->body._sender );
			}
			break;
		case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:		// "[%s] 님께서\n클랜 가입 초대를 수락하였습니다."
		case NOTE_MESSAGE_CLAN_JOIN_DENIAL:		// "[%s] 님께서\n클랜 가입 신청을 거부하였습니다."
		case NOTE_MESSAGE_CLAN_SECESSION:		// "[%s] 님께서\n클랜에서 탈퇴하였습니다."
			{
				TTCHAR szName[ NET_NICK_NAME_SIZE ];
				i3::string_ncopy_nullpad( szName, &pNote->body._message[2], *pcSize - 1 );
				i3::sprintf( wstrMessage, GAME_RCSTRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), szName );
			}
			break;
		}
		
		const i3::stack_wstring& strMessage = wstrMessage;

		pNote->body._messageSize = UINT8(sizeof(pNote->body._message) / sizeof(TTCHAR));
		i3::safe_string_copy( pNote->body._message, strMessage, pNote->body._messageSize );			
	}
}


// 선물 쪽지 추가
bool MailBoxContext::AddNoteGift( NOTE_GIFT * pnoteNew, bool bAlram /*= false*/)
{


	I3ASSERT( pnoteNew != nullptr );

	m_NoteGiftList.push_back( pnoteNew );
	this->SetNoteNewFlag();

	if(bAlram)
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RECEIVE_NOTE);

	return true;
}

// 선물 쪽지 삭제
bool MailBoxContext::DeleteNoteGift( T_GiftDBIdx GiftDBIdx )
{
	INT32	nCount	= GetNoteGiftCount();

	for( INT32 i = 0; i < nCount; i++)
	{
		NOTE_GIFT* pNoteGift	= (NOTE_GIFT*)GetNoteGift( i);
		if( pNoteGift->gift._i32GiftWareDBIdx == GiftDBIdx )
		{
			I3MEM_SAFE_FREE( pNoteGift);
			i3::vu::erase_index(m_NoteGiftList, i);
			return true;
		}
	}

	return false;
}

void MailBoxContext::UpdateNoteGift(AUTH_GIFT * pAuthGift)
{
	INT32 nCount = GetNoteGiftCount();

	for( INT32 i = 0; i < nCount; i++)
	{
		NOTE_GIFT* pNoteGift = (NOTE_GIFT*)GetNoteGift( i);

		if( pNoteGift->gift._i32GiftWareDBIdx == pAuthGift->m_TDBIdx )
		{
			pNoteGift->gift._eDeliver = static_cast<ENUM_TYPE_GIFT_DELIVER>(pAuthGift->m_ui8Deliver);
			pNoteGift->gift._ui32Expire = pAuthGift->m_ui32Expire;
			
			switch( pAuthGift->m_ui8Deliver ) 
			{
			case TYPE_GIFT_DELIVER_READ		: pNoteGift->note.header._state	= NS_READED; break;
			case TYPE_GIFT_DELIVER_RECEIVED	: pNoteGift->note.header._state	= NS_READED; break;
			default							: pNoteGift->note.header._state	= NS_UNREAD; break;
			}
		}
	}
}

void MailBoxContext::DeleteOldNoteGift(void)
{
	INT32 i32DeleteIdx = 0;
	NOTE_GIFT * pDeleteGift = (NOTE_GIFT*)GetNoteGift(i32DeleteIdx);

	INT32	nCount	= GetNoteGiftCount();
	for( INT32 idx = 0; idx < nCount; ++idx)
	{
		NOTE_GIFT * pNoteGift = (NOTE_GIFT*)GetNoteGift(idx);

		if(pDeleteGift->gift._i32GiftWareDBIdx > pNoteGift->gift._i32GiftWareDBIdx)
		{
			pDeleteGift = pNoteGift;
			i32DeleteIdx = idx;
		}
	}

	I3MEM_SAFE_FREE( pDeleteGift);
	i3::vu::erase_index(m_NoteGiftList, i32DeleteIdx);
}

void MailBoxContext::AddGift( const GIFT_BUFFER& Gift, const UINT8 nSenderNickSize, const UINT8 nMessageSize )
{
	// revision 30185
	if( m_NoteGiftList.size() >= MAX_SHOP_USER_GIFT_COUNT ) 
	{
		I3PRINTLOG(I3LOG_NOTICE, "Gift Note Count OverFlow" );
		DeleteOldNoteGift();
	}

	NOTE_GIFT*		pnoteGift	= (NOTE_GIFT*)i3MemAlloc(sizeof(NOTE_GIFT));
	NOTE_RECEIVE*	pnote		= &(pnoteGift->note);
	GIFT_BUFFER*	pgift		= &(pnoteGift->gift);

	i3mem::FillZero( pnoteGift, sizeof( NOTE_GIFT));

	// 선물 복사
	i3mem::Copy( pgift, &Gift, sizeof(GIFT_BUFFER));

	// 쪽지 헤더
	pnote->header._id			= pgift->_i32GiftWareDBIdx;
	pnote->header._type			= NT_GIFT;

	switch( Gift._eDeliver ) 
	{
	case TYPE_GIFT_DELIVER_READ		: pnote->header._state	= NS_READED; break;
	case TYPE_GIFT_DELIVER_RECEIVED	: pnote->header._state	= NS_READED; break;
	default							: pnote->header._state	= NS_UNREAD; break;
	}

	UINT32		unSecond	= UserInfoContext::i()->GetInventorySecTime( Gift._ui32Expire );

	if( 0 > (INT32)unSecond ) unSecond = 0;
	if( unSecond >= ITEM_TERM_ADJUST_VALUE) unSecond -= ITEM_TERM_ADJUST_VALUE;

	pnote->header._remainDay	= (UINT8)ConvertSecToUseDay( unSecond );


	// 쪽지 바디
	pnote->body._user			= pgift;		//	선물하기 Note에서 pNote->body._user 포인터는 GIFT_BUFFER 구조체 포인터임.

	i3::string_ncopy_nullpad( pnote->body._sender, Gift._strSenderNick, nSenderNickSize );
	pnote->body._senderSize		= nSenderNickSize;

	i3::stack_wstring wstrMessage	= Gift._strMessage;

	i3::lrtrim( wstrMessage );

	if( wstrMessage.size() > 0 )	// 메세지가 있으면 
	{
		i3::string_ncopy_nullpad( pnote->body._message, wstrMessage, nMessageSize );

		pnote->body._messageSize = nMessageSize;

		ConvertEventGiftMessage(pnote->body, wstrMessage);
	}
	else						// 메세지가 없으면 기본 메세지
	{
		// [%s] 님께서 보낸 선물이 도착했습니다.\n받은 선물은 인벤토리에 저장됩니다.
		i3::sprintf(wstrMessage, GAME_RCSTRING("STR_SHOP_NOTE_RECEIVE_GIFT"), pnote->body._sender);

//		i3::snprintf( pnote->body._message, NET_NOTE_SIZE, GAME_RCSTRING("STR_SHOP_NOTE_RECEIVE_GIFT"), pnote->body._sender );
		const i3::stack_wstring& strMessage = wstrMessage;
		i3::safe_string_copy(pnote->body._message, strMessage, NET_NOTE_SIZE);

		pnote->body._messageSize	= (UINT8)(i3::generic_string_size( pnote->body._message) + 1);								
	}

	// 보낸 사람 타입
	if( i3::generic_is_equal( pnote->body._sender, _TT("GM") )  )
	{
		pnoteGift->sendertype	= GST_GM;
	}
	else
	{
		pnoteGift->sendertype	= GST_USER;
	}

	AddNoteGift( pnoteGift );
}

void MailBoxContext::ConvertEventGiftMessage(NOTE_BODY & pOut, const i3::wliteral_range& wMsgRng)
{
	i3::wliteral_range check_msg = L"EVENT_";

	if( i3::contain_string_head(wMsgRng, check_msg) == true )
	{
//		i3::string strEvtNum = strMessage.substr( check_msg.length() );

		i3::wliteral_range wstrEvtNum( wMsgRng.begin() + check_msg.size(), wMsgRng.end() );

		INT32 i32Event = i3::atoi( wstrEvtNum );
		
		if (i32Event == ITEM_INSERT_REQUEST_UNKNOWN) 
			return;

		i3::stack_wstring wstrEventName;
		i3::stack_wstring wstrEventMsg;

		bool bExistEventName = true;

		switch(i32Event)
		{
			case ITEM_INSERT_REQUEST_TS_BONUS		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_TS"); break;
			case ITEM_INSERT_REQUEST_RANKUP			: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_RANKUP"); break;
			case ITEM_INSERT_REQUEST_MISSION_CARD	: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_MISSION_CARD"); break;
			case ITEM_INSERT_REQUEST_GET_TITLE		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_TITLE"); break;
			case ITEM_INSERT_REQUEST_USER_GIFT		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GIFT"); break;
			case ITEM_INSERT_REQUEST_ATTENDANCE		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_ATTENDANCE"); break;
			
			case ITEM_INSERT_REQUEST_EVENT_CONNECT	: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_LOGIN"); break;
			case ITEM_INSERT_REQUEST_EVENT_NEWUSER	: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NEWUSER"); break;
			case ITEM_INSERT_REQUEST_WEB_GIFT		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GMSEND"); break;
			case ITEM_INSERT_REQUEST_DORMANT		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_DORMANT"); break;

			case ITEM_INSERT_REQUEST_BUY			:
			case ITEM_INSERT_REQUEST_CREATE_NEWUSER :
			case ITEM_INSERT_REQUEST_CAPSULE		:
			case ITEM_INSERT_REQUEST_GACHA			: 
			case ITEM_INSERT_REQUEST_MEDAL_REWARD	: 
			case ITEM_INSERT_REQUEST_DOMI_REWARD	: 
			case ITEM_INSERT_REQUEST_AUTH			:
			default									:
				{
					bExistEventName = false;
					
					if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
						wstrEventMsg = GAME_RCSTRING("STR_POPUP_GIFTPRESENT");
					else
						wstrEventMsg = GAME_RCSTRING("STR_POPUP_MESSAGE_EVENT_PRIZE_PAY");
				}
				break;
		}

		if (!UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT)) //선물함이 꺼져 있을 경우 해당 스트링으로 통일
			wstrEventMsg = GAME_RCSTRING("STR_POPUP_MESSAGE_EVENT_PRIZE_PAY");

		else if (bExistEventName)
			i3::sprintf(wstrEventMsg, GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NAME"), wstrEventName);
		
		const i3::rc_wstring& strEventMsg = wstrEventMsg;
		i3::safe_string_copy(pOut._message, strEventMsg, sizeof(pOut._message) / sizeof(TTCHAR));

		pOut._messageSize = (UINT8)i3::generic_string_size(pOut._message);
	}

	
	else if(UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
	{
		i3::stack_wstring wstrGMMsg;
		i3::stack_wstring wstrEventMsg;

		if (i3::generic_is_equal(pOut._sender, _TT("GM"))) //웹툴을 통한 GM선물 태그
			i3::sprintf(wstrEventMsg, GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NAME"), GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GMSEND"));
		else                                               //유저들끼리 선물샵을 이용해서 선물해도 태그를 붙여줘야 함
			i3::sprintf(wstrEventMsg, GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NAME"), GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GIFT"));

		i3::sprintf(wstrGMMsg, L"%s %s", wstrEventMsg, pOut._message);

		const i3::rc_wstring& strConverMsg = wstrGMMsg;
		i3::safe_string_copy(pOut._message, strConverMsg, sizeof(pOut._message) / sizeof(TTCHAR));
	}
}

void MailBoxContext::ResetGiftReqList(void)
{ 
	m_GiftReqList.clear();
	m_i32ResultCount = 0; 
	m_ui8GiftReqType = 0;
}

void MailBoxContext::BuildGiftReqList(UINT8 type, INT32 * list, INT32 count)
{
	m_ui8GiftReqType = type;

	for(INT32 idx = 0 ; idx < count ; ++idx)
	{
		m_GiftReqList.push_back( GIFT_REQ_DATA(list[idx]) );
	}
}

void MailBoxContext::SetGiftReqResult(INT32 type, AUTH_GIFT * pAuthGift)
{
	if(EV_SUCCESSED(pAuthGift->m_i32Rv ) )
	{
		// 성공이면, 선물 노트를 재기록합시다..
		if(type == TYPE_SHOP_GIFT_DELETE )
		{
			DeleteNoteGift(pAuthGift->m_TDBIdx);
		}
		else
			UpdateNoteGift(pAuthGift);
	}
	else
	{
		// 실패면 로그 남김.
		i3::string strErrorLog;
		i3::sprintf( strErrorLog, "AGF[%d] : %x ", type, (UINT32)pAuthGift->m_i32Rv);
		I3PRINTLOG(I3LOG_NOTICE, strErrorLog );
	}

	for( UINT32 idx = 0 ; idx < m_GiftReqList.size() ; ++idx)
	{
		if(m_GiftReqList[idx]._idx == pAuthGift->m_TDBIdx)
		{
			m_GiftReqList[idx]._success = EV_SUCCESSED(pAuthGift->m_i32Rv);
			break;
		}
	}
}

INT32 MailBoxContext::GetGiftReqSuccessCount(void)
{
	INT32 count = 0;

	for( UINT32 idx = 0 ; idx < m_GiftReqList.size() ; ++idx)
	{
		if(m_GiftReqList[idx]._success == true)
			count++;
	}
	
	return count;
}