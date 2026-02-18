#ifndef __S2_MO_STRUCT_ROOM_H__
#define __S2_MO_STRUCT_ROOM_H__

#pragma pack( push, 1)

struct PACKET_ROOM_JOIN_ACK : public S2MOPacketBaseResultT<PROTOCOL_ROOM_JOIN_ACK>
{
	S2MOValue<UINT8>										m_ui8Slot;											// Join User SlotNum( Error : Slot Num < 0 )
	S2MOValue<ROOM_INFO_BASIC>								m_RoomInfoBasic;									// Room Basic Info
	S2MOValue<ROOM_INFO_ADD>								m_RoomInfoAdd;										// Room Additional Info
	S2MOValue<ROOM_NOW_SCORE>								m_RoomNowScore;										// RoomNowScore

	S2MOValue<char, NET_ROOM_PW>							m_strRoomPasswd;									// Room Password
	S2MOValue<INT8>											m_i8StartCountDown;									// Room StartCountDown
	S2MOValue<UINT8>										m_ui8MainSlotIdx;									// Room Creator SlotNum
	S2MOValue<UINT8>										m_ui8PersonNow;										// Current User Count
	S2MOValue<INT8>											m_i8EnemyAiCount;									// enemy AI Count
	S2MOValue<INT8>											m_i8DifficultyLevel;								// AI DefficultyLevel
	S2MOValue<UINT8>										m_ui8EnterType;										// S2MO_ROOM_ENTER
	
	S2MOValue<S2_SLOT_TOTAL_INFO, SLOT_MAX_COUNT>			m_aSlotTotalInfo;									// Room Slot Info

	PACKET_ROOM_JOIN_ACK()
	{
		// Last Update : 2015-10-14 13:22:43
		_SetValue( m_ui8Slot );
		_SetValue( m_RoomInfoBasic );
		_SetValue( m_RoomInfoAdd );
		_SetValue( m_RoomNowScore );
		_SetValue( m_strRoomPasswd );
		_SetValue( m_i8StartCountDown );
		_SetValue( m_ui8MainSlotIdx );
		_SetValue( m_ui8PersonNow );
		_SetValue( m_i8EnemyAiCount );
		_SetValue( m_i8DifficultyLevel );
		_SetValue( m_ui8EnterType );
		_SetValue( m_aSlotTotalInfo );
	}
};

struct PACKET_ROOM_GET_SLOTONEINFO_ACK : public S2MOPacketBaseT<PROTOCOL_ROOM_GET_SLOTONEINFO_ACK>
{
	S2MOValue<S2_SLOT_TOTAL_INFO>							m_SlotTotalInfo;

	PACKET_ROOM_GET_SLOTONEINFO_ACK()
	{
		// Last Update : 2015-10-14 15:02:21
		_SetValue( m_SlotTotalInfo );
	}
};

struct PACKET_ROOM_GET_SLOTINFO_ACK : public S2MOPacketBaseT<PROTOCOL_ROOM_GET_SLOTINFO_ACK>
{
	S2MOValue<UINT8>										m_ui8MainSlotIdx;
	S2MOValue<SLOT_INFO, SLOT_MAX_COUNT>					m_aSlotInfo;

	PACKET_ROOM_GET_SLOTINFO_ACK()
	{
		// Last Update : 2015-10-14 15:03:01
		_SetValue( m_ui8MainSlotIdx );
		_SetValue( m_aSlotInfo );
	}
};

struct PACKET_ROOM_GET_PLAYERINFO_ACK : public S2MOPacketBaseResultT<PROTOCOL_ROOM_GET_PLAYERINFO_ACK>
{
	S2MOValue<UINT8>										m_ui8SlotIdx;
	S2MOValue<USER_INFO_BASIC>								m_UserBasicInfo;
	S2MOValue<USER_INFO_RECORD>								m_UserRecordInfo;
	S2MOValue<UINT8, MAX_EQUIP_USER_TITLE>					m_aui8EquipUserTitle;
	S2MOValue<S2MO_CHAR_EQUIP_INFO>							m_CharEquipInfo;
	S2MOValue<ITEM_INFO, CHAR_EQUIPMENT_COMMON_COUNT>		m_aCommonEquips;

	PACKET_ROOM_GET_PLAYERINFO_ACK()
	{
		// Last Update : 2015-10-14 15:19:51
		_SetValue( m_ui8SlotIdx );
		_SetValue( m_UserBasicInfo );
		_SetValue( m_UserRecordInfo );
		_SetValue( m_aui8EquipUserTitle );
		_SetValue( m_CharEquipInfo );
		_SetValue( m_aCommonEquips );
	}
};

struct PACKET_ROOM_GET_USER_ITEM_ACK : public S2MOPacketBaseT<PROTOCOL_ROOM_GET_USER_ITEM_ACK>
{
	S2MOValue<S2MO_CHAR_EQUIP_INFO>							m_CharEquipInfo;
	S2MOValue<ITEM_INFO, CHAR_EQUIPMENT_COMMON_COUNT>		m_aCommonEquips;
	S2MOValue<T_ItemID, MAX_INVEN_COUNT>					m_aTItemID;

	PACKET_ROOM_GET_USER_ITEM_ACK()
	{
		_SetValue( m_CharEquipInfo );
		_SetValue( m_aCommonEquips );
		_SetValue( m_aTItemID );
	}

};

struct PACKET_ROOM_CHARA_SHIFT_POS_ACK : public S2MOPacketBaseResultT<PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK>
{
	S2MOValue<UINT8, TEAM_COUNT>							m_ui8SlotIdx;

	PACKET_ROOM_CHARA_SHIFT_POS_ACK()
	{
		_SetValue( m_ui8SlotIdx );
	}
};

struct PACKET_ROOM_GET_USER_EQUIPMENT_ACK : public S2MOPacketBaseResultT<PROTOCOL_ROOM_GET_USER_EQUIPMENT_ACK>
{
	S2MOValue<UINT8>										m_ui8SlotIdx;
	S2MOValue<T_ItemID, TEAM_COUNT>							m_TCharItemID;
	S2MOValue<T_ItemID, CHAR_EQUIPMENT_WEAPON_COUNT>		m_TBattleEquipWeapon;
	S2MOValue<T_ItemID, CHAR_EQUIPMENT_PARTS_COUNT>			m_TBattleEquipParts;

	PACKET_ROOM_GET_USER_EQUIPMENT_ACK()
	{
		_SetValue( m_ui8SlotIdx );
		_SetValue( m_TCharItemID );
		_SetValue( m_TBattleEquipWeapon );
		_SetValue( m_TBattleEquipParts );
	}
};


#pragma pack( pop )

#endif