#ifndef __MODULE_SOCKET_ZLOG_H__
#define __MODULE_SOCKET_ZLOG_H__

class CModuleZLogSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleZLogSocket );
private:
	BOOL					m_bConnect;
public :
	CModuleZLogSocket(void);
	virtual ~CModuleZLogSocket(void); 
	virtual void			PacketParsing(i3NetworkPacket  * pPacket);
	virtual BOOL			OnCreate(UINT32 ui32IP, UINT16 ui16Port);		
	virtual void			OnDestroy(void);

	virtual void			DisConnect();

	virtual INT32			SendPacketMessage(i3NetworkPacket* packet);
		
	BOOL					IsActive(void) { return m_bConnect; }
	BOOL					IsConnect(void)
	{
		if (GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE;
	}

}; 
#endif

