#ifndef	__MODULE_SOCKET_CONTROL_H__
#define __MODULE_SOCKET_CONTROL_H__

class CModuleControlSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleControlSocket ); 

	BOOL				m_bConnect; 
	INT32				m_i32LogThreadIdx;

	UINT32				m_ui32LastSendTime;
	UINT32				m_ui32LastRecvTime;
	UINT32				m_ui32SystemDelay;

public : 
	CModuleControlSocket(void); 
	virtual ~CModuleControlSocket(void); 

	virtual void		PacketParsing(i3NetworkPacket  * pPacket); 

	virtual BOOL		OnCreate( UINT32 ui32IP, UINT16 ui16Port, INT32 i32LogThreadIdx );
	virtual void		OnDestroy(void); 
	virtual void		DisConnect();
		
	UINT32				GetLastRecvTime(void)							{ return m_ui32LastRecvTime;			}
	UINT32				GetLastSendTime()								{ return m_ui32LastSendTime;			}
	UINT32				GetSystemDelay()								{ return m_ui32SystemDelay;				}

	void				SetLastSendTime( UINT32 ui32SendTime )			{ m_ui32LastSendTime = ui32SendTime;	}
	
	BOOL				IsActive(void)			{ return m_bConnect; }
	BOOL				IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
};

#endif
