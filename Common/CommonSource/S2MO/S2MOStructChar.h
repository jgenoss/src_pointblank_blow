#ifndef __S2_MO_STRUCT_CHAR_H__
#define __S2_MO_STRUCT_CHAR_H__

//#include "S2MOStruct.h"

#pragma pack( push, 1)

struct PACKET_CHAR_CREATE_CHARA_REQ : public S2MOPacketBaseT<PROTOCOL_CHAR_CREATE_CHARA_REQ>
{
	S2MOValue<UINT8>								m_ui8CreateType;											// 기본 생성, 포인트/캐쉬 구매. SIA_GOODS_BUY
	S2MOValue<T_GoodsID>							m_TGoodsID;													// 케릭터 GoodsID
	S2MOValue<UINT8>								m_ui8Class;													// 병과( v1.5만 사용 )
	S2MOStringW<NET_CHARA_NICK_NAME_SIZE>			m_strCharaName;												// 캐릭터 이름 ( v1.5만 사용 )

	PACKET_CHAR_CREATE_CHARA_REQ()
	{
		// Last Update : 2015-10-20 20:55:03
		_SetValue( m_ui8CreateType );
		_SetValue( m_TGoodsID );
		_SetValue( m_ui8Class );
		_SetValue( m_strCharaName );
	}
};

struct PACKET_CHAR_CREATE_CHARA_ACK : public S2MOPacketBaseResultT<PROTOCOL_CHAR_CREATE_CHARA_ACK>
{
	S2MOValue<bool>									m_bAddCharaSlot;											// 캐릭터 슬롯 추가 여부.
	S2MOValue<UINT8>								m_ui8SlotIdx;												// 캐릭터 슬롯 번호.
	S2MOValue<UINT8>								m_ui8SlotState;												// 캐릭터 활성화 여부. MULTI_SLOT_STATE
	S2MOValue<UINT8>								m_ui8CreateType;											// 기본 생성, 포인트/캐쉬 구매. SIA_GOODS_BUY
	S2MOValue<UINT32>								m_ui32Point;												// 유저의 현재 Point
	S2MOValue<UINT32>								m_ui32Cash;													// 유저의 현재 Cash
	S2MOValue<S2MO_CHAR_BASE_INFO>					m_BaseInfo;
	S2MOValue<S2MO_CHAR_SKILL_INFO>					m_SkillInfo;
	S2MOValue<S2MO_CHAR_EQUIP_INFO>					m_CharEquipInfo;

	PACKET_CHAR_CREATE_CHARA_ACK()
	{
		// Last Update : 2015-10-23 17:29:29
		_SetValue( m_bAddCharaSlot );
		_SetValue( m_ui8SlotIdx );
		_SetValue( m_ui8SlotState );
		_SetValue( m_ui8CreateType );
		_SetValue( m_ui32Point );
		_SetValue( m_ui32Cash );
		_SetValue( m_BaseInfo );
		_SetValue( m_SkillInfo );
		_SetValue( m_CharEquipInfo );
	}
};

struct PACKET_CHAR_CHANGE_CHARA_NICK_ACK : public S2MOPacketBaseResultT<PROTOCOL_CHAR_CHANGE_CHARA_NICK_ACK>
{
};

struct PACKET_CHAR_CHANGE_EQUIP_REQ : public S2MOPacketBaseT<PROTOCOL_CHAR_CHANGE_EQUIP_REQ>
{
	S2MOValue<UINT8, TEAM_COUNT>										m_ui8TeamSlot; 							// 팀별 대표 캐릭 슬롯. V1.5는 0번째 배열이 메인 슬롯.								

	S2MOValue<bool>														m_bCommonEquipFlag;						// 공통파츠 변경 유무
	S2MOValue<ITEM_INFO, CHAR_EQUIPMENT_COMMON_COUNT>					m_aCommonEquips;

	S2MOValue<UINT8>													m_ui8ChangeSlotCount;					// 변경한 캐릭장착정보 개수
	S2MOValue<S2_CHAR_CHANGE_EQUIP_INFO, S2MO_MULTI_SLOT_PACKET_COUNT>	m_ChangeEquipInfo;

	PACKET_CHAR_CHANGE_EQUIP_REQ()
	{
		// Last Update : 2015-10-16 15:34:54
		_SetValue( m_ui8TeamSlot );
		_SetValue( m_bCommonEquipFlag );
		_SetValue( m_aCommonEquips );
		_SetValue( m_ui8ChangeSlotCount );
		_SetValue( m_ChangeEquipInfo );
	}
};

struct PACKET_CHAR_CHANGE_EQUIP_ACK : public S2MOPacketBaseResultT<PROTOCOL_CHAR_CHANGE_EQUIP_ACK>
{
};

struct PACKET_CHAR_CHANGE_STATE_ACK : public S2MOPacketBaseResultT<PROTOCOL_CHAR_CHANGE_STATE_ACK>
{
	S2MOValue<UINT8>								m_ui8SlotIdx;	// 캐릭터 슬롯 번호
	S2MOValue<UINT8>								m_ui8SlotState;	// 캐릭터 슬롯 상태. MULTI_SLOT_STATE

	PACKET_CHAR_CHANGE_STATE_ACK()
	{
		// Last Update : 2015-10-12 14:24:54
		_SetValue( m_ui8SlotIdx );
		_SetValue( m_ui8SlotState );
	}
};


#pragma pack( pop )

#endif	