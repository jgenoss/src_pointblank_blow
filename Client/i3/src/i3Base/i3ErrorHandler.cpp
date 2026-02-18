#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3Debug.h"

#include "i3System.h"
#include "i3MapFileParser.h"
#include "string/ext/utf16_to_mb.h"

#if defined( I3_WINDOWS)
#include "i3ErrorHandlerWindows.h"
#include "i3ErrorUnExpectionWindows.h"
#include "winsock2.h"
#endif

#if defined( I3_WINCE)
#include "winsock2.h"
#endif

#include "i3RSABF.h"
#include "string/stack_string.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/generic_string_cat.h"

static I3ERROR_HANDLE s_i3LogDefaultCallBack( I3LOG_TYPE /* type */, char * pszWhere, char * pszMsg, void * pUserData )
{
#ifdef I3_WINDOWS
	HWND hWnd = (HWND)pUserData;
	char szTemp[1024];
	sprintf( szTemp, "%s %s\r\n계속 진행하시겠습니까?", pszWhere, pszMsg );
	int nRet = ::MessageBox( hWnd, szTemp, NULL, MB_YESNO );
	if( nRet == IDYES )
	{
		return I3ERROR_HANDLE_CONTINUE;
	}
#endif

	return I3ERROR_HANDLE_BREAK;
}

static UINT32			s_LogChannel[I3LOG_TYPE_MAX] =
{
	I3ERR_CHN_STDOUT | I3ERR_CHN_DEBUG,
	I3ERR_CHN_STDOUT | I3ERR_CHN_DEBUG,
	I3ERR_CHN_STDERR | I3ERR_CHN_DEBUG,
	I3ERR_CHN_STDOUT | I3ERR_CHN_DEBUG,
	I3ERR_CHN_STDOUT | I3ERR_CHN_DEBUG,
	0
};

const char * s_LogTypeString[I3LOG_TYPE_MAX] = 
{
	"N",
	"W",
	"F",
	"V",
	"D"
};

// 파일 생성 여부
static bool bLogStarted = false;

static const char *			s_pszErrorFile = nullptr;
static INT32				s_ErrorLine = 0;
static const char *			s_pszErrorFunc = nullptr;
static I3LOG_TYPE			s_LogType = I3LOG_NOTICE;
static I3LOGCALLBACK		s_pLogProc[ I3LOG_TYPE_MAX];
static I3EXPECTIONCALLBACK	s_pUnExpectionProc;					// 20070419 김현우
static void *				s_pLogData[ I3LOG_TYPE_MAX];
static char					s_szWhere[512], s_szMsg[2048];
static char					s_szChnName[ I3LOG_TYPE_MAX][256];
void *						s_hwndMain = nullptr;
bool						s_bEnterDialog = false;

i3::rsa_bf_log_enc*			s_bf_log = nullptr;

namespace
{
	struct cleanup_for_log_enc
	{
		~cleanup_for_log_enc()
		{
			i3::delete_rsa_bf_enc(s_bf_log);
		}
	} _cleanup_for_log_enc_;
}


bool i3Error::StartEncLogFile(const char* szLogPath)		// 이게 i3Viewer의 __startlog()함수내부에 추가되야함..
{
	// 테스트용 함수 추가..
	//i3::stack_string db_path = szLogPath;
	//db_path = db_path.substr(0, db_path.rfind(".") + 1);
	//db_path += "db";

	i3::stack_string public_key_path = szLogPath;
	public_key_path = public_key_path.substr(0, public_key_path.rfind(".") + 1);
	public_key_path += "pub";

	// DB파일 즉석생성 (실제는 여기서 하면 안됨)
	// i3::save_public_private_key_to_db( db_path, 128);
	//

	// 고객용 공개키 파일 즉석 생성 (실제는 여기서 하면 안됨)
	// i3::save_public_key_from_db(db_path, public_key_path);
	//

	i3::delete_rsa_bf_enc(s_bf_log);
	s_bf_log = i3::create_rsa_bf_enc(public_key_path, szLogPath) ;

	return s_bf_log != nullptr;
}

bool i3Error::IsLogFileEncrypt()
{
	return s_bf_log != nullptr;
}

void i3Error::WriteEncryptLog(const char* szMsg)
{
	s_bf_log->log(szMsg);
}


I3_EXPORT_BASE 
void i3Error::Init(void)
{	
	memset( s_pLogProc, 0, sizeof(s_pLogProc));
	memset( s_pLogData, 0, sizeof(s_pLogData));
	memset( s_szChnName, 0, sizeof(s_szChnName));

#if defined( I3_WINDOWS)
	for(INT32 i = 0; i < I3LOG_TYPE_MAX; i++)
	{
		s_pLogProc[i]	= DefWindowsLogProc;
	}

	s_LogChannel[I3LOG_FATAL] |= I3ERR_CHN_CALLBACK;	

	s_pUnExpectionProc	= DefWindowsExpectionProc;	
	//s_pLogExProc		= DefWindowsLogProc;

#endif
}
I3_EXPORT_BASE
void I3assertForCoverity()
{
	//빈함수
}

I3_EXPORT_BASE
void i3Error::SetMainWindow( void * hwnd)
{
	s_hwndMain = hwnd;
}

I3_EXPORT_BASE
bool i3Error::isErrorReporting(void)
{
	return s_bEnterDialog;
}

I3_EXPORT_BASE 
void i3Error::SetEnv( const char * pszFile, INT32 line, const char * pszFunc, I3LOG_TYPE lv)
{
	s_pszErrorFile = pszFile;
	s_ErrorLine = line;
	s_pszErrorFunc = pszFunc;
	s_LogType = lv;

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if(lv == I3LOG_QA)
		s_LogType = I3LOG_NOTICE;
#endif

	switch(lv)
	{
	case I3LOG_NOTICE:
	case I3LOG_WARN:
	case I3LOG_FATAL:
		sprintf( s_szWhere, "[%s(%d) : [%s]]", pszFile, line, pszFunc);
		break;

	default:
		s_szWhere[0] = '\0';
		break;
	}	
}

I3_EXPORT_BASE
const char* i3Error::GetEnvErrorFile()
{
	return s_pszErrorFile;
}

I3_EXPORT_BASE
INT32 i3Error::GetEnvErrorLine()
{
	return s_ErrorLine;
}

I3_EXPORT_BASE
const char* i3Error::GetEnvErrorFunc()
{
	return s_pszErrorFunc;
}

I3_EXPORT_BASE
I3LOG_TYPE i3Error::GetEnvLogType()
{
	return s_LogType;
}

I3_EXPORT_BASE 
void	i3Error::SetChannel( I3LOG_TYPE type, UINT32 chn)
{
	s_LogChannel[type] = chn;
}

I3_EXPORT_BASE 
void	i3Error::SetCallback( I3LOG_TYPE type, I3LOGCALLBACK pUserProc, void * pUserData)
{
	s_pLogProc[ type] = pUserProc;
	s_pLogData[ type] = pUserData;

	if( pUserProc != nullptr)
	{
		s_LogChannel[type] |= I3ERR_CHN_CALLBACK;
	}
	else
	{
		s_LogChannel[type] &= ~I3ERR_CHN_CALLBACK;
	}
}

I3_EXPORT_BASE
void i3Error::SetDefaultCallback( I3LOG_TYPE type, void * pUserData )
{
	SetCallback( type, s_i3LogDefaultCallBack, pUserData );
}


I3_EXPORT_BASE 
void	i3Error::SetChannelName( I3LOG_TYPE type, const PSTR pszName)
{
	i3::safe_string_copy( s_szChnName[type], pszName, 256);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
// I3ExceptionHandler

static I3ExceptionHandler	s_pExpectionOuter;

LONG WINAPI CheckUnhandledException( PEXCEPTION_POINTERS pExceptionInfo )
{
#if defined( I3_WINDOWS)
	UINT old_mode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif
	s_pExpectionOuter.SetCreateMiniDump( pExceptionInfo );
	//s_pExpectionOuter.ExceptionInformation( pExceptionInfo );

#if defined( I3_WINDOWS)
	::SetErrorMode(old_mode);
#endif
	return 0;
}

I3_EXPORT_BASE
void i3Error::SetExceptionHandler( const char* pszDmpFileName )
{
	s_pExpectionOuter.SetPath( pszDmpFileName );

	SetUnhandledExceptionFilter( CheckUnhandledException );	// 20070419 김현우
}

I3_EXPORT_BASE 
void	i3Error::SetExceptionHandlerOption( INT32 nOption )
{
	s_pExpectionOuter.SetExceptionHandlerOption( nOption );
}

static char		s_Packet[4096];

static void _logFile(const i3::literal_range& rngMsg)
{
#if defined( I3_WINDOWS)
	HANDLE hFile;
	char * pszHostName;

	pszHostName = s_szChnName[ s_LogType];

	SYSTEMTIME		SystemTime;
	//Cal Local Time
	::GetLocalTime( &SystemTime );	
	INT32 Hour		=	SystemTime.wHour; 
	INT32 Minute	=	SystemTime.wMinute; 		
	INT32 Second	=	SystemTime.wSecond;

	sprintf( s_Packet, "[%02d:%02d:%02d][%s]%s : %s\r\n", Hour, Minute, Second, s_LogTypeString[s_LogType], s_szWhere, rngMsg.c_str());
	//sprintf( s_Packet, "%s : %s\r\n", s_szWhere, s_szMsg);

	////////////////////////////////////////////////////////////////////
	if ( i3Error::IsLogFileEncrypt() )
	{
		i3Error::WriteEncryptLog(s_Packet);
		return;
	}
	/////////////////////////////////////////////////////////////////////

	if( bLogStarted )
	{
		hFile = ::CreateFile( pszHostName, GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ, 
			NULL, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
	}
	else
	{
		hFile = ::CreateFile( pszHostName, GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		bLogStarted = true;
	}
	if( hFile == INVALID_HANDLE_VALUE)
	{
		bLogStarted = false;
		std::string directory_path = pszHostName;
		
		// 폴더 생성
		directory_path.erase(directory_path.rfind("\\"));
		::CreateDirectory(directory_path.c_str(), NULL);

		hFile = ::CreateFile( pszHostName, GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if(hFile == INVALID_HANDLE_VALUE)
		{
			I3TRACE( "_log : Could not open %s file.\n", pszHostName);
			return;
		}

		bLogStarted = true;
	}

	::SetFilePointer( hFile, 0, NULL, FILE_END);

	DWORD len;
	::WriteFile( hFile, s_Packet, strlen( s_Packet), &len, NULL);

	CloseHandle( hFile);
#endif
}

static void _logUDP(void)
{
#if !defined( I3_WIPI) && !defined( I3_WINCE )
	SOCKET			hSock;
	SOCKADDR_IN		sa;
	I3_ERROR_INFO *	pMsg;
	UINT32			addr, WhereLen, MsgLen;
	char *			pszHostName, * pszDest;

	pszHostName = s_szChnName[ s_LogType];
	pMsg = (I3_ERROR_INFO *) s_Packet;
	WhereLen = strlen( s_szWhere);
	MsgLen = strlen( s_szMsg);

	pMsg->m_Version = htons( 0x0100);
	pMsg->m_Type = htons( (UINT16) s_LogType);
	pMsg->m_WhereLength = htons( (UINT16)WhereLen);
	pMsg->m_MsgLength = htons( (UINT16)MsgLen);

	// Where
	pszDest = s_Packet + sizeof(I3_ERROR_INFO);
	memcpy( pszDest, s_szWhere, WhereLen);

	// Msg
	pszDest = pszDest + WhereLen;
	memcpy( pszDest, s_szMsg, MsgLen);


	#if defined( I3_WINDOWS)
		hSock = socket( AF_INET, SOCK_DGRAM, 0 );	/******** DataGram ********/ 
	#elif defined( I3_PS2)
		hSock = sceInsockSocket( AF_INET, SOCK_DGRAM, 0);
	#endif
	
	if( hSock == INVALID_SOCKET )
	{
		#if defined(I3_WINDOWS)
			{
				WSADATA	wsd;

				switch( WSAGetLastError())
				{
					case WSANOTINITIALISED :
						WSAStartup( MAKEWORD(2, 2), &wsd);

						hSock = socket( AF_INET, SOCK_DGRAM, 0);
						if( hSock == INVALID_SOCKET)
							return;
						break;
				}// end switch()
				
			}// end if()
			
		#elif defined(I3_PS2)
			I3TRACE("_Log(), socket creation error. error code = %d", sceInsockErrno);
			return;
			
		#else
			I3TRACE("_Log(), socket creation error. error code = %d");
			return;
			
		#endif
	}

	memset( &sa, 0, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons( I3LOG_UDP_PORT);
	
	if( isalpha( *pszHostName))
	{
		hostent * pHost;

		pHost = gethostbyname( pszHostName);
		if( pHost == nullptr)
		{
			closesocket( hSock);
			return;
		}

		addr = inet_addr( pHost->h_addr_list[0]);
	}
	else
	{
		addr = inet_addr( pszHostName);
	}

	#if defined(I3_WINDOWS)
		sa.sin_addr.S_un.S_addr = addr;
	#else
		sa.sin_addr.s_addr = addr;
	#endif

	#if defined( I3_WINDOWS)
		INT32 rv = sendto( hSock, s_Packet, sizeof(I3_ERROR_INFO) + WhereLen + MsgLen, 0, (SOCKADDR*)&sa, sizeof(sa) );
		if( rv == SOCKET_ERROR)
		{
			closesocket( hSock);
			return;
		}
	#elif defined( I3_PS2)
		sceInsockSendto( hSock, s_Packet, sizeof(I3_ERROR_INFO) + WhereLen + MsgLen, 0, (SOCKADDR *)&sa, sizeof(sa));
	#endif
	
	#if defined(I3_WINDOWS)
		closesocket( hSock);
	#elif defined(I3_PS2)
		sceInsockShutdown(hSock, 0);
	#else
		shutdown(hSock, 0);
	#endif
#endif
}

static void _logPipe(void)
{
}

I3_EXPORT_BASE
void	i3Error::Log( const i3::literal_range& rngMsg)
{
	
	if( s_LogChannel[ s_LogType] & I3ERR_CHN_STDOUT)
	{
		fprintf( stdout, "%s : %s\n", s_szWhere, rngMsg.c_str());
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_STDERR)
	{
		fprintf( stderr, "%s : %s\n", s_szWhere, rngMsg.c_str());
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_DEBUG)
	{
		I3TRACE( "%s : %s\n", s_szWhere, rngMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_FILE)
	{
		_logFile(rngMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_NETWORK)
	{
		_logUDP();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_PIPE)
	{
		_logPipe();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_CALLBACK)
	{
		if( s_pLogProc[ s_LogType] != nullptr)
		{
			s_bEnterDialog = true;
			if( s_pLogProc[ s_LogType]( s_LogType, s_szWhere, (char*)rngMsg.c_str(), s_pLogData[ s_LogType]) == I3ERROR_HANDLE_BREAK)
			{
#if defined( I3_DEBUG )
				I3ASSERT_0;
#endif
			}
			s_bEnterDialog = false;
		}
	}

}

I3_EXPORT_BASE
void	i3Error::Log( const i3::wliteral_range& wrngMsg)
{
	i3::custom_stack_string<512>::type str;
	i3::utf16_to_mb(wrngMsg, str);
	i3Error::Log(str);
}
/*
I3_EXPORT_BASE 
void	i3Error::Log( const char * pszMsg, ...)
{
	va_list marker;

	va_start( marker, pszMsg);

	vsprintf( s_szMsg, pszMsg, marker);

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_STDOUT)
	{
		fprintf( stdout, "%s : %s\n", s_szWhere, s_szMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_STDERR)
	{
		fprintf( stderr, "%s : %s\n", s_szWhere, s_szMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_DEBUG)
	{
		I3TRACE( "%s : %s\n", s_szWhere, s_szMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_FILE)
	{
		_logFile();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_NETWORK)
	{
		_logUDP();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_PIPE)
	{
		_logPipe();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_CALLBACK)
	{
		if( s_pLogProc[ s_LogType] != nullptr)
		{
			s_bEnterDialog = true;
			if( s_pLogProc[ s_LogType]( s_LogType, s_szWhere, s_szMsg, s_pLogData[ s_LogType]) == I3ERROR_HANDLE_BREAK)
			{
#if defined( I3_DEBUG )
 			I3ASSERT_0;
#endif
			}
			s_bEnterDialog = false;
		}
	}
}
*/

// 20070411 김현우
I3ExceptionHandler::I3ExceptionHandler() : m_DumpFileName("i3BugReport.i3b")
{
	m_uDumpType = I3MINDUMP_NORMAL;
}

I3ExceptionHandler::~I3ExceptionHandler()
{
}

void	I3ExceptionHandler::SetPath( const char* pszDmpFileName )
{
	if( pszDmpFileName == nullptr) return;
	
	m_DumpFileName = pszDmpFileName;
}

void	I3ExceptionHandler::SetExceptionHandlerOption( INT32 nOption )
{
	switch( nOption )
	{
		case 0:			m_uDumpType = I3MINDUMP_NORMAL;								break;
		case 1:			m_uDumpType = I3MINDUMP_WITHDATASEGS;						break;
		case 2:			m_uDumpType = I3MINDUMP_WITHFULLMEMORY;						break;
		case 3:			m_uDumpType = I3MINDUMP_WITHHANDLEDATA;						break;
		case 4:			m_uDumpType = I3MINDUMP_FILTERMEMORY;						break;
		case 5:			m_uDumpType = I3MINDUMP_SCANMEMORY;							break;
		case 6:			m_uDumpType = I3MINDUMP_WITHUNLOADEDMODULES;				break;
		case 7:			m_uDumpType = I3MINDUMP_WITHINDIRECTLYREFERENCEDMEMORY;		break;
		case 8:			m_uDumpType = I3MINDUMP_FILTERMODULEPATHS;					break;
		case 9:			m_uDumpType = I3MINDUMP_WITHPROCESSTHREADDATA;				break;
		case 10:		m_uDumpType = I3MINDUMP_WITHPRIVATEREADWRITEMEMORY;			break;
		case 11:		m_uDumpType = I3MINDUMP_WITHOUTOPTIONALDATA;				break;
		case 12:		m_uDumpType = I3MINDUMP_WITHFULLMEMORYINFO;					break;
		case 13:		m_uDumpType = I3MINDUMP_WITHTHREADINFO;						break;
		case 14:		m_uDumpType = I3MINDUMP_WITHCODESEGS;						break;
		default:		m_uDumpType = I3MINDUMP_NORMAL;								break;
	}
}

void	I3ExceptionHandler::ExceptionInformation( PEXCEPTION_POINTERS pExceptionInfo )
{
#if defined( I3_WINDOWS )

	char szEmailContentsBuffer[ sizeof(SYSTEM_INFO) + 256];
	char szAddress[256];	
	char szTemp[64];

	szEmailContentsBuffer[ 0 ] = 0;
	szAddress[0] = 0;

	// 실질적인 에러 주소만 있으면 호출스택과 에러 위치 확인가능..

	//pExceptionRecord->ExceptionCode		- 에러 코드
	//pExceptionRecord->ExceptionAddress	- 에러 주소

	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

	i3::generic_string_cat( szAddress, "[Error Code]\n" );
	sprintf( szTemp, "Code = 0x%08X\n\n", pExceptionRecord->ExceptionCode);	
	i3::generic_string_cat( szAddress, szTemp );
	
	PCONTEXT pContextRecord = pExceptionInfo->ContextRecord;	
	PDWORD pFrame;
	pFrame = (PDWORD)pContextRecord->Ebp;	

	i3::generic_string_cat( szAddress, "[Error Address]\n" );
	sprintf( szTemp, "Address = 0x%08X\n\n", pExceptionRecord->ExceptionAddress );
	i3::generic_string_cat( szAddress, szTemp );	

	i3::generic_string_cat( szAddress, "[Call Stack]\n" );
	i3::generic_string_cat( szAddress, "이부분은 비어있습니다.\n" );
	
	/*
	int i = 1;
	DWORD pc = pContextRecord->Eip;
	PDWORD pPrevFrame;
	// 호출스택을 위해 루프
	//do {		
		///////////////////////////////////////////////////////////////////////////
		// 주소값만 찍어주기		
		//sprintf( szTemp, "%02d = 0x%08X\n", i, pc );
		//i3::generic_string_cat( szAddress, szTemp );
		///////////////////////////////////////////////////////////////////////////
		pc = pFrame[1];		
		pPrevFrame = pFrame;
		pFrame = (PDWORD)pFrame[0];
		i++;

		//if((DWORD)pFrame & 3) break;
		//if(IsBadWritePtr(pFrame, sizeof(PVOID) * 2)) break;
	//} while( pFrame > pPrevFrame && i < 2 );
	*/

	//char szPath[1024] = {0,};
	//::GetModuleFileName(nullptr, szPath, 256);
	//GetLastWriteTime(szPath)

	// System 정보 수집
	GetSystemInfo( szEmailContentsBuffer );

	i3::generic_string_cat( szEmailContentsBuffer, szAddress);

	//SendMailBugReport( szEmailContentsBuffer);
	
	SetCreateMiniDump( pExceptionInfo );

#if !defined( I3_NO_PROFILE)
	if(s_pUnExpectionProc( szEmailContentsBuffer ) == I3ERROR_HANDLE_BREAK)
	{
		I3ASSERT_0;
	}	
#else//Exception Info를 notice채널파일에 write함..포인트블랭크용..임시땜빵 - 필히 수정볼것 - raja
	HANDLE hFile;
	char * pszHostName = s_szChnName[ I3LOG_NOTICE];
	hFile = ::CreateFile( pszHostName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE)
	{
		I3TRACE( "_log : Could not open %s file.\n", pszHostName);
		return;
	}

	::SetFilePointer( hFile, 0, NULL, FILE_END);

	DWORD len;
	::WriteFile( hFile, szEmailContentsBuffer, strlen( szEmailContentsBuffer), &len, NULL);

	CloseHandle( hFile);

	// 오류보고 창 띄우는 부분
	char szFile[ 256 ];
	sprintf( szFile, "ErrorHandler.exe" );
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;
	si.cb = sizeof( STARTUPINFO );
	si.dwFlags = 0;

	::CreateProcess( szFile, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
	// 오류보고 창 띄우는 부분
#endif
#endif
}

void	I3ExceptionHandler::SetCreateMiniDump( PEXCEPTION_POINTERS pExceptionInfo )
{
#if defined( I3_WINDOWS)
// miniDump 실행
	HANDLE hPro = GetCurrentProcess();
	DWORD dwProcessID = GetCurrentProcessId();
	DWORD dwThreadID = GetCurrentThreadId();
	MINIDUMP_EXCEPTION_INFORMATION sExceptionInfo;

	sExceptionInfo.ThreadId = dwThreadID;
	sExceptionInfo.ExceptionPointers = pExceptionInfo;
	sExceptionInfo.ClientPointers = FALSE;

	char szFileName[ 512 ];

	SYSTEMTIME st; 
    GetLocalTime(&st); 

	sprintf( szFileName, "%s_%04d%02d%02d%02d%02d", m_DumpFileName.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );

	HANDLE hFile = ::CreateFile( szFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	::MiniDumpWriteDump(hPro, dwProcessID, hFile, (MINIDUMP_TYPE)m_uDumpType, &sExceptionInfo, NULL, NULL);
	
	CloseHandle( hFile );
#endif

}
// 20070411 김현우
 
void	I3ExceptionHandler::GetSystemInfo( char * pszTemp)
{
	i3::generic_string_cat( pszTemp, "[O/S info]\n");
	i3System::GetOSInfo( pszTemp);
	i3::generic_string_cat( pszTemp, "\n\n");

	i3::generic_string_cat( pszTemp, "[CPU info]\n");
	i3System::GetCPUInfo(pszTemp);
	i3::generic_string_cat( pszTemp, "\n\n");
	
	i3::generic_string_cat( pszTemp, "[GPU info]\n");
	i3System::GetGPUInfo(pszTemp);
	i3::generic_string_cat( pszTemp, "\n\n");

	i3::generic_string_cat( pszTemp, "[Memory Info]\n");
	i3System::GetMemoryInfo(pszTemp);
	i3::generic_string_cat( pszTemp, "\n\n");

	i3::generic_string_cat( pszTemp, "[Multimedia info]\n ");
	i3System::GetMultiMediaInfo(pszTemp);
	i3::generic_string_cat( pszTemp, "\n\n");

	i3::generic_string_cat( pszTemp, "[DirectX info]\n");
	i3System::GetDXInfo(pszTemp);
	i3::generic_string_cat( pszTemp, "\n\n");

	i3System::GetProcessList(pszTemp);
	i3::generic_string_cat( pszTemp, "\n");
}

const char *		i3Error::getMsg( CODE code)
{
	switch( code)
	{
		case _NOERROR	:				return "No error.";
		case _OPENFAIL :				return "Open failed.";
		case _READFAIL	:				return "Read failed.";
		case _WRITEFAIL :				return "Write failed.";
		case _TIMEOUT :				return "Timeout.";

		case _OUTOFMEMORY :			return "Out of memory. This error may be involved by a bug. please report it to developers.";
		case _OUTOFVIDEOMEMORY :		return "Out of video memory. you may have to update graphic card driver software.";
		
		case _ENCRIPT_VIOLATION :		return "Encription was broken. This error could be caused by a malicious software. You may solve it by running anti-virus software.";

		case _INVALIDFORMAT :			return "Invalid format. This file has invalid or unsupported format, or could be corrupted.";
		case _OLDVERSION :				return "Unsupported old version.";
		case _UNKNOWNCLASSMETA :		return "Unknown class meta. It's bug! please report it to developer.";
	}

	return "Unknown error";
}


/*
I3_EXPORT_BASE 
void	i3Error::I3BCLog(const char * pszFile, INT32 line, const char * pszFunc, const char * pszMsg, ...)
{
	i3Error::SetEnv( pszFile, line, pszFunc, I3LOG_WARN);

	va_list marker;

	va_start( marker, pszMsg);

	vsprintf( s_szMsg, pszMsg, marker);

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_STDOUT)
	{
		fprintf( stdout, "%s : %s\n", s_szWhere, s_szMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_STDERR)
	{
		fprintf( stderr, "%s : %s\n", s_szWhere, s_szMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_DEBUG)
	{
		I3TRACE( "%s : %s\n", s_szWhere, s_szMsg);
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_FILE)
	{
		_logFile();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_NETWORK)
	{
		_logUDP();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_PIPE)
	{
		_logPipe();
	}

	if( s_LogChannel[ s_LogType] & I3ERR_CHN_CALLBACK)
	{
		if( s_pLogProc[ s_LogType] != nullptr)
		{
			if( s_pLogProc[ s_LogType]( s_LogType, s_szWhere, s_szMsg, s_pLogData[ s_LogType]) == I3ERROR_HANDLE_BREAK)
			{
#if defined( I3_DEBUG )
				I3ASSERT_0;
#endif
			}
		}
	}
}
*/
