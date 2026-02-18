#ifndef	__MODULE_SOCKET_CONTROL_H__
#define __MODULE_SOCKET_CONTROL_H__

class CModuleControlSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleControlSocket ); 
private: 	
	BOOL	m_bConnect; 
public : 		
	UINT32			m_iControlSendTime; 
	UINT32			m_iLastRecvTime;	
	UINT32			m_iHeartBitCheckTime;

	UINT32			m_ui32PortIdx;
public : 
	CModuleControlSocket(void); 
	virtual ~CModuleControlSocket(void); 
	virtual void PacketParsing(i3NetworkPacket  * pPacket); 
	virtual BOOL OnCreate(char * Address, UINT16 PortNo); 	
	virtual void OnDestroy(void); 
	virtual void			DisConnect();
		
	BOOL	IsActive(void)			{ return m_bConnect; }
	BOOL	IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
};

#endif
