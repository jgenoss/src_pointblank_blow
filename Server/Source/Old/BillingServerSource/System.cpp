#include "pch.h"
#include "System.h"

CSystem*	g_pSystem	= NULL;

I3_CLASS_INSTANCE( CSystem, i3ElementBase );

CSystem::CSystem()
{
	m_rLocalTime		= 0.0f; 
	m_ui32ServerTime	= 10000; //계산을 위해서 이 값부터 시작... UINT32 때문

	m_hCpuQuery			= NULL;
	m_hCpuCounter		= 0;
	m_hHandleQuery		= NULL;
	m_hHandleCounter	= 0;
}

CSystem::~CSystem()
{
	OnDestroy();
}

BOOL CSystem::OnCreate()
{
	char		strTemp[SERVER_STRING_COUNT]; 

	//Create CPU Speed
	if( ERROR_SUCCESS != PdhOpenQuery(NULL, NULL, &m_hCpuQuery) )							return FALSE;

	i3String::Copy( strTemp, "\\Processor(_Total)\\% Processor Time", SERVER_STRING_COUNT );
	if( ERROR_SUCCESS != PdhAddCounter( m_hCpuQuery, strTemp, 0, &m_hCpuCounter ) )			return FALSE;

	if( ERROR_SUCCESS != PdhCollectQueryData(m_hCpuQuery) )									return FALSE;

	//Create Handle Count 
	if( ERROR_SUCCESS != PdhOpenQuery(NULL, NULL, &m_hHandleQuery) )						return FALSE;

	i3String::Format( strTemp, SERVER_STRING_COUNT, "\\Process(_Total)\\Handle Count" );
	if( ERROR_SUCCESS != PdhAddCounter( m_hHandleQuery, strTemp, 0, &m_hHandleCounter ) )	return FALSE;

	if( ERROR_SUCCESS != PdhCollectQueryData(m_hHandleQuery) )								return FALSE;

	return TRUE;
}

void CSystem::OnDestroy()
{
}

void CSystem::OnUpdate()
{
	PDH_STATUS				pdhStatus;		
	DWORD					ctrType;
	PDH_FMT_COUNTERVALUE	fmtValue;

	//1. CPU 점유율
	pdhStatus = PdhCollectQueryData (m_hCpuQuery);
	pdhStatus = PdhGetFormattedCounterValue( m_hCpuCounter, PDH_FMT_DOUBLE, &ctrType, &fmtValue );
	m_i32CpuQuery = (INT32)fmtValue.doubleValue;

	//2. Handle Count
	pdhStatus = PdhCollectQueryData (m_hHandleQuery);
	pdhStatus = PdhGetFormattedCounterValue( m_hHandleCounter, PDH_FMT_DOUBLE, &ctrType, &fmtValue);
	m_i32HandleQuery = (INT32)fmtValue.doubleValue;

	RenewTime();
}

void CSystem::RenewTime()
{ 
	//Cal Server Time 
	REAL32 rDeltaTime	= m_Timer.GetDeltaSec(); 
	m_rLocalTime		= m_rLocalTime + rDeltaTime;		
	if( m_rLocalTime > 1.0f)
	{
		//정수부 더하기 
		UINT32 ui32Time		= (UINT32)m_rLocalTime; 
		m_ui32ServerTime	= m_ui32ServerTime + ui32Time;

		//정수부 삭제 
		REAL32	rTime		= (REAL32)ui32Time; 
		m_rLocalTime		= m_rLocalTime - rTime;
	}
    
	//Cal Local Time
	::GetLocalTime( &m_SystemTime );

	m_i32Year		=	m_SystemTime.wYear;
	m_i32Month		=	m_SystemTime.wMonth; 
	m_i32Day		=	m_SystemTime.wDay; 	
	m_i32Hour		=	m_SystemTime.wHour; 
	m_i32Minute		=	m_SystemTime.wMinute;	 
	m_i32Second		=	m_SystemTime.wSecond;

	m_ui32DateTime	= ((m_i32Year - 2000) * 100000000) + (m_i32Month * 1000000) + (m_i32Day * 10000) + (m_i32Hour* 100) + m_i32Minute;
}