#include "pch.h"
#include "Record.h"

extern REAL32 g_rHalf;
UINT8 g_uiSeedForHalf = 0;

void InitForCheckGravity()
{
PB_VM_S 
	g_uiSeedForHalf = (UINT8) ((i3Math::Rand() % 7) + 1);
	GlobalFunc::EncryptValue( &g_rHalf, g_uiSeedForHalf );
PB_VM_E
}

bool CheckHankingInRecord()
{
PB_VM_S
	REAL64 rTemp = 0.5f;
	REAL32 rHalf = 0.f;
	rHalf = GlobalFunc::DecryptValue( &g_rHalf, g_uiSeedForHalf );

	if(rHalf == rTemp)
	{
		return true;
	}
	else
	{
		return false;
	}
PB_VM_E	
}

INT32	TotalHeadShotPercent( const USER_INFO_RECORD * pUserRecord)
{
	CHECK_ASSERT_RV(pUserRecord, 0);

	if ( pUserRecord->_dkillcount == 0 )
		return 0;

	REAL32	dKillCount	= (REAL32) pUserRecord->_dkillcount;
	REAL32	headShot	= (REAL32) pUserRecord->_headshot;

	return static_cast<INT32>((headShot * 100.0f / dKillCount)+0.5f);
}

INT32	SeasonHeadShotPercent( const USER_INFO_RECORD * pUserRecord)
{
	CHECK_ASSERT_RV(pUserRecord, 0);

	if ( pUserRecord->_sdkillcount == 0 )
		return 0;

	REAL32	dKillCount	= (REAL32) pUserRecord->_sdkillcount;
	REAL32	headShot	= (REAL32) pUserRecord->_sheadshot;

	return static_cast<INT32>((headShot * 100.0f / dKillCount)+0.5f);
}