#ifndef _RelayGroup_h
#define _RelayGroup_h

#include "RelayMember.h"
#include "UdpBuffer.h"

class CRelayMember;

class CRelayGroup: public i3ElementBase
{
	I3_CLASS_DEFINE( CRelayGroup );

	ENUM_GROUP_STATE	m_eState;
	BATTLE_ROOM_INFO	m_RelayRoomInfo;
	CRelayMember*		m_pRelayMember;
	UINT32				m_wEncryptKey;	
	UINT32				m_wCreateTime;
	UINT16				m_sPort;


public:
	CRelayGroup();
	virtual ~CRelayGroup();

	BOOL				Create();
	UINT32				Register_C( BATTLE_ROOM_INFO* pRelayRoomInfo );
	void				Revoke_C(void);
	BOOL				JoinGroup( INT32 i32Thread, RELAY_MEMBER_INFO* pRelayMember, UINT16 sPort );
	BOOL				Withdraw( UINT8 cSlotIdx );
	BOOL				Withdraw_C( UINT8 cSlotIdx );

	BOOL				CheckMember( UINT8 cSlotIdx );

	BOOL				GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer );
	BOOL				CheckEncryptKey( UINT32 wEncryptKey );

	void				SetGroupCreateTime( )							{	m_wCreateTime = g_pConfig->GetServerTime();	};

	CRelayMember*		GetMember()										{	return m_pRelayMember;			};
	CRelayMember*		GetMember( INT32 nIdx )							{	return &m_pRelayMember[ nIdx ];	};
	BATTLE_ROOM_INFO*	GetRoomInfo()									{	return &m_RelayRoomInfo;		};
	ENUM_GROUP_STATE	GetGroupState()									{	return m_eState;				};
	void				SetGroupState( ENUM_GROUP_STATE	eState )		{	m_eState = eState;				};
	UINT32				GetGroupCreateTime()							{	return m_wCreateTime;			};
	UINT32				GetEncryptKey()									{	return m_wEncryptKey;			};
	UINT16				GetPort()										{	return m_sPort;					};
};

#endif
