#include "pch.h"
#include "PCCafeInfo.h"

CPCCafeInfo::CPCCafeInfo(void)
{
	m_ui32PCCafeGUID		= 0;
	m_i32BillingGUID		= 0;
	m_i8WorkState			= (INT8)PCCAFE_WORK_STATE_NONE;
	m_ui32ChargeCheckTime	= 0;
	m_ui32StateTime			= 0;
}

CPCCafeInfo::~CPCCafeInfo(void)
{
}

void CPCCafeInfo::SetChargeCheckTime()
{
	m_ui32ChargeCheckTime = i3ThreadTimer::GetServerTime() + PCCAFE_CHARGE_CHECK_TIME_SECOND;
}

BOOL CPCCafeInfo::IsChargeCheckTime()
{
	if ( m_ui32ChargeCheckTime <= i3ThreadTimer::GetServerTime() )
		return TRUE;
	return FALSE;
}

void CPCCafeInfo::SetState( PCCAFE_WORK_STATE eState )
{
	m_i8WorkState	= (INT8)eState;
	m_ui32StateTime	= i3ThreadTimer::GetServerTime();
}

BOOL CPCCafeInfo::CheckTimeOut()
{
	if ( m_ui32StateTime + PCCAFE_LOGIN_TIMEOUT	< i3ThreadTimer::GetServerTime() )
		return TRUE;
	return FALSE;
}