#include "pch.h"

struct disable_warning_lnk_4221 {} _;

#ifdef DOMI_USER_REVIVAL

#include "DomiRevivalController.h"
#include "UIHUD_Domination_Manager.h"

#include "../UIHudManager.h"
#include "../UIHudRespawnTimer.h"

#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

namespace
{
	class Behavior
	{
	public:
		virtual void operator()() = 0;
	};

	class None : public Behavior
	{
	public:
		virtual void operator()() {}
	}g_none;

	// 전멸 : 전체 부활 HUD 출력
	class TimeToUseWholeRevival : public Behavior
	{
	public:
		virtual void operator()()
		{
			iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
			UIHudRespawnTimer* respawnTimer = static_cast<UIHudRespawnTimer*>(hud);

			if( respawnTimer->is_working())
			{
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_HOLD);
			}

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::revival);

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_VISIBLE, dui::annihilation);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::annihilation);

		}
	}g_wholeRevival;

	class TimeToRespawn : public Behavior
	{
	public:
		virtual void operator()()
		{
			iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
			UIHudRespawnTimer* respawnTimer = static_cast<UIHudRespawnTimer*>(hud);

			if( respawnTimer->is_working())
			{
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_RELEASE_HOLD);
			}
			else
			{
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_START);
			}

			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::annihilation);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_VISIBLE, dui::revival);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::revival);
		}
	}g_respawn;

	class TimeToUseIndivisualRevival : public Behavior
	{
	public:
		virtual void operator()()
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::annihilation);

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_VISIBLE, dui::revival);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::revival);		
		}
	}g_indivisualRevival;

	class ClearIfPrinted : public Behavior
	{
	public:
		virtual void operator()()
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::annihilation);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::revival);

			iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
			if (hud->IsVisible())
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_RESET);
		}
	}g_clear;

	Behavior* g_behavior[RB_MAX] = 
	{
		&g_none, &g_wholeRevival, &g_respawn, &g_indivisualRevival, &g_clear
	};


	// 네트워크 유저 대상으로 살아있는지 여부 판단.
	bool SomeoneAlive()
	{
		for (INT32 i=0; i<4; i++)
		{
			INT32 userIdx = i*2; // 0, 2, 4, 6
			if (userIdx == BattleSlotContext::i()->getMySlotIdx()) continue;

			bool isPlaying = UserContext::i()->IsPlayingSlot(userIdx) ? true : false;
			if (isPlaying)
			{
				CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(userIdx);
				if (chara && (chara->isCharaStateMask(CHARA_STATE_DEATH) == false))
					return true;
			}
		}

		return false;
	}

	bool NoTheOthers()
	{	
		for (INT32 i=0; i<4; i++)
		{
			INT32 userIdx = i*2;
			if (userIdx == BattleSlotContext::i()->getMySlotIdx()) continue;
			if ( UserContext::i()->IsPlayingSlot(userIdx))
				return false;
		}

		return true;
	}

	REVIVAL_BEHAVIOR What()
	{
		CGameCharaBase* local = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if (local == nullptr) return RB_MAX;

		bool localDead = local->isCharaStateMask(CHARA_STATE_DEATH);
		bool local_intrusion = ( UserContext::i()->IsPlayingSlot( BattleSlotContext::i()->getMySlotIdx()) == false);
		 
		if (local_intrusion)				return RB_TIME_TO_USE_WHOLE_REVIVAL;	// 인터미션은 진입 후 바로 리스폰하기 때문에 난입 관전은 웨이브 중에만 있음.
		if (!localDead)						return RB_COMPLETED_RESPAWN;				// 리스폰 했을 경우
			
		if (localDead && NoTheOthers())		return RB_TIME_TO_USE_WHOLE_REVIVAL;		// 혼자만 플레이하고 있는 경우
		if (localDead && !SomeoneAlive())	return RB_TIME_TO_USE_WHOLE_REVIVAL;		// 같이 플레이 하다가 다죽은 경우....
		if (localDead && SomeoneAlive())	return RB_TIME_TO_RESPAWN;
		
		
		I3PRINTLOG(I3LOG_NOTICE, "##revival log >>>>>>");
		I3PRINTLOG(I3LOG_NOTICE, "##revival log my slot(%d)", BattleSlotContext::i()->getMySlotIdx());
		for( INT32 i = 0; i <= 6; ++i)
		{
			CGameCharaBase* chara = nullptr;
			if( !(i%2))
			{
				chara = g_pCharaManager->getCharaByNetIdx( i, false);
				if( chara)
				{
					I3PRINTLOG(I3LOG_NOTICE,"  -- slot(%d), nick(%s), is playing(%d), state death(%d)", i, BattleSlotContext::i()->getNickForSlot(i), 
						UserContext::i()->IsPlayingSlot(i), chara->isCharaStateMask(CHARA_STATE_DEATH));
				}
			}
		}
		I3PRINTLOG(I3LOG_NOTICE, "##revival log <<<<<<");

		return RB_NONE;
	}
}

RevivalController::RevivalController() : m_keyInput(new DomiRevivalKeyInput::KeyInput)
{
	MEMDUMP_NEW( m_keyInput, sizeof( DomiRevivalKeyInput::KeyInput));
	DomiRevivalKeyInput::CustomKeyInput * pCb = new DomiRevivalKeyInput::CustomKeyInput;
	MEMDUMP_NEW( pCb, sizeof( DomiRevivalKeyInput::CustomKeyInput));
	m_keyInput->SetCallback( pCb);
}
RevivalController::~RevivalController()
{
	I3_SAFE_DELETE( m_keyInput);
}

void RevivalController::Show()
{
	INT32 what = static_cast<INT32>(What());
	(*g_behavior[what])();
}

void RevivalController::ImmediateRespawn()
{
	//사용한 유저는 바로 리스폰 시킬수 있도록 합니다.
	iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
	UIHudRespawnTimer* respawnTimer = static_cast<UIHudRespawnTimer*>(hud);
	if( respawnTimer->is_working())
		HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_IMMEDIATE);
	else
		GameEventSender::i()->SetEventOnce( EVENT_RESPAWN_BATTLE);
}

void RevivalController::ProcessKeyInput()
{
	m_keyInput->ProcessKeyInput();
}



using namespace DomiRevivalKeyInput;

void CustomKeyInput::OnKeyInput()
{
	// U 버튼 : 개인부활
	if (g_pFramework->getKeyboard()->GetStrokeState(I3I_KEY_U))
	{
		if (domi_state_info::i()->single_respawn_count > 0)
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INPUT, I3I_KEY_U);
		}
		else
		{
			//domi_util::buy_domi_item(CASHITEM_GROUP_INDIVIDUAL_REVIVAL);
		}
	}

	// I 버튼 : 전체부활
	if (g_pFramework->getKeyboard()->GetStrokeState(I3I_KEY_I))
	{
		if (domi_state_info::i()->everyone_respawn_count > 0)
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INPUT, I3I_KEY_I);
		}
		else
		{
			//domi_util::buy_domi_item(CASHITEM_GROUP_WHOLE_REVIVAL);
		}
	}
}

KeyInput::~KeyInput()
{
	I3_SAFE_DELETE(m_cb);
}

void KeyInput::ProcessKeyInput()
{
	m_cb->OnKeyInput();
}

void KeyInput::SetCallback(KeyInputCallback* cb)
{
	m_cb = cb;
}

#endif