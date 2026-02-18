/**
 * \file	Network\UDPPacketReceiver.h
 *
 * Declares the udp packet receiver class.
 */


#ifndef __UDPPACKETRECIVER_H
#define __UDPPACKETRECIVER_H

//////////////////////////////////////////////////////////////////////
//								Define								//
//////////////////////////////////////////////////////////////////////
#define	RECV_BUFFER_COUNT			1024	//2048
#define RECV_BUFFER_MISSION_COUNT	4				// 난입시 받는 미션 정보 버퍼 카운트
#define RECV_BUFFER_PING_COUNT		32				// 핑 체크용 정보 버퍼 카운트

//////////////////////////////////////////////////////////////////////
//								Class								//
//////////////////////////////////////////////////////////////////////

class UDPPacketReceiver : public i3Thread
{
	I3_CLASS_DEFINE( UDPPacketReceiver, i3Thread );
protected:
	volatile bool		m_bIsRun;
	volatile bool		m_bExitResult;
	i3Signal *			m_pConnectSig;

	//Packet Recv Time 
	REAL32	m_pRecvTime[USER_SLOT_COUNT];
	REAL32	m_DediRecvTime; 

	//Game Recv Data Buffer
	UINT32				m_WriteIdx;
	UINT32				m_ReadIdx;
	RECV_P2P_DATA		m_RecvData[ RECV_BUFFER_COUNT ]; 

	/////////////////////////////////////////////////////////////////////////////////////////////
	//Character Packet Info 
	INT32						m_iRecvPlayerInfoIdx;
	GAMEINFO_CHARA_NETDATA		m_pRecvCharaInfo[ RECV_BUFFER_COUNT ];


	//..........................................................................................
	GAMEINFO_AI_DINO *			m_pRecvAIDino; //DINOs list에 넣기 위한 buffer..
	INT32						m_iRecvAIDinosInfoIdx;
	GAMEINFO_AI_DINOs_NETDATA *	m_pRecvAIDinosInfo;

	INT32						m_iRecvDomiObjectInfoIdx;
	GAMEINFO_DOMIOBJECT_CONT *	m_pRecvDomiObjectInfo;

	INT32						m_iRecvDomiEventInfoIdx;
	GAMEINFO_DOMIEVENT_CONT *	m_pRecvDomiEventInfo;

	INT32						m_iRecvSentryGunDegreeIdx;
	GAMEINFO_SENTRYGUN_DEGREE *	m_pRecvSentryGunDegree;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//일반 유저만 사용합니다. 
	INT32								m_iRecvGrenadeInfoIdx;
	GAMEINFO_THROW_WEAPON_NETDATA		m_pRecvGrenadeInfo[RECV_BUFFER_COUNT];					// Grenade data, C5

	INT32								m_iRecvObjectInfoIdx;
	GAMEINFO_OBJECT_NETDATA				m_pRecvObjectInfo[RECV_BUFFER_COUNT];					//Object Data

	INT32								m_iRecvDropedWeaponInfoIdx;
	GAMEINFO_DROPEDWEAPON_NETDATA		m_pRecvDropedWeaponInfo[RECV_BUFFER_COUNT];				//Drop Weapon

	INT32								m_iRecvNPCInfoIdx;
	GAMEINFO_NPC *						m_pRecvNPCInfo;						//NPC

	INT32								m_iRecvMissionInfoIdx;									// 난입시에만 받습니다. 
	GAMEINFO_MISSION_INTRUDER			m_pRecvMissionIntrudeInfo[RECV_BUFFER_MISSION_COUNT];	// 난입시에만 받습니다. 

protected:
	bool	_CheckingNetwork( sockaddr_in * pInAddr, UINT8 nIndex, REAL32 fTime );		//추가됨 
	// 주소 체크
	bool	_CheckNetAddress( SOCKADDR_IN * pInAddr, INT32 Idx );
	//캐릭터 액션 체크용
	bool	_CheckActionState( UINT32 nState );

	bool	_CheckIndex( UINT8 type, INT32 idx);
	bool	_CheckCode( UINT8 type, UINT16 Code, UINT16 Rv);

	//////////////////////////////////////////////////////////////////////////
	//								패킷 파서								//
	//////////////////////////////////////////////////////////////////////////
	void	_PacketParsing( char * pPacket, INT32 nSize, sockaddr * pAddr );
	 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	INT32	_GamePacketParsing( char * pPacket, INT32 nSize, UINT8 iPing, UINT8 iRoundNum);		// 전체 패킷에 대해 처리합니다.
	void	_DediPingPacketParsing( N_GAME_HEADER *	pHeader ); 

public:
	UDPPacketReceiver();
	virtual ~UDPPacketReceiver();
	void	Create( INT32 ThreadLevel );
	void	Reset( STAGE_MODE type);
	void	Connect( STAGE_MODE type);
	void	Destroy( void);

	void OnDestroy();
	virtual UINT32 OnRunning( void * pUserData ) override;
	INT32	GetRecvCount( void ){ return m_WriteIdx - m_ReadIdx; }	// Recv buffer	
	void	PlusReadPos( void)					{ m_ReadIdx++; }
	RECV_P2P_DATA *	ReadGameData( void );
	
	/** \brief ReadPos에서부터의 idx position의 data를 반환합니다. */
	RECV_P2P_DATA * ReadGameData( INT32 idx);
	void	resetRecvTime(INT32 slotIdx)		{ I3_BOUNDCHK(slotIdx, SLOT_MAX_COUNT); m_pRecvTime[slotIdx] = 0.0f;	}
public:

	INT32				m_nLostTimeCount;

};

#endif	// __UDPPACKETRECIVER_H
