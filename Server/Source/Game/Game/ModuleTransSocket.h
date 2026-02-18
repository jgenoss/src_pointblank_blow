#ifndef	__MODULE_SOCKET_TRANS_H__
#define __MODULE_SOCKET_TRANS_H__

class CModuleTransSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleTransSocket ); 
private: 	
	BOOL			m_bConnect; 	
	char			m_strRoomNotice[NOTICE_STRING_MAX_COUNT]; 

public : 
	UINT32			m_ui32LastRecvTime;
	UINT32			m_ui32LastSendTime;

	UINT32			m_ui32ShopVersionCheckTime;
public : 
	CModuleTransSocket(void); 
	virtual ~CModuleTransSocket(void); 

	virtual void	PacketParsing(i3NetworkPacket  * pPacket); 
	virtual INT32	SendPacketMessage(i3NetworkPacket* packet);
	virtual BOOL	OnCreate( UINT32 ui32IP, UINT16 ui16PortNo); 	
	virtual void	OnDestroy(void);

	virtual void	DisConnect();
	
		
	UINT32			GetLastRecvTime(void)	{ return m_ui32LastRecvTime; }
	BOOL			IsActive(void)			{ return m_bConnect; }
	BOOL			IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
	
};

#endif