#include "pch.h"
#include "ModuleContextNc.h"
#include "ServerContext.h"
#include "UserSession.h"
#include "Room.h"
#include "RoomManager.h"
#include "UserManager.h"

//	MM/DD/YYYY HH:MI:SS.00 -> YYMMDDHHMI
#define ConverTimeFromtSTRToUINT32( strDate )			(*(strDate+0)-'0')*10000000 +\
														(*(strDate+1)-'0')*1000000 +\
														(*(strDate+3)-'0')*100000 +\
														(*(strDate+4)-'0')*10000 +\
														(*(strDate+8)-'0')*1000000000 +\
														(*(strDate+9)-'0')*100000000 +\
														(*(strDate+11)-'0')*1000 +\
														(*(strDate+12)-'0')*100 +\
														(*(strDate+14)-'0')*10 +\
														(*(strDate+15)-'0');
//	YYMMDDHHMI -> MM/DD/YYYY HH:MI:SS.00
#define ConverTimeFromtUINT32ToSTR( strDate, ui32Date)	i3String::Format( strDate, SERVER_STRING_COUNT, "%02d/%02d/20%02d %02d:%02d:00.000",\
														((ui32Date%100000000)/1000000), ((ui32Date%1000000)/10000), (ui32Date/100000000), ((ui32Date%10000)/100), (ui32Date%100) );

UINT32 _GetDiffTime( UINT64 ui64DTLogIn )
{
	struct tm tmLogIn;
	
	tmLogIn.tm_year		= (INT32)((ui64DTLogIn / 10000000000) + 100);			ui64DTLogIn %= 10000000000;
	tmLogIn.tm_mon		= (INT32)((ui64DTLogIn / 100000000) - 1);				ui64DTLogIn %= 100000000;
	tmLogIn.tm_mday		= (INT32)(ui64DTLogIn / 1000000);						ui64DTLogIn %= 1000000;
	tmLogIn.tm_hour		= (INT32)(ui64DTLogIn / 10000) ;						ui64DTLogIn %= 10000;
	tmLogIn.tm_min		= (INT32)(ui64DTLogIn / 100) ;							ui64DTLogIn %= 100; 
	tmLogIn.tm_sec		= (INT32)ui64DTLogIn; 
	tmLogIn.tm_isdst	= SUMMER_TIME_FOLLOW_SYSTEM;

	time_t tmTLogin = mktime( &tmLogIn );

	struct tm tmLogOut;
	
	tmLogOut.tm_year	= i3ThreadTimer::LocalTime().GetYear();
	tmLogOut.tm_mon		= i3ThreadTimer::LocalTime().GetMonth();
	tmLogOut.tm_mday	= i3ThreadTimer::LocalTime().GetDay();
	tmLogOut.tm_hour	= i3ThreadTimer::LocalTime().GetHour();
	tmLogOut.tm_min		= i3ThreadTimer::LocalTime().GetMinute();
	tmLogOut.tm_sec		= i3ThreadTimer::LocalTime().GetSecond();
	tmLogOut.tm_isdst	= SUMMER_TIME_FOLLOW_SYSTEM;
	
	time_t tmTLogOut = mktime(&tmLogOut);

	return (UINT32)(difftime( tmTLogOut, tmTLogin ));
}

enum ZLOG_INSERT_TYPE
{
	ZLOG_LDATA01,	ZLOG_LDATA02,	ZLOG_LDATA03,
	ZLOG_IDATA01,	ZLOG_IDATA02,	ZLOG_IDATA03,
	ZLOG_IDATA04,	ZLOG_IDATA05,	ZLOG_IDATA06,
	ZLOG_IDATA07,	ZLOG_IDATA08,	ZLOG_IDATA09,
	ZLOG_IDATA10,	ZLOG_IDATA11,	ZLOG_IDATA12,
	ZLOG_IDATA13,	ZLOG_IDATA14,	ZLOG_IDATA15,
	ZLOG_IDATA16,	ZLOG_IDATA17,
	ZLOG_SDATA01,	ZLOG_SDATA02,	ZLOG_SDATA03,
	ZLOG_SDATA04,	ZLOG_SDATA05,	ZLOG_SDATA06,
	ZLOG_SDATA07,	ZLOG_SDATA08,	ZLOG_SDATA09,
	ZLOG_SDATA10,
};

#ifdef I3_DEBUG
#define ASSERT_COMP( expr )  { char Temp[ (expr) ? 1 : 0 ]; Temp;	} 
#else
#define ASSERT_COMP( expr )  (void)0
#endif

// ID, ID의 스트링 사이즈
#define START_DATA( ID, size )			memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], ID, size );				i16LogSize+= size;\
										if( NULL == pSession )\
										{\
											memcpy( &strSendBuffer[i16LogSize], "&,0", 3 );				i16LogSize += 3;\
										}\
										else\
										{\
											_itoa(pSession->m_ui8LocaleCode, strtoa, 10);				i32StrLenth = i3String::Length(strtoa);\
											memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
											memcpy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;\
										}\
										strSendBuffer[NEW_WHERE_DATA_IDX]	= 'z';		strSendBuffer[NEW_WHERE_STRING_IDX]	= 'z';
// Insert Type, 값, String 일때 size
#define ADD_LDATA( datatype, values )	ASSERT_COMP( (ZLOG_LDATA01<= datatype) && (ZLOG_LDATA03 >= datatype) );\
										_i64toa(values, strtoa, 10);	i32StrLenth = i3String::Length(strtoa);\
										memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;	i32DataIdx++;
#define ADD_IDATA( datatype, values )	ASSERT_COMP( (ZLOG_IDATA01<= datatype) && (ZLOG_IDATA17 >= datatype) );\
										_itoa(values, strtoa, 10);		i32StrLenth = i3String::Length(strtoa);\
										if( ZLOG_IDATA01==datatype )	strSendBuffer[NEW_WHERE_DATA_IDX]	= ((char)i32DataIdx) + 'a';\
										memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;	i32DataIdx++;
#define ADD_SDATA( datatype, values )	ASSERT_COMP( (ZLOG_SDATA01<= datatype) && (ZLOG_SDATA10 >= datatype) );\
										i32StrLenth = i3String::Length(values);\
										if( ZLOG_SDATA01==datatype )	strSendBuffer[NEW_WHERE_STRING_IDX]	= ((char)i32DataIdx) + 'a';\
										memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], values, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;
#define END_DATA()						memcpy( &strSendBuffer[i16LogSize], "\r\n\0", 3 );			i16LogSize += 2;

//////////////////////////////////////////////////////////////
//LOG
#define	LOG_SEND_BUFFER_COUNT				100
#define NEW_WHERE_DATA_IDX					0
#define NEW_WHERE_STRING_IDX				NEW_WHERE_DATA_IDX+1
#define NEW_STRING_START_IDX				NEW_WHERE_STRING_IDX+1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_CLASS_INSTANCE( CModuleContextNC, i3Thread );
CModuleContextNC * g_pModuleContextNC = NULL; 

CModuleContextNC::CModuleContextNC(void)
{
	m_bRunning				= FALSE;
	m_ui32ThreadCount		= 0;
	m_i32DateHour			= 0;
	m_hFile					= INVALID_HANDLE_VALUE;
	m_hBackupFile			= INVALID_HANDLE_VALUE;

	//Log
	m_i32RingCount			= 0;
	m_pRingBuffer			= NULL;
}

CModuleContextNC::~CModuleContextNC(void)
{	
	OnDestroy(); 
}

INT32 CModuleContextNC::OnCreate( UINT32 ui32ThreadCount )
{
	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer 

	m_i32RingCount = ZLOG_RING_BUFFER_USER + ui32ThreadCount;
	m_pRingBuffer = new NSM_CRingBuffer[ m_i32RingCount ];
	if( NULL == m_pRingBuffer )																		return SERVER_ERROR_LOG;
	for( INT32 i = 0 ; i < m_i32RingCount ; i++ )
	{
		if( FALSE == m_pRingBuffer[i].OnCreate( sizeof(LOG_SEND_MESSAGE), LOG_SEND_BUFFER_COUNT ) )	return SERVER_ERROR_LOG;
	}

	if( FALSE == _CreateFile( m_hFile, g_pContextMain->m_strZLogFilePath ) )						return SERVER_ERROR_LOG;
	if( FALSE == _CreateFile( m_hBackupFile, g_pContextMain->m_strZLogBackupPath ) )				return SERVER_ERROR_LOG;

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleContextB", 0, 4096, NULL, PRIORITY_NORMAL) )				return SERVER_ERROR_LOG; 
		
	g_pLog->WriteLog( L"[SUCC] MODULE_B(NC LOG_D, NC GIP) CONTEXT READY!");

	_CleanupOldBackup(g_pContextMain->m_i32ZLogKeepDay);

	return EVENT_ERROR_SUCCESS; 
}

void CModuleContextNC::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_DELETE_ARRAY( m_pRingBuffer );
}

UINT32 CModuleContextNC::OnRunning( void * pUserData)
{
	g_pLog->AddLogThread( L"MDCTB", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

	INT32				i;
	INT32				i32Count;
	INT32				i32SendCount;
	INT32				i32WriteSize;
	LOG_SEND_MESSAGE*	pSendMessage;
	BOOL				bWorking;

	m_bRunning = TRUE;
	while( m_bRunning )
	{
		bWorking = FALSE;
		//Main Thread 
		for( i = 0 ; i < m_i32RingCount ; i++ )
		{
			i32SendCount = m_pRingBuffer[i].GetBufferCount();
			for( i32Count = 0 ; i32Count < i32SendCount ; i32Count++ )
			{
				pSendMessage = (LOG_SEND_MESSAGE*)m_pRingBuffer[i].Pop();
				::WriteFile( m_hFile, pSendMessage->_szMessage, pSendMessage->_Size, (LPDWORD)&i32WriteSize, NULL );
				::WriteFile( m_hBackupFile, pSendMessage->_szMessage, pSendMessage->_Size, (LPDWORD)&i32WriteSize, NULL );
				m_pRingBuffer[i].PopIdx();
				bWorking = TRUE;
			}
		}

		if( m_i32DateHour != i3ThreadTimer::LocalTime().GetHour() )
		{
			_CreateFile( m_hFile, g_pContextMain->m_strZLogFilePath );
			_CreateFile( m_hBackupFile, g_pContextMain->m_strZLogBackupPath );
			bWorking = TRUE;
		}

		if( FALSE == bWorking )
		{	// 한일이 없으면 1ms 휴식.
			::WaitForSingleObject( m_Handle, 1);
		}
	}
	return 0; 
}

BOOL CModuleContextNC::_CreateFile( HANDLE &hFile, char* strLogPath )
{
	char strFileName[SERVER_STRING_COUNT];
	//Close Old File 
	if( INVALID_HANDLE_VALUE != hFile )
	{
		::CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE; 
	}

	//Create New File 
	i3String::Format( strFileName, SERVER_STRING_COUNT, "%s\\%04d%02d%02d%02d_G%02d.txt", strLogPath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour(), g_pContextMain->m_i32ServerIdx );

	hFile = ::CreateFile( strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
	{
		::CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE; 
		return FALSE;
	}
	else
	{
		SetFilePointer( hFile, 0, NULL, FILE_END );
	}

	m_i32DateHour = i3ThreadTimer::LocalTime().GetHour();

	return TRUE;
}

UINT32 CModuleContextNC::_CleanupOldBackup( INT32 i32Day )
{
	//Config값이 -1이면 자동삭제를 진행하지 않습니다.
	if( -1 == i32Day ) return 0;

	BOOL bRv = FALSE;
	struct _finddata_t	sc_file;
	char strSerachFolder[SERVER_STRING_COUNT];
	strcpy( strSerachFolder, g_pContextMain->m_strZLogBackupPath );
	strcat( strSerachFolder, "\\*.*" );

	intptr_t hFile = _findfirst( strSerachFolder, &sc_file );
	if( -1L == hFile )
	{
		_findclose(hFile);
		// 로그남깁니다.
		//WRITE_LOG_FILE_MANAGER( "[ERR] Make Folder List Fail" );
		return bRv;
	}

	// 대기열 폴더 리스트를 재 정렬 합니다.
	while( 0 == _findnext( hFile, &sc_file ) ) 
	{
		// ".", ".." 는 검색 대상에서 제외 시킵니다.
		if( '.' == sc_file.name[0] ) continue;

		INT32 i32Time;
		i32Time = _GetFileDate( sc_file.name );
		SYSTEMTIME FileTime;
		
		FileTime.wYear		= (WORD)(i32Time / 1000000);
		FileTime.wMonth		= (i32Time / 10000) % 100;
		FileTime.wDay		= (i32Time / 100) % 100;
		FileTime.wHour		= i32Time % 100;
		FileTime.wMinute	= 0;
		FileTime.wSecond	= 0;

		DATE32 stDate;
		stDate = FileTime;

		if( i3ThreadTimer::LocalTime().DiffTime( stDate ) < i32Day * 86400 )		continue;

		char strDeleteFile[SERVER_STRING_COUNT];
		i3String::Format( strDeleteFile, SERVER_STRING_COUNT, "%s\\%s", g_pContextMain->m_strZLogBackupPath, sc_file.name );
		DeleteFile( strDeleteFile );
				
	}
	_findclose(hFile);

	return 0;
}

void CModuleContextNC::LogSendMessage( INT32 i32ThreadIdx, LOG_MESSAGE LogId, CUserSession * pSession, void* pArg0, void* pArg1, void* pArg2, void* pArg3 )
{
	LOG_SEND_MESSAGE*	pSendBuffer;
	NSM_CRingBuffer*	pRing;

	switch( i32ThreadIdx )
	{
	case	MAIN_THREAD_IDX				: pRing = &m_pRingBuffer[ ZLOG_RING_BUFFER_MAIN ];				break;
	case	MODULE_CAST_THREAD_IDX		: pRing = &m_pRingBuffer[ ZLOG_RING_BUFFER_CAST ];			break;
	case	MODULE_TRANS_THREAD_IDX		: pRing = &m_pRingBuffer[ ZLOG_RING_BUFFER_TRANS ];				break;
	case	MODULE_CLAN_THREAD_IDX		: pRing = &m_pRingBuffer[ ZLOG_RING_BUFFER_CLAN ];				break;
	case	MODULE_MESSENGER_THREAD_IDX	: pRing = &m_pRingBuffer[ ZLOG_RING_BUFFER_MESS ];				break;
	default								: pRing = &m_pRingBuffer[ ZLOG_RING_BUFFER_USER+i32ThreadIdx ];	break;
	}
	pSendBuffer = (LOG_SEND_MESSAGE*)pRing->PushPointer();
	if( NULL == pSendBuffer )
	{
		g_pLog->WriteLog( L"LogD Send Buffer Full" );
		return;
	}

	//2. Set Buffer
	INT16 i16LogSize = NEW_STRING_START_IDX;
	char strtoa[32];
	INT32 i32StrLenth;
	INT32 i32DataIdx	= 3;	// L 앞에 들어가는 내용은 시간, ID, LocaleCode 세개입니다.
	char* strSendBuffer = pSendBuffer->_szMessage;
	sprintf( &strSendBuffer[i16LogSize], "%02d/%02d/%04d %02d:%02d:%02d.00",
		i3ThreadTimer::LocalTime().GetMonth(),
		i3ThreadTimer::LocalTime().GetDay(),
		i3ThreadTimer::LocalTime().GetYear(),
		i3ThreadTimer::LocalTime().GetHour(),
		i3ThreadTimer::LocalTime().GetMinute(),
		i3ThreadTimer::LocalTime().GetSecond() );
	i16LogSize += 22;

	switch(LogId)
	{
	case LOG_MESSAGE_U_R_KICK_LEAVE_ROOM:	//강제 종료
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return; 

			LOG_KICK* pLog = (LOG_KICK*)pArg0;
			
			START_DATA( "100", 3 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA02, pLog->m_TCallUID);
			ADD_IDATA( ZLOG_IDATA01, pSession->m_i32ChannelNum );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_i32RoomIdx );
			ADD_IDATA( ZLOG_IDATA03, pLog->m_eType);
			ADD_IDATA( ZLOG_IDATA04, pLog->m_eReason);
			ADD_IDATA( ZLOG_IDATA05, pLog->m_ui8Good);
			ADD_IDATA( ZLOG_IDATA06, pLog->m_ui8Bad);
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_LOGIN		:		//로그 인
	{
		if (NULL == pSession)	return;

		UINT32 ui32Clan = 0;
		if (0 < pSession->m_UserInfoBasic.m_ui32Clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic.m_ui32ClanState)		ui32Clan = pSession->m_UserInfoBasic.m_ui32Clanidx;

		char strNick[MAX_NICK_NAME_LOG_SIZE];

		//@TODO 지로그 UNICODE 작업
		char strNickName[MAX_NICK_NAME_LOG_SIZE];
		i3String::CopyWCHARtoASCII(strNickName, MAX_NICK_NAME_LOG_SIZE, pSession->m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE);

		LogChangeString(strNick, strNickName, MAX_NICK_NAME_LOG_SIZE);

		char strID[NET_ID_SIZE];
		LogChangeString(strID, pSession->m_strId, NET_ID_SIZE);

		char strPCSpec[MAX_PATH];
		S2DB::ConvertStringToBinary(strPCSpec, MAX_PATH, (char*)&pSession->m_stPCSpec, sizeof(USER_INFO_PCSPECS));

		START_DATA("1001", 4);
		ADD_LDATA(ZLOG_LDATA01, pSession->m_i64UID);
		ADD_IDATA(ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank);
		ADD_IDATA(ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Point);
		ADD_IDATA(ZLOG_IDATA03, pSession->m_UserInfoBasic.m_ui32Cash);
		ADD_IDATA(ZLOG_IDATA04, pSession->m_UserInfoBasic.m_ui32Exp);
		ADD_IDATA(ZLOG_IDATA05, pSession->m_UserInfoBasic.m_ui8PCCafe);
		ADD_IDATA(ZLOG_IDATA06, ui32Clan);
		ADD_IDATA(ZLOG_IDATA07, pSession->m_ui32AccConnectTime);
		ADD_IDATA(ZLOG_IDATA08, pSession->m_dt32LastLoginTime.GetDateTimeYYMMDDHHMI());
		ADD_IDATA(ZLOG_IDATA09, pSession->m_ui8Country);
		ADD_SDATA(ZLOG_SDATA01, strNick);
		ADD_SDATA(ZLOG_SDATA02, strID);
		ADD_SDATA(ZLOG_SDATA03, pSession->GetIPStringA());
		ADD_SDATA(ZLOG_SDATA04, pSession->m_stHackCheckLogin.m_strDXMD5);

		if ( TRUE == _CheckZLogWrite(ZLOG_WRITE_TYPE_PCSPEC))
		{ 
			ADD_SDATA(ZLOG_SDATA05, strPCSpec);
		}
		 
		END_DATA();
		}
		break; 
	case LOG_MESSAGE_U_LOGOUT		:		//로그 아웃
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;

			INT32 i32ChannelNum = (INT32)*((INT32*)pArg0);
			INT32 i32RoomIdx	= (INT32)*((INT32*)pArg1);
			
			char strNick[MAX_NICK_NAME_LOG_SIZE];

			//@TODO 지로그 UNICODE 작업
			char strNickName[MAX_NICK_NAME_LOG_SIZE];
			i3String::CopyWCHARtoASCII( strNickName, MAX_NICK_NAME_LOG_SIZE, pSession->m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE );

			LogChangeString( strNick, strNickName, MAX_NICK_NAME_LOG_SIZE);
			char strID[ NET_ID_SIZE ];
			LogChangeString( strID, pSession->m_strId, NET_ID_SIZE );

			char strTRecord[SERVER_STRING_COUNT];
			char strSRecord[SERVER_STRING_COUNT];
			char strLoginTime[SERVER_STRING_COUNT];
			
			sprintf( strTRecord, "%d/%d/%d/%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw, pSession->m_UserInfoRecord._killcount, pSession->m_UserInfoRecord._death, pSession->m_UserInfoRecord._headshot);
			sprintf( strSRecord, "%d/%d/%d/%d/%d/%d/%d", pSession->m_UserInfoRecord._smatch, pSession->m_UserInfoRecord._swin, pSession->m_UserInfoRecord._slose, pSession->m_UserInfoRecord._sdraw, pSession->m_UserInfoRecord._skillcount, pSession->m_UserInfoRecord._sdeath, pSession->m_UserInfoRecord._sheadshot ); 
			pSession->m_dt32LoginDate.GetSmallDate( strLoginTime, SERVER_STRING_COUNT );

			UINT32 ui32TSTime = (UINT32)i3ThreadTimer::LocalTime().DiffTime( pSession->m_dt32LoginDate );

			UINT32 ui32Clan = 0;
			if (0 < pSession->m_UserInfoBasic.m_ui32Clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic.m_ui32ClanState)		ui32Clan = pSession->m_UserInfoBasic.m_ui32Clanidx;

			START_DATA( "1002", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Point );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_UserInfoBasic.m_ui32Cash );
			ADD_IDATA( ZLOG_IDATA04, pSession->m_UserInfoBasic.m_ui32Exp );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_UserInfoBasic.m_ui8PCCafe );
			ADD_IDATA( ZLOG_IDATA06, ui32Clan );
			ADD_IDATA( ZLOG_IDATA07, ui32TSTime );
			ADD_IDATA( ZLOG_IDATA08, pSession->m_UserInfoServer.m_ui32LoginBattleTime );
			ADD_IDATA( ZLOG_IDATA09, i32ChannelNum );
			ADD_IDATA( ZLOG_IDATA10, i32RoomIdx );
			ADD_IDATA( ZLOG_IDATA11, pSession->m_ui32KillReason );
			ADD_IDATA( ZLOG_IDATA12, pSession->m_ui8Age );
			ADD_IDATA( ZLOG_IDATA13, pSession->m_ui8Country );
			ADD_IDATA( ZLOG_IDATA14, pSession->GetAveragePing());
			ADD_IDATA( ZLOG_IDATA15, pSession->m_SessionIdx );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strID );
			ADD_SDATA( ZLOG_SDATA03, pSession->GetIPStringA() );
			ADD_SDATA( ZLOG_SDATA04, strTRecord );
			ADD_SDATA( ZLOG_SDATA05, strSRecord );
			ADD_SDATA( ZLOG_SDATA06, strLoginTime );
			//ADD_SDATA( ZLOG_SDATA07, pSession->m_strEndReason);
			END_DATA();
		}
		break;	
	case LOG_MESSAGE_U_CREATE_NICK		:	//캐릭터 생성 
		{
			if( NULL == pSession )	return;

			char strNick[ NET_NICK_NAME_SIZE ];

			//@TODO 지로그 UNICODE 작업
			char strNickName[ NET_NICK_NAME_SIZE ];
			i3String::CopyWCHARtoASCII( strNickName, NET_NICK_NAME_SIZE, pSession->m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE );

			LogChangeString( strNick, strNickName, NET_NICK_NAME_SIZE );

			START_DATA( "1003", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, 1 );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CHANGE_NICK		:	// 케릭터 이름 변경
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			char strNick[ NET_NICK_NAME_SIZE ];

			//@TODO 지로그 UNICODE 작업
			char strNickName[ NET_NICK_NAME_SIZE ];
			i3String::CopyWCHARtoASCII( strNickName, NET_NICK_NAME_SIZE, pSession->m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE );

			LogChangeString( strNick, strNickName, NET_NICK_NAME_SIZE );

			char strOldNick[ NET_NICK_NAME_SIZE ];
			LogChangeString( strOldNick, (char*)pArg0, NET_NICK_NAME_SIZE );

			START_DATA( "1003", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, 2 );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strOldNick );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_LEVEL_UP			:	//캐릭터 레벨 업
		{
			if( NULL == pSession )	return;

			START_DATA( "1004", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_ui32AccConnectCount );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_UserInfoServer.m_ui32TotalBattleTime );
			END_DATA();
		}
		break; 
	case LOG_MESSAGE_U_GET_POINT		:	// 포인트 획득
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			INT32 i32GetPoint  = (INT32)*((INT32*)pArg0);
			INT32 i32GetRoute	= (INT32)*((INT32*)pArg1);
			//INT32 i32EventType	= (INT32)*((INT32*)pArg2);

			START_DATA( "1101", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Point );
			ADD_IDATA( ZLOG_IDATA03, i32GetPoint );
			ADD_IDATA( ZLOG_IDATA04, i32GetRoute );
			//ADD_IDATA( ZLOG_IDATA05, i32EventType );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_GET_EXP			:	// 경험치 획득
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			INT32 i32GetExp  = (INT32)*((INT32*)pArg0);
			INT32 i32GetRoute	= (INT32)*((INT32*)pArg1);
			     

			START_DATA( "1105", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Exp );
			ADD_IDATA( ZLOG_IDATA03, i32GetExp );
			ADD_IDATA( ZLOG_IDATA04, i32GetRoute );
			END_DATA();
		}
		break;
		////////////////////////////////////////////////////////////////////////////////////////////////////////
	case LOG_MESSAGE_U_SHOP_BUY			:	//구입
		{			
			// 포인트 소진으로 통합 - 서동권
			// 수리  에서 기록합니다. 이부분은 구매에 관한 부분

			if( NULL == pSession )									return;
			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 )	return;
			UINT32	ui32Point	= 0;
			UINT32	ui32Cash	= 0;

			UINT32	ui32GameGoodsId = (UINT32)*((UINT32*)pArg0);
			UINT8	ui8BuyType		= (UINT8)*((UINT8*)pArg1);
			INT32	i32SpendType	= (INT32)*((INT32*)pArg2);			

			SHOP_GOODS_EXPANSION*		pGoods		= g_pContextMain->GetShop()->GetGoodsData( ui32GameGoodsId );
			if( NULL != pGoods )
			{
				ui32Point		= pGoods->GetPoint();
				ui32Cash		= pGoods->GetCash();
			}

			switch( ui8BuyType )
			{
			case GOODS_BUY_POINT :
				{
					START_DATA( "1102", 4 );
					ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
					ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
					ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Point );
					ADD_IDATA( ZLOG_IDATA03, ui32Point );
					ADD_IDATA( ZLOG_IDATA04, i32SpendType );
					ADD_IDATA( ZLOG_IDATA05, ui32GameGoodsId );
					ADD_IDATA( ZLOG_IDATA06, 1 );
					ADD_IDATA( ZLOG_IDATA07, pSession->m_ui8Country);
					END_DATA();
				}
				break; 


			case GOODS_BUY_CASH :
				{
					UINT32 ui32Clan = 0;
					if (0 < pSession->m_UserInfoBasic.m_ui32Clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic.m_ui32ClanState)		ui32Clan = pSession->m_UserInfoBasic.m_ui32Clanidx;

					START_DATA( "1103", 4 );
					ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
					ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
					ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Cash );
					ADD_IDATA( ZLOG_IDATA03, ui32Cash );
					ADD_IDATA( ZLOG_IDATA04, i32SpendType );
					ADD_IDATA( ZLOG_IDATA05, ui32GameGoodsId );
					ADD_IDATA( ZLOG_IDATA06, 1 );
					ADD_IDATA( ZLOG_IDATA07, pSession->m_UserInfoServer.m_ui32LastBuyCash );
					ADD_IDATA( ZLOG_IDATA08, ui32Clan );
					ADD_IDATA(ZLOG_IDATA09, pSession->m_ui8Country);
					END_DATA();
				}
				break;
			default:
				{
					g_pLog->WriteLogSession( pSession, L"ZLog Error LOG_MESSAGE_U_SHOP_BUY, %d", ui8BuyType );
					return;
				}
				break;
			}
		}
		break;
	case LOG_MESSAGE_U_REPAIR				: // 수리
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			REPAIR_INFO* stRepair = ( REPAIR_INFO* )pArg0;

			INT32 i32SpendType  = (INT32)SPEND_TYPE_REPAIR;

			if( stRepair->m_ui32Point > 0 )
			{
				START_DATA( "1102", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
				ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Point );
				ADD_IDATA( ZLOG_IDATA03, stRepair->m_ui32Point );
				ADD_IDATA( ZLOG_IDATA04, i32SpendType  );
				ADD_IDATA( ZLOG_IDATA05, stRepair->m_ui32ItemID );
				ADD_IDATA( ZLOG_IDATA06, stRepair->m_ui32Arg );
				ADD_IDATA(ZLOG_IDATA07, pSession->m_ui8Country);
				END_DATA();
			}
			else if( stRepair->m_ui32Cash > 0 )
			{
				UINT32 ui32Clan = 0;
				if (0 < pSession->m_UserInfoBasic.m_ui32Clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic.m_ui32ClanState)		ui32Clan = pSession->m_UserInfoBasic.m_ui32Clanidx;

				START_DATA( "1103", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
				ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Cash );
				ADD_IDATA( ZLOG_IDATA03, stRepair->m_ui32Cash );
				ADD_IDATA( ZLOG_IDATA04, i32SpendType  );
				ADD_IDATA( ZLOG_IDATA05, stRepair->m_ui32ItemID );
				ADD_IDATA( ZLOG_IDATA06, stRepair->m_ui32Arg );
				ADD_IDATA( ZLOG_IDATA07, pSession->m_UserInfoServer.m_ui32LastBuyCash );
				ADD_IDATA( ZLOG_IDATA08, ui32Clan );
				ADD_IDATA(ZLOG_IDATA09, pSession->m_ui8Country);
				END_DATA();
			}
			else
			{
				g_pLog->WriteLogSession( pSession, L"ZLog Error LOG_MESSAGE_U_REPAIR, %d, %d, %d", stRepair->m_ui32ItemID, stRepair->m_ui32Point, stRepair->m_ui32Cash );
				return;
			}
		}
		break;
	case LOG_MESSAGE_U_INVEN_USE		:	//사용
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			UINT32	ui32GameItemId	= (UINT32)*((UINT32*)pArg0);
			char	strExpireDate[ 16 ];
			char	strLogDItemName[MAX_ITEM_NAME];
			strExpireDate[0]	= '\0';
			strLogDItemName[0]	= '\0';

			
			START_DATA( "1201", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, ui32GameItemId );
			
			if( pArg1 )
			{	// pArg1 이 NULL 이 아니면 아이템인증입니다.
				INVEN_BUFFER* pInvenItem = (INVEN_BUFFER*)pArg1;
				ADD_IDATA( ZLOG_IDATA02, pInvenItem->_ui32ItemArg );
			}
			else if( pArg2 )
			{	// 이 경우 캡슐 아이템입니다.
				INT32 i32Capsule = (UINT8)*(UINT8*)(pArg2);
				ADD_IDATA( ZLOG_IDATA02, 0 );
				ADD_IDATA( ZLOG_IDATA03, i32Capsule );
			}
			END_DATA();
		}
		break; 
	case LOG_MESSAGE_U_GET_ITEM			:
		{
			if( NULL == pSession )	return;

			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 || NULL == pArg3) return;

			INVEN_BUFFER* pItem		= (INVEN_BUFFER*)pArg0;
			INT8	i8RequestType	= (INT8)*((INT8*)pArg1);
			UINT32	ui32InsertArg	= (UINT32)*((UINT32*)pArg2);
			UINT32	ui32CapsuleItemID = (UINT32)*((UINT32*)pArg3);

			START_DATA( "1202", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, i8RequestType );
			ADD_IDATA( ZLOG_IDATA03, pItem->_ui32ItemID );
			ADD_IDATA( ZLOG_IDATA04, ui32InsertArg );
			ADD_IDATA( ZLOG_IDATA05, pItem->_ui32ItemArg );
			ADD_IDATA( ZLOG_IDATA06, ui32CapsuleItemID);
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_ITEM_DELETE		:
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			UINT32 ui32ItemID = (UINT32)*((UINT32*)pArg0);
			START_DATA( "1203", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, ui32ItemID );
			END_DATA();
		}
		break;

	case LOG_MESSAGE_U_CREATE_CLAN		: // 클랜 생성
		{
			if( NULL == pSession )	return;

			if( NULL == pArg0)
			{
				INT32 i32SpendType = (INT32)SPEND_TYPE_CREATE_CLAN;

				START_DATA( "1102", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
				ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Point );
				ADD_IDATA( ZLOG_IDATA03, g_pContextMain->m_i32ClanCreatePoint );
				ADD_IDATA( ZLOG_IDATA04, i32SpendType );
				ADD_IDATA( ZLOG_IDATA07, pSession->m_ui8Country);
				END_DATA();
			}
			else
			{
				// Clan Create 시 Clan name 기록합니다.
				char strClanName[NET_CLAN_NAME_SIZE * sizeof(TTCHAR)];

				//@TODO 지로그 UNICODE 작업
				char strClan[NET_CLAN_NAME_SIZE * sizeof(TTCHAR)];
				i3String::CopyWCHARtoASCII(strClan, NET_CLAN_NAME_SIZE, pSession->m_UserInfoBasic.m_strClanName, NET_CLAN_NAME_SIZE);

				LogChangeString(strClanName, strClan, NET_CLAN_NAME_SIZE);

				START_DATA( "1301", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Clanidx );
				ADD_SDATA( ZLOG_SDATA01, strClanName);
				END_DATA();
			}
		}
		break;
	case LOG_MESSAGE_U_CLAN_DELETE		: // 클랜 해체
		{
			if( NULL == pSession )										return;
			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 )		return;

			UINT8	ui8ClanRank = (UINT8)*((UINT8*)pArg0);
			UINT32	ui32ClanExp = (UINT32)*((UINT32*)pArg1);
			INT32	i32ClanPoint = (INT32)*((REAL32*)pArg2);

			char strClanName[ NET_CLAN_NAME_SIZE * sizeof(TTCHAR) ];
			
			//@TODO 지로그 UNICODE 작업
			char strClan[ NET_CLAN_NAME_SIZE * sizeof(TTCHAR) ];
			i3String::CopyWCHARtoASCII( strClan, NET_CLAN_NAME_SIZE, pSession->m_UserInfoBasic.m_strClanName, NET_CLAN_NAME_SIZE );

			LogChangeString( strClanName, strClan, NET_CLAN_NAME_SIZE );

			START_DATA( "1302", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Clanidx );
			ADD_IDATA( ZLOG_IDATA02, ui8ClanRank );
			ADD_IDATA( ZLOG_IDATA03, ui32ClanExp );
			ADD_IDATA( ZLOG_IDATA04, i32ClanPoint );
			ADD_SDATA( ZLOG_SDATA01, strClanName );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_JOIN		: // 클랜 가입
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			T_UID i64ClanMemberUID = (T_UID)*((T_UID*)pArg0);

			START_DATA( "1303", 4 );
			ADD_LDATA( ZLOG_LDATA01, i64ClanMemberUID );
			ADD_LDATA( ZLOG_LDATA02, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Clanidx );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_SECESSION			: // 클랜 탈퇴
		{
			if( NULL == pSession )		return;

			START_DATA( "1304", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Clanidx );
			ADD_IDATA( ZLOG_IDATA02, 1 );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_DEPORTATION			: // 클랜 제명
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			T_UID i64MemberUID = (T_UID)*((T_UID*)pArg0);

			START_DATA( "1304", 4 );
			ADD_LDATA( ZLOG_LDATA01, i64MemberUID );
			ADD_LDATA( ZLOG_LDATA02, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Clanidx );
			ADD_IDATA( ZLOG_IDATA02, 2 );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_JOIN_REQ	: // 클랜 가입 요청
		{
			if( NULL == pSession )	return;

			START_DATA( "1305", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Clanidx );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_R_BATTLE_END_ROOM	: // 방 - 배틀 종료 
		{
			if( NULL == pArg0 )		return;
			CRoom* pRoom = (CRoom*)pArg0;

			INT32	i32TeamCount[ TEAM_COUNT ];
			INT32	i32MissionCount[ TEAM_COUNT ];
			
			i3mem::FillZero( i32TeamCount, sizeof(INT32)*TEAM_COUNT );
			i3mem::FillZero( i32MissionCount, sizeof(INT32)*TEAM_COUNT );

			TYPE_ROOM_INFO* pInfo = pRoom->m_pRoomMode->GetTypeRoomInfo();

			INT32 i32Team;
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if( SLOT_STATE_BATTLE != pInfo->_aSlotInfo[i]._State ) continue;

				i32Team = i % TEAM_COUNT;

				i32TeamCount[i32Team]		+= 1;
				i32MissionCount[i32Team]	+= pInfo->_aui8AccMissionCount[i];
			}

			char	strMission[SERVER_STRING_COUNT];
			i3String::Format( strMission, SERVER_STRING_COUNT, "RED:%d/BLU:%d", i32MissionCount[TEAM_RED], i32MissionCount[TEAM_BLUE] );
			
			char	strRecord[ SERVER_STRING_COUNT ];
			i3String::Format( strRecord, SERVER_STRING_COUNT, "RED:%d_%d/BLU:%d_%d", pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount, pInfo->_aTeamScoreTotal[TEAM_RED].m_DeathCount, 
							pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount, pInfo->_aTeamScoreTotal[TEAM_BLUE].m_DeathCount );

			char	strPlayerCount[ SERVER_STRING_COUNT ];
			i3String::Format( strPlayerCount, SERVER_STRING_COUNT, "RED:%d/BLU:%d", i32TeamCount[TEAM_RED], i32TeamCount[TEAM_BLUE] );
		
			UINT32 ui32PlayTime = pRoom->GetRoomBattleTime();
			
			UINT8 ui8OptionFlag = 0;
			if (GET_ROOM_INFO_RANDOM_MODE(pInfo->_InfoBasic._InfoFlag))
				ui8OptionFlag = 1;

			START_DATA( "1403", 4 );
			ADD_LDATA( ZLOG_LDATA01, pRoom->m_Info.m_ui64BattleUID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, StageID::GetStageUID( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA03, g_pContextMain->GetRuleIdx( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._InfoBasic._PersonPlayerMax );
			ADD_IDATA( ZLOG_IDATA05, pRoom->m_i32WinTeam );
			ADD_IDATA( ZLOG_IDATA06, pRoom->m_i8IsClanMatch );
			ADD_IDATA( ZLOG_IDATA07, pRoom->m_Info._ui32GameStartDate );
			ADD_IDATA( ZLOG_IDATA08, ui32PlayTime );			
			ADD_IDATA( ZLOG_IDATA09, pRoom->m_Info._ui8MiEndType );
			ADD_IDATA( ZLOG_IDATA10, pInfo->_InfoAdd._SubType);
			ADD_IDATA( ZLOG_IDATA11, ui8OptionFlag);
			ADD_SDATA( ZLOG_SDATA01, strMission );
			ADD_SDATA( ZLOG_SDATA02, strRecord );
			ADD_SDATA( ZLOG_SDATA03, strPlayerCount );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_MATCHING	: // 클랜 매칭
		{
			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 )		return;

			CRoom* pRoom					= (CRoom*)pArg0;
			CLAN_RESULT*	pClanResult		= (CLAN_RESULT*)pArg1;
			INT32	i32Matchmaking			= (INT32)*((INT32*)pArg2);

			INT32 i32Win;
			INT32 i32Los;
			INT32 i32Draw;

			if( pClanResult[0]._Win == 1 )
			{
				i32Win	= 0;
				i32Los	= 1;
				i32Draw = 1;
			}
			else if( pClanResult[1]._Win == 1 )
			{
				i32Win	= 1;
				i32Los	= 0;
				i32Draw	= 2;
			}
			else // 무승부일 경우
			{
				i32Win	= 0;
				i32Los	= 1;
				i32Draw = 0;
			}

			START_DATA( "1404", 4 );
			ADD_LDATA( ZLOG_LDATA01, pRoom->m_Info.m_ui64BattleUID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, StageID::GetStageUID( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA03, g_pContextMain->GetRuleIdx( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._InfoBasic._PersonPlayerMax );
			ADD_IDATA( ZLOG_IDATA05, pClanResult[ i32Win ]._ClanIdx );
			ADD_IDATA( ZLOG_IDATA06, pClanResult[ i32Los ]._ClanIdx );
			ADD_IDATA( ZLOG_IDATA07, i32Draw );
			ADD_IDATA( ZLOG_IDATA08, pClanResult[ i32Win ]._Exp );
			ADD_IDATA( ZLOG_IDATA09, pClanResult[ i32Los ]._Exp );
			ADD_IDATA( ZLOG_IDATA10, i32Matchmaking );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_R_BATTLE_START		: //배틀 시작 (각자)
		{
			if( NULL == pSession )			return;
			if( NULL == pArg0 )				return;
			CRoom* pRoom		= (CRoom*)pArg0;

			// 난입 유저 체크
			INT32 i32Intrusion	= 1;
			if( (pRoom->m_Info._ui32FirstBattleStartUser & (0x1 << pSession->m_i32SlotIdx)) )	
				i32Intrusion = 0;

			// 장착 호칭
			char	strUseTitle[SERVER_STRING_COUNT];
			sprintf( strUseTitle, "%d/%d/%d ", pSession->m_aui8EquipUserTitle[0], pSession->m_aui8EquipUserTitle[1], pSession->m_aui8EquipUserTitle[2] );

			// 방장유무
			INT32	i32MainUser;
			if( pSession->m_i32SlotIdx == pRoom->m_i32MainSlotIdx )	i32MainUser = 1; 
			else													i32MainUser = 0; 

			// 소속팀
			UINT8	ui8Teamidx = (pSession->m_i32SlotIdx % TEAM_COUNT); 

			// 슬롯 번호
			INT32 i32SlotIdx = pSession->m_MultiSlot.GetTeamSlot((TEAM_TYPE)ui8Teamidx);

			S2MO_CHAR_BASE_INFO MainCharInfo;
			pSession->m_MultiSlot.GetCharBaseInfo(i32SlotIdx, &MainCharInfo);
			
			ITEM_INFO aCharEquipment[ CHAR_EQUIPMENT_COUNT ];
			pSession->m_MultiSlot.GetCharEquipmentAll(i32SlotIdx, aCharEquipment);
			
			ITEM_INFO aCommonEquipment[ CHAR_EQUIPMENT_COMMON_COUNT ];
			pSession->m_MultiSlot.GetCommonEquipmentAll( aCommonEquipment); 

			S2MO_CHAR_SKILL_INFO SkillInfo;
			pSession->m_MultiSlot.GetCharSkillInfo(i32SlotIdx ,  &SkillInfo);

			
			START_DATA( "1506", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA02, pRoom->m_Info.m_ui64BattleUID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, StageID::GetStageUID( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA03, g_pContextMain->GetRuleIdx( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._InfoBasic._PersonPlayerMax );
			ADD_IDATA( ZLOG_IDATA05, ui8Teamidx );			
			ADD_IDATA( ZLOG_IDATA06, pRoom->m_i8IsClanMatch );
			ADD_IDATA( ZLOG_IDATA07, i32Intrusion );
			ADD_IDATA( ZLOG_IDATA08, i32MainUser );
			ADD_IDATA( ZLOG_IDATA09, SkillInfo.m_ui8Class );
			ADD_IDATA( ZLOG_IDATA10, aCharEquipment[ CHAR_EQUIPMENT_CHARA].m_TItemID );
			ADD_IDATA( ZLOG_IDATA11, aCharEquipment[ CHAR_EQUIPMENT_PRIMARY ].m_TItemID );
			ADD_IDATA( ZLOG_IDATA12, aCharEquipment[ CHAR_EQUIPMENT_SECONDARY ].m_TItemID );
			ADD_IDATA( ZLOG_IDATA13, aCharEquipment[ CHAR_EQUIPMENT_MELEE ].m_TItemID );
			ADD_IDATA( ZLOG_IDATA14, aCharEquipment[ CHAR_EQUIPMENT_THROWING1 ].m_TItemID );
			ADD_IDATA( ZLOG_IDATA15, aCharEquipment[ CHAR_EQUIPMENT_THROWING2 ].m_TItemID );
			ADD_IDATA( ZLOG_IDATA16, aCommonEquipment[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID );
			ADD_IDATA( ZLOG_IDATA17, pSession->m_UserInfoInvItemData._ui8Item );

			char strSData1[MAX_PATH], strSData2[MAX_PATH], strSData3[MAX_PATH], strSData4[MAX_PATH], strSData5[MAX_PATH];
			i3String::Format( strSData1, MAX_PATH, "HEAD:%d/FACE:%d/UPPER:%d,LOWER:%d", aCharEquipment[ CHAR_EQUIPMENT_HEAD ].m_TItemID, 
				aCharEquipment[ CHAR_EQUIPMENT_FACE ].m_TItemID, aCharEquipment[ CHAR_EQUIPMENT_UPPER ].m_TItemID, aCharEquipment[ CHAR_EQUIPMENT_LOWER ].m_TItemID );
			ADD_SDATA( ZLOG_SDATA01, strSData1 );
			i3String::Format( strSData2, MAX_PATH, "GLOVE:%d/BELT:%d/HOLSTER:%d,SKIN:%d", aCharEquipment[ CHAR_EQUIPMENT_GLOVE ].m_TItemID, 
				aCharEquipment[ CHAR_EQUIPMENT_BELT ].m_TItemID, aCharEquipment[ CHAR_EQUIPMENT_HOLSTER ].m_TItemID, aCharEquipment[ CHAR_EQUIPMENT_SKIN ].m_TItemID );
			ADD_SDATA( ZLOG_SDATA02, strSData2 ); 
			i3String::Format( strSData3, MAX_PATH, "MAIN SKILL:%d/LEVEL:%d", SkillInfo.m_ui8MainSkill, SkillInfo.m_ui8MainSkillLv );
			ADD_SDATA( ZLOG_SDATA03, strSData3 ); 
			i3String::Format( strSData4, MAX_PATH, "ASSIST SKILL:%d/LEVEL:%d", SkillInfo.m_ui8AssistSkill, SkillInfo.m_ui8AssistSkillLv );
			ADD_SDATA( ZLOG_SDATA04, strSData4 ); 
			i3String::Format( strSData5, MAX_PATH, "COMMON SKILL:%d/LEVEL:%d", SkillInfo.m_ui8CommonSkill, SkillInfo.m_ui8CommonSkillLv );
			ADD_SDATA( ZLOG_SDATA05, strSData5 );

			END_DATA();
		 }
		break; 

	case LOG_MESSAGE_U_R_BATTLE_END				: // 배틀 종료 (각자)
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			
			CRoom* pRoom		= (CRoom*)pArg0;
			INT32 i32EndType	= (INT32)*((INT32*)pArg1);
			INT32 i32UserIdx	= pSession->m_i32SlotIdx;

			// 난입 유저 체크
			INT32 i32Intrusion	= 1;
			if( (pRoom->m_Info._ui32FirstBattleStartUser & (0x1 << i32UserIdx)) )	i32Intrusion = 0;
		
			// 소속팀
			UINT8	ui8Teamidx = (i32UserIdx % TEAM_COUNT); 
			// 장착 호칭
			char	strUseTitle[SERVER_STRING_COUNT];
			i3String::Format( strUseTitle, SERVER_STRING_COUNT, "%d/%d/%d ",
				pSession->m_aui8EquipUserTitle[0],
				pSession->m_aui8EquipUserTitle[1],
				pSession->m_aui8EquipUserTitle[2]
			);
			// Total 전적
			char strRocord[SERVER_STRING_COUNT];
			i3String::Format(strRocord, SERVER_STRING_COUNT, "%d/%d/%d/%d/%d/%d/%d/%d",
				pSession->m_UserInfoRecord._match,
				pSession->m_UserInfoRecord._win,
				pSession->m_UserInfoRecord._lose,
				pSession->m_UserInfoRecord._draw,
				pSession->m_UserInfoRecord._killcount,
				pSession->m_UserInfoRecord._death,
				pSession->m_UserInfoRecord._headshot,
				pSession->m_UserInfoRecord._AssistCount
				);

			// 얻은 전적
			char strGetRocord[SERVER_STRING_COUNT];
			i3String::Format(strGetRocord, SERVER_STRING_COUNT, "%d/%d/%d/%d",
				pRoom->m_Info._aM_UserScore[i32UserIdx].m_KillCount,
				pRoom->m_Info._aM_UserScore[i32UserIdx].m_DeathCount,
				pRoom->m_Info._aui16M_Headshot[i32UserIdx],
				pRoom->m_Info._aM_UserScore[i32UserIdx].m_AssistCount
				);
			// Point 상세
			char strPoint[SERVER_STRING_COUNT];
			i3String::Format( strPoint, SERVER_STRING_COUNT, "%d/%d/%d/%d",
				pRoom->m_Info._aui16AccPoint[i32UserIdx],
				pRoom->m_aSlotBonus[i32UserIdx].ui16Point[ TYPE_BATTLE_RESULT_EVENT_ITEM ],
				pRoom->m_aSlotBonus[i32UserIdx].ui16Point[ TYPE_BATTLE_RESULT_EVENT_PCCAFE ],
				pRoom->m_aSlotBonus[i32UserIdx].ui16Point[ TYPE_BATTLE_RESULT_EVENT_EVENT ]
			);
			// Exp 상세
			char strExp[SERVER_STRING_COUNT];
			i3String::Format( strExp, SERVER_STRING_COUNT, "%d/%d/%d/%d",
				pRoom->m_Info._aui16AccExp[pSession->m_i32SlotIdx],
				pRoom->m_aSlotBonus[i32UserIdx].ui16Exp[ TYPE_BATTLE_RESULT_EVENT_ITEM ],
				pRoom->m_aSlotBonus[i32UserIdx].ui16Exp[ TYPE_BATTLE_RESULT_EVENT_PCCAFE ],
				pRoom->m_aSlotBonus[i32UserIdx].ui16Exp[ TYPE_BATTLE_RESULT_EVENT_EVENT ]
			);
			// play round
			char strRound[SERVER_STRING_COUNT];
			i3String::Format(strRound, SERVER_STRING_COUNT, "%d/%d",
				pRoom->m_Info.m_ui8PlayRoundCound[i32UserIdx],
				pRoom->m_pRoomMode->GetRoundCount());
			
			// 포인트 상세
			// 경험치 상세
			
			INT32 i32GetPoint	= pRoom->m_Info._aui16AccPoint[i32UserIdx] + pRoom->m_Info._aui16EventPoint[i32UserIdx];
			INT32 i32GetExp		= pRoom->m_Info._aui16AccExp[i32UserIdx] + pRoom->m_Info._aui16EventExp[i32UserIdx];

			START_DATA( "1507", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA02, pRoom->m_Info.m_ui64BattleUID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, StageID::GetStageUID( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA03, g_pContextMain->GetRuleIdx( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._InfoBasic._PersonPlayerMax );
			ADD_IDATA( ZLOG_IDATA05, ui8Teamidx );
			ADD_IDATA( ZLOG_IDATA06, pRoom->m_i8IsClanMatch );
			ADD_IDATA( ZLOG_IDATA07, i32Intrusion );
			ADD_IDATA( ZLOG_IDATA08, pSession->m_ui32PlayTime );
			ADD_IDATA( ZLOG_IDATA09, pRoom->m_Info._ui8MiEndType );
			ADD_IDATA( ZLOG_IDATA10, i32EndType );
			ADD_IDATA( ZLOG_IDATA11, pSession->m_ui32RoomBattleCount );
			ADD_IDATA( ZLOG_IDATA12, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA13, pSession->m_UserInfoBasic.m_ui32Point );
			ADD_IDATA( ZLOG_IDATA14, pSession->m_UserInfoBasic.m_ui32Exp );
			ADD_IDATA( ZLOG_IDATA15, i32GetPoint );
			ADD_IDATA( ZLOG_IDATA16, i32GetExp );
			ADD_SDATA( ZLOG_SDATA01, strUseTitle );
			ADD_SDATA( ZLOG_SDATA02, strRocord );
			ADD_SDATA( ZLOG_SDATA03, strGetRocord );
			ADD_SDATA( ZLOG_SDATA04, strPoint );
			ADD_SDATA( ZLOG_SDATA05, strExp );
			ADD_SDATA( ZLOG_SDATA06, strRound);
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_R_DEATH:
		{
			if( NULL == pSession )									return;
			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 )	return;
			CUserSession* pKillSession		= (CUserSession*)pArg0;
			CRoom* pRoom					= (CRoom*)pArg1;
			LOG_KILL_DEATH_INFO * pKillInfo	= (LOG_KILL_DEATH_INFO*)pArg2;

			// 소속팀
			UINT8	ui8KillTeamidx	= (pKillSession->m_i32SlotIdx % TEAM_COUNT)+1; 
			UINT8	ui8DeathTeamidx = (pSession->m_i32SlotIdx % TEAM_COUNT)+1; 
			// 진행 시간
			UINT32	ui32BattleTime	= pRoom->GetRoomBattleTime();

			char strKillXYZ[64];
			char strDeathXYZ[64];

			sprintf( strKillXYZ, "%d/%d/%d",	(INT32)pKillInfo->m_r32Kill_X,	(INT32)pKillInfo->m_r32Kill_Y,	(INT32)pKillInfo->m_r32Kill_Z );
			sprintf( strDeathXYZ, "%d/%d/%d",	(INT32)pKillInfo->m_r32Death_X,	(INT32)pKillInfo->m_r32Death_Y,	(INT32)pKillInfo->m_r32Death_Z );

			START_DATA( "1508", 4 );
			ADD_LDATA( ZLOG_LDATA01, pKillSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA02, pSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA03, pRoom->m_Info.m_ui64BattleUID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, StageID::GetStageUID( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA03, g_pContextMain->GetRuleIdx( pRoom->m_Info._InfoBasic._StageID ) );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._InfoBasic._PersonPlayerMax );
			ADD_IDATA( ZLOG_IDATA05, pKillInfo->m_ui32WeaponNum );
			ADD_IDATA( ZLOG_IDATA06, pKillSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA07, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA08, ui8KillTeamidx );
			ADD_IDATA( ZLOG_IDATA09, ui8DeathTeamidx );
			ADD_IDATA( ZLOG_IDATA10, ui32BattleTime );
			ADD_IDATA( ZLOG_IDATA11, pKillInfo->m_ui32DeathType );
			ADD_SDATA( ZLOG_SDATA01, strKillXYZ );
			ADD_SDATA( ZLOG_SDATA02, strDeathXYZ );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_R_RESPAWN				: //리스폰 
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			CRoom* pRoom = (CRoom*)pArg0;

			UINT8  ui8CharType;

			if( pRoom->m_Info._MiDinoInfo.IsDinoIdx( (UINT8)(pSession->m_i32SlotIdx) ) )
			{
				ui8CharType = 3;
			}
			else
			{
				ui8CharType = (pSession->m_i32SlotIdx % TEAM_COUNT)+1;
			}

			START_DATA( "1504", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA02, pRoom->m_Info.m_ui64BattleUID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_i32RespawnCount );
			ADD_IDATA( ZLOG_IDATA04, pSession->m_aTBattleEquipWeapon[CHAR_EQUIPMENT_WEAPON_PRIMARY].m_TItemID );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_aTBattleEquipWeapon[CHAR_EQUIPMENT_WEAPON_SECONDARY].m_TItemID );
			ADD_IDATA( ZLOG_IDATA06, pSession->m_aTBattleEquipWeapon[CHAR_EQUIPMENT_WEAPON_MELEE].m_TItemID );
			ADD_IDATA( ZLOG_IDATA07, pSession->m_aTBattleEquipWeapon[CHAR_EQUIPMENT_WEAPON_THROWING1].m_TItemID );
			ADD_IDATA( ZLOG_IDATA08, pSession->m_aTBattleEquipWeapon[CHAR_EQUIPMENT_WEAPON_THROWING2].m_TItemID );
			ADD_IDATA( ZLOG_IDATA09, ui8CharType );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_R_ESCAPE				: // 탈출
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;

			UINT8 ui8TouchDownCount			= (UINT8)*((UINT8*)pArg0);
			UINT8 ui8TouchDownChainCount	= (UINT8)*((UINT8*)pArg1);
			UINT64 ui64BattleUID			= (UINT64)*((UINT64*)pArg2);

			START_DATA( "1505", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA02, ui64BattleUID );
			ADD_IDATA( ZLOG_IDATA01, ui8TouchDownCount );
			ADD_IDATA( ZLOG_IDATA02, ui8TouchDownChainCount );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_TITLE_BUY		:	//호칭 구입
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			
			UINT8	ui8UserTitleId			= (UINT8)*((UINT8*)pArg0);
			LOG_GET_TITLE* pstLogGetTitle	= (LOG_GET_TITLE*)pArg1;
						
			char strMedal[SERVER_STRING_COUNT];
			sprintf( strMedal, "%d/%d/%d/%d", pSession->m_ui32Miniature, pSession->m_ui32Insignia, pSession->m_ui32Order, pSession->m_ui32Master );
			char strUse[SERVER_STRING_COUNT];
			sprintf( strUse, "%d/%d/%d/%d", pstLogGetTitle->_i32Miniature, pstLogGetTitle->_i32Insignia, pstLogGetTitle->_i32Order, pstLogGetTitle->_i32Master );

			START_DATA( "2001", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, ui8UserTitleId );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_UserInfoServer.m_ui32TotalBattleTime );
			ADD_SDATA( ZLOG_SDATA01, strMedal );
			ADD_SDATA( ZLOG_SDATA02, strUse );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_QUEST_FINISH		:	//임무카드 완료
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			LOG_MISSION* pstMission = (LOG_MISSION*)pArg0;

			char strMedal[SERVER_STRING_COUNT];
			sprintf( strMedal, "%d/%d/%d/%d", pSession->m_ui32Miniature, pSession->m_ui32Insignia, pSession->m_ui32Order, pSession->m_ui32Master );

			char strGetMedal[SERVER_STRING_COUNT];
			sprintf( strGetMedal, "%d/%d/%d/%d", pstMission->_i32Miniature, pstMission->_i32Insignia, pstMission->_i32Order, pstMission->_i32Master );
			
			char strGetItem[SERVER_STRING_COUNT];
			sprintf( strGetItem, "%d/%d/%d/%d/%d", pstMission->_ui32GetItem[0], pstMission->_ui32GetItem[1], pstMission->_ui32GetItem[2], pstMission->_ui32GetItem[3], pstMission->_ui32GetItem[4] );

			char strGetLastMedal[SERVER_STRING_COUNT];
			strGetLastMedal[0] = '\0';
			char strGetLastItem[SERVER_STRING_COUNT];
			strGetLastItem[0] = '\0';

			if( pstMission->_bFinish )
			{			
				sprintf( strGetLastMedal, "%d/%d/%d/%d", pstMission->_i32LastMiniature, pstMission->_i32LastInsignia, pstMission->_i32LastOrder, pstMission->_i32LastMaster );
				sprintf( strGetLastItem, "%d/%d/%d/%d/%d", pstMission->_ui32LastGetItem[0], pstMission->_ui32LastGetItem[1], pstMission->_ui32LastGetItem[2], pstMission->_ui32LastGetItem[3], pstMission->_ui32LastGetItem[4] );
			}

			START_DATA( "2002", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, pstMission->_ui8CardType );
			ADD_IDATA( ZLOG_IDATA03, pstMission->_ui8CardSet );
			ADD_IDATA( ZLOG_IDATA04, pstMission->_bFinish );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_UserInfoBasic.m_ui32Point );
			ADD_IDATA( ZLOG_IDATA06, pSession->m_UserInfoBasic.m_ui32Exp );
			ADD_IDATA( ZLOG_IDATA07, pstMission->_ui32Point );
			ADD_IDATA( ZLOG_IDATA08, pstMission->_ui32Exp );
			ADD_IDATA( ZLOG_IDATA09, pstMission->_ui32LastPoint );
			ADD_IDATA( ZLOG_IDATA10, pstMission->_ui32LastExp );
			ADD_SDATA( ZLOG_SDATA01, strMedal );
			ADD_SDATA( ZLOG_SDATA02, strGetMedal );
			ADD_SDATA( ZLOG_SDATA03, strGetItem );
			ADD_SDATA( ZLOG_SDATA04, strGetLastMedal );
			ADD_SDATA( ZLOG_SDATA05, strGetLastItem );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_QUEST_DELETE		:	//임무카드 삭제
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			UINT8	ui8CardType		= (UINT8)*((UINT8*)pArg0);
			UINT16*	ui16CardActive	= (UINT16*)pArg1;

			// 15/0/0/0/15/1/ <-- 이런식으로 표시 됩니다.
			char strMission[ SERVER_STRING_COUNT ];
			char strValue[ SERVER_STRING_COUNT ];
			_itoa( ui16CardActive[0], strValue, 10 );
			i3String::Copy( strMission, strValue, SERVER_STRING_COUNT );
			for( UINT8 i = 1 ; i < MAX_CARD_PER_CARDSET ; ++i )
			{
				_itoa( ui16CardActive[i], strValue, 10 );
				i3String::Concat( strMission, "/" );
				i3String::Concat( strMission, strValue );
			}
			
			START_DATA( "2003", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, ui8CardType );
			ADD_SDATA( ZLOG_SDATA01, strMission );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_PENALTY				: // 게임 도중 무단 이탈로 인한 패널티
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			UINT32 ui32Penalty = (UINT32)*((UINT32*)pArg0);
			INT32 i32SpendType = (INT32)SPEND_TYPE_PENALTY;

			START_DATA( "1102", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic.m_ui32Point );
			ADD_IDATA( ZLOG_IDATA03, ui32Penalty );
			ADD_IDATA( ZLOG_IDATA04, i32SpendType );
			ADD_IDATA(ZLOG_IDATA07, pSession->m_ui8Country);
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CHATTING:
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			if( NULL == pArg1 )		return;

			UINT32 ui32ChattingType = (UINT32)*((UINT32*)pArg0);

			char strNick[NET_NICK_NAME_SIZE];

			//@TODO 지로그 UNICODE 작업
			char strNickName[ NET_NICK_NAME_SIZE ];
			i3String::CopyWCHARtoASCII( strNickName, NET_NICK_NAME_SIZE, pSession->m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE );

			LogChangeString( strNick, strNickName, NET_NICK_NAME_SIZE );

			char strID[ NET_ID_SIZE ];
			LogChangeString( strID, pSession->m_strId, NET_ID_SIZE );

			char strMessgae[ MAX_STRING_COUNT ];
			LogChangeString( strMessgae, (char*)pArg1, MAX_STRING_COUNT-2 );

			START_DATA( "3001", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, ui32ChattingType );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strID );
			ADD_SDATA( ZLOG_SDATA03, pSession->GetIPStringA() );
			ADD_SDATA( ZLOG_SDATA04, strMessgae );
			END_DATA();
		}
		break;

	case LOG_MESSAGE_U_CHATTING_HACK:
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			if( NULL == pArg1 )		return;

			UINT32 ui32ChattingType = (UINT32)*((UINT32*)pArg0);

			char strNick[NET_NICK_NAME_SIZE];

			//@TODO 지로그 UNICODE 작업
			char strNickName[ NET_NICK_NAME_SIZE ];
			i3String::CopyWCHARtoASCII( strNickName, NET_NICK_NAME_SIZE, pSession->m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE );

			LogChangeString( strNick, strNickName, NET_NICK_NAME_SIZE );

			char strID[ NET_ID_SIZE ];
			LogChangeString( strID, pSession->m_strId, NET_ID_SIZE );

			char strMessgae[ MAX_STRING_COUNT ];
			LogChangeString( strMessgae, (char*)pArg1, MAX_STRING_COUNT-2 );

			START_DATA( "3002", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, ui32ChattingType );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strID );
			ADD_SDATA( ZLOG_SDATA03, pSession->GetIPStringA() );
			ADD_SDATA( ZLOG_SDATA04, strMessgae );
			END_DATA();
		}
		break;

	case LOG_MESSAGE_U_HACKING:
	{
		if (NULL == pSession)	return;
		if (NULL == pArg0)		return;
		if (NULL == pArg1)		return;
		if (NULL == pArg2)		return;

		HACK_TYPE_BASE *pHackBase = (HACK_TYPE_BASE*)pArg0;
		UINT64 *Ui64PIdx = (UINT64*)pArg1;
		LOG_USG *stLog = (LOG_USG*)pArg2;

		UINT16 ui16Type[MAX_HACK_TYPE] = { 0, };
		UINT8 ui8IsRealTime = 1;
		char strDXMD5[DX_DLL_MD5_SIZE];
		strDXMD5[0] = '\0';

		// Login시 검출되는 로그 일 경우
		if (pSession->m_i64UID == -1)
		{
			ui8IsRealTime = 0;

			HACK_CHECK_LOGIN * pLogin = static_cast<HACK_CHECK_LOGIN*>(pHackBase);
			i3String::Copy(strDXMD5, pLogin->m_strDXMD5, DX_DLL_MD5_SIZE);
		}

		for (INT32 i = 0; i < MAX_HACK_TYPE; i++)
		{
			ui16Type[i] = pHackBase->m_ui8Type[i];
		}
		char DXRealTime1[MAX_PATH];		char DXRealTime2[MAX_PATH];		char DXRealTime3[MAX_PATH];		char DXRealTime4[MAX_PATH];
		_ui64toa_s(Ui64PIdx[0], DXRealTime1, MAX_PATH, 10);
		_ui64toa_s(Ui64PIdx[1], DXRealTime2, MAX_PATH, 10);
		_ui64toa_s(Ui64PIdx[2], DXRealTime3, MAX_PATH, 10);
		_ui64toa_s(Ui64PIdx[3], DXRealTime4, MAX_PATH, 10);

		START_DATA("1801", 4);
		ADD_LDATA(ZLOG_LDATA01, pSession->m_i64UID);
		//ADD_LDATA(ZLOG_LDATA02, Ui64PIdx[0]);
		//ADD_LDATA(ZLOG_LDATA03, Ui64PIdx[1]);
		ADD_IDATA(ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank);
		ADD_IDATA(ZLOG_IDATA02, stLog->m_ui8Kick);
		ADD_IDATA(ZLOG_IDATA03, ui8IsRealTime);
		ADD_IDATA(ZLOG_IDATA04, ui16Type[0]);
		ADD_IDATA(ZLOG_IDATA05, ui16Type[1]);
		ADD_IDATA(ZLOG_IDATA06, ui16Type[2]);
		ADD_IDATA(ZLOG_IDATA07, ui16Type[3]);
		ADD_IDATA(ZLOG_IDATA08, ui16Type[4]);
		ADD_IDATA(ZLOG_IDATA09, ui16Type[5]);
		ADD_IDATA(ZLOG_IDATA10, ui16Type[6]);
		ADD_IDATA(ZLOG_IDATA11, ui16Type[7]);
		ADD_IDATA(ZLOG_IDATA12, pSession->m_ConIp);

		ADD_SDATA(ZLOG_SDATA01, DXRealTime1);
		ADD_SDATA(ZLOG_SDATA02, DXRealTime2);
		ADD_SDATA(ZLOG_SDATA03, DXRealTime3);
		ADD_SDATA(ZLOG_SDATA04, DXRealTime4);
		ADD_SDATA(ZLOG_SDATA05, strDXMD5);

		END_DATA();
	}
	break;
	
	case LOG_MESSAGE_U_SHOP_PRESENT		:	//선물하기
		//함수에 있음
		break; 
	
	case LOG_MESSAGE_U_FRIEND_ACCEPT	:	//친구 신청
		//함수에 있음
		break;
	case LOG_MESSAGE_U_FRIEND_INSERT	:	//친구 추가
		//함수에 있음
		break;
	case LOG_MESSAGE_U_FRIEND_DELETE	:	//친구 삭제 
		//함수에 있음
		break;
	case LOG_MESSAGE_U_MEDAL_LEVELUP				: // 메달 레벨 달성. 
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			LOG_MEDAL_LEVELUP_INFO* pInfo = (LOG_MEDAL_LEVELUP_INFO*)pArg0;

			START_DATA( "4001", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, (UINT8)pInfo->m_eMedalType );
			ADD_IDATA( ZLOG_IDATA03, (UINT8)pInfo->m_eMedalSetType );
			ADD_IDATA( ZLOG_IDATA04, pInfo->m_ui16MedalIdx );
			ADD_IDATA( ZLOG_IDATA05, pInfo->m_ui16MedalSetIdx );
			ADD_IDATA( ZLOG_IDATA06, (UINT8)pInfo->m_eLevel );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CHARA_CREATE				: // 캐릭터생성(구매)
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			LOG_CHARA_CREATE* pInfo = (LOG_CHARA_CREATE*)pArg0;
	 
			START_DATA( "1701", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, pInfo->m_ui8SlotIdx );
			ADD_IDATA( ZLOG_IDATA03, pInfo->m_ui32CharaItemID );
			ADD_IDATA( ZLOG_IDATA04, pInfo->m_ui8CreateType );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_UserInfoBasic.m_ui32Point );
			ADD_IDATA( ZLOG_IDATA06, pInfo->m_ui32SpendPoint );
			ADD_IDATA( ZLOG_IDATA07, pSession->m_UserInfoBasic.m_ui32Cash );
			ADD_IDATA( ZLOG_IDATA08, pInfo->m_ui32SpendCash );
			END_DATA();
		}
		break;

	case LOG_MESSAGE_U_CHARA_DELETE				: // 캐릭터삭제
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			UINT8 ui8SlotIdx= (UINT8)*((UINT8*)pArg0);

			S2MO_CHAR_BASE_INFO CharaInfo;
			pSession->m_MultiSlot.GetCharBaseInfo( ui8SlotIdx, &CharaInfo);

			ITEM_INFO	CharItemInfo;
			pSession->m_MultiSlot.GetCharEquipment( ui8SlotIdx, CHAR_EQUIPMENT_CHARA, &CharItemInfo);
	 
			START_DATA( "1702", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, ui8SlotIdx );
			ADD_IDATA( ZLOG_IDATA03, CharItemInfo.m_TItemID );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLASS_SETTING				: // 병과 선택.
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			if( NULL == pArg1 )		return;
			
			UINT8 ui8SlotIdx= (UINT8)*((UINT8*)pArg0);
			UINT8 ui8Class= (UINT8)*((UINT8*)pArg1);
			
			START_DATA( "1703", 4 );
			
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, ui8SlotIdx );
			ADD_IDATA( ZLOG_IDATA03, ui8Class ); 
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_USED_COUPON					: // 쿠폰 사용
		{
			if( NULL == pSession )	return;

			char strCouponCode[GIFTCOUPON_LENGTH + 1];
			i3String::ToLower( strCouponCode, GIFTCOUPON_LENGTH, (char*)pArg0 );
			strCouponCode[GIFTCOUPON_LENGTH] = '\0';
			INT32* pi32GoodsID = (INT32*)pArg1;

			START_DATA( "6001", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );	// UID
			ADD_IDATA( ZLOG_IDATA01, pi32GoodsID[0] );		// GoodsID 1
			ADD_IDATA( ZLOG_IDATA02, pi32GoodsID[1] );		// GoodsID 2
			ADD_IDATA( ZLOG_IDATA03, pi32GoodsID[2] );		// GoodsID 3
			ADD_IDATA( ZLOG_IDATA04, pi32GoodsID[3] );		// GoodsID 4
			ADD_SDATA( ZLOG_SDATA01, strCouponCode );		// Coupon Number
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_GACHA_SHOP				: // 뽑기상점 이용
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			if( NULL == pArg1 )		return;
			if( NULL == pArg2 )		return;

			UINT32 ui32UserMoney = (UINT32)*((UINT32*)pArg0);
			PACKET_GACHA_SS_RESULT_HEADER* pResult	= ( PACKET_GACHA_SS_RESULT_HEADER* )pArg1;
			PACKET_GACHA_RESULT_DATA* pData			= ( PACKET_GACHA_RESULT_DATA* )pArg2;

			char strTotal[ 100 ] = {0,}  ;
			char strResult[ 2 ];

			for( INT32 i = 0; i < pResult->m_i32ResultCount; ++i ) 
			{
				_itoa( pData[i].m_eGachaResult, strResult, 10 ); 
				i3String::Concat( strTotal,strResult );
				i3String::Concat( strTotal,"/" );
			}

			START_DATA( "1104", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic.m_ui32Rank );
			ADD_IDATA( ZLOG_IDATA02, ui32UserMoney );
			ADD_IDATA( ZLOG_IDATA03, pResult->m_ui32Price );
			ADD_IDATA( ZLOG_IDATA04, pResult->m_ui8BuyType  );
			ADD_IDATA( ZLOG_IDATA05, pResult->m_TGoodsID );
			ADD_IDATA( ZLOG_IDATA06, pResult->m_i32ResultCount );
			ADD_SDATA( ZLOG_SDATA01, strTotal );
			END_DATA(); 
		}
		break;
	default: 
		{
			i16LogSize = 0;
		}
		break;
	}

	if( i16LogSize == 0 )
	{
		g_pLog->WriteLogSession( pSession, L"LogSendMessage :: Unknown Log Type - %d", LogId );
		return;
	}

	if( i16LogSize > LOG_MESSAGE_MAX )
	{
		i16LogSize = LOG_MESSAGE_MAX;
	}

	//Set Size
	pSendBuffer->_Size = i16LogSize;

	pRing->PushPointerIdx();

	return; 
}

void CModuleContextNC::LogMedalLevelUp( INT32 i32ThreadIdx, CUserSession * pSession , LOG_MEDAL_LEVELUP_INFO *pLogInfo )
{
	MEDAL_LEVEL eMedalLevel = MEDAL_LEVEL_NONE;
	MQF_MEDAL_INFO* pMedalInfo;
	switch( pLogInfo->m_eMedalType )
	{
	case MEDAL_TYPE_NORMAL:
		{
			pMedalInfo = g_pContextMain->GetMedal_MQFData(pLogInfo->m_ui16MedalIdx, pLogInfo->m_eMedalType );
			if( NULL == pMedalInfo ) return;	// 버그.

			for( UINT8 i = 0; i < MEDAL_LEVEL_MAX ; i++ )
			{
				if( pMedalInfo->m_ui16Count[i] == pLogInfo->m_ui16CompareCount ) 
				{
					eMedalLevel = (MEDAL_LEVEL)(i+1);
					break;
				}
			}
		}
		break;
	case MEDAL_TYPE_SET:
		{
			pMedalInfo = g_pContextMain->GetMedal_MQFData(pLogInfo->m_ui16MedalIdx, pLogInfo->m_eMedalType );
			if( NULL == pMedalInfo ) return;	// 버그.

			if( pMedalInfo->m_ui16Count[MEDAL_LEVEL_MAX_SET-1]  == pLogInfo->m_ui16CompareCount ) 
			{
				eMedalLevel = MEDAL_LEVEL_MAX_SET;
			}

		}
		break;
	}

	if( MEDAL_LEVEL_NONE < eMedalLevel )
	{
		pLogInfo->m_eLevel = eMedalLevel;
		LogSendMessage( i32ThreadIdx, LOG_MESSAGE_U_MEDAL_LEVELUP, pSession, pLogInfo );
	}

	return;
}

INT32 CModuleContextNC::_GetFileDate( char* strHour )
{	
	INT32 i32Hour = 0;
	for( INT32 i = 0 ; i < 10 ; i++ )
	{
		i32Hour = i32Hour * 10 + (strHour[i]-'0');
	}
	return i32Hour;
}