// Error 발생시에 E-Mail을 보내기 위한 클래스(다른용도로 사용도 가능함) - SMTP사용
#if defined( I3_WINDOWS)
#include <winsock2.h>
#include <vector>
#include <string>

using namespace	std;

const INT32		DEFAULT_PROTOCOL	=	0;
const INT32		NO_FLAGS			=	0;
const INT32		SMTP_BUF_SIZE		=	4096;
const INT32		SMTP_DATE_SIZE		=	500;

#define BASE64_MAXLINE				76
#define EOL							"\r\n"

class TRecipient
{
	public:
		char*				GetEmail(){ return (char*)m_csEmail.c_str(); };
		void				SetEmail( const std::string csEmail ){ if( csEmail.length() ) m_csEmail = csEmail; };
	private:
		std::string			m_csEmail;		
};

// 20070504	김현우
class TAttach
{
	public:
		char				m_szFilename[260];
		char*				m_pszEncoded;
};
// 20070504	김현우

class i3SMTPMail  
{
	public:		
							i3SMTPMail();	
		virtual				~i3SMTPMail();

		BOOL				AddRecipient( const char email[], const char name[]="" );		
		BOOL				ConnectServer( const char* server, const unsigned short port = NULL );
		BOOL				Disconnect();
		BOOL				GetConnectStatus();
		
		BOOL				Send();
		void				SetMessageBody( const char* body );	
		void				SetSubject( const char* subject );	
		void				SetSenderName( const char* name );	
		void				SetSenderEmail( const char* email );	
		void				SetReplyTo( const char* replyto );	
		void				SetXMailer( const char* xmailer );

		// 20070504	김현우
		BOOL				SetFileAttach( char* sFilename );
		INT32				Base64BufferSize( INT32 nInputSize );
		BOOL				EncodeBase64( const char* aIn, INT32 aInLen, char* aOut );
		// 20070504	김현우

	private:	
		bool				bCon;
		char				m_cHostName[MAX_PATH];
		char*				m_pcFromEmail;
		char*				m_pcFromName;
		char*				m_pcSubject;
		char*				m_pcMsgBody;
		char*				m_pcXMailer;
		char*				m_pcReplyTo;
		char*				m_pcIPAddr;	

		WSADATA				wsaData;
		SOCKET				hSocket;

		vector<TRecipient>	Recipients;
		
		char*				FormatHeader();	
		bool				FormatMessage();
		SOCKET				ConnectServerSocket( const char* server, const unsigned short port = NULL );
		
		BOOL				SendSocketData( char* pcMsg );


		// 20070504	김현우
		vector<TAttach>		Attach;
		// 20070504	김현우
};

#endif