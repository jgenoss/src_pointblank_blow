#ifndef __PROTOCOL_ROOM_STRUCT_H__
#define __PROTOCOL_ROOM_STRUCT_H__

#pragma pack( push, 1)
////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_ROOM_GET_SLOTINFO_REQ	// 방의 슬롯 정보와 닉네임을 보내기
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_ROOM_NEW_GET_SLOTINFO_REQ
{
};

struct	PACKET_ROOM_NEW_GET_SLOTINFO_ACK
{
	INT32				m_i32Slot;
	SLOT_INFO			m_SlotInfo[SLOT_MAX_COUNT];	
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_ROOM_NEW_GET_SLOTONEINFO_REQ	// 방의 슬롯 정보와 닉네임을 보내기
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_ROOM_NEW_GET_SLOTONEINFO_REQ
{
};

struct	PACKET_ROOM_NEW_GET_SLOTONEINFO_ACK
{
	INT32				m_i32Slot;
	SLOT_INFO			m_SlotInfo;
	char				m_strNickName[NET_NICK_NAME_SIZE];
	UINT8				m_NickColor;
	UINT32				m_CharaItemID;
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_ROOM_CHARA_SHIFT_POS_REQ	// 룸에서 대표 캐릭을 교체시
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_ROOM_NEW_CHARA_SHIFT_POS_REQ
{
	UINT8		m_ui8CharaSlotIdx;								// 캐릭터 대표 3종 슬롯 번호( 0번배열:화면에 나오는 대표캐릭, 그외는 선택한 대표캐릭)
};

//struct	PACKET_ROOM_NEW_CHARA_SHIFT_POS_ACK
//{
//	INT32		m_i32Rv;										// 대표 캐릭 교체 성공,실패 응답
//	UINT8		m_ui8CharaSlotIdx;								// 캐릭터 대표 3종 슬롯 번호( 0번배열:화면에 나오는 대표캐릭, 그외는 선택한 대표캐릭)
//};

struct	PACKET_ROOM_NEW_CHARA_SHIFT_POS_OTHER_ACK
{
	INT32		m_i32Slot;										// 대표 캐릭을 교체한 슬롯(룸에 있는 유저들이 사용함)
	T_ItemID		m_i32CharaItemID;								// 대표 캐릭의 아이템 아이디(룸의 슬롯에 표시할 초상화를 위해)
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_ROOM_GET_USER_EQUIPMENT_REQ	// 유저 장착 정보 보기(룸에서 캐릭터뷰에 표시)
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_ROOM_NEW_GET_USER_EQUIPMENT_REQ
{
	INT8			m_i8SlotIdx;
};

struct	PACKET_ROOM_NEW_GET_USER_EQUIPMENT_ACK
{
	INT32			m_i32Rv;
	INT8			m_i8SlotIdx;
	POS_MULTI_SLOT	m_PosMultiSlot;
};




// 준영 ViewOtherItem
////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_ROOM_GET_USER_ITEM_ACK	// 유저 장착 정보 보기(캐쉬형태)
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_H_ROOM_GET_USER_ITEM_ACK
{
	POS_MULTI_SLOT	m_PosMultiSlot;	// 대표 캐릭 장착 정보
	INT32			m_i32ItemCount;	// 캐쉬 아이템 갯수. 최대 MAX_INVEN_COUNT
};

// 아래 구조체를 m_i32ItemCount 갯수만큼 붙인다.
// 캐쉬 아이템 정보 
struct	PACKET_D_ROOM_GET_USER_ITEM
{
	INT32			m_i32ItemID;	// 캐쉬 아이템 ItemID.
};


//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif