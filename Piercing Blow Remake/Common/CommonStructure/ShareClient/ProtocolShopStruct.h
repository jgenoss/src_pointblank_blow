#ifndef __PROTOCOL_SHOP_STRUCT_H__
#define __PROTOCOL_SHOP_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SHOP_LEAVE_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_SHOP_NEW_LEAVE_REQ
{
	UINT8										m_ui8CharaSlotIdx;						// 캐릭터 대표 슬롯 번호(화면에 표시중인 캐릭고유번호(m_ui8CharaSlotIdx))
	CHANGE_CHARA_EQUIP_REQ						m_ChangeCharaReq;
};

//struct PACKET_D_SHOP_NEW_LEAVE
//{
//	CHANGE_CHARA_EQUIP_REQ	m_stEquipInfo;
//};

struct PACKET_D_SHOP_NEW_LEAVE_CHANGE_CHARA_EQUIP_SETTING
{
	CHANGE_CHARA_EQUIP_SETTING					m_CharaEquipSetting;
};

struct PACKET_D_SHOP_NEW_LEAVE_CHANGE_CHARA_EQUIP_SETTING_WEAPON
{
	CHANGE_CHARA_EQUIP_SETTING_WEAPON			m_CharaWeaponInfo;						// 무기 정보
};

struct PACKET_D_SHOP_NEW_LEAVE_CHANGE_CHARA_EQUIP_SETTING_PARTS
{
	CHANGE_CHARA_EQUIP_SETTING_PARTS			m_CharaPartsInfo;						// 파츠 정보
};

struct PACKET_SHOP_NEW_LEAVE_ACK
{
	UINT8										m_ui8CharaSlotIdx;						// 캐릭터 대표 슬롯 번호(화면에 표시중인 캐릭고유번호(m_ui8CharaSlotIdx))
	CHANGE_CHARA_EQUIP_ACK						m_ChangeCharaAck;
};

struct PACKET_SHOP_NEW_LEAVE
{
	CHANGE_CHARA_EQUIP_SETTING					m_ChangeCharaEquip;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SHOP_NEW_RS_ROULETTE_START_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_SHOP_NEW_RS_ROULETTE_START_REQ
{
	BOOL				m_bIsHidden;
	RS_ROULETTE_INFO	m_RsRInfo;
};

struct PACKET_H_SHOP_NEW_RS_ROULETTE_START_ACK
{
	INT32			m_i32Rv;

	RS_TAB			m_eTabIdx;
	RS_ITEM_GRADE	m_eItemGrade;
	UINT32			m_ui32PrizeGoodsID;
	UINT8			m_ui8RsIdx;
	UINT8			m_ui8TabUseCount;
	UINT32			m_ui32Cash;
	TItemCnt		m_ui16CharaCount;
	TItemCnt		m_ui16WeaponCount;
	TItemCnt		m_ui16ItemCount;

};

struct PACKET_D_SHOP_NEW_RS_ROULETTE_START
{
	INVEN_BUFFER	m_InvenBuffer;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif