#include "i3CommonType.h"

#include "i3SMTPMail.h"
#include "i3String.h"

#if defined( I3_WINDOWS )
// Error 발생시에 E-Mail을 보내기 위한 클래스(다른용도로 사용도 가능함) - SMTP사용
i3SMTPMail::i3SMTPMail()
{
	bCon			= false;

	m_pcFromEmail	= NULL;
	m_pcFromName	= NULL;
	m_pcSubject		= NULL;
	m_pcMsgBody		= NULL;
	m_pcXMailer		= NULL;
	m_pcReplyTo		= NULL;
	m_pcIPAddr		= NULL;

    WORD wVer		= MAKEWORD(2,2);	
	
	WSAStartup( wVer, &wsaData );

	hSocket = INVALID_SOCKET;
}

i3SMTPMail::~i3SMTPMail()
{
	delete[]	m_pcFromEmail;
	delete[]	m_pcFromName;
	delete[]	m_pcSubject;
	delete[]	m_pcMsgBody;
	delete[]	m_pcXMailer;
	delete[]	m_pcReplyTo;

	if( bCon )	Disconnect();	
}

BOOL i3SMTPMail::SendSocketData( char* pcMsg )
{
	if( send( hSocket, (char*)pcMsg, strlen( pcMsg ), NO_FLAGS ) == SOCKET_ERROR )	return	FALSE;
	
	return	TRUE;
}

BOOL i3SMTPMail::Disconnect()
{
	if ( !bCon ) return FALSE;

	BYTE		sReceiveBuffer[SMTP_BUF_SIZE];
	int			iLength		= 0;

	if( !SendSocketData( "QUIT\r\n" ) )	return	FALSE;	
	
    iLength		=	recv( hSocket, 
						  (char*)sReceiveBuffer,
						  sizeof( sReceiveBuffer ), 
						  NO_FLAGS );

	if( iLength == SOCKET_ERROR)
		return FALSE;

    sReceiveBuffer[iLength] = '\0';

	bCon	=	false;	
	hSocket	=	INVALID_SOCKET;

	return	TRUE;
}

BOOL i3SMTPMail::ConnectServer( const char* server, const unsigned short port )
{
	if( bCon )		Disconnect();
	bCon	=	FALSE;
	hSocket =	INVALID_SOCKET;

	hSocket =	ConnectServerSocket( server, port );
	if( hSocket != INVALID_SOCKET ) 
	{		
		BYTE		sReceiveBuffer[SMTP_BUF_SIZE];
		int			iLength		= 0;		
		char		buf[SMTP_BUF_SIZE];      
		
		strcpy( buf, "HELO " );
        strcat( buf, server );
        strcat( buf, "\r\n" );

        if( !SendSocketData( (char*)buf ) )	return	FALSE;		

        iLength		=	recv( hSocket, 
							  (char*)sReceiveBuffer, 
							  sizeof( sReceiveBuffer ), 
							  NO_FLAGS );

		if( iLength == SOCKET_ERROR)
			return FALSE;

        sReceiveBuffer[iLength] = '\0';
	} 
	else	return	FALSE;	

	bCon	=	TRUE;
	return	TRUE;
}

SOCKET i3SMTPMail::ConnectServerSocket( const char* server, const unsigned short port )
{
	int				nConnect;
	short			nProtocolPort;
	LPHOSTENT		lpHostEnt;
    LPSERVENT		lpServEnt;
	SOCKADDR_IN		sockAddr;        

	SOCKET			hServerSocket = INVALID_SOCKET;
     
    lpHostEnt	=	gethostbyname( server );

    if ( lpHostEnt ) 
	{		
        hServerSocket	=	socket( PF_INET, SOCK_STREAM, DEFAULT_PROTOCOL );

        if ( hServerSocket != INVALID_SOCKET ) 
		{
			if ( port != NULL )	nProtocolPort = port;
			else
			{
				lpServEnt	=	getservbyname( "mail", DEFAULT_PROTOCOL );                 
				if ( lpServEnt == NULL )	nProtocolPort = htons( IPPORT_SMTP ); 
				else						nProtocolPort = lpServEnt->s_port;
			}

			i3mem::FillZero( &sockAddr, sizeof( sockAddr));

            sockAddr.sin_family		= AF_INET;
            sockAddr.sin_port		= nProtocolPort;
            sockAddr.sin_addr		= *((LPIN_ADDR)*lpHostEnt->h_addr_list);

            nConnect	=	connect( hServerSocket, 
									 (PSOCKADDR)&sockAddr, 
									 sizeof(sockAddr) );
            if( nConnect )	
			{
				closesocket(hServerSocket);
				hServerSocket = INVALID_SOCKET;
			}

        }
    }

    return	hServerSocket;
}

BOOL i3SMTPMail::Send()
{
	if( m_pcFromEmail == NULL ) return	FALSE;

	BYTE		sReceiveBuffer[SMTP_BUF_SIZE];
	int			iLength		= 0;
	int			iEnd		= 0;
	char		buf[SMTP_BUF_SIZE];
	BOOL	bResult = FALSE;

	//  보내는 사람 지정
	strcpy( buf, "MAIL FROM: <" );
    strcat( buf, m_pcFromEmail );
    strcat( buf, ">\r\n" );

    if( send( hSocket, (LPSTR)buf, strlen(buf), NO_FLAGS ) == SOCKET_ERROR ) 
	{		
		return	FALSE;
	}

    iLength = recv( hSocket, 
					(LPSTR)sReceiveBuffer + iEnd,
					sizeof( sReceiveBuffer ) - iEnd, 
					NO_FLAGS );
    iEnd	+= iLength;

    sReceiveBuffer[ iEnd ] = '\0';
	//  보내는 사람 지정


	// 받는 사람 지정
	char*	token;

	for( UINT32 i = 0; i < Recipients.size(); i++ ) 
	{
		token	= strtok( Recipients.at(i).GetEmail(), "<" );
		token	= strtok( NULL, "<" );	

		if( token == NULL )	token	= strtok( Recipients.at(i).GetEmail(), "<" );
		strcpy( buf, "RCPT TO: <" );
		strcat( buf, token );
		strcat( buf, "\r\n" );

		if( !SendSocketData( (char*)buf ) )	return	FALSE;		

		iLength		= recv( hSocket, 
							(LPSTR)sReceiveBuffer + iEnd,
							sizeof( sReceiveBuffer ) - iEnd, 
							NO_FLAGS );
		iEnd		+= iLength;

		sReceiveBuffer[iEnd] = '\0';
	}
	// 받는 사람 지정


	// 본문입력 시작
	// DATA 명령을 전송하여 본문을 입력할것이라고 전달
	if( !SendSocketData( (char*)"DATA\r\n" ) ) return	FALSE;   

	iLength		= recv( hSocket, 
						(LPSTR)sReceiveBuffer + iEnd,
						sizeof( sReceiveBuffer ) - iEnd, 
						NO_FLAGS );
    iEnd		+= iLength;

    sReceiveBuffer[iEnd] = '\0';
	// DATA 명령을 전송하여 본문을 입력할것이라고 전달


	// 본문 헤더
	char*		msgHeader	= FormatHeader();
	if( msgHeader == NULL ) 
	{
		goto ExitPart;
	}

	if( !SendSocketData( (char*)msgHeader ) )
	{
		goto ExitPart;
	}
	// 본문 헤더
	
	// 본문 글내용
	if( Attach.size() > 0 ) {
		char* psBodyHeader = "\r\n--#BOUNDARY#\r\nContent-Type: text/plain; charset=euc-kr\r\n\r\n";
		if (!SendSocketData( (char*)psBodyHeader))
		{
			goto ExitPart;
		}
	}

	if( !SendSocketData( (char*)m_pcMsgBody ) )
	{
		goto ExitPart;
	}
	// 본문 글내용

	// 첨부파일
	for( UINT32 i = 0 ; i < Attach.size() ; i++ ) {
		char pszContent[260];
		sprintf(pszContent, "\r\n\r\n--#BOUNDARY#\r\nContent-Type: application/octet-stream; name=%s\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=%s\r\n\r\n", 
			Attach.at(i).m_szFilename, Attach.at(i).m_szFilename);

		if( !SendSocketData( (char*)pszContent ) )					goto ExitPart;
		if( !SendSocketData( (char*)Attach.at(i).m_pszEncoded ) )	goto ExitPart;
	}

	for( UINT32 i = 0 ; i < Attach.size() ; i++ ) {
		if( !SendSocketData( (char*)"\r\n--#BOUNDARY#--" ) )		goto ExitPart;
	}
	// 첨부파일

	// 본문 내용 종료
	if( !SendSocketData( (char*)"\r\n.\r\n" ) )						goto ExitPart;
	// 본문 내용 종료

	iLength		= recv( hSocket, 
						(LPSTR)sReceiveBuffer + iEnd,
						sizeof( sReceiveBuffer ) - iEnd, 
						NO_FLAGS );
    iEnd		+= iLength;

    sReceiveBuffer[iEnd] = '\0';

	bResult = TRUE;

ExitPart:
	if( msgHeader != NULL)
	{
		delete[]	msgHeader;
	}

	return bResult;
}

char* i3SMTPMail::FormatHeader()
{	
	if( Recipients.size() <= 0 )	return	NULL;
	
	int		s			=	0;
	char*	msgHeader	=	new char[SMTP_BUF_SIZE];	
	UINT32 i;
	
	for( i = s = 0; i < Recipients.size(); i++ ) 
	{		
		s	+=	strlen( Recipients.at(i).GetEmail() ) + 1;
	} 
	if( s == 0 )	s = 1; 
	char*	to	=	new char[s];

	TCHAR szDate[SMTP_DATE_SIZE];
	TCHAR sztTime[SMTP_DATE_SIZE];

	to[0] = '\0';		
	for( i = 0; i < Recipients.size(); i++ ) 
	{		
		i > 0 ? strcat( to,"," ) : strcat( to, "" );
		strcat( to, Recipients.at(i).GetEmail() );
	}

	SYSTEMTIME	st	= { 0 };
	::GetSystemTime( &st );
	::GetDateFormat( LOCALE_SYSTEM_DEFAULT, 
					 0, 
					 &st, 
					 "ddd',' dd MMM yyyy", 
					 szDate, 
					 sizeof( szDate ) );
	::GetTimeFormat( LOCALE_SYSTEM_DEFAULT,
					 TIME_FORCE24HOURFORMAT,
					 &st,
					 "HH':'mm':'ss tt",
					 sztTime,
					 sizeof( sztTime ) );

	wsprintf( msgHeader, "DATE: %s %s\r\n", szDate, sztTime );	

	if( m_pcFromName != NULL ) 
	{
		strcat( msgHeader, "FROM: " );
		char*	line	=	new char[ strlen( m_pcFromName ) + strlen( m_pcFromEmail ) + 5 ];
		sprintf( line, "\"%s\"<%s>", m_pcFromName, m_pcFromEmail );
		strcat( msgHeader, line );
		strcat( msgHeader, "\r\n" );
		delete[] line;
	}

	strcat( msgHeader, "To: " );
	strcat( msgHeader, to );
	strcat( msgHeader, "\r\n" );
	strcat( msgHeader, "Cc: " );
	strcat( msgHeader, "\r\n" );

	if( m_pcSubject != NULL ) 
	{
		strcat( msgHeader, "Subject: " );
		strcat( msgHeader, m_pcSubject );
		strcat( msgHeader, "\r\n" );
	}
	if( m_pcXMailer != NULL ) 
	{
		strcat( msgHeader,"X-Mailer: " );
		strcat( msgHeader, m_pcXMailer );
		strcat( msgHeader, "\r\n" );
	}

	if( m_pcReplyTo != NULL ) 
	{
		strcat( msgHeader, "Reply-To: " );
		strcat( msgHeader, m_pcReplyTo );
		strcat( msgHeader, "\r\n" );
	}

	if( Attach.size() > 0 ) {		// 파일이 있을때
		strcat( msgHeader, "MIME-Version: 1.0\r\nContent-type: multipart/mixed; boundary=\"#BOUNDARY#\"\r\n" );
		strcat( msgHeader, "\r\n" );
	}
	else {							// 파일이 없을때..
		strcat( msgHeader, "MIME-Version: 1.0\r\nContent-type: text/plain; charset=euc-kr\r\n" );
	}
	
	delete[] to;

	return	msgHeader;	
}

void i3SMTPMail::SetSenderName( const char* name )
{
	unsigned int uio = m_pcFromName ? strlen( m_pcFromName ) : 0;
	unsigned int uin = name ? strlen( name ) : 0;

	if ( uio < uin )
	{
		if( m_pcFromName != NULL)
			free(	m_pcFromName);

		m_pcFromName = name ? strdup( name ) : 0;
	}
	else
	{
		if( m_pcFromName != NULL)
		{
			if( name )
				strcpy( m_pcFromName, name );
			else 
			{
				free(	m_pcFromName); 
				m_pcFromName = 0;
			}
		}
	}
}

void i3SMTPMail::SetMessageBody( const char* body )
{
	unsigned int uio = m_pcMsgBody ? strlen( m_pcMsgBody ) : 0;
	unsigned int uin = body ? strlen( body ) : 0;

	if( uio < uin )
	{
		if( m_pcMsgBody != NULL)
			free( m_pcMsgBody);

		m_pcMsgBody	=	body ? strdup( body ) : 0;
	}
	else
	{
		if( m_pcMsgBody != NULL)
		{
			if( body )
				strcpy( m_pcMsgBody, body );
			else 
			{
				free( m_pcMsgBody); 
				m_pcMsgBody	= 0;
			}
		}
	}
}

void i3SMTPMail::SetSubject( const char* subject )
{	

	unsigned int uio = m_pcSubject ? strlen( m_pcSubject ) : 0;
	unsigned int uin = subject ? strlen( subject ):0;

	if( uio < uin )
	{
		delete[]	m_pcSubject;
		m_pcSubject = subject ? strdup( subject ) : 0;
	}
	else
	{
		if( m_pcSubject != NULL)
		{
			if( subject )	
				strcpy( m_pcSubject, subject );
			else 
			{
				delete[] m_pcSubject; 
				m_pcSubject = 0;
			}
		}
	}
}

void i3SMTPMail::SetSenderEmail( const char*  email )
{	
	unsigned int uio = m_pcFromEmail ? strlen( m_pcFromEmail ) : 0;
	unsigned int uin = email ? strlen( email ) : 0;

	if( uio < uin )
	{
		if( m_pcFromEmail != NULL)
			free( m_pcFromEmail);

		m_pcFromEmail = email ? strdup( email ) : 0;
	}
	else
	{
		if( m_pcFromEmail != NULL)
		{
			if( email )	
				strcpy( m_pcFromEmail, email );
			else 
			{
				free(	m_pcFromEmail); 
				m_pcFromEmail = 0;
			}
		}
	}
}

void i3SMTPMail::SetReplyTo( const char* replyto )
{	
	unsigned int uio = m_pcReplyTo ? strlen( m_pcReplyTo ) : 0;
	unsigned int uin = replyto ? strlen( replyto ) : 0;

	if( uio < uin )
	{
		if( m_pcReplyTo != NULL)
			free( m_pcReplyTo);

		m_pcReplyTo =	replyto ? strdup( replyto ) : 0;
	}
	else
	{
		if( m_pcReplyTo != NULL)
		{
			if( replyto )
				strcpy( m_pcReplyTo, replyto );
			else 
			{
				free(	m_pcReplyTo); 
				m_pcReplyTo = 0;
			}
		}
	}
}

void i3SMTPMail::SetXMailer( const char* xmailer )
{
	unsigned int uio = m_pcXMailer ? strlen( m_pcXMailer ) : 0;
	unsigned int uin = xmailer ? strlen( xmailer ) : 0;

	if( uio < uin )
	{
		if( m_pcXMailer != NULL)
			free( m_pcXMailer);

		m_pcXMailer =	xmailer ? strdup( xmailer ) : 0;
	}
	else
	{
		if( m_pcXMailer != NULL)
		{
			if( xmailer )	
				strcpy( m_pcXMailer, xmailer );
			else 
			{
				free( m_pcXMailer); 
				m_pcXMailer = 0;
			}
		}
	}
}

BOOL i3SMTPMail::GetConnectStatus()
{
	return	bCon;
}

BOOL i3SMTPMail::AddRecipient( const char* email, const char* name )
{
	int		s = strlen( email );
	if( s == 0 )	return	FALSE;

	TRecipient	r;	
	char*		pcBuf = new char[ s + strlen( name ) + 4 ];
	
	sprintf( pcBuf, "%s<%s>", name, email );	
	r.SetEmail( pcBuf );
	Recipients.insert( Recipients.end(), r );
	delete[] pcBuf;

	return	TRUE; 
}

BOOL i3SMTPMail::SetFileAttach(char* szFilename)
{
	FILE* pFile = NULL;
	pFile = fopen( szFilename , "rb");
	if(NULL == pFile )			return FALSE;

	INT32 hFile		= _fileno(pFile);
	INT32 nFileSize = _filelength(hFile);

	char* pszIn = new char[nFileSize]; 
	fread( pszIn, sizeof(char), nFileSize, pFile );

	//////호환성을 위해 base64로 인코딩하기위한 준비

	//SMTP 에 사용될 64버퍼를 할당한다.
	TAttach		tAttach;
	tAttach.m_pszEncoded = NULL;

	INT32 nOutSize			= Base64BufferSize(nFileSize);
	tAttach.m_pszEncoded	= new char[nOutSize];

	//64버퍼로 인코딩한다.
	EncodeBase64( pszIn, nFileSize, tAttach.m_pszEncoded );
	
	delete [] pszIn;
	pszIn = NULL;

	fclose( pFile );

	TCHAR szName[256];
	i3String::SplitFileName( szFilename, szName, TRUE );
	i3String::Copy( tAttach.m_szFilename, szName, MAX_PATH);
	
	Attach.push_back( tAttach );
	
	return TRUE;
}

INT32 i3SMTPMail::Base64BufferSize(INT32 nInputSize)
{
	INT32 nOutSize = (nInputSize+2)/3*4;                    // 3:4 conversion ratio
	nOutSize += strlen(EOL)*nOutSize/BASE64_MAXLINE + 3;  // Space for newlines and NUL
	return nOutSize;
}

BOOL i3SMTPMail::EncodeBase64(const char* pszIn, INT32 nInLen, char* pszOut )
{
	char pbase64tab[] =	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"abcdefghijklmnopqrstuvwxyz0123456789+/";

	//Set up the parameters prior to the main encoding loop
	INT32 nInPos	= 0;
	INT32 nOutPos	= 0;
	INT32 nLineLen	= 0;

	
	// Get three characters at a time from the input buffer and encode them
	for (INT32 i = 0; i < nInLen / 3; ++i) 
	{
		//Get the next 2 characters
		INT32 c1 = pszIn[nInPos++] & 0xFF;
		INT32 c2 = pszIn[nInPos++] & 0xFF;
		INT32 c3 = pszIn[nInPos++] & 0xFF;

		//Encode into the 4 6 bit characters
		pszOut[nOutPos++] = pbase64tab[(c1 & 0xFC) >> 2];
		pszOut[nOutPos++] = pbase64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
		pszOut[nOutPos++] = pbase64tab[((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6)];
		pszOut[nOutPos++] = pbase64tab[c3 & 0x3F];
		nLineLen += 4;

		//Handle the case where we have gone over the max line boundary
		if (nLineLen >= BASE64_MAXLINE - 3) 
		{
			char* cp = EOL;
			pszOut[nOutPos++] = *cp++;
			if(*cp) 
				pszOut[nOutPos++] = *cp;
			nLineLen = 0;
		}
	}

	// Encode the remaining one or two characters in the input buffer
	char* cp;
	switch (nInLen % 3) 
	{
	case 0:
		{
			cp = EOL;
			pszOut[nOutPos++] = *cp++;
			if (*cp) 
				pszOut[nOutPos++] = *cp;
			break;
		}
	case 1:
		{
			int c1 = pszIn[nInPos] & 0xFF;
			pszOut[nOutPos++] = pbase64tab[(c1 & 0xFC) >> 2];
			pszOut[nOutPos++] = pbase64tab[((c1 & 0x03) << 4)];
			pszOut[nOutPos++] = '=';
			pszOut[nOutPos++] = '=';
			cp = EOL;
			pszOut[nOutPos++] = *cp++;
			if (*cp) 
				pszOut[nOutPos++] = *cp;
			break;
		}
	case 2:
		{
			INT32 c1 = pszIn[nInPos++] & 0xFF;
			INT32 c2 = pszIn[nInPos] & 0xFF;
			pszOut[nOutPos++] = pbase64tab[(c1 & 0xFC) >> 2];
			pszOut[nOutPos++] = pbase64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
			pszOut[nOutPos++] = pbase64tab[((c2 & 0x0F) << 2)];
			pszOut[nOutPos++] = '=';
			cp = EOL;
			pszOut[nOutPos++] = *cp++;
			if (*cp) 
				pszOut[nOutPos++] = *cp;
			break;
		}   
	}
	pszOut[nOutPos] = 0;
	return TRUE;
}

#endif