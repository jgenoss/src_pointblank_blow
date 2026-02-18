#include "pch.h"
#include "TimeUtil.h"

/*
 * Nt time functions.
 *
 * RtlTimeToTimeFields, RtlTimeFieldsToTime and defines are taken from ReactOS and
 * adapted to wine with special permissions of the author. This code is
 * Copyright 2002 Rex Jolliff (rex@lvcablemodem.com)
 *
 * Copyright 1999 Juergen Schmied
 * Copyright 2007 Dmitry Timoshkov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define NONAMELESSUNION
#define NONAMELESSSTRUCT

/*
static int init_tz_info(RTL_TIME_ZONE_INFORMATION *tzi);

static RTL_CRITICAL_SECTION TIME_tz_section;
static RTL_CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &TIME_tz_section,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": TIME_tz_section") }
};
static RTL_CRITICAL_SECTION TIME_tz_section = { &critsect_debug, -1, 0, 0, 0, 0 };
*/

#define TICKSPERSEC        10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       1  /* Jan 1, 1601 was Monday */
#define DAYSPERWEEK        7
#define EPOCHYEAR          1601
#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12
#define DAYSPERQUADRICENTENNIUM (365 * 400 + 97)
#define DAYSPERNORMALCENTURY (365 * 100 + 24)
#define DAYSPERNORMALQUADRENNIUM (365 * 4 + 1)

/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)
/* 1601 to 1980 is 379 years plus 91 leap days */
#define SECS_1601_TO_1980  ((379 * 365 + 91) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1980 (SECS_1601_TO_1980 * TICKSPERSEC)
/* max ticks that can be represented as Unix time */
#define TICKS_1601_TO_UNIX_MAX ((SECS_1601_TO_1970 + INT_MAX) * TICKSPERSEC)


static const int MonthLengths[2][MONSPERYEAR] =
{
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static inline int IsLeapYear(int Year)
{
        return Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0) ? 1 : 0;
}

/******************************************************************************
 *       RtlTimeToSecondsSince1980 [NTDLL.@]
 *
 * Convert a time into a count of seconds since 1980.
 *
 * PARAMS
 *   Time    [I] Time to convert.
 *   Seconds [O] Destination for the converted time.
 *
 * RETURNS
 *   Success: true.
 *   Failure: false, if the resulting value will not fit in a DWORD.
 */
bool  _RtlTimeToSecondsSince1980( const LARGE_INTEGER *Time, LPDWORD Seconds )
{
    ULONGLONG tmp = ((ULONGLONG)Time->u.HighPart << 32) | Time->u.LowPart;
    tmp = tmp / TICKSPERSEC; // RtlLargeIntegerDivide( tmp, TICKSPERSEC, NULL );
    tmp -= SECS_1601_TO_1980;
    if (tmp > 0xffffffff)
	{
		*Seconds = 0;
		return false;
	}

    *Seconds = (DWORD)tmp;
    return true;
}

/******************************************************************************
 *       RtlSecondsSince1980ToTime [NTDLL.@]
 *
 * Convert a count of seconds since 1980 to a time.
 *
 * PARAMS
 *   Seconds [I] Time to convert.
 *   Time    [O] Destination for the converted time.
 *
 * RETURNS
 *   Nothing.
 */
void _RtlSecondsSince1980ToTime( DWORD Seconds, LARGE_INTEGER *Time )
{
    ULONGLONG secs = Seconds * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1980;
    Time->u.LowPart  = (DWORD)secs;
    Time->u.HighPart = (DWORD)(secs >> 32);
}

UINT32 TimeUtil::CalcTimeDifference( INT32 tmAbs0,	INT32	tmAbs1)
{
	SYSTEMTIME stm0, stm1;
	UINT32 temp;

	// System Time으로 변환
	temp = tmAbs0;
	stm0.wYear		= (WORD)(2000 + (temp / 100000000));	temp = temp % 100000000;
	stm0.wMonth		= (WORD)(temp / 1000000);				temp = temp % 1000000;
	stm0.wDay		= (WORD)(temp / 10000);					temp = temp % 10000;
	stm0.wHour		= (WORD)(temp / 100);					temp = temp % 100;
	stm0.wMinute	= (WORD)(temp);
	stm0.wSecond	= 0;
	stm0.wMilliseconds = 0;
	stm0.wDayOfWeek = 0;

	temp = tmAbs1;
	stm1.wYear		= (WORD)(2000 + (temp / 100000000));	temp = temp % 100000000;
	stm1.wMonth		= (WORD)(temp / 1000000);				temp = temp % 1000000;
	stm1.wDay		= (WORD)(temp / 10000);					temp = temp % 10000;
	stm1.wHour		= (WORD)(temp / 100);					temp = temp % 100;
	stm1.wMinute	= (WORD)(temp);
	stm1.wSecond	= 0;
	stm1.wMilliseconds = 0;
	stm1.wDayOfWeek = 0;

	return CalcTimeDifference(stm0, stm1);
}

UINT32 TimeUtil::CalcTimeDifference(const SYSTEMTIME & stm0, const SYSTEMTIME & stm1 )
{
	FILETIME ftm0,	ftm1;
	SystemTimeToFileTime( &stm0, &ftm0);
	SystemTimeToFileTime( &stm1, &ftm1);

	DWORD sec0, sec1;

	_RtlTimeToSecondsSince1980( (LARGE_INTEGER *) &ftm0, &sec0);
	_RtlTimeToSecondsSince1980( (LARGE_INTEGER *) &ftm1, &sec1);

	return (UINT32)sec1 - sec0;
}

SYSTEMTIME TimeUtil::ToSystemTime( UINT32 tmAbs)
{
	SYSTEMTIME stm;
	UINT32 temp;

	// System Time으로 변환
	temp = tmAbs;
	stm.wYear			= (WORD)(2000 + (temp / 100000000));	temp = temp % 100000000;
	stm.wMonth			= (WORD)(temp / 1000000);				temp = temp % 1000000;
	stm.wDay			= (WORD)(temp / 10000);					temp = temp % 10000;
	stm.wHour			= (WORD)(temp / 100);					temp = temp % 100;
	stm.wMinute			= (WORD)(temp);
	stm.wSecond			= 0;
	stm.wMilliseconds	= 0;
	stm.wDayOfWeek		= 0;

	return stm;
}
