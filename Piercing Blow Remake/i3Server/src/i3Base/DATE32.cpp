#include "i3CommonType.h"
#include "DATE32.h"
#include "i3String.h"

DATE32::DATE32()
{
	m_ui32Date = 0;
}

DATE32::DATE32(UINT32 ui32Date)
{
	m_ui32Date = ui32Date;
}

DATE32::~DATE32()
{
}

DATE32&	DATE32::operator=(const SYSTEMTIME& pTime)
{
	INT32 i32Year = pTime.wYear;
	
	// ������ ���� ���� Ȯ���Ͽ� �߸��� ���̸� �ʱⰪ���� ����.
	if ( DATE32_YEAR_MAX < i32Year || DATE32_YEAR_INIT > i32Year )
		m_ui32Date = DATE32_INIT; // 2017591296 => 2000-01-01 00:00:00
	else
	{
		m_ui32Date = ( (i32Year - DATE32_YEAR_INIT) << 26) |
			(pTime.wMonth << 22) |
			(pTime.wDay << 17) |
			(pTime.wHour << 12) |
			(pTime.wMinute << 6) |
			pTime.wSecond;
	}

	return *this;
}

DATE32&	DATE32::operator=(const tm& pTime)
{
	INT32 i32Year = pTime.tm_year + 1900;
	
	// ������ ���� ���� Ȯ���Ͽ� �߸��� ���̸� �ʱⰪ���� ����.
	if ( DATE32_YEAR_MAX < i32Year|| DATE32_YEAR_INIT > i32Year )
		m_ui32Date = DATE32_INIT; // 2017591296 => 2000-01-01 00:00:00
	else
	{
		m_ui32Date = ( (i32Year - DATE32_YEAR_INIT) << 26) |
			((pTime.tm_mon + 1) << 22) |
			(pTime.tm_mday << 17) |
			(pTime.tm_hour << 12) |
			(pTime.tm_min << 6) |
			pTime.tm_sec;
	}

	return *this;
}

DATE32&	DATE32::operator=(const DATE& pTime)
{
	COleDateTime oleDate(pTime);

	INT32 i32Year = oleDate.GetYear();

	// ������ ���� ���� Ȯ���Ͽ� �߸��� ���̸� �ʱⰪ���� ����.
	if ( DATE32_YEAR_MAX < i32Year || DATE32_YEAR_INIT > i32Year )
	{
		m_ui32Date = DATE32_INIT; // 2017591296 => 2000-01-01 00:00:00
	}
	else
	{
		m_ui32Date = ( (i32Year - DATE32_YEAR_INIT) << 26) |
			(oleDate.GetMonth() << 22) |
			(oleDate.GetDay() << 17) |
			(oleDate.GetHour() << 12) |
			(oleDate.GetMinute() << 6) |
			oleDate.GetSecond();
	}

	return *this;
}

DATE32&	DATE32::operator=(const UINT32 pTime)
{
	m_ui32Date = pTime;

	return *this;
}

DATE32& DATE32::operator=(const DATE32& pTime)
{
	m_ui32Date = pTime.m_ui32Date;

	return *this;
}

UINT32 DATE32::GetDateTimeBin(INT32 i32DateType)
{
	return (m_ui32Date & i32DateType);
}

UINT32 DATE32::GetDateTimeYYYYMMDD()
{
	return GetYear() * 10000 + GetMonth() * 100 + GetDay();
}

UINT32 DATE32::GetDateTimeYYMMDDHHMI()
{
	return (GetYear() % 100) * 100000000 + GetMonth() * 1000000 + GetDay() * 10000 + GetHour() * 100 + GetMinute();
}

UINT32 DATE32::GetDateTimeMMDDHHMISS()
{
	return GetMonth() * 100000000 + GetDay() * 1000000 + GetHour() * 10000 + GetMinute() * 100 + GetSecond();
}

void DATE32::GetSmallDate(char* strDate, INT32 i32Length)
{
	UINT32 ui32Year = GetYear();
	UINT32 ui32Month = GetMonth();
	UINT32 ui32Day = GetDay();
	UINT32 ui32Hour = GetHour();
	UINT32 ui32Minute = GetMinute();
	UINT32 ui32Second = GetSecond();

	// ������ ���� Ȯ���Ͽ� �߸��� ���̸� �ʱⰪ���� ����.
	if ((DATE32_YEAR_INIT > ui32Year || DATE32_YEAR_MAX < ui32Year) ||
		(0 == ui32Month || 12 < ui32Month || 0 == ui32Day || 31 < ui32Day || 23 < ui32Hour || 59 < ui32Minute))
	{// �⺻�������� �����ϸ� 2000-01-01 00:00 �� �����Ͽ����� ��
		ui32Year = 2000;
		ui32Month = 01;
		ui32Day = 01;
		ui32Hour = 00;
		ui32Minute = 00;
		ui32Second = 00;
	}

	i3String::Format(strDate, i32Length, "%04d-%02d-%02d %02d:%02d:%02d",
		ui32Year, ui32Month, ui32Day, ui32Hour, ui32Minute, ui32Second);
}

void DATE32::GetSmallDate(wchar_t* wstrDate, INT32 i32Length)
{
	UINT32 ui32Year = GetYear();
	UINT32 ui32Month = GetMonth();
	UINT32 ui32Day = GetDay();
	UINT32 ui32Hour = GetHour();
	UINT32 ui32Minute = GetMinute();
	UINT32 ui32Second = GetSecond();

	// ������ ���� Ȯ���Ͽ� �߸��� ���̸� �ʱⰪ���� ����.
	if ((DATE32_YEAR_INIT > ui32Year || DATE32_YEAR_MAX < ui32Year) ||
		(0 == ui32Month || 12 < ui32Month || 0 == ui32Day || 31 < ui32Day || 23 < ui32Hour || 59 < ui32Minute))
	{// �⺻�������� �����ϸ� 2000-01-01 00:00 �� �����Ͽ����� ��
		ui32Year = 2000;
		ui32Month = 01;
		ui32Day = 01;
		ui32Hour = 00;
		ui32Minute = 00;
		ui32Second = 00;
	}

	i3String::Format(wstrDate, i32Length, L"%04d-%02d-%02d %02d:%02d:%02d",
		ui32Year, ui32Month, ui32Day, ui32Hour, ui32Minute, ui32Second);
}

void DATE32::CheckDATE32()
{
	UINT32 ui32Year		= GetYear();
	UINT32 ui32Month	= GetMonth();
	UINT32 ui32Day		= GetDay();
	UINT32 ui32Hour		= GetHour();
	UINT32 ui32Minute	= GetMinute();
	UINT32 ui32Second	= GetSecond();

	// ������ ���� Ȯ���Ͽ� �߸��� ���̸� �ʱⰪ���� ����.
	if ((DATE32_YEAR_INIT > ui32Year || DATE32_YEAR_MAX < ui32Year) ||
		(0 == ui32Month || 12 < ui32Month || 0 == ui32Day || 31 < ui32Day || 23 < ui32Hour || 59 < ui32Minute || 59 < ui32Second))
	{
		m_ui32Date = DATE32_INIT; // 2017591296 => 2000-01-01 00:00:00
	}
}

void DATE32::AddTime(I3DATE_ADD_TYPE eAddType, INT32 i32Time)
{
	struct tm oldTime;
	oldTime.tm_year = (GetYear() - 1900) + (I3DATE_ADD_TYPE_YEAR == eAddType ? i32Time : 0);
	oldTime.tm_mon = GetMonth() - 1 + (I3DATE_ADD_TYPE_MONTH == eAddType ? i32Time : 0);
	oldTime.tm_mday = GetDay() + (I3DATE_ADD_TYPE_DAY == eAddType ? i32Time : 0);
	oldTime.tm_hour = GetHour() + (I3DATE_ADD_TYPE_HOUR == eAddType ? i32Time : 0);
	oldTime.tm_min = GetMinute() + (I3DATE_ADD_TYPE_MINUTE == eAddType ? i32Time : 0);
	oldTime.tm_sec = GetSecond() + (I3DATE_ADD_TYPE_SECOND == eAddType ? i32Time : 0);
	oldTime.tm_isdst = SUMMER_TIME_FOLLOW_SYSTEM;
	mktime(&oldTime);
	this->operator=(oldTime);
}

INT32 DATE32::DiffTime(DATE32 dt32Date)
{
	struct tm tmTime1;

	tmTime1.tm_year = dt32Date.GetYear() - 1900;
	tmTime1.tm_mon = dt32Date.GetMonth() - 1;
	tmTime1.tm_mday = dt32Date.GetDay();
	tmTime1.tm_hour = dt32Date.GetHour();
	tmTime1.tm_min = dt32Date.GetMinute();
	tmTime1.tm_sec = dt32Date.GetSecond();
	tmTime1.tm_isdst = SUMMER_TIME_FOLLOW_SYSTEM;

	time_t tm_tTime1 = mktime(&tmTime1);

	struct tm tmTime2;

	tmTime2.tm_year = GetYear() - 1900;
	tmTime2.tm_mon = GetMonth() - 1;
	tmTime2.tm_mday = GetDay();
	tmTime2.tm_hour = GetHour();
	tmTime2.tm_min = GetMinute();
	tmTime2.tm_sec = GetSecond();
	tmTime2.tm_isdst = SUMMER_TIME_FOLLOW_SYSTEM;

	time_t tm_tTime2 = mktime(&tmTime2);

	return (INT32)(difftime(tm_tTime2, tm_tTime1));
}