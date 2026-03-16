#ifndef __S2_MO_STRUCT_INVENTORY_H__
#define __S2_MO_STRUCT_INVENTORY_H__

struct PACKET_INVENTORY_LEAVE_REQ : public S2MOPacketBaseT<PROTOCOL_INVENTORY_LEAVE_REQ>
{
};

struct PACKET_INVENTORY_LEAVE_ACK : public S2MOPacketBaseResultT<PROTOCOL_INVENTORY_LEAVE_ACK>
{
};

struct PACKET_INVENTORY_GET_INFO_ACK : public S2MOPacketBaseT<PROTOCOL_INVENTORY_GET_INFO_ACK>
{
	S2MOValue<UINT8>									m_ui8ChangeType;
	S2MOValue<INVEN_BUFFER, MAX_INVEN_COUNT>	m_Inventory;

	PACKET_INVENTORY_GET_INFO_ACK()
	{
		_SetValue( m_ui8ChangeType );
		_SetValue( m_Inventory );
	}
};

#endif