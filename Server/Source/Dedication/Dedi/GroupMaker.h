// GroupMaker.h
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		
//	

#ifndef _GroupMaker_h
#define _GroupMaker_h

#include "UdpBuffer.h"

class CDediRoom;

class CGroupMaker: public i3ElementBase
{
	I3_CLASS_DEFINE(CGroupMaker);

	BOOL			m_bAt;
	INT32			m_i32AllocatedGroupCount;	// 미리 할당된 그룹
	INT32			m_i32RoomCount;			// 현재 사용중인 그룹
	INT32*			m_pi32GroupIdx;
	INT32			m_i32GroupCreateCount;
	INT32			m_i32GroupDeleteCount;
	CDediRoom**	m_ppRooms;

public:

	CGroupMaker();
	virtual ~CGroupMaker();

	BOOL			Create(void);
	void			OnUpdate_C	 ( UINT32 wUpdateCheckStartIdx );

	BOOL			GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer );
	CDediRoom*		GetGroup( INT32 wGroupIdx) const;
	CDediRoom*		GetGroupInBattle( INT32 i32GroupIdx ) const;

	UINT32			MakeGroupId_C( );
	UINT32			RegisterGroup_C		( P_BATTLE_ROOM_CREATE_ACK* pInfo, const P_BATTLE_ROOM_CREATE_REQ *pReq, char* pExtInfo, UINT16 ui16ExtSize, int SocketIndex ); 
	EVENT_ERROR		EnterGroup_C		( P_BATTLE_USER_ENTER_REQ* pReq );
	EVENT_ERROR		LeaveGroup_C		( P_BATTLE_USER_LEAVE_REQ* pReq );	// 유저 퇴장
	BOOL			RevokeGroup_C		( UINT32 wGroupIdx);
	BOOL			RevokeGroupUpdate_C	( UINT32 wGroupIdx );
	BOOL			SetRoundInfo_C		( P_BATTLE_ROUND_REQ* pRoundInfo );
	BOOL			SetDinoInfo_C		( P_BATTLE_M_DINO_INFO_REQ* pDinoInfo );
	BOOL			RespawnReq_C		( P_BATTLE_RESPAWN_REQ* pRespawn ); 
	BOOL			SetQACommand_C		( P_BATTLE_QA_AUTO_REQ* pQAInfo );

	UINT32			GetGroupCount()		{	return m_i32RoomCount;	};
	INT32			GetAllocGroupCount(){	return m_i32AllocatedGroupCount; };
	void			SetAtMode( BOOL bStart )	{	m_bAt = bStart;		};
	BOOL			GetAtMode()			{	return m_bAt;		};
	INT32			GetAllUserCount(void);
};

extern CGroupMaker* g_pRoomManager;

#endif
