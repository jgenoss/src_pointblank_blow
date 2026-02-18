// Error 발생시에 E-Mail을 보내기 위한 클래스(다른용도로 사용도 가능함) - SMTP사용
#if defined( I3_WINDOWS)
#include <winsock2.h>
//#include <vector>
//#include <string>

//using namespace	std;			// 헤더에는 웬간하면 using namespace 쓰지 말것....인클루드가 잘못되면 전체솔루션에 퍼진다..(2012.11.14.수빈)

const INT32		DEFAULT_PROTOCOL	=	0;
const INT32		NO_FLAGS			=	0;
const INT32		SMTP_BUF_SIZE		=	4096;
const INT32		SMTP_DATE_SIZE		=	500;

#define BASE64_MAXLINE				76
#define EOL							"\r\n"

class TRecipient
{
	public:
		TRecipient() {}
		TRecipient(const TRecipient& rhs) : m_csEmail(rhs.m_csEmail) {}
		TRecipient& operator=(const TRecipient& rhs) { m_csEmail = rhs.m_csEmail; return *this; }
		char*				GetEmail() const { return (char*)m_csEmail.c_str(); };
		void				SetEmail( const i3::string& csEmail ){ if( csEmail.length() ) m_csEmail = csEmail; };
	private:
		i3::string			m_csEmail;		
};

// 20070504	김현우
class TAttach
{
	public:
		i3::rc_string		m_Filename;
		char*				m_pszEncoded;
};
// 20070504	김현우

class i3SMTPMail  
{
	public:		
		I3_EXPORT_BASE					i3SMTPMail();	
		I3_EXPORT_BASE virtual			~i3SMTPMail();

		I3_EXPORT_BASE bool				AddRecipient( const char email[], const char name[]="" );		
		I3_EXPORT_BASE bool				ConnectServer( const char* server, const unsigned short port = 0);
		I3_EXPORT_BASE bool				Disconnect();
		I3_EXPORT_BASE bool				GetConnectStatus();
		
		I3_EXPORT_BASE bool				Send();
		I3_EXPORT_BASE void				SetMessageBody( const char* body );	
		I3_EXPORT_BASE void				SetSubject( const char* subject );	
		I3_EXPORT_BASE void				SetSenderName( const char* name );	
		I3_EXPORT_BASE void				SetSenderEmail( const char* email );	
		I3_EXPORT_BASE void				SetReplyTo( const char* replyto );	
		I3_EXPORT_BASE void				SetXMailer( const char* xmailer );

		// 20070504	김현우
		I3_EXPORT_BASE bool				SetFileAttach( char* sFilename );
		I3_EXPORT_BASE INT32			Base64BufferSize( INT32 nInputSize );
		I3_EXPORT_BASE bool				EncodeBase64( const char* aIn, INT32 aInLen, char* aOut );
		// 20070504	김현우

	private:	
		bool				bCon = false;
		char*				m_pcFromEmail = nullptr;
		char*				m_pcFromName = nullptr;
		char*				m_pcSubject = nullptr;
		char*				m_pcMsgBody = nullptr;
		char*				m_pcXMailer = nullptr;
		char*				m_pcReplyTo = nullptr;
		char*				m_pcIPAddr = nullptr;

		WSADATA				wsaData;
		SOCKET				hSocket = INVALID_SOCKET;

		i3::vector<TRecipient>	Recipients;
		
		char*				FormatHeader();	
		bool				FormatMessage();
		SOCKET				ConnectServerSocket( const char* server, const unsigned short port = 0);
		
		bool				SendSocketData( char* pcMsg );


		// 20070504	김현우
		i3::vector<TAttach>		Attach;
		// 20070504	김현우
};

#endif