#include "pch.h"
#include "AttendanceContext.h"

AttendanceContext::AttendanceContext() : m_ActivateEventType(ATTENDANCE_TYPE_DORMANT)
{
}

AttendanceContext::~AttendanceContext()
{

}

ATTENDANCE_TYPE AttendanceContext::GetCurrrentType()
{
	return m_ActivateEventType;
}

void AttendanceContext::SetNextType()
{
	int iType = (int)m_ActivateEventType;

	iType++;

	if (iType < ATTENDANCE_TYPE_MAX)
	{
		m_ActivateEventType = (ATTENDANCE_TYPE)iType;
	}
}

AttendanceData* AttendanceContext :: SetData(ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_NORMAL*/)
{
	if (type < ATTENDANCE_TYPE_DORMANT || type >= ATTENDANCE_TYPE_MAX)
		return nullptr;

	return &m_AttendanceData[type];
}

const AttendanceData*	AttendanceContext :: GetData(ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_NORMAL*/) const
{
	if (type < ATTENDANCE_TYPE_DORMANT || type >= ATTENDANCE_TYPE_MAX)
		return nullptr;

	 return &m_AttendanceData[type]; 
}

const ATTENDANCE_USER*	AttendanceContext :: GetAttendanceUser(ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_NORMAL*/) const
{
	if (type < ATTENDANCE_TYPE_DORMANT || type >= ATTENDANCE_TYPE_MAX)
		return nullptr;

	return &m_AttendanceData[type].m_AttendanceUser;
}

void AttendanceContext::SetAttendanceState(UINT8 eState, ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_NORMAL*/)
{
	if (type < ATTENDANCE_TYPE_DORMANT || type >= ATTENDANCE_TYPE_MAX)
		return;

	m_AttendanceData[type].m_AttendanceUser.m_ui8EventState = eState;
}

void AttendanceContext ::SetAttendanceSelectItem(UINT8 uiItem, ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_NORMAL*/)
{
	if (type < ATTENDANCE_TYPE_DORMANT || type >= ATTENDANCE_TYPE_MAX)
		return;

	m_AttendanceData[type].m_AttendanceUser.m_ui8ItemNum = uiItem;
}

const ATTENDANCE_INFO * AttendanceContext::GetAttendanceInfo(ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_NORMAL*/) const
{
	if (type < ATTENDANCE_TYPE_DORMANT || type >= ATTENDANCE_TYPE_MAX)
		return nullptr;

	return &m_AttendanceData[type].m_AttendanceInfo;
}

bool AttendanceContext::IsAttendanceEvent(ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_MAX*/) const
{
	if (type == ATTENDANCE_TYPE_MAX)
	{
		for (int i = ATTENDANCE_TYPE_DORMANT; i < ATTENDANCE_TYPE_MAX; ++i)
		{
			if (m_AttendanceData[i].IsAttendanceEvent() == true)
				return true;
		}
	}
	else
	{
		if(type >= ATTENDANCE_TYPE_DORMANT && type < ATTENDANCE_TYPE_MAX)
			return m_AttendanceData[type].IsAttendanceEvent();
	}

	return false;
}

bool AttendanceContext::IsEventTarget(ATTENDANCE_TYPE type/* = ATTENDANCE_TYPE_MAX*/) const
{
	if (type == ATTENDANCE_TYPE_MAX)
	{
		for (int i = ATTENDANCE_TYPE_DORMANT; i < ATTENDANCE_TYPE_MAX; ++i)
		{
			if (m_AttendanceData[i].IsEventTarget() == true)
				return true;
		}
	}
	else
	{
		if (type >= ATTENDANCE_TYPE_DORMANT && type < ATTENDANCE_TYPE_MAX)
			return m_AttendanceData[type].IsEventTarget();
	}

	return false;
}