#ifndef _GroupMaker_h
#define _GroupMaker_h

#include "UdpBuffer.h"

class CRelayGroup;

class CGroupMaker: public i3ElementBase
{
	I3_CLASS_DEFINE(CGroupMaker);

	UINT32			m_wGroupCount;
	UINT32*			m_pwGroupIdx;
	UINT32			m_wGroupCreateCount;
	UINT32			m_wGroupDeleteCount;
	CRelayGroup**	m_ppGroups;

public:
	CGroupMaker();
	virtual ~CGroupMaker();

	BOOL			Create(void);
	void			OnUpdate_C( UINT32 wUpdateCheckStartIdx );

	BOOL			JoinGroup( INT32 i32Thread, RELAY_MEMBER_INFO* pRelayMember, UINT16 sPort );
	BOOL			WithdrawGroup_C(UINT32 wGroupIdx, UINT8 cSlotIdx );
	//BOOL			WithdrawGroup( RELAY_MEMBER_INFO* pRelayMember );

	BOOL			GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer );
    
	CRelayGroup*	GetGroup(UINT32 wGroupIdx) const;

	UINT32			MakeGroupId_C( );
	UINT32			RegisterGroup_C( BATTLE_ROOM_INFO* pRelayRoomInfo, UINT32 wGroupIdx );
	BOOL			RevokeGroup_C(UINT32 wGroupIdx);
	BOOL			RevokeGroupUpdate_C( UINT32 wGroupIdx );

	UINT32			GetGroupCount()						{	return m_wGroupCount;	};
};

extern CGroupMaker* g_pGroupMaker;

#endif
