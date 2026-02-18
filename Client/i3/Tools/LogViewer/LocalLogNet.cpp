#include "stdafx.h"
#include "LocalLogNet.h"
volatile long m_fast_critical_section;


void InitializeCriticalSection(void)
{
	m_fast_critical_section = 0;
}

void DeleteCriticalSection(void)
{
	m_fast_critical_section = 0;
}

// our own LOCK function
void EnterCriticalSection(void)
{
	while (InterlockedCompareExchange(&m_fast_critical_section, 1, 0) != 0) Sleep(0);
}
void LeaveCriticalSection(void)
{
	m_fast_critical_section = 0;
}
UINT WINAPI ThreadFunc(void *arg)
{
	while (true)
	{
		CLocalLogNet* pGit = (CLocalLogNet*)arg;
		//
		if (pGit)
		{
			if (pGit->m_State)
			{
				pGit->CreatePipeProcess(pGit->GetName());
				pGit->ReadPipeData();
			}
			else
			{
				break;
			}
		}

		Sleep(1);
	}
	return 0;
}

CLocalLogNet::CLocalLogNet()
{
	m_hPipe = INVALID_HANDLE_VALUE;
	m_State = true;
	InitializeCriticalSection();
}


CLocalLogNet::~CLocalLogNet()
{
	m_State = false;

	CloseHandle(m_hTread);
	if (::WaitForSingleObject(m_hPipe, 0) == WAIT_OBJECT_0)		// 프로세스 실행 종료 시점..
	{
		CloseHandle(m_hPipe);
	}
	m_hPipe = NULL;
}

void CLocalLogNet::CreatePipeProcess(i3::string command)
{
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		i3::string  _name = "\\\\.\\pipe\\";
		_name += command;

		m_hPipe = CreateNamedPipe(
			_name.c_str(),					// pipe name 
			PIPE_ACCESS_INBOUND,		// read/write access, we're only writing...
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_BYTE |		// message-read mode 
			PIPE_WAIT,                // blocking mode 
			1,						// max. instances  
			4096,						// output buffer size 
			0,						// input buffer size (we don't read data, so 0 is fine)
			1,						// client time-out 
			NULL);                    // no security attribute 
		if (m_hPipe == INVALID_HANDLE_VALUE)
		{	// failure
			MessageBox(NULL, "CreateNamedPipe failed", "Logger failed", MB_OK);
			return;
		}

		STARTUPINFO si;
//		PROCESS_INFORMATION pi;
		GetStartupInfo(&si);

		i3::string processName = "\\";
		processName += command;
		processName += ".exe";
// 		BOOL bRet = CreateProcess(NULL, (char*)processName.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
//  		if (!bRet)
//  		{
//  			CloseHandle(m_hPipe);
//  			m_hPipe = INVALID_HANDLE_VALUE;
//  			return;
//  		}

		BOOL bConnected = ConnectNamedPipe(m_hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (!bConnected)
		{
			MessageBox(NULL, "ConnectNamedPipe failed", "Logger failed", MB_OK);

			CloseHandle(m_hPipe);
			m_hPipe = INVALID_HANDLE_VALUE;
			return;
		}
	}
}


void CLocalLogNet::Init(i3::string sWorkBase, i3::string sLogName)
{
	m_name = sLogName;
	m_hTread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, &m_dwThreadID);
	WaitForSingleObject(m_hTread, 1000);
}



void CLocalLogNet::ReadPipeData()
{
	if (m_hPipe!= INVALID_HANDLE_VALUE)
	{
		DWORD cbRead;
		wchar_t wchBuf[1024];
		BOOL bSuccess;
		do
		{
			ZeroMemory(wchBuf, sizeof(wchBuf));
			// Read from the pipe.
			bSuccess = ReadFile(
				m_hPipe,    // pipe handle
				wchBuf,    // buffer to receive reply
				sizeof(wchBuf),      // size of buffer
				&cbRead,  // number of bytes read
				NULL);    // not overlapped

			if (!bSuccess && GetLastError() != ERROR_MORE_DATA)
			{
				CloseHandle(m_hPipe);
				m_hPipe = INVALID_HANDLE_VALUE;
				break; // stop only on failure
			}
			else
			{
				char temp[1024];
				int nRet = WideCharToMultiByte(CP_ACP, 0, wchBuf, -1, temp, 1024, NULL, NULL);
				m_inout.push_back(temp);
			}

		} while (true/*!bSuccess*/);  // repeat loop if ERROR_MORE_DATA
	}
}

