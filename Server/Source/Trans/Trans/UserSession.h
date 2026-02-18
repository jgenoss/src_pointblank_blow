#ifndef __USERSESSION_H__
#define __USERSESSION_H__

struct SV_STATE_LOG
{
	INT32					m_i32RecvCount;
	INT32					m_i32RecvSize;
	INT32					m_i32CompletedCount;
	INT32					m_i32CompletedSize;
};

struct IOCP_RING_PUSH
{
	INT32					m_i32ServerIdx;
	char					m_pBuffer[ PACKETBUFFERSIZE ];
};

class CUserSession : public i3NetworkSession
{
	I3_CLASS_DEFINE( CUserSession ); 
private:

	SV_STATE_LOG*			m_pStateLog;

	i3RingBuffer**			m_ppRing;
	INT32*					m_pLogIdx;
	INT32					m_i32ServerIdx;

public:
	CUserSession();
	virtual ~CUserSession();

	virtual BOOL			Create( i3RingBuffer** ppRing, INT32* pLogIdx, SV_STATE_LOG* pStateLog );
	virtual void			Destroy();

	BOOL					OnConnect( SOCKET hSocket, struct sockaddr_in * pAddr, INT32 i32ServerIdx );
	virtual BOOL			OnConnectInit();
	virtual BOOL			OnDisconnect( BOOL bForceMainThread = FALSE );

	virtual INT32			PacketParsing(char * pPacketBuffer, INT32 i32Size);

	virtual	BOOL			SendPacketMessage( i3NetworkPacket * pPacket );

	INT32					GetServerIdx()					{	return m_i32ServerIdx;	};
};

#endif
