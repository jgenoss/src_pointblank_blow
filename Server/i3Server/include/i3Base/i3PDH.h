#ifndef __I3PDH__
#define __I3PDH__

#pragma comment(lib, "pdh.lib")

#include <pdh.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LOCAL_DISK_COUNT 26
#define MAX_STATUS_COUNT ( PST_MAX + MAX_LOCAL_DISK_COUNT )

struct PDH_STATUS_INFO
{
	char					m_szFullCounterPath[MAX_PATH];
	HQUERY					m_hQuery;
	HCOUNTER				m_hCounter;
};

enum PDH_STATE_TYPE
{
	PST_CPU_CUR,
	PST_CPU_TOTAL,
	PST_HANDLE_CUR,
	PST_HANDLE_TOTAL,
	PST_THREAD_CUR,
	PST_THREAD_TOTAL,
	PST_MEMORY_TOTAL_USE_PERSENT,
	PST_MEMORY_MEMORY_LIMIT,
	PST_MEMORY_CUR,
	PST_LOCALDISK_FREE_SPACE_PERSENT,	// 반드시 PST_MAX 전에 있어야합니다.
	PST_MAX
};

enum PERFORM_WARNING_TYPE
{
	PWT_NO,
	PWT_CPU,
	PWT_MEMORY,
	PWT_DISK
};

class I3_EXPORT_BASE i3PDH
{
private:
	INT32				m_i32LocalDiskCount;
	UINT32				m_i32PerformRevisionTime;	
	PDH_STATUS_INFO		m_pStateInfo[MAX_STATUS_COUNT]; // 26은 로컬 디스크 최대 개수
	BOOL				m_bSuccessCreate;
	SYSTEM_INFO			m_SystemInfo;
	INT64				m_i64LastValues[MAX_STATUS_COUNT];
	INT32				m_i32PerCheckTerm[MAX_STATUS_COUNT];

public:
	INT32				m_i32WarningLimitCPU;					// 퍼포먼스가 문제있다고 판단할 값(%)
	INT32				m_i32WarningLimitMemory;
	INT32				m_i32WarningLimitLocalDiskFreeSpace;
	INT32				m_i32PerformCheckTime;

public:
	i3PDH(void);
	~i3PDH(void);

public:
	BOOL Create(char* _strProcessName = "");	//return FALSE 일경우 Destroy함수를호출하여야한다.
	void Destroy();
	
	INT64 GetValue( INT32 i32PstType );
	INT32 GetLocalDiskCount() { return m_i32LocalDiskCount; }

	INT32 CheckAllPerformWarning(); //  퍼포먼스에 문제가 생기면  TRUE를 리턴한다.
};

#endif