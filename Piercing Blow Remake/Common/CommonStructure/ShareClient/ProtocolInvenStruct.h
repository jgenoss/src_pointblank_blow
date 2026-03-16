#ifndef __PROTOCOL_INVEN_STRUCT_H__
#define __PROTOCOL_INVEN_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_INVENTORY_LEAVE_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_INVENTORY_NEW_LEAVE_REQ
{
	UINT8						m_ui8CharaSlotIdx;										// 캐릭터 대표 슬롯 번호(화면에 표시중인 캐릭고유번호(m_ui8CharaSlotIdx))
	CHANGE_CHARA_EQUIP_REQ		m_ChangeCharaReq;
};

struct PACKET_D_INVENTORY_LEAVE_CHANGE_CHARA_EQUIP_SETTING
{
	ITEM_INFO									m_EquipmentInfo[ CHAR_EQUIPMENT_COUNT ];	
};

// 응답
struct PACKET_INVENTORY_NEW_LEAVE_ACK
{
	UINT8										m_ui8CharaSlotIdx;						// 캐릭터 대표 슬롯 번호(화면에 표시중인 캐릭고유번호(m_ui8CharaSlotIdx))
	CHANGE_CHARA_EQUIP_ACK						m_ChangeCharaAck;
};
// 성공실패 관계없이 1개이상의 장비 변경 정보가 있으면 아래 패킷 전송
struct PACKET_INVENTORY_NEW_LEAVE
{
	CHANGE_CHARA_EQUIP_SETTING					m_ChangeCharaEquip;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_INVENTORY_NEW_GET_INFO_REQ(ACK)
// 인벤정보 변경시 변경된 물품의 인벤정보갱신
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_INVENTORY_NEW_GET_INFO_ACK
{
	TItemCnt m_ui16CharaCount;
	TItemCnt m_ui16WeaponCount;
	TItemCnt m_ui16ItemCount;
};

struct PACKET_D_INVENTORY_NEW_GET_INFO
{
	INVEN_BUFFER	m_InvenBuffer;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif