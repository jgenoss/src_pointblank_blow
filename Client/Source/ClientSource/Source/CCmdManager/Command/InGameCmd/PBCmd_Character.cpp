#include "pch.h"
#include "PBCmd_Character.h"
#include "../../../Cheatkey_LimitPred.h"
#include "../TBoundChk.h"
#include "../../../BattleSlotContext.h"
#include "../../../UtilBattle.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Cheatkey
{
	//--------------------------------------------------
	// 자신을 제외한 모든 캐릭터 죽이기
	// ex) "/switchc 캐릭터타입
	//--------------------------------------------------
	SwitchCharacter::SwitchCharacter(ILimitPred* p) : ICommand(p), m_charaType(CHARA_RES_ID_UNKNOWN) {}

	bool SwitchCharacter::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() != 1) return false;

		m_charaType = static_cast<CHARA_RES_ID>(::atoi(params.at(0).c_str()));
		if (m_charaType <= CHARA_RES_ID_UNKNOWN || CHARA_RES_ID_DINO_E <= m_charaType) return false;

		return true;
	}

	bool SwitchCharacter::_Execute()
	{
		if( (m_charaType > CHARA_RES_ID_HUMAN_S) && (m_charaType < CHARA_RES_ID_HUMAN_E) ||
			(m_charaType > CHARA_RES_ID_DINO_S) && (m_charaType < CHARA_RES_ID_DINO_E) )
		{
			UINT32	nCharaUseMap[ SLOT_MAX_COUNT];
			INT32	nUseCharaCount = 0;

			i3mem::FillZero( nCharaUseMap, sizeof( nCharaUseMap));

			for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);
				if( pChara == nullptr )	continue;
				nCharaUseMap[ nUseCharaCount] = pChara->getCharaNetIndex();
				nUseCharaCount++;
			}

			if( nUseCharaCount > 0)
			{
				g_DebugChara[ 0] = m_charaType;
				g_DebugChara[ 1] = m_charaType;

				g_pCharaManager->RemoveAllChara();

				for( INT32 i = 0; i < nUseCharaCount; i++)
				{
					CGameCharaBase * pChara = g_pCharaManager->AddChara( nCharaUseMap[ i], g_DebugChara[ nCharaUseMap[ i] %2]);
					if( pChara != nullptr)
					{
						g_pCharaManager->RespawnChara( pChara->getCharaNetIndex());
						pChara->addCharaStateMask( CHARA_STATE_FIRSTRESPAWN);
					}
				}
			}
		}

		return true;
	}

	ICommand* SwitchCharacter::Creator()
	{
		return new SwitchCharacter(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}


	bool Skill::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() != 3) return false;

		m_category = ::atoi(params[0].c_str());
		m_idx = ::atoi(params[1].c_str());
		m_level = ::atoi(params[2].c_str());

		if (!TBoundChk<INT32>(m_category, 0, CATEGORYLIST_MAX-1)) return false;
		if (!TBoundChk<INT32>(m_idx, 0, std::numeric_limits<INT32>::max())) return false;
		if (!TBoundChk<INT32>(m_level, 0, std::numeric_limits<INT32>::max())) return false;

		return true;
	}

	bool Skill::_Execute()
	{
 		const INT32 idx = BattleSlotContext::i()->getMySlotIdx();

		NET_CHARA_INFO* pInfo = BattleSlotContext::i()->setNetCharaInfo(idx);
		SKILLSYSTEM_APPLY_INFO* pSkillsystem = pInfo->setSkillsystem(m_category);
		pSkillsystem->setSkillData(m_category, m_idx, m_level);
		util_battle::ApplySkillsystem(idx);

		return true;
	}

	ICommand* Skill::Creator()
	{
		return new Skill(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	bool Invincible::_Execute()
	{
		CGameCharaBase* localPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if (localPlayer)
			localPlayer->setInvincibleTime(FLT_MAX);

		return true;
	}

	ICommand* Invincible::Creator()
	{
		return new Invincible(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}