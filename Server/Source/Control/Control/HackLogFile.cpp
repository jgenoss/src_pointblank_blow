#include "pch.h"
#include "HackLogFile.h"
#include "UserSession.h"

I3_CLASS_INSTANCE(CHackLogFile, i3Thread);

CHackLogFile * g_pHackLogFile = NULL;
#define LOG_BUFFER_COUNT	500

CHackLogFile::CHackLogFile(void)
{
	m_hFile				= INVALID_HANDLE_VALUE;
	m_i32Hour			= 0;
	m_pMainBuffer		= NULL;
	m_ui32WorkingCounter= 0;
	m_bIsRunning		= FALSE;
}

CHackLogFile::~CHackLogFile(void)
{
	OnDestroy(); 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CHackLogFile::OnCreate()
{
	m_ui32WorkingCounter = 0;

	//Main
	m_pMainBuffer = new Local_CRingBuffer;
	if(m_pMainBuffer == NULL) return FALSE;
	if(!m_pMainBuffer->OnCreate( sizeof(HACK_LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddRingBuffer( m_pMainBuffer );

	//Create File
	if(!FileOpen())return FALSE; 

	//Create Thread
	if( !i3Thread::Create("CHackLogFile", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE;


	return TRUE;
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CHackLogFile::OnDestroy(void)
{
	m_bIsRunning = FALSE;
	i3Thread::WaitForFinish();

	if( m_pMainBuffer!= NULL) delete m_pMainBuffer;

	return;
}

UINT32 CHackLogFile::OnRunning( void * pUserData )
{
	BOOL bWorking;

	m_bIsRunning = TRUE;
	while( m_bIsRunning )
	{
		bWorking = FALSE;

		if(m_i32Hour != i3ThreadTimer::LocalTime().GetHour() )FileOpen();

		HACK_LOG_FILE_MESSAGE * pLogFileMessage; 

		for(UINT32 i = 0; i < m_ui32WorkingCounter; i++)
		{
			if( m_pWorkingBuffer[i]->GetBufferCount() > 0)
			{
				pLogFileMessage = (HACK_LOG_FILE_MESSAGE *)m_pWorkingBuffer[i]->Pop();
				::WriteFile( m_hFile, pLogFileMessage->strLog , pLogFileMessage->i32Length, (LPDWORD)&pLogFileMessage->i32Length, NULL ); 
				m_pWorkingBuffer[i]->PopIdx();

				bWorking = TRUE;
			}
		}

		if( !bWorking )
		{
			::WaitForSingleObject( m_Handle, 1 );
		}
	}

	return 0; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CHackLogFile::FileOpen(void)
{	
	BOOL bRv = TRUE;

	char strFileName[256];

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}

	//Create New File
	UINT32 ui32Date = (i3ThreadTimer::LocalTime().GetYear() * 1000000) + ( i3ThreadTimer::LocalTime().GetMonth() * 10000) + (i3ThreadTimer::LocalTime().GetDay() * 100) + i3ThreadTimer::LocalTime().GetHour();
	i3String::Format(strFileName, 256, "%s\\%s_%d.txt", g_pControl->m_strHackLogFilePath, "Hack", ui32Date );
#ifdef UNICODE
	WCHAR16 wchar[256]; 
	i3String::CopyASCIItoWCHAR(wchar, strFileName);
	m_hFile = ::CreateFile(wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hFile = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
		bRv = FALSE;
	}
	else
	{
		SetFilePointer( m_hFile, 0, NULL, FILE_END);

		INT32 i32Len;
		char strMessage[MAX_STRING_COUNT];
		char strTemp[64];

		/////////////////////////////////////////////////////////////////////////////////////
		//Day
		{
			i3String::Copy( strMessage, "Hack List Day", MAX_STRING_COUNT);
			for(INT32 i= 1; i < HACK_TYPE_MAX; i++)
			{
				i3String::Format( strTemp, 64, "[%d:%d]", i, g_pControl->m_pui32DayHackCount[i] );
				i3String::Concat(strMessage, strTemp);
			}

			if(m_i32Day != i3ThreadTimer::LocalTime().GetDay())
			{
				i3mem::FillZero( g_pControl->m_pui32DayHackCount, (sizeof(UINT32) * HACK_TYPE_MAX));
				m_i32Day = i3ThreadTimer::LocalTime().GetDay();
			}

			i3String::Concat(strMessage, "\r\n");
			i32Len = i3String::Length( strMessage );
			::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
		}

		/////////////////////////////////////////////////////////////////////////////////////
		//Hour 
		{
			i3String::Copy( strMessage, "Hack List Hour", MAX_STRING_COUNT);
			for(INT32 i= 1; i < HACK_TYPE_MAX; i++)
			{
				i3String::Format( strTemp, 64, "[%d:%d]", i, g_pControl->m_pui32HourHackCount[i] );
				i3String::Concat(strMessage, strTemp);
			}

			i3String::Concat(strMessage, "\r\n");
			i3mem::FillZero( g_pControl->m_pui32HourHackCount, (sizeof(UINT32) * HACK_TYPE_MAX));
			
			i32Len = i3String::Length( strMessage );
			::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
		}
	}

	m_i32Hour = i3ThreadTimer::LocalTime().GetHour();

	return bRv;
}

void CHackLogFile::WriteLog( INT64 i64Uid, UINT32 ui32Type)
{
	HACK_LOG_FILE_MESSAGE * pLogFileMessage = NULL;
	Local_CRingBuffer * pRingBuffer;

	pRingBuffer = m_pMainBuffer;

	pLogFileMessage = (HACK_LOG_FILE_MESSAGE *)pRingBuffer->PushPointer();
	if( NULL == pLogFileMessage )	return;

	if( ui32Type < HACK_TYPE_MAX)
	{
		g_pControl->m_pui32DayHackCount[ui32Type]++;
		g_pControl->m_pui32HourHackCount[ui32Type]++;
		i3String::Format( pLogFileMessage->strLog, HACK_STRING_SIZE, "[%02d:%02d:%02d]%I64d %d\r\n", i3ThreadTimer::LocalTime().GetHour(), i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond(), i64Uid, ui32Type);
		pLogFileMessage->i32Length = i3String::Length(pLogFileMessage->strLog);
	}
	else 
	{
		i3String::Format( pLogFileMessage->strLog, HACK_STRING_SIZE, "[%02d:%02d:%02d]%I64d %d Unknown \r\n", i3ThreadTimer::LocalTime().GetHour(), i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond(), i64Uid, ui32Type);
		pLogFileMessage->i32Length = i3String::Length(pLogFileMessage->strLog);
	}

	pRingBuffer->PushPointerIdx();

	return;
}
