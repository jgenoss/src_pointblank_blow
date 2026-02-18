#ifndef	__MODULE_SOCKET_KNOCK_H__
#define __MODULE_SOCKET_KNCOK_H__

class CModuleKnockSoket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleKnockSoket );
private:
	BOOL			m_bConnect;
	INT32			m_i32LogIdx;
public :
	UINT32			m_ui32KnockSendTime;
	UINT32			m_ui32LastRecvTime;
public : 
	CModuleKnockSoket(void);
	virtual ~CModuleKnockSoket(void);
	virtual void	PacketParsing(i3NetworkPacket  * pPacket);
	virtual BOOL	OnCreate(char * pAddress, UINT16 ui16PortNo, INT32 i32LogIdx );
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