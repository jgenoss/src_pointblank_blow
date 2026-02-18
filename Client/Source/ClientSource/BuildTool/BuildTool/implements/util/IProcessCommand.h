#pragma once

#include <string>
#include <windows.h>
#include <vector>

class IProcessCommand
{
public:
	virtual ~IProcessCommand() {}

	bool operator()();

private:
	virtual std::wstring MakeCommandLine() = 0;
	virtual bool ExecuteProcess(const std::wstring& wstrCommandLine) = 0;
	virtual void WaitProcessDone() = 0;
	virtual void CloseHandle() = 0;
};

class ProcessCommand : public IProcessCommand
{
public:
	virtual ~ProcessCommand() {}

private:
	virtual bool ExecuteProcess(const std::wstring& wstrCommandLine);
	virtual void WaitProcessDone();
	virtual void CloseHandle();

	HANDLE m_handle;
};

// 윈도우 콘솔 입출력 리다이렉션 프로세스.
// 참조 : http://kkamagui.tistory.com/85

class PipeProcessCommand : public IProcessCommand
{
public:
	virtual ~PipeProcessCommand() {}

	const std::vector<BYTE>& GetBuffer() const { return m_buff; }

private:
	virtual bool ExecuteProcess(const std::wstring& wstrCommandLine);
	virtual void WaitProcessDone();
	virtual void CloseHandle() {}

	struct PipeProcessHandle
	{
		HANDLE hPipeRead;
		HANDLE hProcess;
	};

	bool WatchPipeProcess(struct PipeProcessHandle& handle, std::vector<BYTE>& out);

	PipeProcessHandle m_handle;
	std::vector<BYTE> m_buff;
};

BOOL RegFindFromKey(HKEY hUserKey, const wchar_t* SubKey, const wchar_t* FindName, wchar_t* outVal);
BOOL IsWow64();
BOOL Is64BitWindows();