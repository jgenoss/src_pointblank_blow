#if !defined( __CUDPHOLEPUNCHERFORCLIENT_H )
#define __CUDPHOLEPUNCHERFORCLIENT_H

//////////////////////////////////////////////////////////////////////////
//								DEFINE									//
//////////////////////////////////////////////////////////////////////////
#define UDPHOLE_SENDCOUNT			3		// 10초 정도 해봅니다.
#define UDPHOLE_SENDTIME			5.0f	// 5초에 한번씩 보내준다.
#define UDPHOLE_SENDTIMEFORPEER		0.5f	// 초당 2회 보냅니다.
#define UDPHOLE_RECVTIME			5.0f		
#define UDPHOLE_TIMEOUT				60.f	// 30초

#define CLIENT_UDP_TIME_OUT			10.0f
#define UDP_SEND_TIME				0.1f	//10번
#define RECVOK_SEND_TIME			0.05f	//20번 
#define RECV_SEND_TIME_OUT			1.0f

#define CLIENT_RENDEZVOUS_TIME_OUT	5.0f
#define RENDEZVOUS_SEND_TIME		1.0f	//10번

#define CLIENT_RELAY_TIME_OUT		20.0f
#define RELAY_SEND_TIME				1.0f	//10번
#define RELAY_SEND_MASTER_TIME		0.5f
//////////////////////////////////////////////////////////////////////////
//								ENUM									//
//////////////////////////////////////////////////////////////////////////
enum UDPHOLE_STATE
{
	UDPHOLE_STATE_NOTHING = 0,	
	UDPHOLE_STATE_MAIN_START_WAIT,		//게임을 처음 시작할때 
	UDPHOLE_STATE_MAIN_START_RECV,		//한텀.

	UDPHOLE_STATE_MAIN_ROOM_PRESTART,	//방에서 게임 시작할때 
	UDPHOLE_STATE_MAIN_ROOM_START,		//한텀. 
	UDPHOLE_STATE_MAIN_ROOM,			//방장 & NAT 일때.
};

class CUdpHolePuncherForClient : public i3ElementBase
{
	I3_CLASS_DEFINE( CUdpHolePuncherForClient, i3ElementBase );
protected:
	UINT32				m_ServerState;					// 홀펀칭 상태 (서버하고)
	SERVER_UDP_STATE	m_ServerUDPState;				// 홀펀칭 대상 상태 (서버하고)

	REAL32				m_fJoinTimeOut;
	REAL32				m_fJoinSendTime;

	REAL32				m_UserCheckSendTime[ USER_SLOT_COUNT ];
	REAL32				m_UserCheckSendTimeOutTime[ USER_SLOT_COUNT ];

private: 
	void	_ClientHolePunch(REAL32 rDeltatime); 
//	void	_ClientHolePunchRendezvous( REAL32 rDeltaTime );
	void	_ClientHolePunchRelay( REAL32 rDeltaTime );
	bool	_ClientHolePunchEach(INT32 Num, REAL32 rDeltatime); 

public:	
	void	ClientStart(INT32 Num);					//클라이언트 홀펀칭 시작	
	void	ClientGameLeave(INT32 Num);				//클라이어트가 게임을 중단	
	void	setServerState( UINT32 nState )									{ m_ServerState		= nState; }
	void	setServerUDPState( SERVER_UDP_STATE eState )					{ m_ServerUDPState	= eState; }

public:
	CUdpHolePuncherForClient();
	virtual ~CUdpHolePuncherForClient();

	void	OnCreate( void );
	void	OnDestroy();
	void	OnUpdate( REAL32 rDeltaTime );

	// P2P 간의 홀펀칭입니다.
	void	P2PHolePunchSend( UINT32 IP, UINT16 Port );
	void	P2PHolePunchEndSend( UINT32 IP, UINT16 Port ); 
	bool	P2PHolePunchRecv( UINT32 Ip, UINT16 Port, char * pPacket );

	//void	RendezvousHolePunchSend( UINT32 IP, UINT16 Port );
	//void	RendezvousHolePunchRecv( char* pPacket );

	void	RelayHolePunchSend( UINT32 IP, UINT16 Port );
	void	RelayHolePunchRecv( char* pPacket );
	void	RelayHolePunchEndSend( UINT32 IP, UINT16 Port );
};

#endif	// __CUDPHOLEPUNCHERFORCLIENT_H
