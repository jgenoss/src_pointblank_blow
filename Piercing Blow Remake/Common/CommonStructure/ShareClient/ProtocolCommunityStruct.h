#ifndef __PROTOCOL_COMMUNITY_STRUCT_H__
#define __PROTOCOL_COMMUNITY_STRUCT_H__

#pragma pack( push, 1)

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_NEW_FIND_SESSION_USER_POSITION_REQ,			// 동일서버   유저 위치 찾기 요청(불법)
// PROTOCOL_BASE_NEW_FIND_UID_USER_POSITION_REQ,				// UID로	  유저 위치 찾기 요청(불법)
// PROTOCOL_BASE_NEW_FIND_NICK_USER_POSITION_REQ,				// 닉네임으로 유저 위치 찾기 요청(불법)
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_BASE_FIND_SESSION_USER_POSITION_REQ				// 동일서버 유저 간략 정보 요청
{
	INT32				m_i32UserSessionIdx;
};
struct	PACKET_BASE_FIND_UID_USER_POSITION_REQ					// UID로 유저 간략 정보 요청
{
	T_UID				m_i64FindUID;
};
struct	PACKET_BASE_FIND_NICK_USER_POSITION_REQ					// 닉네임으로 유저 간략 정보 요청
{
	char				m_strNick[NET_NICK_NAME_SIZE];
};

struct	PACKET_BASE_USER_POSITION_ACK
{
	INT32				m_i32Rv;						// 성공이면 진행 실패면 무시
	T_UID				m_i64FindUID;					// 유저 고유번호
	//UINT32				m_ui32UserConnectState;		// 유저 현재 위치(MAKE_FRIEND_STATE)

	INT32				m_State;
	INT32				m_ServerIdx;
	INT32				m_ChannelNum;
	INT32				m_RoomIdx;
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_FIND_SESSION_GET_USER_BASIC_INFO_REQ		//유저간략정보 보기(3가지 방법 중 택 1)
// PROTOCOL_BASE_FIND_UID_GET_USER_BASIC_INFO_REQ			//유저간략정보 보기(3가지 방법 중 택 1)
// PROTOCOL_BASE_FIND_NICK_GET_USER_BASIC_INFO_REQ			//유저간략정보 보기(3가지 방법 중 택 1)
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_BASE_FIND_SESSION_USER_BASIC_INFO_REQ			// 동일서버 유저 간략 정보 요청
{
	INT32				m_i32UserSessionIdx;					// 찾을 유저
};
struct	PACKET_BASE_FIND_UID_USER_BASIC_INFO_REQ				// UID로 유저 간략 정보 요청
{
	T_UID				m_i64FindUID;							// 찾을 유저
};
struct	PACKET_BASE_FIND_NICK_USER_BASIC_INFO_REQ				// 닉네임으로 유저 간략 정보 요청
{
	char				m_strNick[NET_NICK_NAME_SIZE];			// 찾을 유저
};

struct	PACKET_BASE_USER_BASIC_INFO_ACK
{
	INT32		m_i32Rv;								// 성공이면 진행 실패면 무시

	T_UID		m_i64FindUID;							// 찾을 유저

	UINT32		m_clanMark;
	TTCHAR		m_strClanName[NET_CLAN_NAME_SIZE];

	UINT32		m_rank;
	UINT32		m_exp;
	TTCHAR		m_nickName[NET_NICK_NAME_SIZE];
	UINT8		m_nickColor;
	UINT32		m_killCount;
	UINT32		m_deathCount;

	INT32		m_State;
	INT32		m_ServerIdx;
	INT32		m_ChannelNum;
	INT32		m_RoomIdx;
	UINT32		m_ui32TotalBattleTime;

	UINT8		m_ui8SubTask;

	UINT32		m_DisguiseRank;
	UINT32		m_AprilFoolRank;
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_BASE_NEW_FIND_UID_GET_USER_DETAIL_INFO_REQ			//유저상세정보 보기
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_BASE_FIND_UID_USER_DETAIL_INFO_REQ				// UID로 유저 상세 정보 요청
{
	T_UID					m_i64FindUID;
};

struct	PACKET_H_BASE_USER_DETAIL_INFO_ACK
{
	INT32					m_i32Rv;				// 성공이면 진행 실패면 무시

	USER_INFO_RECORD		m_UserInfoRecord;		// 유저 전적 정보
	USER_INFO_LAST_GAME		m_UserInfoLastGame;		// 마지막 게임정보(온라인 유저 아니면 초기값)

	UINT8					m_iCount;				// 보유 캐릭슬롯수
};

struct	PACKET_D_BASE_USER_DETAIL_INFO_CHARA
{
	UINT8					m_ui8CharaState;
	UINT8					m_ui8SlotNum;
	UINT32					m_ui32CharaItemID;
	TTCHAR					m_strCharaName[NET_CHARA_NICK_NAME_SIZE];
	UINT32					m_ui32CreateDate;
	CHARA_SLOT_SKILL		m_CharaSkill;
};

////////////////////////////////////////////////////////////////////////////////////
// PROTOCOL_COMMUNITY_USER_INVITED_REQ	// 배틀룸에 유저찾기로 검색된 유저 초대
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_COMMUNITY_USER_INVITED_REQ
{
	INT32			m_SessionIdx;			// 초대한 유저Session
	T_UID			m_i64UID;				// 초대한 유저UID
	T_UID			m_i64InviteUser;		// 초대할 유저UID
};

struct	PACKET_COMMUNITY_USER_INVITED_ACK
{
	INT32			m_i32Rv;				// 성공이면 진행 실패면 무시
};

struct	PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK
{
	UINT32			m_ConnectState;					// 접속위치
	TTCHAR			m_strNick[NET_NICK_NAME_SIZE];	// 초대할 유저Nick
};

//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// PACKET_BASE_GET_USER_SUBTASK_REQ	// 로비 유저 초대시 가져오는 추가 정보
///////////////////////////////////////////////////////////////////////////////////
struct	PACKET_H_BASE_USER_SUBTASK_ACK
{
	INT32			m_SessionId;					// SessionId
	UINT8			m_Subtask;						// SubTask
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)
#endif