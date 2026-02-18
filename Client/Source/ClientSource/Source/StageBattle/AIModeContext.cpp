#include "pch.h"
#include "AIModeContext.h"

#include "BattleSlotContext.h"

AIModeContext::AIModeContext()
{
	FriendCount = 0;
	
	EnemyCount = 0;
	NewEnemyCount = 0;

	StartLevel = 0;
	CurrentLevel = 0;

	for( size_t i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		CurrentAILevel[i] = 0;
		AIEntry[i].Reset();
		UserScore[i] = 0;
	}

	KillCount = 0;
}

void AIModeContext::PrepareAIWeapon( void)
{
	if( KillCount == 10 ||	KillCount == 30 ||
		KillCount == 50 ||	KillCount == 70 ||
		KillCount == 90 ||	KillCount == 110 ||
		KillCount == 130 ||	KillCount == 150 ||
		KillCount == 170 ||	KillCount == 190 )
	{
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if( i % TEAM_COUNT != BattleSlotContext::i()->getMySlotIdx() % TEAM_COUNT)
			{
				UINT32 idx = i / TEAM_COUNT;
				g_pCharaManager->LoadAiBindData( CurrentLevel+1, idx, static_cast<TEAM_TYPE>(i % TEAM_COUNT) );

				AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData( idx);
				if( pBindData != nullptr)
				{
					g_pWeaponManager->PushLoadWeapon( pBindData->_primaryWeaponName, true);
					g_pWeaponManager->PushLoadWeapon( pBindData->_secondaryWeaponName, true);
				}
			}
		}
	}
}
