#ifndef	__MODULE_SOCKET_CONTROL_H__
#define __MODULE_SOCKET_CONTROL_H__

class CModuleControlSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleControlSocket );
private:
	BOOL			m_bConnect;
	INT32			m_i32LogIdx;
public : 
	UINT32			m_ui32ControlSendTime;

	UINT32			m_ui32LastSendTime;
	UINT32			m_ui32LastRecvTime;
public :
	CModuleControlSocket(void); 
	virtual ~CModuleControlSocket(void);

	virtual void	PacketParsing(i3NetworkPacket  * pPacket);
	virtual BOOL	OnCreate( UINT32 ui32ControlIP, UINT16 ui16PortNo, INT32 i32LogIdx );
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