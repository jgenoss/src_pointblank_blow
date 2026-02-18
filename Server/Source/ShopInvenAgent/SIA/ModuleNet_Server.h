#ifndef __MODULE_SERVER_NET_
#define __MODULE_SERVER_NET_

class CModuleNet_Server : public i3NetworkClientSocket2
{
	I3_CLASS_DEFINE( CModuleNet_Server );

private		:
	SOCKET					m_Socket; 
	UINT32					m_ui32ServerIP; 
	UINT32					m_ui32WorkingIdx; 

	INT32					m_i32LogFileHour;
	UINT32					m_ui32SendCount;
	UINT32					m_ui32RecvCount;

	i3RingBuffer*			m_pRecvBuffer;
	i3RingBuffer*			m_pSendBuffer;
public		: 
	CModuleNet_Server(); 
	virtual ~CModuleNet_Server(); 

	UINT32					GetIP(void)					{ return m_ui32ServerIP; }	
	UINT32					GetWorkingIdx(void)			{ return m_ui32WorkingIdx; }

	void					SetServer(SOCKET socket, INT32 i32WorkingIdx)
	{
		m_ui32WorkingIdx = i32WorkingIdx; 
		SetSocket(socket); 
		return;
	}

	BOOL					Create( UINT32 iServerIp, i3RingBuffer *pRecvBuffer, i3RingBuffer* pSendBuffer );	
	void					Destroy(void); 

	BOOL					Send(void);

	virtual INT32			PacketParsing(char * pPacket, INT32 i32PacketSize);

	void					Dump(void); 
};

#endif

