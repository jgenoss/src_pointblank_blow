#include "stdafx.h"
#include "IProcessCommand.h"

#pragma warning(disable:4996)

bool IProcessCommand::operator()()
{
	std::wstring wstrCommandLine = MakeCommandLine();

	if (!ExecuteProcess(wstrCommandLine))
		return false;

	WaitProcessDone();

	CloseHandle();

	return true;
}

bool ProcessCommand::ExecuteProcess(const std::wstring& wstrCommandLine)
{
	PROCESS_INFORMATION pi;		::memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	STARTUPINFOW		si;		::memset(&si, 0, sizeof(STARTUPINFOW));

	si.cb = sizeof(STARTUPINFOW);
	si.hStdError = NULL;
	si.hStdOutput = NULL;
	si.hStdInput = NULL;
	si.dwFlags = STARTF_USESTDHANDLES;
	si.wShowWindow = SW_SHOW;

	BOOL cp_res = ::CreateProcessW(NULL, const_cast<wchar_t*>(wstrCommandLine.c_str()), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	if (cp_res == FALSE)
	{
		return false;
	}

	::CloseHandle(pi.hThread);

	m_handle = pi.hProcess;

	return true;
}

void ProcessCommand::WaitProcessDone()
{
	::WaitForSingleObject(m_handle, INFINITE);
}

void ProcessCommand::CloseHandle()
{
	::CloseHandle(m_handle);
}


// 
bool PipeProcessCommand::ExecuteProcess(const std::wstring& wstrCommandLine)
{
	HANDLE read_pipe = NULL;
	HANDLE write_pipe = NULL;

	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength = sizeof(SECURITY_ATTRIBUTES);	attr.bInheritHandle = TRUE;	attr.lpSecurityDescriptor = NULL;

		if (::CreatePipe(&read_pipe, &write_pipe, &attr, 0) == FALSE)
		{
			return false;
		}
	}

	{
		PROCESS_INFORMATION pi;		::memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		STARTUPINFOW		si;		::memset(&si, 0, sizeof(STARTUPINFOW));

		si.cb = sizeof(STARTUPINFOW);
		si.hStdError = NULL;
		si.hStdOutput = write_pipe;
		si.hStdInput = NULL;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.wShowWindow = SW_SHOW;

		BOOL cp_res = ::CreateProcessW(NULL, const_cast<wchar_t*>(wstrCommandLine.c_str()), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

		if (cp_res == FALSE)
		{
			return false;
		}

		::CloseHandle(pi.hThread);
		::CloseHandle(write_pipe);

		m_handle.hPipeRead = read_pipe;
		m_handle.hProcess = pi.hProcess;
	}

	return true;
}

void PipeProcessCommand::WaitProcessDone()
{
	while (WatchPipeProcess(m_handle, m_buff))
		::WaitForSingleObject(m_handle.hProcess, 1);
}

bool PipeProcessCommand::WatchPipeProcess(PipeProcessHandle& handle, std::vector<BYTE>& out)
{
	if (handle.hProcess == 0)
		return false;

	DWORD dwTotal = 0;

	BOOL res = ::PeekNamedPipe(handle.hPipeRead, NULL, 0, NULL, &dwTotal, NULL);

	if (res && dwTotal > 0)
	{
		size_t start_pos = out.size();
		out.resize(start_pos + dwTotal);

		DWORD dwRead = 0;

		while (dwTotal > 0)
		{
			::ReadFile(handle.hPipeRead, &out[start_pos], dwTotal, &dwRead, NULL);
			dwTotal -= dwRead;
			start_pos += dwRead;
		}
	}

	if (::WaitForSingleObject(handle.hProcess, 0) == WAIT_OBJECT_0)
	{
		::CloseHandle(handle.hProcess);		handle.hProcess = 0;
		::CloseHandle(handle.hPipeRead);	handle.hPipeRead = 0;
		return false;
	}

	return true;
}

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "IsWow64Process");

	if (fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}

	return bIsWow64;
}

BOOL Is64BitWindows()
{
#if defined(_WIN64)
	return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows
	// so must sniff
	BOOL f64 = FALSE;
	return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
	return FALSE; // Win64 does not support Win16
#endif
}

// ÂüÁ¶ url : http://rageworx.tistory.com/4
BOOL RegFindFromKey(HKEY hUserKey, const wchar_t* SubKey, const wchar_t* FindName, wchar_t* outVal)
{
	HKEY hKey, hSubKey;
	LONG lRet;
	DWORD dwSubKeyNumber,
		  dwCount,
		  dwValueNumber,
		  dwValueCount,
		  dwSubKeySize = 80;
	DWORD dwNameSize = 80, dwValueSize = 80;
	FILETIME FileTime;
	int n = 0;
	static TCHAR szSubKey[80], szName[80], szValue[80];
	static TCHAR szBuff[80], szPrint[256];

	const bool is64Windows = Is64BitWindows() == 1;

	if (is64Windows)
		lRet = RegOpenKeyEx(hUserKey, SubKey, 0, KEY_READ | KEY_WOW64_64KEY, &hKey);
	else
		lRet = RegOpenKeyEx(hUserKey, SubKey, 0, KEY_READ, &hKey);

	if (lRet != ERROR_SUCCESS)
		return FALSE;

	lRet = RegQueryInfoKey(hKey,
		NULL,
		0,
		0,
		&dwSubKeyNumber,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&FileTime);

	if (lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}

	dwCount = 0;

	do
	{
		RegEnumKeyEx(hKey, dwCount, szSubKey, &dwSubKeySize, NULL, NULL, 0, &FileTime);
		dwSubKeySize = 80;
		::wcscpy_s(szBuff, sizeof(szBuff), SubKey);
		if (szBuff[::wcslen(szBuff)] != L'\\')
		{
			::wcscat_s(szBuff, sizeof(szBuff), L"\\");
		}
		::wcscat_s(szBuff, sizeof(szBuff), szSubKey);

		if (is64Windows)
			lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szBuff, 0, KEY_READ | KEY_WOW64_64KEY, &hSubKey);
		else
			lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szBuff, 0, KEY_READ, &hSubKey);

		if (lRet != ERROR_SUCCESS)
			return FALSE;

		lRet = RegQueryInfoKey(hSubKey, NULL, 0, 0, NULL, NULL, NULL,
			&dwValueNumber, NULL, NULL, NULL, &FileTime);

		dwValueCount = 0;

		while (dwValueNumber > dwValueCount)
		{
			RegEnumValue(hSubKey, dwValueCount, szName, &dwNameSize, NULL,
				NULL, (unsigned char *)szValue, &dwValueSize);
			dwNameSize = dwValueSize = 80;

			if (!::wcscmp(szName, FindName))
			{
				::wcscpy(outVal, szValue);
				RegCloseKey(hSubKey);
				return TRUE;
			}

			n++;
			dwValueCount++;
		}
		RegCloseKey(hSubKey);
		dwCount++;
	} while (dwCount<dwSubKeyNumber);

	RegCloseKey(hKey);

	return FALSE;
}