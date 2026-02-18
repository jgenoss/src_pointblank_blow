#ifndef __SMTP_MAIL_H__
#define __SMTP_MAIL_H__

#define SMTP_PORT						25		// Smtp 서버 포트.
#define	SMTP_SUCCESS_RETURN_CODE_SIZE	4		// SMTP 서버 리턴코드 사이즈

#define DEF_ACK_WAIT_TIME				3		// Smtp ACK 패킷 대기 시간(s) 기본값.
#define MAX_ACK_WAIT_TIME				10		// Smtp ACK 패킷 대기 시간(s) 최대값.

#define DEF_EMAIL_SEND_DELAY_TIME		10		// 메일 전송 대기 시간(s) 기본값.
#define MAX_EMAIL_SEND_DELAY_TIME		30		// 메일 전송 대기 시간(s) 최대값.

#define MAX_RECV_EMAIL_ADDRESS			10		// email 받는 주소최대갯수.
#define MAX_LENGTH_EMAIL_ADDRESS		64		// email 주소 길이.

#define	MAX_EMAIL_SUBJECT				256	// 메일 제목 최대 크기.
#define MAX_EMAIL_BODY					512	// 메일 본문 최대 크기.

#define MAX_EMAIL_SIZE					1024	// 메일 제목 + 본문 + a

#define CREATE_SMTPEMAIL_SUCCESS						1	// Create 성공.
#define CREATE_SMTPEMAIL_FAIL_BASE_INFO					-1	// Create 실패 - 이메일 전송 필수 정보 없음.
#define CREATE_SMTPEMAIL_FAIL_SOCKET_CREATE				-2	// Create 실패 - 소켓 생성 실패
#define CREATE_SMTPEMAIL_FAIL_SOCKET_CONNECT			-3	// Create 실패 - 소켓 Connect 실패
#define CREATE_SMTPEMAIL_FAIL_SOCKET_SET_OPTION			-4	// Create 실패 - 소켓 옵션 지정 실패
#define CREATE_SMTPEMAIL_FAIL_SEND_BUFFER_CREATE		-5	// Create 실패 - Send Buffer 생성 실패
#define CREATE_SMTPEMAIL_FAIL_THREAD_CREATE				-6	// Create 실패 - Thread 생성 실패

// SMTP 서버와의 메일 전송 단계별 성공 리턴코드
#define	SMTP_SUCCESS_RETURN_CODE_HELLO_BEGIN	220		// 가장 처음 HELO 일떄	
#define	SMTP_SUCCESS_RETURN_CODE_HELLO_AFTER	250		// 두번째 HELO 부터
#define	SMTP_SUCCESS_RETURN_CODE_FROMTO			250
#define	SMTP_SUCCESS_RETURN_CODE_DATA			354
#define	SMTP_SUCCESS_RETURN_CODE_EMAIL			250
#define	SMTP_SUCCESS_RETURN_CODE_QUIT			221


// SMTP 서버와의 메일 전송 단계
enum STEP_SEND_EMAIL
{
	STEP_SEND_EMAIL_NONE		= 0,	// 연결 안된 상태.
	STEP_SEND_EMAIL_HELLO		= 1,	// 연결 시작.
	STEP_SEND_EMAIL_FROMTO		= 2,	// 메일 송/수신인 주소 입력.
	STEP_SEND_EMAIL_DATA		= 3,	// 메일 작성 시작.
	STEP_SEND_EMAIL_EMAIL		= 4,	// 메일 제목,내용 입력.

	STEP_SEND_EMAIL_MAX,
};

#pragma pack( push, 1 )

struct EMAIL_SEND_BUFFER
{
	char			m_strSubject[MAX_EMAIL_SUBJECT];	// 메일 제목.
	char			m_strBody[MAX_EMAIL_BODY];			// 메일 본문.

	void Reset()
	{
		m_strSubject[ 0 ]	= '\0';
		m_strBody[ 0 ]		= '\0';
	}
};


struct RECV_EMAIL_ADDRESS
{
	char				m_strEmailAddress[MAX_LENGTH_EMAIL_ADDRESS];		// 메일 받을 주소.
};


struct BASE_EMAIL_INFO	// 이메일 보내기 위한 필수 정보.
{
	UINT32				m_ui32SmtpServerAddress;							// smtp 서버 주소.
	char				m_strSenderEmailAddress[MAX_LENGTH_EMAIL_ADDRESS];	// 보내는 사람 email주소.
	UINT32				m_ui32RecvEmailCount;								// 전송할 email 계정 갯수. 최대 MAX_EMAIL_ACCOUNT
	UINT32				m_ui32AckWaitTime;									// Smtp 서버 응답 대기 시간.(s)
	UINT32				m_ui32EmailSendDelayTime;							// 메일 전송 대기 시간.(s)
	RECV_EMAIL_ADDRESS	m_aRecvEmailAddress[MAX_RECV_EMAIL_ADDRESS];		// 수신자 email 주소.	 
};

#pragma pack( pop )



class CSmtpMail : public i3Thread
{
	I3_CLASS_DEFINE( CSmtpMail );

protected:
	BASE_EMAIL_INFO		m_BaseEmailInfo;			// 메일 보내기 위한 기본 정보.


// Param
/////////////////////////////////////////////////////////////////////////////////
////////////////////	SOCKET		/////////////////////////////////////////////

	SOCKET				m_socket;
	WSAEVENT			m_recvEvent;
	INT32				m_i32receivedPacketSize;
	char				m_receiveBuffer[PACKETBUFFERSIZE];

	BOOL				m_bIsSendingEmail;			// Email 보내고 있는 상태인지.
	STEP_SEND_EMAIL		m_eStepSendEmail;			// 메일 전송 단계.	
	
	UINT32				m_ui32LastSendTime;			// SMTP 서버에 패킷 전송한 시간.
	UINT32				m_ui32LastMailFinishTime;	// 마지막 메일 종료( 성공이건 실패건) 시간.

	EMAIL_SEND_BUFFER	m_CurrenEmailSend;					// 현재 보내고 있는 메일 정보.
	UINT32				m_ui32CurRecvEmailAddressCount;		// 진행중인 이메일 받을 주소 갯수. 체크용.


/////////////////////////////////////////////////////////////////////////////////
////////////////////	THREAD		/////////////////////////////////////////////
	BOOL				m_bRunning;
	UINT32				m_ui32SmtpConnectTime;
	INT32				m_i32SmtpMailIdx;

// Func
public: 
	CSmtpMail(void);
	virtual ~CSmtpMail(void);

	INT32	OnCreate( BASE_EMAIL_INFO *pBaseEmailInfo  );
	void	OnDestroy(void);



/////////////////////////////////////////////////////////////////////////////////
////////////////////	SOCKET		/////////////////////////////////////////////
	INT32	CreateSocket(UINT32 ui32SmtpServerAddress); 
	void	DestroySocket(void);

	void	OnReceive(void);

	bool	SelectEvent(void);

	INT32	PacketParsing(char * pPacket, INT32 i32Packetsize);
	INT32	SendPacketMessage(char * pPacket, INT32 i32size);

	BOOL	IsConnect(void)	
	{
		if( m_socket == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}


/////////////////////////////////////////////////////////////////////////////////
////////////////////	THREAD		/////////////////////////////////////////////
	INT32	SmtpServerConnect();
	BOOL	SendEmailStart(void);	// 이메일 전송 시작

	void	FailSendPacket( STEP_SEND_EMAIL eStep, char * pSendPacket );
	void	RecvErrorPacket( STEP_SEND_EMAIL eStep ,char * pErrorCode );

	void	Reset(void);
	
	virtual UINT32			OnRunning( void * pUserData );			// CSmtpMail 클래스에서 구현될 루프.
	virtual bool			OnRunningEx();		// CSmtpMail의 자식 클래스에서 구현될 루프.

	INT32	SmtpServerConnect(UINT32 ui32SmtpServerAddress);
};

#endif
