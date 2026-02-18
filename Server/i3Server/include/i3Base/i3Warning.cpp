#include "i3CommonType.h"
#include "i3DateTime.h"
#include "i3Warning.h"


i3Warning::i3Warning(void)
{
	for( INT32 i = 0; i < WARNING_TYPE_COUNT; i++)
	{
		for( INT32 j = 0; j < WARNING_LEVEL_COUNT; j++ )
		{
			m_ui32WriteTime[i][j] = 0;
			m_ui32ReadTime[i][j] = 0;
		}
	}
}


i3Warning::~i3Warning(void)
{
}

UINT32	i3Warning::GetWarningInfo( INT32 i32WarningType )
{
	if( 0 > i32WarningType )						return 0;
	if( WARNING_TYPE_COUNT - 1 < i32WarningType )	return 0;

	UINT32 ui32WarningInfo = 0;

	for (INT32 i = 0; i < WARNING_LEVEL_COUNT; i++)
	{
		if ( m_ui32WriteTime[i32WarningType][i] > m_ui32ReadTime[i32WarningType][i] )
		{
			ui32WarningInfo = ui32WarningInfo | GetFlagWarningLevel( i + 1);
			m_ui32ReadTime[i32WarningType][i] = i3ThreadTimer::GetServerTime();
		}
	}

	return ui32WarningInfo;
}

BOOL	i3Warning::SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel )
{
	if( 0 > i32WarningType )						return FALSE;
	if( WARNING_TYPE_COUNT - 1 < i32WarningType )	return FALSE;
	if( WARNING_LEVEL_NO > i32WarningLevel )		return FALSE;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return FALSE;

	UINT32 ui32ServerTime = i3ThreadTimer::GetServerTime();

	m_ui32WriteTime[i32WarningType][i32WarningLevel - 1] = ui32ServerTime;	// 해당 워닝 레벨에 현재 서버 시간 기록.

	return TRUE;
}

UINT32	i3Warning::GetFlagWarningLevel( INT32 i32WarningLevel )
{
	if( WARNING_LEVEL_NO >= i32WarningLevel )		return 0;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return 0;

	return (0x1 << (i32WarningLevel - 1));
}