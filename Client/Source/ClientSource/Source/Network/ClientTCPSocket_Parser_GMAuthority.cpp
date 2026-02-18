#include "pch.h"
#include "ClientTCPSocket.h"
#include "../GMAuthorityContext.h"
#include "../UserInfoContext.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"
#include "UI/UIPopupGMOption.h"

#include "i3Base/string/ext/mb_to_utf16.h"


namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_GMCHAT_START_CHAT_ACK : return "PROTOCOL_GMCHAT_START_CHAT_ACK";
		case PROTOCOL_GMCHAT_SEND_CHAT_ACK : return "PROTOCOL_GMCHAT_SEND_CHAT_ACK";
		case PROTOCOL_GMCHAT_END_CHAT_ACK : return "PROTOCOL_GMCHAT_END_CHAT_ACK";
		case PROTOCOL_GMCHAT_APPLY_PENALTY_ACK : return "PROTOCOL_GMCHAT_APPLY_PENALTY_ACK";
		case PROTOCOL_GMCHAT_NOTI_USER_PENALTY_ACK : return "PROTOCOL_GMCHAT_NOTI_USER_PENALTY_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_GMCHAT_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_GMCHAT_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingGMAuthority(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_GMAuthority.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_GMAuthority.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_GMAuthority_Function( void)
{
	RegisterParser( PROTOCOL_GMCHAT_START_CHAT_ACK,			&ClientTCPSocket::__Parse_GMAuthority_OpenPopup);
	RegisterParser( PROTOCOL_GMCHAT_SEND_CHAT_ACK,			&ClientTCPSocket::__Parse_GMAuthority_ReceiveChat);
	RegisterParser( PROTOCOL_GMCHAT_END_CHAT_ACK,			&ClientTCPSocket::__Parse_GMAuthority_ClosePopup);
	RegisterParser( PROTOCOL_GMCHAT_APPLY_PENALTY_ACK,		&ClientTCPSocket::__Parse_GMAuthority_ApplyPenalty);
	RegisterParser( PROTOCOL_GMCHAT_NOTI_USER_PENALTY_ACK,	&ClientTCPSocket::__Parse_GMAuthority_PenaltyNotice);
}

void ClientTCPSocket::__Parse_GMAuthority_OpenPopup( i3NetworkPacket * pPacket)
{
	PACKET_GMCHAT_START_CHAT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult))
	{
		GMAuthorityContext::i()->SetUserUID(Recv.m_i64UserUID);
		
		i3::rc_wstring wstrNickName = &Recv.m_strNickname;

		GMAuthorityContext::i()->SetUserNickName(wstrNickName);

		g_pFramework->GetUIMainframe()->OpenPopup(UPW_GM_OPTION);

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GM_AUTHORITY_POPUP_OPEN , Recv.m_TResult);
	}
	else if ( Recv.m_TResult == EVENT_ERROR_GMCHAT_USER_NOT_INSERVER)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_GM_POPUP_OFFLINEUSER"));
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_GM_POPUP_FAIL"));
	}
}

void ClientTCPSocket::__Parse_GMAuthority_ReceiveChat( i3NetworkPacket * pPacket)
{
	PACKET_GMCHAT_SEND_CHAT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	i3::rc_wstring wstrChatMessage = &Recv.m_strChatMessage;

	GMAuthorityContext::i()->SetChatMessage(wstrChatMessage);

//	GMAuthorityContext::i()->SetMessageLenth(Recv.m_ui16Chatsize + 1);
	i3::rc_wstring wstrNickName = &Recv.m_strNickname;

	GMAuthorityContext::i()->SetUserNickName(wstrNickName);

	if ( g_pFramework->IsBattleStage())
	{
		// 게임중일 경우 귓속말로 받음

		INT32 _NickSize = wstrNickName.size() + 2;

		i3::stack_wstring wstrWhisperChatMessage;
		i3::safe_string_copy(wstrWhisperChatMessage, &Recv.m_strChatMessage + _NickSize, Recv.m_strChatMessage.GetCount() - _NickSize + 1);

		g_pFramework->getChatBox()->SetChatReplyTarget( GMAuthorityContext::i()->GetUserNickName());
		g_pFramework->getChatBox()->PutUserWhisper( GMAuthorityContext::i()->GetUserNickName(), wstrWhisperChatMessage, true);
	}
	else
	{
		if (g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_GM_OPTION))
		{
			GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GM_AUTHORITY_CHAT_RECEIVE , Recv.m_TResult);
		}
		else
		{
			if(g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_GM_CHATBOX))
			{
				GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GM_AUTHORITY_CHAT_RECEIVE , Recv.m_TResult);
			}
			else
			{
				g_pFramework->GetUIMainframe()->OpenPopup(UPW_GM_CHATBOX);

				GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GM_AUTHORITY_CHAT_RECEIVE , Recv.m_TResult);
			}
		}
	}
}

// GM이 유저관리 팝업을 닫았을 경우
void ClientTCPSocket::__Parse_GMAuthority_ClosePopup( i3NetworkPacket * pPacket)
{
	PACKET_GMCHAT_END_CHAT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if (!g_pFramework->IsBattleStage())
	{
		// 제재받던 유저는 GM이 관리 팝업 종료시 안내를 받고 채팅박스를 닫습니다
		if (UserInfoContext::i()->GetMyRank() != RANK_GAMEMASTER)
		{
			UIMainFrame * pMainFrame = g_pFramework->GetUIMainframe();
			if( pMainFrame != nullptr && pMainFrame->IsOpenedPopup(UPW_GM_CHATBOX) )
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_GM_POPUP_CHATSAVEMESSAGE"));
				g_pFramework->GetUIMainframe()->ClosePopup(UPW_GM_CHATBOX);
			}
		}
	}

	// 종료시 GM 닉네임은 비워줍니다.
	GMAuthorityContext::i()->SetUserNickName(L"");
}

void ClientTCPSocket::__Parse_GMAuthority_ApplyPenalty( i3NetworkPacket * pPacket)
{
	PACKET_GMCHAT_APPLY_PENALTY_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	// 유저에게 패널티를 적용 후 GM이 받는 패킷(사용하는 곳 없음)
}

void ClientTCPSocket::__Parse_GMAuthority_PenaltyNotice( i3NetworkPacket * pPacket)
{

	PACKET_GMCHAT_NOTI_USER_PENALTY_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	// 채팅 금지
	if(Recv.m_ui8PenaltyType == GM_OPTION_REESRTICT_CHAT)
	{
		switch(Recv.m_i32PenaltyAmount)
		{
		// 채팅 금지를 해체했을 경우
		case 0:
			if(g_pFramework->IsBattleStage())
				g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_GM_POPUP_CHATRELEASE"));
			else
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_GM_POPUP_CHATRELEASE"));
			break;
		// 채팅 금지를 받았을 경우
		default:
			i3::rc_wstring message;
			INT32 i32PenaltyAmount = Recv.m_i32PenaltyAmount;
			i3::sprintf( message, GAME_RCSTRING("STR_GM_POPUP_CHATBAN"), i32PenaltyAmount /60);
			if(g_pFramework->IsBattleStage())
				g_pFramework->getChatBox()->PutSystemChatting(message);
			else
				GameUI::MsgBox(MSG_TYPE_GAME_OK, message);
			break;
		}
	}
	// 블럭을 받을 경우
	else if (Recv.m_ui8PenaltyType == GM_OPTION_BAN_PLAYER)
	{
		switch(Recv.m_i32PenaltyAmount)
		{
		case 0:
			GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_GM_POPUP_BANONE"));
			break;
		case PERMANENT_BLOCK:
			GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_GM_POPUP_BANMESSAGE"));
			break;
		default:
			i3::rc_wstring message;
			INT32 i32PenaltyAmount = Recv.m_i32PenaltyAmount;
			i3::sprintf( message, GAME_RCSTRING("STR_GM_POPUP_BANDAY"), i32PenaltyAmount );
			GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, message);
			break;
		}
	}
	else
	{
		I3TRACE("제재 내역이 없습니다.\n");
	}
}