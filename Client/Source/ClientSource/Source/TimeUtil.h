#if !defined( __TIME_UTIL_H)
#define __TIME_UTIL_H

namespace TimeUtil
{
	UINT32	CalcTimeDifference( INT32 tmAbs0,	INT32	tmAbs1);
	UINT32	CalcTimeDifference( const SYSTEMTIME & stm0, const SYSTEMTIME & stm1 );
	SYSTEMTIME	ToSystemTime( UINT32 tmAbs );
};

#endif
