#ifndef	__MODULE_SOCKET_CAST_H__
#define __MODULE_SOCKET_CAST_H__

class CRoom;

class CModuleCastSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleCastSocket ); 
private: 	
	BOOL					m_bConnect; 
public : 	
	UINT32					m_ui32CastSendTime;

	UINT32					m_ui32LastSendTime;
	UINT32					m_ui32LastRecvTime;

	void					_ReloadPacket( i3NetworkPacket* pPacket );

private : 
	CRoom*					_GetRoom( UINT8 ui8Channel, UINT16 ui16Room );

public : 
	CModuleCastSocket(void); 
	virtual ~CModuleCastSocket(void); 

	virtual void			PacketParsing(i3NetworkPacket  * pPacket); 
	virtual BOOL			OnCreate( UINT32 ui32IP, UINT16 ui16PortNo);
	virtual void			OnDestroy(void); 
		
	virtual void			DisConnect();

	UINT32					GetLastRecvTime(void)	{ return m_ui32LastRecvTime; }
	BOOL					IsActive(void)			{ return m_bConnect; }
	BOOL					IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
};

#endif