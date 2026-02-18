#ifndef __WEB_SESSION_H
#define __WEB_SESSION_H

class CWebSession : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CWebSession ); 
private:
	BOOL		m_bIsWorking;
	UINT32		m_ui32RecvTime;
	UINT32		m_ui32WorkTime;

public:
	INT32		m_i32Idx;

	CWebSession();
	virtual ~CWebSession();

	void	OnCreate(SOCKET Socket);
	virtual void	OnDestroy(void);

	virtual void PacketParsing(i3NetworkPacket * pPacket);

	// 세션이 연결되어있는지 확인
	BOOL	GetIsActive(void)
	{
		if( GetSocket() == INVALID_SOCKET ) return FALSE;
		return TRUE; 
	}

	// 세션과 연결된 시간을 확인
	void	SetRecvTime(INT32 i32Set)		{	m_ui32RecvTime = i32Set; }
	UINT32	GetRecvTime(void)				{	return m_ui32RecvTime;	}

	// 요청 받은 작업을 처리중인지 확인
	void	SetWorkTime(INT32 i32Set)		{	if( i32Set ) m_ui32WorkTime = i32Set; else m_ui32WorkTime = 0;	}
	UINT32	GetWorkTime(void)				{	return m_ui32WorkTime;	}
};

#endif