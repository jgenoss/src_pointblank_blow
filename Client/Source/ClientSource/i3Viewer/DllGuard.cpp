// i3Viewer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "DllGuard.h"
#include <TlHelp32.h>
#include <algorithm>

#if defined( USE_HACK_DETECTED )

#include <stdio.h>
#include <windows.h>
#include <winnt.h>

typedef LONG NTSTATUS;
#define FASTCALL __fastcall
#define NT_SUCCESS(Status) (((NTSTATUS)Status) >= 0)
#ifndef NTAPI
#define NTAPI WINAPI
#endif
#ifndef UNICODE_STRING
typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;
#endif

typedef HMODULE (WINAPI *LOADLIBRARYA)(
  LPCSTR lpFileName
);
typedef HMODULE (WINAPI *LOADLIBRARYW)(
  LPCWSTR lpFileName
);
typedef HMODULE (WINAPI *LOADLIBRARYEXA)(
  LPCSTR lpFileName,
  HANDLE hFile,
  DWORD dwFlags
);
typedef HMODULE (WINAPI *LOADLIBRARYEXW)(
  LPCWSTR lpFileName,
  HANDLE hFile,
  DWORD dwFlags
);
typedef NTSTATUS (NTAPI *LDRLOADDLL)(PWCHAR, ULONG, PUNICODE_STRING, PHANDLE);
#define CALCULATE_JUMP_OFFSET(_BaseOffset, _RelativeOffset) ( \
		(LONG)(_BaseOffset) + (LONG)(_RelativeOffset) + 5 \
)
#define MAKE_JUMP_OFFSET(_BaseOffset, _TargetOffset) ( \
		(LONG)(_TargetOffset) - (LONG)(_BaseOffset) - 5 \
)

LOADLIBRARYA _LoadLibraryA, _tLoadLibraryA;
LOADLIBRARYW _LoadLibraryW, _tLoadLibraryW;
LOADLIBRARYEXA _LoadLibraryExA, _tLoadLibraryExA;
LOADLIBRARYEXW _LoadLibraryExW, _tLoadLibraryExW;
LDRLOADDLL _LdrLoadDll, _tLdrLoadDll;
DWORD dwUser32Start, dwUser32End;

BOOL WINAPI CheckCallerAddress(IN PVOID Caller)
{
	// 새 스레드에서 호출되었는지 확인
	if( Caller == NULL )
	{
		I3PRINTLOG(I3LOG_VIEWER, "New Thread DLL 인젝션이 감지되었습니다.");
	}
	// 커널 모드 메모리?
	else if( (DWORD)Caller >= (DWORD)0x80000000L )
	{
		I3PRINTLOG(I3LOG_VIEWER,  "New Thread DLL 인젝션이 감지되었습니다." );
	}
	// 비정상적 메모리?
	else if(IsBadCodePtr( (FARPROC)Caller ))
	{
		I3PRINTLOG(I3LOG_VIEWER,  "New Thread DLL 인젝션이 감지되었습니다." );
	}
	else
	{
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery(Caller, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		// Private Memory에서 호출되었음
		if(mbi.Type == MEM_PRIVATE)
		{
			I3PRINTLOG(I3LOG_VIEWER,  "Private Memory DLL 인젝션이 감지되었습니다." );
		}
		else
		{
			// 혹시 Caller가 BaseThreadStartThunk 에서 시작했나?
			LPBYTE pbCaller = (LPBYTE) Caller;
			if(*pbCaller == (BYTE)0x50 && 
			   /*pbCaller[1] == (BYTE)0xE8 && */
			   CALCULATE_JUMP_OFFSET(pbCaller + 1, *(PLONG)(pbCaller + 2)) == (LONG)ExitThread)
			{
				I3PRINTLOG(I3LOG_VIEWER,  "CreateRemoteThread DLL 인젝션이 감지되었습니다." );
			}

			// 혹시 Caller가 ClientLoadLibrary 에서 시작했나?
			if(((DWORD)pbCaller >= dwUser32Start && (DWORD)pbCaller < dwUser32End) ||
				*(pbCaller-7) == (BYTE)0x1C)
			{
				I3PRINTLOG(I3LOG_VIEWER,  "SetWindowsHookEx DLL 인젝션이 감지되었습니다." );
			}
		}
	}

	return TRUE;
}

__declspec(naked) HMODULE WINAPI _nLoadLibraryA(
	LPCSTR lpFileName
)
{
	__asm
	{
		XOR EAX, EAX
		JNZ $+30000

		PUSHAD					  // 레지스터 값 보관
		PUSH DWORD PTR [ESP+0x20] // 호출자 주소 push
		CALL CheckCallerAddress   // 호출자 검증
		TEST EAX, EAX			  // 반환값이 FALSE 인가?
		JZ _NotVerifiedCaller	  // 잘못된 호출

		// 올바른 호출
		POPAD
		JMP _tLoadLibraryA

_NotVerifiedCaller:
		POPAD
		XOR EAX, EAX			  // EAX = 0
		RETN 0x4				  // 리턴
	}
}

__declspec(naked) HMODULE WINAPI _nLoadLibraryW(
	LPCWSTR lpFileName
)
{
	__asm
	{
		XOR EAX, EAX
		JNZ $+30000

		PUSHAD					  // 레지스터 값 보관
		PUSH DWORD PTR [ESP+0x20] // 호출자 주소 push
		CALL CheckCallerAddress   // 호출자 검증
		TEST EAX, EAX			  // 반환값이 FALSE 인가?
		JZ _NotVerifiedCaller	  // 잘못된 호출

		// 올바른 호출
		POPAD
		JMP _tLoadLibraryW

_NotVerifiedCaller:
		POPAD
		XOR EAX, EAX			  // EAX = 0
		RETN 0x4				  // 리턴
	}
}

__declspec(naked) HMODULE WINAPI _nLoadLibraryExA(
	LPCSTR lpFileName,
	HANDLE hFile,
	DWORD dwFlags
)
{
	__asm
	{
		XOR EAX, EAX
		JNZ $+30000

		PUSHAD					  // 레지스터 값 보관
		PUSH DWORD PTR [ESP+0x20] // 호출자 주소 push
		CALL CheckCallerAddress   // 호출자 검증
		TEST EAX, EAX			  // 반환값이 FALSE 인가?
		JZ _NotVerifiedCaller	  // 잘못된 호출

		// 올바른 호출
		POPAD
		JMP _tLoadLibraryExA

_NotVerifiedCaller:
		POPAD
		XOR EAX, EAX			  // EAX = 0
		RETN 0xC				  // 리턴
	}
}

__declspec(naked) HMODULE WINAPI _nLoadLibraryExW(
	LPCWSTR lpFileName,
	HANDLE hFile,
	DWORD dwFlags
)
{
	__asm
	{
		XOR EAX, EAX
		JNZ $+30000

		PUSHAD					  // 레지스터 값 보관
		PUSH DWORD PTR [ESP+0x20] // 호출자 주소 push
		CALL CheckCallerAddress   // 호출자 검증
		TEST EAX, EAX			  // 반환값이 FALSE 인가?
		JZ _NotVerifiedCaller	  // 잘못된 호출

		// 올바른 호출
		POPAD
		JMP _tLoadLibraryExW

_NotVerifiedCaller:
		POPAD
		XOR EAX, EAX			  // EAX = 0
		RETN 0xC				  // 리턴
	}
}

__declspec(naked)
NTSTATUS
NTAPI
_nLdrLoadDll(
  IN PWCHAR               PathToFile OPTIONAL,
  IN ULONG                Flags OPTIONAL,
  IN PUNICODE_STRING      ModuleFileName,
  OUT PHANDLE             ModuleHandle )
{
	__asm
	{
		XOR EAX, EAX
		JNZ $+30000

		PUSHAD					  // 레지스터 값 보관
		PUSH DWORD PTR [ESP+0x20] // 호출자 주소 push
		CALL CheckCallerAddress   // 호출자 검증
		TEST EAX, EAX			  // 반환값이 FALSE 인가?
		JZ _NotVerifiedCaller	  // 잘못된 호출

		// 올바른 호출
		POPAD
		JMP _tLdrLoadDll

_NotVerifiedCaller:
		POPAD
		XOR EAX, EAX			  // EAX = 0
		RETN 0x10				  // 리턴
	}
}

VOID FASTCALL HookApi( LPBYTE TargetFunction, LPBYTE Trampoline, LPVOID DetouredFunction, DWORD MinimumSize )
{
	//
	// Don't Use Disassembly Library ;-)
	// But, Only Supports 2 byte & 5 byte for XP =_=;
	//
	
	DWORD OldProtect;
	VirtualProtect( TargetFunction-5, 5+MinimumSize, PAGE_EXECUTE_READWRITE, &OldProtect );
	
	if(MinimumSize == 2)
	{
		LPBYTE JmpOffset = TargetFunction - 5;
		*JmpOffset = (BYTE)0xE9; // JMP LONG
		*(PLONG)(JmpOffset + 1) = MAKE_JUMP_OFFSET(JmpOffset, DetouredFunction);
		RtlMoveMemory(Trampoline, TargetFunction, MinimumSize);
		
		JmpOffset = Trampoline + MinimumSize;
		*JmpOffset = (BYTE)0xE9;
		*(PLONG)(JmpOffset + 1) = MAKE_JUMP_OFFSET(JmpOffset, (LPBYTE)TargetFunction + MinimumSize);

		*TargetFunction = (BYTE)0xEB; // JMP SHORT
		TargetFunction[1] = (BYTE)0xF9;
	}
	else if(MinimumSize == 5)
	{
		RtlMoveMemory(Trampoline, TargetFunction, MinimumSize);

		LPBYTE JmpOffset = Trampoline + MinimumSize;
		*JmpOffset = (BYTE)0xE9;
		*(PLONG)(JmpOffset + 1) = MAKE_JUMP_OFFSET(JmpOffset, (LPBYTE)TargetFunction + MinimumSize);

		JmpOffset = TargetFunction;
		*JmpOffset = (BYTE)0xE9; // JMP LONG
		*(PLONG)(JmpOffset + 1) = MAKE_JUMP_OFFSET(JmpOffset, DetouredFunction);
	}
	
	VirtualProtect( TargetFunction-5, 5+MinimumSize, OldProtect, &OldProtect );
}

VOID FASTCALL UnHookApi( LPBYTE TargetFunction, LPBYTE Trampoline, DWORD MinimumSize )
{
	DWORD OldProtect;
	VirtualProtect( TargetFunction, MinimumSize, PAGE_EXECUTE_READWRITE, &OldProtect );
	RtlMoveMemory( TargetFunction, Trampoline, MinimumSize );
	VirtualProtect( TargetFunction, MinimumSize, OldProtect, &OldProtect );
}

BOOL Enable_GuardDll(HINSTANCE hinstDLL, DWORD dwReason, LPVOID)
{
#if defined( USE_HACK_DETECTED )
	#if !defined( NC_BUILD )
		return TRUE;
	#endif
#else
	return TRUE;
#endif

	HMODULE hKernel32 = GetModuleHandle( TEXT("kernel32") );
	HMODULE hNtDLL = GetModuleHandle( TEXT("ntdll") );
	HMODULE hUser32 = GetModuleHandle( TEXT("user32") );
	// So Simple Anti-OllyDebugger ;-)
	__asm
	{
		XOR EDX, EDX
			JNZ $+54321h
	}
	if(!hKernel32 || !hNtDLL) return FALSE;
	if(!hUser32)
	{
		hUser32 = LoadLibrary( TEXT("user32") );
		if(!hUser32) return FALSE;
	}

	_LoadLibraryA = (LOADLIBRARYA) GetProcAddress(hKernel32, "LoadLibraryA");
	if(!_LoadLibraryA)	return FALSE;
	_LoadLibraryW = (LOADLIBRARYW) GetProcAddress(hKernel32, "LoadLibraryW");
	if(!_LoadLibraryW)	return FALSE;
	_LoadLibraryExA = (LOADLIBRARYEXA) GetProcAddress(hKernel32, "LoadLibraryExA");
	if(!_LoadLibraryExA)	return FALSE;
	_LoadLibraryExW = (LOADLIBRARYEXW) GetProcAddress(hKernel32, "LoadLibraryExW");
	if(!_LoadLibraryExW)	return FALSE;
	_LdrLoadDll = (LDRLOADDLL) GetProcAddress(hNtDLL, "LdrLoadDll");
	if(!_LdrLoadDll)	return FALSE;

	_tLoadLibraryA = (LOADLIBRARYA) VirtualAlloc(NULL, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!_tLoadLibraryA)	return FALSE;
	_tLoadLibraryW = (LOADLIBRARYW) VirtualAlloc(NULL, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!_tLoadLibraryA)	return FALSE;
	_tLoadLibraryExA = (LOADLIBRARYEXA) VirtualAlloc(NULL, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!_tLoadLibraryA)	return FALSE;
	_tLoadLibraryExW = (LOADLIBRARYEXW) VirtualAlloc(NULL, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!_tLoadLibraryA)	return FALSE;
	_tLdrLoadDll = (LDRLOADDLL) VirtualAlloc(NULL, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!_tLoadLibraryA)	return FALSE;

	// USER32.DLL 범위 구함
	dwUser32Start = (DWORD)hUser32;

	PIMAGE_DOS_HEADER idh = (PIMAGE_DOS_HEADER)hUser32;
	if(idh->e_magic != IMAGE_DOS_SIGNATURE) return FALSE; // IMAGE_DOS_SIGNATURE 체크
	PIMAGE_NT_HEADERS inh = (PIMAGE_NT_HEADERS)(dwUser32Start +	idh->e_lfanew);
	if(inh->Signature != IMAGE_NT_SIGNATURE) return FALSE; // IMAGE_NT_SIGNATURE 체크
	dwUser32End ^= (dwUser32Start ^ dwUser32End);
	dwUser32End += inh->OptionalHeader.SizeOfImage;

	// API Hooking Start~!

	//				  | Initial Opcode Len
	// -------------------------------------
	// LoadLibraryA	  |	  5 Bytes
	// LoadLibraryW	  |	  5 Bytes
	// LoadLibraryExA |	  5 Bytes
	// LoadLibraryExW |	  2 Bytes
	// LdrLoadDll	  |	  5 Bytes

	HookApi( (LPBYTE)_LoadLibraryA, (LPBYTE)_tLoadLibraryA, (LPVOID)_nLoadLibraryA, 5 );
	HookApi( (LPBYTE)_LoadLibraryW, (LPBYTE)_tLoadLibraryW, (LPVOID)_nLoadLibraryW, 5 );
	HookApi( (LPBYTE)_LoadLibraryExA, (LPBYTE)_tLoadLibraryExA, (LPVOID)_nLoadLibraryExA, 5 );
	HookApi( (LPBYTE)_LoadLibraryExW, (LPBYTE)_tLoadLibraryExW, (LPVOID)_nLoadLibraryExW, 2 );
	HookApi( (LPBYTE)_LdrLoadDll, (LPBYTE)_tLdrLoadDll, (LPVOID)_nLdrLoadDll, 5 );		
}

void Disable_GuardDll()
{
#if defined( USE_HACK_DETECTED )
	#if !defined( NC_BUILD )
		return;
	#endif
#else
	return;
#endif
	// API 후킹 해제

	UnHookApi( (LPBYTE)_LoadLibraryA, (LPBYTE)_tLoadLibraryA, 5 );
	VirtualFree( _tLoadLibraryA, 0, MEM_RELEASE );

	UnHookApi( (LPBYTE)_LoadLibraryW, (LPBYTE)_tLoadLibraryW, 5 );
	VirtualFree( _tLoadLibraryW, 0, MEM_RELEASE );

	UnHookApi( (LPBYTE)_LoadLibraryExA, (LPBYTE)_tLoadLibraryExA, 5 );
	VirtualFree( _tLoadLibraryExA, 0, MEM_RELEASE );

	UnHookApi( (LPBYTE)_LoadLibraryExW, (LPBYTE)_tLoadLibraryExW, 2 );
	VirtualFree( _tLoadLibraryExW, 0, MEM_RELEASE );

	UnHookApi( (LPBYTE)_LdrLoadDll, (LPBYTE)_tLdrLoadDll, 5 );
	VirtualFree( _tLdrLoadDll, 0, MEM_RELEASE );
}

#endif

#if defined( USE_DLL_DETECTED )

void _GetProcessModules(DWORD dwPID, ModuleList & List)
{
	List.clear();

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPALL, dwPID );
	if( hModuleSnap == INVALID_HANDLE_VALUE )
	{
		return;
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof( MODULEENTRY32 );

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if( !Module32First( hModuleSnap, &me32 ) )
	{
		CloseHandle( hModuleSnap );    // Must clean up the
		return;
	}

	ModuleList::iterator It;
	std::string dllName;

	// Now walk the module list of the process,
	// and display information about each module
	do
	{
		dllName = me32.szModule;

		std::transform(dllName.begin(), dllName.end(),
			dllName.begin(), tolower);

		It = List.find( dllName );

		if( It == List.end() )
		{
			List.insert( dllName );
		}
	} while( Module32Next( hModuleSnap, &me32 ) );

	CloseHandle( hModuleSnap );
}

ModuleList g_ModuleList;
void Install_ProcessModules()
{
	_GetProcessModules( GetCurrentProcessId(), g_ModuleList );
}

bool Detect_ProcessModule(ModuleList & List)
{
	ModuleList moduleList;
	_GetProcessModules( GetCurrentProcessId(), moduleList);

	ModuleList::iterator It = g_ModuleList.begin();
	ModuleList::iterator End = g_ModuleList.end();
	ModuleList::iterator findIt;

	while( It != End )
	{
		findIt = moduleList.find( *It );

		if( findIt != moduleList.end() )
		{
			moduleList.erase( findIt );
		}

		++It;
	}

	List = moduleList;

	return ( List.size() != 0 );
}

void Uninstall_ProcessModules()
{
	g_ModuleList.clear();
}

void Log_ProcessModule(ModuleList & List)
{
	ModuleList::iterator It = List.begin();
	ModuleList::iterator End = List.end();

	while( It != End )
	{
		I3PRINTLOG(I3LOG_VIEWER,  "%s", It->c_str() );
		++It;
	}
}

#endif

DWORD GetProcessesCount(const std::string & ProgramName)
{
	HANDLE hProcess = NULL;
	PROCESSENTRY32 pe32 = {0};

	DWORD ret = 0;

	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0 );

	pe32.dwSize = sizeof( PROCESSENTRY32 );

	if( Process32First(hProcess, &pe32) )
	{
		while(Process32Next(hProcess, &pe32))
		{
			if( ProgramName == std::string( pe32.szExeFile ) )
			{
				++ret;
			}
		} 
	}

	CloseHandle(hProcess);

	return ret;
}

DWORD GetProcessesCount(DWORD dwPID)
{
	HANDLE hProcess = NULL;
	PROCESSENTRY32 pe32 = {0};

	DWORD ret = 0;

	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0 );

	pe32.dwSize = sizeof( PROCESSENTRY32 );

	if( Process32First(hProcess, &pe32) )
	{
		while(Process32Next(hProcess, &pe32))
		{
			if( dwPID == pe32.th32ProcessID )
			{
				++ret;
			}
		} 
	}

	CloseHandle(hProcess);

	return ret;
}
