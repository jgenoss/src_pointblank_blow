#ifndef __PROTOCOL_SKILL_STRUCT_H__
#define __PROTOCOL_SKILL_STRUCT_H__

#pragma pack( push, 1)

struct PACKET_SKILL_SET_REQ	// 1.5 주특기 셋팅 요청 구조체.
{
	UINT8				m_ui8SlotIdx;

	COMMON_SKILL_TYPE	m_CommonSkill;
	ASSIST_SKILL_TYPE	m_AssistSkill;
	MAIN_SKILL_TYPE		m_MainSkill;

	UINT8				m_ui8MainLv;
	UINT8				m_ui8AssistLv;
	UINT8				m_ui8CommonLv;

};

struct PACKET_SKILL_SET_ACK	// 1.5 주특기 셋팅 응답 구조체.
{
	INT32				m_i32Rv;
};

struct PACKET_SKILL_RESET_REQ	// 1.5 주특기 초기화 요청 구조체.
{
};

struct PACKET_SKILL_RESET_ACK	// 1.5 주특기 초기화 응답 구조체.
{
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SET_SPECIALITY_CLASS_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_SKILL_CLASS_SET_REQ	// 1.5 주특기 클래스 셋팅 요청 구조체.
{
	UINT8			m_ui8Slot;		// 캐릭터 슬롯번호
	UINT8			m_ui8Class;		// 주무기 클래스 eSKILL_CLASS_PRIMARY
};
struct PACKET_SKILL_CLASS_SET_ACK	// 1.5 주특기 클래스 셋팅 응답 구조체.
{
	INT32			m_i32Rv;
	UINT8			m_ui8Slot;		// 캐릭터 슬롯번호
	UINT8			m_ui8Class;		// 주무기 클래스 eSKILL_CLASS_PRIMARY
};

#pragma pack(pop)

#endif