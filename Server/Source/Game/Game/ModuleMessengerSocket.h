#ifndef	__MODULE_SOCKET_MESSENGER_H__
#define __MODULE_SOCKET_MESSENGER_H__

class CModuleMessengerSocket : public i3NetworkClientSocket	  
{
	I3_CLASS_DEFINE( CModuleMessengerSocket ); 
private: 	
	BOOL	m_bConnect; 	
	char	m_strRoomNotice[NOTICE_STRING_MAX_COUNT]; 
protected: 

public : 
	UINT32			m_ui32LastSendTime; 
	UINT32			m_ui32LastRecvTime;
public : 
	CModuleMessengerSocket(void); 
	virtual ~CModuleMessengerSocket(void); 
	virtual void PacketParsing(i3NetworkPacket  * pPacket); 
	virtual BOOL OnCreate( UINT32 ui32IP, UINT16 ui16PortNo);
	virtual void OnDestroy(void); 

	virtual void			DisConnect();
	
	UINT32	GetLastRecvTime(void)	{ return m_ui32LastRecvTime; }

	BOOL	IsActive(void)		{ return m_bConnect; }
	BOOL	IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
};

#endif