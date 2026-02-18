#ifndef __PROTOCOL_NEW_STRUCT_H__
#define __PROTOCOL_NEW_STRUCT_H__

#pragma pack( push, 1)

struct PACKET_H_NEW_CHANGE_CHARA_NICK_GROUP_REQ
{
	//////////////////////////////////////////////////////////////////////////
	// Alloc Count
	UINT8		m_ui8CharaSlotCount;						// 닉네임을 변경할 캐릭터 슬롯의 갯수
};

struct PACKET_CHANGE_CHARA_NICK_REQ
{
	UINT8		m_ui8CharaSlotIdx;								// 캐릭터 슬롯 번호
	TTCHAR		m_strNewCharaName[NET_CHARA_NICK_NAME_SIZE];	// 캐릭터 닉네임 16바이트 
};

struct PACKET_D_NEW_CHANGE_CHARA_NICK_GROUP
{
	PACKET_CHANGE_CHARA_NICK_REQ	m_ChangeCharaNick;
};

struct PACKET_NEW_GUIDE_COMPLETE_REQ
{
	UINT16	m_ui16GuideEventIdx;				// 완료한 가이드 이벤트 인덱스. GUIDE_EVENT
};

struct PACKET_H_NEW_SET_CHARA_ACK
{
	INT32				m_i32Rv;								// 에러이면 PACKET_D_NEW_CREATE_CHARA_INFO 를 주지 않음
	UINT32				m_ui32OwnCharaSlotFlag;					// 생성된 캐릭터 슬롯의 flag
	UINT8				m_ui8CharaSlotCount;					// 생성된 캐릭터 슬롯 개수
	UINT8				m_ui8OwnMultiSlotCount;					// 생성된 캐릭터 슬롯 구매횟수 
	
	UINT8				m_ui8CharaSlotIdx;						// 설정된 캐릭터 슬롯 인덱스(배열 인덱스이기도 하며, MULTI_SLOT 의 CHARA_BASIC_INFO(CHARA_INFO) 의 m_ui8CharaSlotIdx 값과 동일)
};

struct PACKET_D_NEW_SET_CHARA_INFO
{
	CHARA_INFO			m_CharaInfo;
};

struct PACKET_GAME_SERVER_STATE_REQ
{
	UINT32		m_ui32ServerIdx;
};	

struct PACKET_H_GAME_SERVER_STATE_ACK
{
	//////////////////////////////////////////////////////////////////////////
	// Alloc Count
	UINT32		m_i32GameServerCount;
};

struct PACKET_D_GAME_SERVER_STATE_GAME_SERVER_INFO
{
	//////////////////////////////////////////////////////////////////////////
	// Alloc Count
	GAME_SERVER_INFO		m_GameServerInfo;
	UINT8					m_pChannelType[GAME_CHANNEL_COUNT];
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif