#ifndef __PROTOCOL_AUTH_STRUCT_H__
#define __PROTOCOL_AUTH_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_SHOP_GOODS_BUY_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_AUTH_NEW_SHOP_GOODS_BUY_REQ
{
	// Alloc Count
	UINT8			m_ui8GoodsCount;		// 구매할 상품 종류의 갯수
};

struct PACKET_D_AUTH_NEW_SHOP_GOODS_BUY_BASKET
{
	BUY_BASKET		m_Basket;
};

// 캐릭을 구매하는 경우에만 이 패킷을 사용한다.
struct PACKET_D_AUTH_NEW_SHOP_GOODS_BUY_CHARA
{
	char			m_strCharaName[NET_CHARA_NICK_NAME_SIZE];	// 캐릭터 닉네임 16바이트
};

struct PACKET_H_AUTH_NEW_SHOP_GOODS_BUY_ACK
{
	UINT32			m_ui32InvenTime;	

	UINT32			m_ui32Point;
	UINT32			m_ui32Cash;	

	TItemCnt		m_ui16CharaCount;
	TItemCnt		m_ui16WeaponCount;
	TItemCnt		m_ui16ItemCount;
};

struct PACKET_D_AUTH_NEW_SHOP_GOODS_BUY_INVEN_BUFFER
{	
	INVEN_BUFFER	m_InvenBuffer;
};

// 캐릭터 구매시 추가 슬롯 정보 전송
//struct PACKET_D_AUTH_NEW_SHOP_GOODS_BUY_CHARA_INFO
//{
//	UINT32					m_ui32OwnCharaSlotFlag;		// 생성된 캐릭터 슬롯의 flag
//
//	CHARA_BASIC_INFO		m_CharaBasicInfo;			// 캐릭터정보 // 주특기
//	CHARA_PARTS_INFO		m_CharPartsInfo;			// 파츠 정보
//	CHARA_WEAPON_INFO		m_CharWeaponInfo;			// 무기 정보
//};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_SHOP_ITEM_EXTEND_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_AUTH_NEW_SHOP_ITEM_EXTEND_REQ
{	
	EXTEND_BASKET		m_ExtendItemData;		// 연장할 기간에 대한 상세 데이터(상품을 던지면 구매와 동일하게 처리)
};

struct PACKET_H_AUTH_NEW_SHOP_ITEM_EXTEND_ACK
{
	UINT32			m_ui32InvenTime;	

	UINT32			m_ui32Point;
	UINT32			m_ui32Cash;	

	TItemCnt		m_ui16CharaCount;
	TItemCnt		m_ui16WeaponCount;
	TItemCnt		m_ui16ItemCount;
};

struct PACKET_D_AUTH_NEW_SHOP_ITEM_EXTEND_INVEN_BUFFER
{	
	INVEN_BUFFER	m_InvenBuffer;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_SHOP_ITEM_AUTH_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_AUTH_NEW_SHOP_ITEM_AUTH_REQ
{	
	T_ItemDBIdx		m_i64WareDBIdx;
};

struct PACKET_H_AUTH_NEW_SHOP_ITEM_AUTH_ACK
{
	INT32			m_i32Rv;
};

struct PACKET_D_AUTH_NEW_SHOP_ITEM_AUTH
{
	INVEN_BUFFER	m_InvenBuffer;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_SHOP_AUTH_GIFT_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_AUTH_NEW_SHOP_AUTH_GIFT_REQ
{
	T_GiftDBIdx		m_i32GiftWareIdx;
};

struct PACKET_H_AUTH_NEW_SHOP_AUTH_GIFT_ACK
{
	INT32			m_i32Rv;
	
	TItemCnt		m_ui16CharaCount;
	TItemCnt		m_ui16WeaponCount;
	TItemCnt		m_ui16ItemCount;
	TItemCnt		m_ui16CouponCount;
};

struct PACKET_D_AUTH_NEW_SHOP_AUTH_GIFT
{
	INVEN_BUFFER	m_InvenBuffer;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_SHOP_ITEM_REPAIR_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_AUTH_NEW_SHOP_ITEM_REPAIR_REQ
{
	T_ItemDBIdx		m_i64WareDBIdx;
};

struct PACKET_H_AUTH_NEW_SHOP_ITEM_REPAIR_ACK
{
	UINT32			m_ui32Point;
	UINT32			m_ui32Cash;	

	TItemCnt		m_ui16CharaCount;
	TItemCnt		m_ui16WeaponCount;
	TItemCnt		m_ui16ItemCount;
};

struct PACKET_D_AUTH_NEW_SHOP_ITEM_REPAIR
{
	INVEN_BUFFER	m_InvenBuffer;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_EQUIPMENT_REPAIR_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_AUTH_NEW_EQUIPMENT_REPAIR_REQ
{
	UINT8			m_ui8CharaSlotIdx;		// 캐릭터 슬롯 번호(배열번호와 동일)
};

// 응답은 [PACKET_H_AUTH_NEW_SHOP_ITEM_REPAIR_ACK] 사용

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_COUPON_AWARD_ITEM_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_AUTH_NEW_COUPON_AWARD_ITEM_REQ
{
	char			m_strCouponNum[MAX_COUPON_NUM+1];	
};

struct PACKET_H_AUTH_NEW_COUPON_AWARD_ITEM_ACK
{
	INT32			m_i32Rv;
	UINT32			m_ui32InvenTime;	

	TItemCnt		m_ui16CharaCount;
	TItemCnt		m_ui16WeaponCount;
	TItemCnt		m_ui16ItemCount;
};

struct PACKET_D_AUTH_NEW_COUPON_AWARD_ITEM_INVEN_BUFFER
{
	INVEN_BUFFER	m_InvenBuffer;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_AUTH_NEW_SHOP_DELETE_CHARA_ITEM_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_AUTH_NEW_SHOP_DELETE_CHARA_ITEM_REQ
{
	UINT8			m_ui8CharaSlotIdx;						// 캐릭터 대표 슬롯 번호(화면에 표시중인 캐릭고유번호(m_ui8CharaSlotIdx))
	T_ItemDBIdx		m_i64WareDBIdx;
};

struct PACKET_AUTH_NEW_SHOP_DELETE_CHARA_ITEM_ACK
{
	INT32			m_i32Rv;
	UINT8			m_ui8CharaSlotIdx;						// 캐릭터 대표 슬롯 번호(화면에 표시중인 캐릭고유번호(m_ui8CharaSlotIdx))	
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif