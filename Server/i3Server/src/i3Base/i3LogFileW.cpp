#include "i3CommonType.h"
#include "i3LogFileW.h"

#include "i3String.h"

I3_CLASS_INSTANCE( i3LogFileW, i3LogFileBase);

struct LOG_FILE_BUFFER
{		
	wchar_t	_strMessage[ LOG_STRING_MAX ];
};

i3LogFileW::i3LogFileW()
{
	m_wstrFileName[0]	= '\0'; 
	m_wstrStartText[0]	= '\0';
}

i3LogFileW::~i3LogFileW(void)
{
	Destroy(); 
}

BOOL i3LogFileW::Create( wchar_t* wstrPathName, wchar_t* wstrFileName, INT32 iSaveType, INT32 iLogBufferCount, INT32 iThreadPri )
{
	//Set init Value
	i3mem::FillZero( m_ppLogBuffer, sizeof(m_ppLogBuffer) );
		
	i3String::Format( m_wstrFileName, LOG_STRING_MAX, L"%s\\%s", wstrPathName, wstrFileName ); 
	
	_UpdateTime();

	wchar_t wstrData[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, __DATE__, -1, wstrData, MAX_PATH);

	wchar_t wstrTime[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, __TIME__, -1, wstrTime, MAX_PATH);
			
	i3String::Format( m_wstrStartText, LOG_STRING_MAX, L"-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, StartDate:%04d-%02d-%02d %02d:%02d --\r\n", 
		m_ui16Ver01, m_ui16Ver02, m_ui32Ver03, m_ui32Ver04, wstrData, wstrTime, m_iYear, m_iMonth, m_iDay, m_iHour, m_iMinute );

	//Create File 
	if( false == _CreateFile() )								return FALSE; 

	return i3LogFileBase::_Create( iSaveType, iLogBufferCount, iThreadPri );
}

void i3LogFileW::Destroy(void)
{
}

void i3LogFileW::_WriteLogFile( wchar_t * wstrLog )
{
	INT32 i32Length = (INT32)(sizeof(wchar_t)*i3String::Length( wstrLog ));
	::WriteFile( m_hFile, wstrLog, i32Length, (LPDWORD)&i32Length, NULL ); 
	I3TRACE( wstrLog );
}

INT32 i3LogFileW::AddLogThread( wchar_t* strPreString, INT32 iType, DWORD dwThreadID )
{	iType;

	INT32 i32Rv = RV_ADD_FAIL; 

	m_AddCs.Enter(); 
	{
		#ifdef USE_THREAD_ID
		i32Rv = _FindWorkIdx( dwThreadID );

		if( RV_ADD_FAIL != i32Rv )
		{
			m_AddCs.Leave();
			return i32Rv;
		}
		#endif

		if( LOG_BUFFER_MAX_COUNTER <= m_ui32WorkCounter )
		{	// 더이상 로그 파일을 생성 할 수 없습니다.
			m_AddCs.Leave();
			return RV_ADD_FAIL;
		}

		m_ppLogBuffer[ m_ui32WorkCounter  ] = i3RingBuffer::NewObject();
		if( NULL == m_ppLogBuffer[ m_ui32WorkCounter ] )
		{	// 메모리 할당에 실패 했습니다.
			m_AddCs.Leave();
			return RV_ADD_FAIL;
		}

		if( false == m_ppLogBuffer[ m_ui32WorkCounter ]->Create( sizeof(LOG_FILE_BUFFER), m_iBufferCount ) )
		{	// Ring Buffer 메모리 할당 실패
			delete m_ppLogBuffer[ m_ui32WorkCounter  ];
			m_AddCs.Leave();
			return RV_ADD_FAIL;
		}
		
		i3String::Copy( m_astrLogInit[ m_ui32WorkCounter ], strPreString, LOG_INIT_STRING );
		i32Rv = m_ui32WorkCounter;

		m_adwThreadID[m_ui32WorkCounter] = dwThreadID;

		m_ui32WorkCounter++;
	}
	m_AddCs.Leave(); 

	return i32Rv; 
}

BOOL i3LogFileW::_Working()
{
	INT32 iLogCount; 
	LOG_FILE_BUFFER * pLogBuffer;	
	BOOL bWorking = FALSE; 

	//Write Log 
	for(UINT32 j = 0; j < m_ui32WorkCounter; j++)
	{
		iLogCount = m_ppLogBuffer[j]->GetBufferCount(); 
		for(INT32 i = 0; i < iLogCount; i++)
		{
			pLogBuffer = (LOG_FILE_BUFFER *) m_ppLogBuffer[j]->Pop();
			_WriteLogFile( pLogBuffer->_strMessage ); 
			m_ppLogBuffer[j]->PopIdx();
			bWorking = TRUE; 
		}
	}
	return bWorking;
}

void i3LogFileW::WriteLog( wchar_t * wstrLogString, ... )
{
	wchar_t wstrTemp[ LOG_STRING_MAX ];

	va_list marker;
	va_start( marker, wstrLogString );
	_vsnwprintf( wstrTemp, LOG_STRING_MAX, wstrLogString, marker );
	va_end( marker);

	_WriteLog( wstrTemp );
}

void i3LogFileW::WriteLog( INT32 i32WarningType, INT32 i32WarningLevel, wchar_t * wstrLogString, ... )
{
	m_Warning.SetWarningInfo(i32WarningType,i32WarningLevel);

	wchar_t wstrTemp[ LOG_STRING_MAX ];

	va_list marker;
	va_start( marker, wstrLogString );
	_vsnwprintf( wstrTemp, LOG_STRING_MAX, wstrLogString, marker );
	va_end( marker);

	_WriteLog( wstrTemp );
}

void i3LogFileW::_WriteLog( wchar_t* wstrLog )
{
#ifdef USE_THREAD_ID
	INT32 i32WorkIdx = _FindWorkIdx();

	if( RV_ADD_FAIL == i32WorkIdx )
	{	
		#ifdef I3_DEBUG
		I3FATAL("Invalid Thread ID : %d\n", GetCurrentThreadId());
		#endif
		return;
	}
#endif

	//Make PreString
	i3RingBuffer*		pRingBuffer = m_ppLogBuffer[i32WorkIdx]; 
	LOG_FILE_BUFFER*	pLogBuffer	= (LOG_FILE_BUFFER*)pRingBuffer->PushPointer();

	if( pLogBuffer != NULL)
	{
		i3String::Format( pLogBuffer->_strMessage, LOG_STRING_MAX, L"[%02d:%02d:%02d][%s]%s\r\n", m_iHour, m_iMinute, m_iSecond, m_astrLogInit[ i32WorkIdx ], wstrLog );
		pRingBuffer->PushPointerIdx();
	}
}

BOOL i3LogFileW::_CreateFile(void)
{
	BOOL Rv = TRUE; 
	wchar_t wstrFileName[ MAX_PATH ]; 
	i3String::Format( wstrFileName, MAX_PATH, L"%s_%02d_%02d_%02d_%02d.txt", m_wstrFileName, m_iYear, m_iMonth, m_iDay, m_iHour);

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	// Create New File
	m_hFile = ::CreateFileW( wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if(m_hFile == INVALID_HANDLE_VALUE)
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		Rv = FALSE; 
	}
	else
	{
		SetFilePointer( m_hFile, 0, NULL, FILE_END);

		m_ui32FileHour	= m_iHour;
		m_ui32FileDay	= m_iDay;

		_WriteLogFile( m_wstrStartText ); 
	}

	return Rv;
}
