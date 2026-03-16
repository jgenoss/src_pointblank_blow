#ifndef __PROTOCOL_LOBBY_STRUCT_H__
#define __PROTOCOL_LOBBY_STRUCT_H__

#pragma pack( push, 1)
//////////////////////////////////////////////////////////////////////////
// PROTOCOL_ROOM_CREATE_REQ			// 방 생성하기
//////////////////////////////////////////////////////////////////////////
struct PACKET_LOBBY_CREATE_ROOM_REQ
{
	ROOM_INFO_BASIC		m_RoomInfoBasic;
	ROOM_INFO_ADD		m_RoomInfoAdd;
	char				m_szRoomPw[NET_ROOM_PW];
	INT8				m_enemyAiCount;
	INT8				m_difficultyLevel;
	UINT8				m_ui8CharMainSlotIdx;			// Main Char Slot Idx
};

struct PACKET_LOBBY_CREATE_ROOM_ACK
{
	INT32				m_i32RoomIdx;
	ROOM_INFO_BASIC		m_RoomInfoBasic;
	ROOM_INFO_ADD		m_RoomInfoAdd;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_ROOM_JOIN_REQ			// 방 입장하기
//////////////////////////////////////////////////////////////////////////
struct PACKET_LOBBY_JOIN_ROOM_REQ
{
	UINT32				m_ui32RoomIdx;					// Room Index
	char				m_szRoomPw[NET_ROOM_PW];		// Room Password
	UINT8				m_cForce;						// Room cForce
	UINT8				m_ui8CharMainSlotIdx;			// Main Char Slot Idx
};

struct PACKET_H_LOBBY_JOIN_ROOM_ACK
{
	INT32				m_i32Slot;						// Join User SlotNum( Error : Slot Num < 0 )
	ROOM_INFO_BASIC		m_RoomInfoBasic;				// Room Basic Info
	ROOM_INFO_ADD		m_RoomInfoAdd;					// Room Additional Info
	ROOM_NOW_SCORE		m_RoomNowScore;					// RoomNowScore

	char				m_strRoomPasswd[NET_ROOM_PW];	// Room Password
	INT8				m_i8StartCountDown;				// Room StartCountDown
	INT32				m_i32MainSlotIdx;				// Room Creator SlotNum
	SLOT_INFO			m_SlotInfo[SLOT_MAX_COUNT];		// Room Slot Info
	UINT8				m_PersonNow;					// Current User Count
	INT8				m_enemyAiCount;					// enemy AI Count
	INT8				m_difficultyLevel;				// AI DefficultyLevel

	UINT8				m_ui8Count;						// User Info Count

};

struct PACKET_D_LOBBY_JOIN_ROOM_USERINFO
{
	UINT8				m_ui8SlotNum;						// User Slot Number
	char				m_strNickName[NET_NICK_NAME_SIZE];	// User NickName
	UINT8				m_ui8NickColor;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_LOBBY_NEW_VIEW_USER_ITEM_REQ
{

};




// 준영 ViewOtherItem
////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK	// 유저 장착 정보 보기(캐쉬형태)
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_H_LOBBY_NEW_VIEW_USER_ITEM_ACK
{
	POS_MULTI_SLOT	m_PosMultiSlot;	// 대표 캐릭 장착 정보
	INT32			m_i32ItemCount;	// 캐쉬 아이템 갯수. 최대 MAX_INVEN_COUNT.
};

// 아래 구조체를 m_i32ItemCount 갯수만큼 붙인다.
// 캐쉬 아이템 정보 
struct	PACKET_D_LOBBY_NEW_VIEW_USER_ITEM
{
	INT32			m_i32ItemID;	// 캐쉬 아이템 ItemID.
};


//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif