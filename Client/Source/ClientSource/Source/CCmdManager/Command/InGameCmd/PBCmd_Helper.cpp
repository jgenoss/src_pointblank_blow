#include "pch.h"
#include "PBCmd_Helper.h"
#include "../../../GameWorld.h"
#include "../../../StageBattle.h"
#include "../../../Cheatkey_LimitPred.h"
#include "CQAWeaponCMgr.h"
#include "BattleHud.h"

namespace Cheatkey
{
	bool Collision::_ParseParams(const i3::vector<i3::string>& params)
	{
		if(params.size()> 2) return false;

		for( UINT32 i = 0; i< params.size(); i++)
			m_iValue[i] = ::atoi(params.at(i).c_str());

		return true;
	}

	bool Collision::_Execute()
	{
		CStageBattle* pStage = g_pFramework->GetStageBattle();
		if( pStage == nullptr)
			return false;

		INT32 nType = SHOW_COLLISION_TYPE_ALL;
		if( m_iValue[0] == 1)		nType = SHOW_COLLISION_TYPE_HIT;
		else if( m_iValue[0] == 2)	nType = SHOW_COLLISION_TYPE_SM;
		else if( m_iValue[0] == 3)	nType = SHOW_COLLISION_TYPE_LIGHT;

		UINT32 opt = 0x01;

		if( m_iValue[1] == 2)
			opt |= 0x02;
		else if( m_iValue[1] == 9)
			opt |= 0x04;

		if( i3::kind_of<CGameWorld*>(pStage->getWorld()))
			((CGameWorld*)pStage->getWorld())->SetEnableViewCol( nType, opt );

		return true;
	}

	ICommand* Collision::Creator()
	{
		return new Collision(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}


	bool ThrowCollisionBox::_Execute()
	{
		CStageBattle* pStage = g_pFramework->GetStageBattle();
		if( pStage)
		{
			if(CQAWeaponCMgr::i()->getOnWeaponCollision())
			{
				CQAWeaponCMgr::i()->ReleasePhysXBox();
				CQAWeaponCMgr::i()->setOnWeaponCollision(false);
			}
			else
			{
				// 크기 정보부터 들어가야 한다.
				CQAWeaponCMgr::i()->setOnWeaponCollision(true);
				CQAWeaponCMgr::i()->setValue(CommandSet::g_value[0], CommandSet::g_value[1], 
					(INT32)CommandSet::g_value[2], (INT32)CommandSet::g_value[3]);
				CQAWeaponCMgr::i()->CreatePhysXBox(pStage->getWorld());

				if ( CHud::i())
					CHud::i()->CreateInGameMessage(CQAWeaponCMgr::i()->getOutCollisionBox());
			}
		}

		return true;
	}

	ICommand* ThrowCollisionBox::Creator()
	{
		return new ThrowCollisionBox(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}

