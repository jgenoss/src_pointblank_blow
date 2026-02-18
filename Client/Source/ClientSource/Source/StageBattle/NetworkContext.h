#if !defined( __NETWORK_CONTEXT_H)
#define __NETWORK_CONTEXT_H

enum SERVER_TYPE
{
	SERVER_TYPE_INVALID	= -1,
	SERVER_TYPE_GATEWAY	= 0,
	SERVER_TYPE_GAME,

	MAX_SERVER_TYPE,
};

enum UDP_STATE
{
	UDP_STATE_NONE	=		1,			// 
	UDP_STATE_SENDRECV,					// 패킷을 보내면서 기다림 
	UDP_STATE_RECVSEND,					// 패킷을 기다리가 받으면 보냄 
	UDP_STATE_RECV_OK,					// 
	UDP_STATE_ERROR_DIFFNAT,			// 다른NAT에 있음 (시작할때 처리해야 하는데 못한거)
	UDP_STATE_DONE,						// 완료 
};

enum UDP_TYPE
{
	UDP_TYPE_NONE	=		0,
	UDP_TYPE_REAL,
	UDP_TYPE_NAT,
};

#if defined(CHECK_CONNECTION_STATE)
enum CONNECTION_STATE
{
	CONNECTION_STATE_INVALID,
	CONNECTION_STATE_ENTERING_TO_SERVER,
	CONNECTION_STATE_ENTERED_TO_SERVER,
	CONNECTION_STATE_LEAVING_FROM_SERVER,
	CONNECTION_STATE_LEAVED_FROM_SERVER,
};
#endif





class NetworkContext : public i3::shared_ginst<NetworkContext>
{
public:
	UDP_STATE UdpHoleState[USER_SLOT_COUNT]; 
	UDP_TYPE UdpHoleType[USER_SLOT_COUNT];
	NET_UDP_INFO NetInfo[USER_SLOT_COUNT];


	UINT32 relayServerIp;			// 릴레이서버 IP
	UINT16 relayServerPort;			// 릴레이서버 Port
	UINT32 relayGroupId;			// 릴레이서버 그룹Id 
	UDP_STATE relayServerState;		
	UINT32 RelayEncryptKey;

	UINT32 RendezvousIP;
	UINT16 RendezvousPort[RENDEZVOUS_SEND_PORT_COUNT];
	UDP_STATE RendezvousState;		// 랑데뷰서버 상태




	INT64 UID;

#ifdef CHECK_CONNECTION_STATE
	CONNECTION_STATE ConnectionState;
#endif



	NetworkContext()
	{
		relayServerIp = 0;
		relayServerPort = 0;
		relayGroupId = 0;
		relayServerState = UDP_STATE_NONE;

		RendezvousIP = 0;
		RendezvousState	= UDP_STATE_NONE;
		i3mem::FillZero( RendezvousPort, sizeof(UINT16)*RENDEZVOUS_SEND_PORT_COUNT);

		UID = -1;

#ifdef CHECK_CONNECTION_STATE
		ConnectionState	= CONNECTION_STATE_INVALID;
#endif
	}

	void RelayHolePunch( UINT32 wIP, UINT16 sPort, UINT32 wGroupId, UINT32 wKey);
	void RendezvousHolePunch();

	void			InitP2P( void);

	void			GiveupBattle( INT32 idx);
	void			EndBattle( void);

	INT32			GetSessionIndex(void);
	
	UINT16			GetGamePort(void);
	void			SetGamePort( UINT16 GamePort);

	bool			GetIsRealIp(void);
	bool			isRealIp(void);
	
	const NET_UDP_INFO*		getMyNetInfo( void) const;
	NET_UDP_INFO*			setMyNetInfo( void);

	void			SetEnteredServer( bool Entered);
	bool			GetEnteredServer();

	void			ChangeSocket(void);
};

#endif