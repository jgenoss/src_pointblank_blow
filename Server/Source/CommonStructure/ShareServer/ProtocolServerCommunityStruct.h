#ifndef __PROTOCOL_SERVER_COMMUNITY_STRUCT_H__
#define __PROTOCOL_SERVER_COMMUNITY_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////
// PROTOCOL_COMMUNITY_USER_INVITED_REQ
//////////////////////////////////////////////////////////////////////
struct PACKET_TRANS_COMMUNITY_USER_INVITED_REQ
{
	INT32			m_i32SessionIdx;		// 초대한 유저Session
	T_UID			m_i64UID;				// 초대한 유저UID
	T_UID			m_i64InviteUser;		// 초대할 유저UID
};

struct PACKET_TRANS_COMMUNITY_USER_INVITED_ACK
{
	INT32			m_i32Rv;				// 성공이면 진행 실패면 무시
	INT32			m_i32SessionIdx;		// 초대한 유저Session
	T_UID			m_i64UID;				// 초대한 유저UID
	T_UID			m_i64InviteUserUID;		// 초대할 유저UID	
};

struct PACKET_TRANS_COMMUNITY_USER_INVITED_REQUEST_ACK
{
	INT32			m_i32SessionIdx;				// 초대할 유저Session
	T_UID			m_i64UID;						// 초대할 유저UID
	UINT32			m_ConnectState;					// 접속위치
	TTCHAR			m_strNick[NET_NICK_NAME_SIZE];	// 초대할 유저Nick
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)
#endif