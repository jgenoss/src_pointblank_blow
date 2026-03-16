#ifndef __PROTOCOL_BASE_STRUCT_H__
#define __PROTOCOL_BASE_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_REQ
//////////////////////////////////////////////////////////////////////////
struct	PACKET_BASE_NEW_CHARA_SHIFT_POS_REQ
{
	UINT8		m_ui8CharaSlotIdx[TEAM_COUNT];			// 캐릭터 대표 3종 슬롯 번호( 0번배열:화면에 나오는 대표캐릭, 그외는 선택한 대표캐릭)
};

struct	PACKET_BASE_NEW_CHARA_SHIFT_POS_ACK
{
	INT32		m_i32Rv;
	UINT8		m_ui8CharaSlotIdx;			// 캐릭터 대표 3종 슬롯 번호( 0번배열:화면에 나오는 대표캐릭, 그외는 선택한 대표캐릭)
};
struct	PACKET_BASE_NEW_CHARA_SHIFT_POS_ACK2
{
	INT32		m_i32Rv;
	UINT8		m_ui8CharaSlotIdx[3];		// 캐릭터 대표 3종 슬롯 번호( 0번배열:화면에 나오는 대표캐릭, 그외는 선택한 대표캐릭)
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_NEW_GET_OPTION_REQ					// 유저 환경 설정 로드
///////////////////////////////////////////////////////////////////////////////////
struct	UserEnvironment
{
	UINT32			m_ui32ChangeFlag;
	UINT8			m_ui8DefaultKey;
	NET_ENV_SET		m_NetEnvSet;
	GAME_KEY_STRUCT	m_KeyMapArray[GAME_KEY_MAP_COUNT];
	char			m_szMacro[MACRO_MAX_COUNT][MACRO_MAX_LENGHT];
};

// [생략] PACKET_BASE_GET_OPTION_REQ
struct	PACKET_H_BASE_GET_OPTION_ACK
{
	INT32			m_i32Rv;								// 성공이면 진행 실패면 무시	
	UINT8			m_ui8DefaultKey;
};

struct	PACKET_D_BASE_GET_OPTION
{
	NET_ENV_SET		m_NetEnvSet;
	GAME_KEY_STRUCT	m_KeyMapArray[GAME_KEY_MAP_COUNT];
};

struct	PACKET_H_BASE_GET_OPTION_MACRO_ACK
{
	INT32			m_i32Rv;								// 성공이면 진행 실패면 무시	
	UINT8			m_ui8Size[MACRO_MAX_COUNT];
};

struct	PACKET_D_BASE_GET_OPTION_MACRO
{
	char			m_szMacro[MACRO_MAX_LENGHT];
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_OPTION_SAVE_REQ				// 유저 환경 설정 저장
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_BASE_OPTION_SAVE_REQ
{	
	UINT32			m_ui32ChangeFlag;
	NET_ENV_SET		m_NetEnvSet;
	GAME_KEY_STRUCT	m_KeyMapArray[GAME_KEY_MAP_COUNT];
};

struct	PACKET_BASE_OPTION_SAVE_ACK
{
	INT32			m_i32Rv;						// 성공이면 진행 실패면 무시
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_OPTION_MACRO_SAVE_REQ			// 유저 환경 설정(MACRO) 저장
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_H_BASE_OPTION_MACRO_SAVE_REQ
{	
	UINT32			m_ui32ChangeFlag;
	UINT8			m_ui8Size[MACRO_MAX_COUNT];
};

struct	PACKET_D_BASE_OPTION_MACRO_SAVE
{
	char			m_szMacro[MACRO_MAX_LENGHT];
};

// [생략] struct	PACKET_BASE_OPTION_SAVE_ACK

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_NEW_REWARD_POPUP_ACK			// 보상 팝업
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_H_BASE_REWARD_POPUP_ACK
{
	INT32			m_i32Rv;
	UINT8			m_i8RequestType;	// ITEM_INSERT_REQUEST_TYPE
};

// m_ui8RequestType가 ITEM_INSERT_REQUEST_TS_BONUS이면
struct	PACKET_D_BASE_REWARD_DAILY_GIFT
{
	UINT32			m_TItemID;
	UINT32			m_ui32ItemArg;
};

// m_ui8RequestType가 ITEM_INSERT_REQUEST_DOMI_REWARD이면
struct	PACKET_D_BASE_REWARD_DOMI
{
	UINT16			m_ui16Point;
	UINT32			m_TItemID;
	UINT32			m_ui32ItemArg;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif