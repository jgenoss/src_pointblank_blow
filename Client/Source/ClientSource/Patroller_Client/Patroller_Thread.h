#pragma once

#include "Thread.h"
#include "Patroller_Shared_Struct.h"
#include "ntsecapi.h"
#include "..\Source\RemoteLog\RemoteLog.h"
#include "Patroller_Client.h"

class Patroller_Thread : public Thread
{
private:
	bool								m_bExit;
	UINT32								m_CurrentProtocol;
	char								m_SendBuf[OUT_BUFF_SIZE];
	UINT64								m_nUID;
	UINT32								m_uiOldTime;
	bool								m_bTimerInit;
	float								m_fElapsedTime;

	UINT32			MakeSendData(void);
	void			SetDLLPath(IPC_DATA_DLL_LIST* pData);


	CRITICAL_SECTION	m_cs;
	struct _AutoCs
	{
		CRITICAL_SECTION* m_pcs;

		_AutoCs( CRITICAL_SECTION* pcs ) : m_pcs(pcs)
		{
			if ( m_pcs->DebugInfo != NULL )
				::EnterCriticalSection( m_pcs );
		}
		~_AutoCs()										{ ::LeaveCriticalSection( m_pcs ); }
	};

public:
	Patroller_Thread();
	virtual ~Patroller_Thread();

	void			SetUID(UINT64 nUID) { m_nUID = nUID; }
	void			SetProtocolOnce(UINT32 nProtocol);

	float			GetDeltaSec(void);

	void			SetThreadExit(bool bExit) { m_bExit = bExit; }

	UINT32			OnRunning(void);

	void			SendErrorLog(ZGuard::ERROR_CODE eErr);

private:

};


typedef struct _PEB_LDR_DATA {
	ULONG	Length;
	BOOLEAN	Initialized;
	PVOID	SsHandle;
	//	UINT8 _PADDING_[12];
	LIST_ENTRY InLoadOrderModuleList; //메모리에 로드된 모듈 순서
	LIST_ENTRY InMemoryOrderModuleList;//메모리에 위치한 모듈 순서
	LIST_ENTRY InInitializationOrderModuleList;//초기화된 모듈 순서
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB {
	BYTE                          Reserved1[2];
	BYTE                          BeingDebugged;
	BYTE                          Reserved2[1];
	PVOID                         Reserved3[2];
	PEB_LDR_DATA* Ldr;
} PEB, *PPEB;

typedef struct _LDR_MODULE {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;

	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDLLName;
	ULONG Flags;
	short LoadCount;
	short TIsIndex;
	LIST_ENTRY HashTableEntry;
	ULONG TimeDateStamp;
} _LDR_MODULE, *PLDR_MODULE;

