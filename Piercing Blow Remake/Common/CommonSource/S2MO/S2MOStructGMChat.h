#ifndef __S2_MO_STRUCT_GM_CHAT_H__
#define __S2_MO_STRUCT_GM_CHAT_H__

struct PACKET_GMCHAT_START_CHAT_REQ : public S2MOPacketBaseT<PROTOCOL_GMCHAT_START_CHAT_REQ>
{
	S2MOValue<UINT8>							m_ui8idxType;					//Idx값 종류 : 1:로비, 2:대기방
	S2MOValue<INT32>							m_i32idx;						//실제Idx값
	S2MOStringW<NET_NICK_NAME_SIZE>				m_strNickname;

	PACKET_GMCHAT_START_CHAT_REQ()
	{
		_SetValue( m_ui8idxType );
		_SetValue( m_i32idx );
		_SetValue( m_strNickname );
	}
};

struct PACKET_GMCHAT_START_CHAT_ACK : public S2MOPacketBaseResultT<PROTOCOL_GMCHAT_START_CHAT_ACK>
{
	S2MOValue<T_UID>							m_i64UserUID;
	S2MOStringW<NET_NICK_NAME_SIZE>				m_strNickname;

	PACKET_GMCHAT_START_CHAT_ACK()
	{
		_SetValue( m_i64UserUID );
		_SetValue( m_strNickname );
	}
};
			
struct PACKET_GMCHAT_SEND_CHAT_REQ : public S2MOPacketBaseT<PROTOCOL_GMCHAT_SEND_CHAT_REQ>
{
	S2MOValue<T_UID>							m_i64UID;									//받는 사람의 UID, User가 보낼 경우 이값은 0이며, GM일경우 1:1대화중인 User의 UID
	S2MOStringW<NET_NICK_NAME_SIZE>				m_strNickname;
	S2MOStringW<MAX_CHATTING_COUNT>				m_strChatMessage;

	PACKET_GMCHAT_SEND_CHAT_REQ()
	{
		_SetValue( m_i64UID );
		_SetValue( m_strNickname );
		_SetValue( m_strChatMessage );
	}
};

struct PACKET_GMCHAT_SEND_CHAT_ACK : public S2MOPacketBaseResultT<PROTOCOL_GMCHAT_SEND_CHAT_ACK>
{
	S2MOStringW<NET_NICK_NAME_SIZE>				m_strNickname;
	S2MOStringW<MAX_CHATTING_COUNT>				m_strChatMessage;

	PACKET_GMCHAT_SEND_CHAT_ACK()
	{
		_SetValue( m_strNickname );
		_SetValue( m_strChatMessage );
	}
};

struct PACKET_GMCHAT_END_CHAT_REQ : public S2MOPacketBaseT<PROTOCOL_GMCHAT_END_CHAT_REQ>
{
	S2MOValue<T_UID>							m_i64UserUID;

	PACKET_GMCHAT_END_CHAT_REQ()
	{
		_SetValue( m_i64UserUID );
	}
};

struct PACKET_GMCHAT_END_CHAT_ACK : public S2MOPacketBaseResultT<PROTOCOL_GMCHAT_END_CHAT_ACK>
{
};

struct PACKET_GMCHAT_APPLY_PENALTY_REQ : public S2MOPacketBaseT<PROTOCOL_GMCHAT_APPLY_PENALTY_REQ>
{
	S2MOValue<T_UID>							m_i64UserUID;
	S2MOValue<UINT8>							m_ui8PenaltyType;			//패널티 타입. 1 - 채팅금지, 2 - 계정 블럭
	S2MOValue<INT32>							m_i32PenaltyAmount;

	PACKET_GMCHAT_APPLY_PENALTY_REQ()
	{
		_SetValue( m_i64UserUID );
		_SetValue( m_ui8PenaltyType );
		_SetValue( m_i32PenaltyAmount );
	}
};

struct PACKET_GMCHAT_APPLY_PENALTY_ACK : public S2MOPacketBaseResultT<PROTOCOL_GMCHAT_APPLY_PENALTY_ACK>
{
};

struct PACKET_GMCHAT_NOTI_USER_PENALTY_ACK : public S2MOPacketBaseT<PROTOCOL_GMCHAT_NOTI_USER_PENALTY_ACK>
{
	S2MOValue<UINT8>							m_ui8PenaltyType;			//패널티 타입. 1 - 채팅금지, 2 - 계정 블럭
	S2MOValue<INT32>							m_i32PenaltyAmount;

	PACKET_GMCHAT_NOTI_USER_PENALTY_ACK()
	{
		_SetValue( m_ui8PenaltyType );
		_SetValue( m_i32PenaltyAmount );
	}
};

#endif