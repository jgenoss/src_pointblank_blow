#include "i3CommonType.h"
#include "i3PDH.h"
#include "i3String.h"
#include "i3DateTime.h"

i3PDH::i3PDH(void)
{
	m_i32LocalDiskCount					= 0;
	m_bSuccessCreate					= FALSE;

	m_i32WarningLimitCPU				= 0;
	m_i32WarningLimitMemory				= 0;
	m_i32WarningLimitLocalDiskFreeSpace	= 0;
	m_i32PerformRevisionTime			= 0;
	m_i32PerformCheckTime				= 0;
	
	memset( &m_SystemInfo, 0, sizeof(SYSTEM_INFO) );

	for (int i = 0; i < MAX_STATUS_COUNT; i++)
			m_i64LastValues[i] = 0;
}

i3PDH::~i3PDH(void)
{
}

BOOL i3PDH::Create(char* _strProcessName )
{
	char strFileName[MAX_PATH];
	char strProcessName[MAX_PATH];
	char strLocalDiskList[256];

	PDH_STATUS			pdhStatus;
	PDH_STATUS_INFO		*stTarget;

	// 코어 개수 구하기
	GetSystemInfo(&m_SystemInfo);

	// 현재 파일 이름
	GetModuleFileNameA(NULL, strFileName , MAX_PATH);

	// ProcessName값이 디폴트 일경우 함수를 사용하여 현재 ProcessName을 구한다. 
	if( *_strProcessName == '\0' )
	{
		_splitpath( strFileName, NULL, NULL, strProcessName, NULL );
	}
	else
	{
		i3String::Copy(strProcessName, _strProcessName, MAX_PATH);
	}

	// Local Disk 목록구하기 Size를 4로나누면 Disk개수가 구해진다.
	m_i32LocalDiskCount = GetLogicalDriveStrings(256, strLocalDiskList) / 4;

	sprintf(m_pStateInfo[PST_CPU_CUR].m_szFullCounterPath,		"\\Process(%s)\\%% Processor Time"	,strProcessName);	
	sprintf(m_pStateInfo[PST_HANDLE_CUR].m_szFullCounterPath,	"\\Process(%s)\\Handle Count"		,strProcessName);
	sprintf(m_pStateInfo[PST_THREAD_CUR].m_szFullCounterPath,	"\\Process(%s)\\Thread Count"		,strProcessName);

	for( int i = 0 ; i < m_i32LocalDiskCount; i++)
	{
		strLocalDiskList[i * 4 + 2] = '\0';
		sprintf(m_pStateInfo[PST_LOCALDISK_FREE_SPACE_PERSENT + i].m_szFullCounterPath, "\\LogicalDisk(%s)\\%% Free Space" , strLocalDiskList + i * 4);
	}

	strcpy_s(m_pStateInfo[PST_CPU_TOTAL].m_szFullCounterPath,					MAX_PATH, "\\Processor(_Total)\\% Processor Time");
	strcpy_s(m_pStateInfo[PST_HANDLE_TOTAL].m_szFullCounterPath,				MAX_PATH, "\\Process(_Total)\\Handle Count");
	strcpy_s(m_pStateInfo[PST_THREAD_TOTAL].m_szFullCounterPath,				MAX_PATH, "\\Process(_Total)\\Thread Count");
	strcpy_s(m_pStateInfo[PST_MEMORY_TOTAL_USE_PERSENT].m_szFullCounterPath,	MAX_PATH, "\\Memory\\% Committed Bytes In Use");
	strcpy_s(m_pStateInfo[PST_MEMORY_MEMORY_LIMIT].m_szFullCounterPath,			MAX_PATH, "\\Memory\\Commit Limit");
	strcpy_s(m_pStateInfo[PST_MEMORY_CUR].m_szFullCounterPath,					MAX_PATH, "\\Memory\\Committed Bytes");
	
	
	for(int i = 0 ; i < PST_MAX + m_i32LocalDiskCount -1 ; i++ )
	{
		stTarget	= &m_pStateInfo[i];

		pdhStatus	= PdhOpenQuery( NULL, NULL, &stTarget->m_hQuery );
		if( ERROR_SUCCESS != pdhStatus )
			return FALSE;
		pdhStatus	= PdhAddCounter(stTarget->m_hQuery, m_pStateInfo[i].m_szFullCounterPath, 0, &stTarget->m_hCounter );	
		if( ERROR_SUCCESS != pdhStatus )	
			return FALSE;
	}

	m_i32WarningLimitCPU				= 95;
	m_i32WarningLimitMemory				= 95;
	m_i32WarningLimitLocalDiskFreeSpace	= 5;

	m_i32PerformRevisionTime			= i3ThreadTimer::GetServerTime();
	m_i32PerformCheckTime				= 60;

	m_bSuccessCreate = TRUE;
	return TRUE;
}

void i3PDH::Destroy()
{
	PDH_STATUS_INFO		*stTarget;

	for(int i = 0 ; i < PST_MAX + m_i32LocalDiskCount -1; i ++)
	{
		stTarget = &m_pStateInfo[i];

		if( stTarget->m_hQuery )
		{
			PdhCloseQuery ( stTarget->m_hQuery );
			stTarget->m_hQuery	= NULL;
		}

	}
}

INT64 i3PDH::GetValue( INT32 i32PstType )
{
	PDH_STATUS_INFO			*stTarget;
	DWORD					ctrType;
	PDH_FMT_COUNTERVALUE	fmtValue;
	INT64					i64Value;
	PDH_STATUS				pdhstatus;

	if (0 > i32PstType)																return -1;
	if (PST_LOCALDISK_FREE_SPACE_PERSENT + m_i32LocalDiskCount - 1 < i32PstType)	return -1;

	stTarget = &m_pStateInfo[ i32PstType ];

	pdhstatus = PdhCollectQueryData( stTarget->m_hQuery );

	if (ERROR_SUCCESS != pdhstatus)		return m_i64LastValues[i32PstType];

	pdhstatus = PdhGetFormattedCounterValue( stTarget->m_hCounter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100 , &ctrType, &fmtValue );
	
	if (ERROR_SUCCESS != pdhstatus)		return m_i64LastValues[i32PstType];
	
		if (PST_CPU_CUR == i32PstType)
			m_i64LastValues[i32PstType] = (INT64)(fmtValue.doubleValue / m_SystemInfo.dwNumberOfProcessors);
		else
			m_i64LastValues[i32PstType] = (INT64)fmtValue.doubleValue;

	return m_i64LastValues[i32PstType];
}

INT32 i3PDH::CheckAllPerformWarning()		// CPU 같은 경우 사용 하는 곳에서 GetValue 함수를 콜하지 않을 경우 값이 계속 0이들어갑니다. 메모리 같은경우 사용된 실제 메모리가 아닌 할당된 메모리 입니다.
{
	if (FALSE == m_bSuccessCreate)	return PWT_NO; // Create할때 문제가 생겼는대 이 함수를		사용한다는건 문제가 있습니다.

	INT64 i64CPUCur			= m_i64LastValues[PST_CPU_CUR];
	INT64 i64MemoryTotalUse	= GetValue(PST_MEMORY_TOTAL_USE_PERSENT);
	INT64 i64LocalDiskFreeSpace;

	if (i64CPUCur <= m_i32WarningLimitCPU) m_i32PerformRevisionTime = i3ThreadTimer::GetServerTime();		// CPU같은경우는 순간 오를수있기때문에 보정을 주었습니다.	

	if ((m_i32PerformRevisionTime + m_i32PerformCheckTime) < i3ThreadTimer::GetServerTime())
		return PWT_CPU;
	if (i64MemoryTotalUse > m_i32WarningLimitMemory)
		return PWT_MEMORY;

	for (INT32 i = 0; i < GetLocalDiskCount(); i++)
	{
		i64LocalDiskFreeSpace = GetValue(PST_LOCALDISK_FREE_SPACE_PERSENT + i);

		if (i64LocalDiskFreeSpace == 0)										continue;			// 이동식 미디어 장치 등은 값이 0으로 나옵니다.
		if (i64LocalDiskFreeSpace <= m_i32WarningLimitLocalDiskFreeSpace)	return PWT_DISK;
	}

	return PWT_NO;
}