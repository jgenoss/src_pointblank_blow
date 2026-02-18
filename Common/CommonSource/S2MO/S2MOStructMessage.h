#ifndef __S2_MO_STRUCT_MESSAGE_H__
#define __S2_MO_STRUCT_MESSAGE_H__

struct PACKET_MESSAGE_ANNOUNCE_ACK : public S2MOPacketBaseT<PROTOCOL_SERVER_MESSAGE_ANNOUNCE>
{
	S2MOValue<INT32>					m_i32Type;	
	S2MOStringW<NOTICE_STRING_COUNT>	m_strChatMessage;
	S2MOValue<INT32>					m_i32RGB;
	
	PACKET_MESSAGE_ANNOUNCE_ACK()
	{
		_SetValue(m_i32Type);	
		_SetValue(m_strChatMessage);
		_SetValue(m_i32RGB);		
	}
};

#endif