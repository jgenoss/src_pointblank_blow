#ifndef	__MODULE_SOCKET_RENDEZVOUS_H__
#define __MODULE_SOCKET_RENDEZVOUS_H__

class CModuleRendezvousSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleRendezvousSocket ); 
private: 	
	BOOL	m_bConnect; 	
	char	m_strRoomNotice[NOTICE_STRING_MAX_COUNT]; 

protected: 

public : 	
	BOOL			m_bNewClanCreate; 
	UINT32			m_TempUserItemCharaCount; 
	UINT32			m_TempUserItemWeaponCount; 
	UINT32			m_iLastRecvTime;

	void			SendClanListReq(void); 	
public : 
	CModuleRendezvousSocket(void); 
	virtual ~CModuleRendezvousSocket(void); 
	virtual void PacketParsing(i3NetworkPacket  * pPacket); 
	virtual BOOL OnCreate(char * Address, UINT16 PortNo); 	
	virtual void OnDestroy(void); 
		
	UINT32	GetLastRecvTime(void)	{ return m_iLastRecvTime; }	

	BOOL	IsActive(void)		{ return m_bConnect; }
	BOOL	IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}

	void	SendUnknownUser_A(UINT32 iDbdix); 
};

#endif