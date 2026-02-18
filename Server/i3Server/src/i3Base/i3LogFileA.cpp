#include "i3CommonType.h"
#include "i3LogFileA.h"

#include "i3String.h"

I3_CLASS_INSTANCE( i3LogFileA, i3LogFileBase );

typedef struct _tagLogFileBuffer
{		
	TCHAR	_strMessage[ LOG_STRING_MAX ];
}LOG_FILE_BUFFER;

i3LogFileA::i3LogFileA()
{
	m_strFileName[0]	= '\0'; 
	m_strStartText[0]	= '\0';
}

i3LogFileA::~i3LogFileA(void)
{
	Destroy(); 
}

BOOL i3LogFileA::Create( TCHAR* wstrPathName, TCHAR* wstrFileName, INT32 iSaveType, INT32 iLogBufferCount, INT32 iThreadPri )
{
	//Set init Value
	i3mem::FillZero( m_ppLogBuffer, sizeof(m_ppLogBuffer) );

	i3String::Format( m_strFileName, LOG_STRING_MAX, "%s\\%s", wstrPathName, wstrFileName ); 
		
	_UpdateTime();

	i3String::Format( m_strStartText, LOG_STRING_MAX, "-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, StartDate:%04d-%02d-%02d %02d:%02d --\r\n", 
		m_ui16Ver01, m_ui16Ver02, m_ui32Ver03, m_ui32Ver04, __DATE__, __TIME__, m_iYear, m_iMonth, m_iDay, m_iHour, m_iMinute );

	//Create File 
	if( false == _CreateFile() )								return FALSE; 

	return i3LogFileBase::_Create( iSaveType, iLogBufferCount, iThreadPri ); 
}

void i3LogFileA::Destroy(void)
{
}

void i3LogFileA::_WriteLogFile( char * wstrLog )
{
	INT32 i32Length = (INT32)i3String::Length( wstrLog ); 
	::WriteFile( m_hFile, wstrLog, i32Length, (LPDWORD)&i32Length, NULL ); 
	I3TRACE( wstrLog );
}

INT32 i3LogFileA::AddLogThread( TCHAR* strPreString, INT32 iType, DWORD dwThreadID )
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

BOOL i3LogFileA::_Working()
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

void i3LogFileA::WriteLog( TCHAR * strLogString, ... )
{
	char strTemp[ LOG_STRING_MAX ];

	va_list marker;
	va_start( marker, strLogString );
	_vsnprintf( strTemp, LOG_STRING_MAX, strLogString, marker );
	va_end( marker);

	_WriteLog( strTemp );
}

void i3LogFileA::WriteLog( INT32 i32WarningType, INT32 i32WarningLevel, TCHAR * strLogString, ... )
{
	m_Warning.SetWarningInfo(i32WarningType,i32WarningLevel);

	char strTemp[ LOG_STRING_MAX ];

	va_list marker;
	va_start( marker, strLogString );
	_vsnprintf( strTemp, LOG_STRING_MAX, strLogString, marker );
	va_end( marker);

	_WriteLog( strTemp );
}

void i3LogFileA::_WriteLog( char* strLog )
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
		i3String::Format( pLogBuffer->_strMessage, LOG_STRING_MAX, "[%02d:%02d:%02d][%s]%s\r\n", m_iHour, m_iMinute, m_iSecond, m_astrLogInit[i32WorkIdx], strLog );
		pRingBuffer->PushPointerIdx();
	}
}

BOOL i3LogFileA::_CreateFile(void)
{
	BOOL Rv = TRUE; 
	TCHAR wstrFileName[ MAX_PATH ]; 
	i3String::Format( wstrFileName, MAX_PATH, "%s_%02d_%02d_%02d_%02d.txt", m_strFileName, m_iYear, m_iMonth, m_iDay, m_iHour);

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	// Create New File
	m_hFile = ::CreateFile( wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

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
				
		_WriteLogFile( m_strStartText ); 
	}

	return Rv;
}
