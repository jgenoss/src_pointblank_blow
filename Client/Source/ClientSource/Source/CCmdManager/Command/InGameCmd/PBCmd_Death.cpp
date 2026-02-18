#include "pch.h"
#include "PBCmd_Death.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../../../BattleSlotContext.h"

namespace Cheatkey
{
	bool Suicide::_Execute()
	{
		CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		if( pMyChara != nullptr)
			pMyChara->Cmd_DamageSelf( 255);
		return true;
	}

	ICommand* Suicide::Creator()
	{
		return new Suicide(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 자신을 제외한 모든 캐릭터 죽이기
	// ex) "/killall
	//--------------------------------------------------
	bool KillemAll::_Execute()
	{
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);
			if( pChara == nullptr )	continue;

			CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
			if( pChara != pMyChara)
			{
				VEC3D vDir;
				i3Vector::Sub( &vDir, pChara->GetPos(), pMyChara->GetPos());
				i3Vector::Normalize( &vDir, &vDir);

				g_pCharaManager->DeathChara( pChara, pChara->getCharaNetIndex(), &vDir, CHARA_DEATH_A, CHARA_HIT_HEAD);
			}
		}

		return true;
	}

	ICommand* KillemAll::Creator()
	{
		return new KillemAll(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}