#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__

#include "UserSession.h"

class CUserSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE( CUserSessionManager );
protected:
	
	// 문제 확인을 위한 땜빵 로그
	INT32				m_i32Hour;
	SV_STATE_LOG*		m_pStateLog;
	// 문제 확인을 위한 땜빵 로그

	INT32				m_i32ThreadCount;
	i3RingBuffer**		m_ppRing;

	INT32				m_i32GameServerCount;
	INT32*				m_pi32GameServerLink;

	INT32				m_i32BattleServerCount;
	INT32*				m_pi32BattleServerLink;

public:
	CUserSessionManager();
	virtual ~CUserSessionManager();

	BOOL					OnCreate( INT32 i32GameCount, INT32 i32BattleCount, INT32 i32ThreadCount );
	virtual void			OnUpdate();
	virtual BOOL			OnDestroy(void);

	virtual ULONG_PTR		ConnectSession( SOCKET hSocket, struct sockaddr_in * pAddr );

	INT32					GetBufferCount()					{	return m_i32ThreadCount;	}
	BOOL					GetBuffer( INT32 i32SessionIdx, INT32& i32ServerIdx, i3NetworkPacket * pPacket );

	BOOL					IsActive( INT32 i32SessionIdx );
	BOOL					SendPacketMessage( INT32 i32SessionIdx, i3NetworkPacket * pPacket );
	BOOL					SendPacketMessageGame( INT32 i32Idx, i3NetworkPacket * pPacket );
	BOOL					SendPacketMessageBattle( INT32 i32Idx, i3NetworkPacket * pPacket );
	
	inline CUserSession*	GetSession(INT32 i32SessionIdx );

	BOOL					SetServerIdx( ASC_SERVER_TYPE eServerType, INT32 i32Idx, INT32 i32SessionIdx );
};

// inline Function
CUserSession* CUserSessionManager::GetSession(INT32 i32SessionIdx )
{
	if(i32SessionIdx >= m_MaxSessionIdx || i32SessionIdx < 0 ) return NULL;  //잘못된 정보를 요청한것 입니다. 간단한 예외처리를 합니다. 
	CUserSession * pSession = (CUserSession *)m_pSessionList[ i32SessionIdx ];
	return pSession; 
}

#endif