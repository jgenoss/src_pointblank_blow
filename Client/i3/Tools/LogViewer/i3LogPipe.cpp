#include "i3CommonType.h"
#include "i3LogPipe.h"

#include "i3String.h"

I3_CLASS_INSTANCE(i3LogPipe, i3ElementBase );

static UINT32 _DefThreadProc( void * pPointer )
{
	((i3LogPipe*)pPointer)->Working(); 
	return 0;
}

volatile long m_fast_critical_section;





i3LogPipe::i3LogPipe()
{
	m_hThread			= NULL;
	m_bRunning			= FALSE;


	m_hPipe				= NULL;
	InitializeCriticalSection();
}

i3LogPipe::~i3LogPipe(void)
{
	Destroy(); 
}

void i3LogPipe::InitializeCriticalSection(void)
{
	m_fast_critical_section = 0;
}

void i3LogPipe::DeleteCriticalSection(void)
{
	m_fast_critical_section = 0;
}

// our own LOCK function
void i3LogPipe::EnterCriticalSection(void)
{
	while (InterlockedCompareExchange(&m_fast_critical_section, 1, 0) != 0) Sleep(0);
}

// our own UNLOCK function
void i3LogPipe::LeaveCriticalSection()
{
	m_fast_critical_section = 0;
}

void i3LogPipe::Destroy(void)
{
	m_bRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );
	CloseHandle(m_hPipe);
	DeleteCriticalSection();
}


void i3LogPipe::Working(void)
{
	BOOL bWorking; 

	m_bRunning = TRUE;

	while( m_bRunning )
	{
		bWorking = FALSE; 
		Connect();
	
		if( FALSE == bWorking )	Sleep(1); 	 
	}

	return; 
}


BOOL i3LogPipe::CreatePipe(wchar_t* wstrFileName, INT32 iThreadPri)
{
	i3String::Format(m_wstrFileName, 128, L"\\\\.\\pipe\\%s", wstrFileName);

	UINT32	ui32ThreadID;
	m_hThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc, this, 0, (DWORD*)&ui32ThreadID);
	if (NULL == m_hThread)										return FALSE;
	if (FALSE == SetThreadPriority(m_hThread, iThreadPri))	return FALSE;
}

void i3LogPipe::Connect()
{
	if (m_hPipe == NULL)
	{
		m_hPipe = CreateFileW(
			m_wstrFileName,   // pipe name
			GENERIC_WRITE/*|GENERIC_WRITE*/ /* GENERIC_WRITE*/, // read and write access
			0,              // no sharing
			NULL,           // no security attributes
			OPEN_EXISTING,  // opens existing pipe
			0,              // default attributes
			NULL);          // no template file

							// Break if the pipe handle is valid.

		Sleep(5);
	}
}

void i3LogPipe::WriteLog(wchar_t * wstrLogString)
{
	INT32 iSize = (INT32)(sizeof(wchar_t)*i3String::Length(wstrLogString));

	DWORD dwWritten = (DWORD)-1;
	// we assume that in iSize < 2^24 , because we're using only 3 bytes of iSize 
	// 32BIT: send DWORD = 4bytes: one byte is the command (COMMAND_CPRINT) , and 3 bytes for size

	DWORD command_plus_size = (1 << 24) | iSize;

	EnterCriticalSection();

	int iRet = (!WriteFile(m_hPipe, wstrLogString, iSize, &dwWritten, NULL)
		|| (int)dwWritten != iSize) ? -1 : (int)dwWritten;
	LeaveCriticalSection();
}