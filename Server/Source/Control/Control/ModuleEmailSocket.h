#ifndef	__MODULE_SOCKET_EMAIL_H__
#define __MODULE_SOCKET_Email_H__

class CModuleEmailSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleEmailSocket ); 
private: 	
	SOCKET				m_socket;
	WSAEVENT			m_recvEvent;
	int					m_receivedPacketSize;
	char				m_receiveBuffer[PACKETBUFFERSIZE];

	BOOL				m_bIsSendingEmail;		// Email 보내고 있는 상태인지.
	BOOL				m_bI;		// SMTP서버로 부터 ACK 수신 대기 상태인지.
	UINT32				m_ui32CurSendEmailCount;// 현재 이메일 전송 횟수. 다보냈으면 0으로 초기화.
	STEP_SEND_EMAIL		m_eStepSendEmail;		// 메일 전송 단계.
	UINT32				m_ui32EmailReceiverCount;	// 메일 받는 주소 추가 갯수.
	
	
	UINT32				m_ui32LastSendTime;	// SMTP 서버에 패킷 전송한 시간.
	UINT32				m_ui32LastRecvTime;	// SMTP 서버로 부터 패킷 받은 시간.

public : 
	CModuleEmailSocket(void); 
	virtual ~CModuleEmailSocket(void); 

	BOOL	OnCreate( UINT32 ui32EmailIP, UINT16 PortNo); 	
	void	OnReceive(void);
	void	OnDestroy(void);

	INT32	PacketParsing(char * pPacket, INT32 Packetsize);
	INT32	SendPacketMessage(char * pPacket, INT32 size);
	
	bool			SelectEvent(void);	

	BOOL	IsConnect(void)	
	{
		if( m_socket == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}

	BOOL			SendEmailStart(void);	// 이메일 전송 시작
	void			SetStepSendMail( STEP_SEND_EMAIL eStepSendEmail ) { m_eStepSendEmail	= eStepSendEmail; }	// 메일 전송 단계 설정.
	STEP_SEND_EMAIL	GetStepSendMail( void	 ) { return m_eStepSendEmail; }	// 메일 전송 단계 가져오기.
	BOOL			GetIsSendingEmail( void ) { return m_bIsSendingEmail; } // Email 보내는 상태인지 체크.

	UINT32	GetEmailReceiverCount( void ) { return m_ui32EmailReceiverCount; } 

	UINT32	GetLastSendTime( void ) { return m_ui32LastSendTime; } 
	UINT32	GetLastRecvTime( void ) { return m_ui32LastRecvTime; } 

	void	FailSendPacket( STEP_SEND_EMAIL eStep, char * pSendPacket );
	void	RecvErrorPacket( STEP_SEND_EMAIL eStep ,char * pErrorCode );

	void	Reset(void);
};

#endif