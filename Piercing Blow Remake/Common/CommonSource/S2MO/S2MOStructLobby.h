#ifndef __S2_MO_STRUCT_LOBBY_H__
#define __S2_MO_STRUCT_LOBBY_H__

struct PACKET_LOBBY_VIEW_USER_ITEM_ACK : public S2MOPacketBaseT<PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK>
{
	S2MOValue<T_ItemID, TEAM_COUNT>								m_TCharItemID;
	S2MOValue<T_ItemID, CHAR_EQUIPMENT_COUNT>					m_aCharaEquip;
	S2MOValue<T_ItemID, CHAR_EQUIPMENT_COMMON_COUNT>			m_aCommonEquips;
	S2MOValue<T_ItemID, MAX_INVEN_COUNT>						m_aTItemID;

	PACKET_LOBBY_VIEW_USER_ITEM_ACK()
	{
		// Last Update : 2015-10-14 18:09:59
		_SetValue( m_TCharItemID );
		_SetValue( m_aCharaEquip );
		_SetValue( m_aCommonEquips );
		_SetValue( m_aTItemID );
	}
};

#endif