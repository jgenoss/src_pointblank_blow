#ifndef	__MODULE_SOCKET_CAST_H__
#define __MODULE_SOCKET_CAST_H__

class CModuleCastSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleCastSocket ); 
private:
	BOOL					m_bConnect;
public :
	UINT32					m_ui32CastSendTime;
	UINT32					m_ui32LastRecvTime;
	UINT32					m_ui32LastSendTime;

	INT32					m_i32PortIdx;
public : 
	CModuleCastSocket(void); 
	virtual ~CModuleCastSocket(void); 
	virtual void			PacketParsing(i3NetworkPacket  * pPacket);
	virtual BOOL			OnCreate( UINT32 ui32IP, UINT16 ui16Port );
	virtual void			OnDestroy(void); 
		
	UINT32					GetLastRecvTime(void)	{ return m_ui32LastRecvTime; }
	BOOL					IsActive(void)			{ return m_bConnect; }
	BOOL					IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
private:
	UINT16					GetServerPort_C( INT32* pi32OutPortIdx );
};

#endif
