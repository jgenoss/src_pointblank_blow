#pragma once

#if defined( USE_HACK_DETECTED )
	BOOL Enable_GuardDll(HINSTANCE hinstDLL, DWORD dwReason, LPVOID);
	void Disable_GuardDll();
#endif

#if defined( USE_DLL_DETECTED )
	#include <set>

	typedef std::set< std::string > ModuleList;

	void Install_ProcessModules();
	bool Detect_ProcessModule(ModuleList & List);
	void Uninstall_ProcessModules();
	void Log_ProcessModule(ModuleList & List);
#endif

DWORD GetProcessesCount(const std::string & ProgramName);
DWORD GetProcessesCount(DWORD dwPID);
