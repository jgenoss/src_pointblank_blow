#include "pch.h"
#include "UIHudObserver.h"

#include "UIHUDIngameChat.h"
#include "UIHudUtil.h"
#include "UI/UIUtil.h"
#include "i3Base/string/ext/sprintf.h"
#include "../StageBattle/ScoreContext.h"
#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "../ObserverBattle.h"
#include "../UtilBattle.h"
#include "../GameCharaWeaponContext.h"
#include "UIBattleFrame.h"
#include "../StageBattle.h"
#include "i3Base/itl/function/function.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"
#include "GameCharaEquipContext.h"

#include "ClientStageInfoUtil.h"

namespace observer_hud
{
	void UICtrl_EnableUserInfo(const bool enable, const CHARACTER_TEAM_TYPE teamType, const INT32 order, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_EnableUserInfo");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		i3Lua::PushBoolean(L, enable);
		UILua::EndLuaFunction(L, 3);
	}

	void UICtrl_EnableTeamUserInfo(const bool enable, const CHARACTER_TEAM_TYPE teamType, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_EnableTeamUserInfo");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushBoolean(L, enable);
		UILua::EndLuaFunction(L, 2);	
	}

	void UICtrl_SelectUserInfo(const bool enable, const CHARACTER_TEAM_TYPE teamType, const INT32 order, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SelectUserInfo");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		i3Lua::PushBoolean(L, enable);
		UILua::EndLuaFunction(L, 3);
	}

	void UICtrl_SetUserWeaponName(const CHARACTER_TEAM_TYPE teamType, const INT32 order, const i3::rc_wstring& wstrWeaponName, const bool isOwnerAlive, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetUserWeaponName");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		i3Lua::PushStringUTF16To8(L, wstrWeaponName.c_str());
		i3Lua::PushBoolean(L, isOwnerAlive);
		UILua::EndLuaFunction(L, 4);
	}

	void UICtrl_SetUserWeaponNameColor_Death(const CHARACTER_TEAM_TYPE teamType, const INT32 order, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetUserWeaponNameColor_Death");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		UILua::EndLuaFunction(L, 2);
	}

	void UICtrl_SetUserKillCount(const CHARACTER_TEAM_TYPE teamType, const INT32 order, const INT32 killCount, const bool isAlive, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetUserKillCount");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		i3Lua::PushInteger(L, killCount);
		i3Lua::PushBoolean(L, isAlive);
		UILua::EndLuaFunction(L, 4);
	}

	void UICtrl_SetUserKillCountColor_Death(const CHARACTER_TEAM_TYPE teamType, const INT32 order, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetUserKillCountColor_Death");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		UILua::EndLuaFunction(L, 2);
	}

	void UICtrl_SetUserHPGauge(const CHARACTER_TEAM_TYPE teamType, const INT32 order, const REAL32 hpRatio, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetUserHPGauge");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		i3Lua::PushNumber(L, hpRatio);
		UILua::EndLuaFunction(L, 3);
	}

	void UICtrl_SetUserNickName(const CHARACTER_TEAM_TYPE teamType, const INT32 order, const i3::rc_wstring& wstrNickname, const bool isAlive, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetUserNickName");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		i3Lua::PushStringUTF16To8(L, wstrNickname.c_str());
		i3Lua::PushBoolean(L, isAlive);
		UILua::EndLuaFunction(L, 4);
	}

	void UICtrl_SetUserNickNameColor_Death(const CHARACTER_TEAM_TYPE teamType, const INT32 order, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetUserNickNameColor_Death");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		UILua::EndLuaFunction(L, 2);
	}

	void UICtrl_SetAliveUserCount(const CHARACTER_TEAM_TYPE teamType, INT32 aliveUserCount, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetAliveUserCount");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, aliveUserCount);
		UILua::EndLuaFunction(L, 2);
	}

	void UICtrl_TurnOnTeamUserInfo(const bool enable, const CHARACTER_TEAM_TYPE teamType, i3UIScene* scene)
	{
		UICtrl_EnableTeamUserInfo(enable, teamType, scene);

		if (enable)
		{
			INT32 order = 1;
			for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
			{
				if (BattleSlotContext::i()->getTeam(i) != teamType) continue;
				if (!UserContext::i()->IsPlayingSlot(i)) continue;
				if (BattleSlotContext::i()->IsGMObserverForClient(i)) continue;
				if (!g_pCharaManager->getCharaByNetIdx(i)) continue;

				UICtrl_EnableUserInfo(true, teamType, order, scene);
				UICtrl_SetUserKillCount(teamType, order, ScoreContext::i()->getIndividualScore(i)->m_KillCount, ub::is_live_character(i), scene);
				UICtrl_SetUserNickName(teamType, order, BattleSlotContext::i()->getNickForSlot(i), ub::is_live_character(i), scene);
				UICtrl_SetUserWeaponName(teamType, order, hu::get_weapon_name(g_pCharaManager->getCharaByNetIdx(i)), ub::is_live_character(i), scene);
				UICtrl_SetUserHPGauge(teamType, order, hu::get_chara_hp_ratio(g_pCharaManager->getCharaByNetIdx(i)), scene);

				order++;
			}

			for (INT32 i=order; i<=MAX_TEAM_USER; i++)
				UICtrl_EnableUserInfo(false, teamType, i, scene);

			UICtrl_SetAliveUserCount(teamType, BattleSlotContext::i()->CheckTotalNumOfAlive(teamType), scene);
		}
	}

	void UICtrl_EnableObservedUserInfo(const bool enable, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_EnableObservedUserInfo_All");
		i3Lua::PushBoolean(L, enable);
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_EnableObservedUserInfo(const bool enable, const CHARACTER_TEAM_TYPE teamType, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_EnableObservedUserInfo");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushBoolean(L, enable);
		UILua::EndLuaFunction(L, 2);
	}

	void UICtrl_EnableObservedUserClan(const bool enable, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_EnableObservedUserClan");
		i3Lua::PushBoolean(L, enable);
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_EnableHelmetProtectionImg(const bool enable, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_EnableHelmetProtectionImg");
		i3Lua::PushBoolean(L, enable);
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_SetObservedUserOrder(const CHARACTER_TEAM_TYPE teamType, const INT32 order, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserOrder");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, order);
		UILua::EndLuaFunction(L, 2);
	}

	void UICtrl_SetObservedUserHP(const CHARACTER_TEAM_TYPE teamType, const INT32 curHP, const INT32 maxHP, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserHP");
		i3Lua::PushInteger(L, teamType);
		i3Lua::PushInteger(L, curHP);
		i3Lua::PushInteger(L, maxHP);
		UILua::EndLuaFunction(L, 3);
	}

	void UICtrl_SetObservedUserKillCount(const INT32 killCount, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserKillCount");
		i3Lua::PushInteger(L, killCount);
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_SetObservedUserDeathCount(const INT32 deathCount, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserDeathCount");
		i3Lua::PushInteger(L, deathCount);
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_SetObservedUserAssistCount(const INT32 assistCount, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserAssistCount");
		i3Lua::PushInteger(L, assistCount);
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_SetObservedUserBulletCount(const INT32 currBulletCnt, const INT32 maxBulletCnt, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserBulletCount");
		i3Lua::PushInteger(L, currBulletCnt);
		i3Lua::PushInteger(L, maxBulletCnt);
		UILua::EndLuaFunction(L, 2);
	}

	void UICtrl_SetObservedUserWeaponName(const i3::rc_wstring& wstrWeaponName, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserWeaponName");
		i3Lua::PushStringUTF16To8(L, wstrWeaponName.c_str());
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_SetObservedUserNickname(const i3::rc_wstring& wstrNickName, i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserNickname");
		i3Lua::PushStringUTF16To8(L, wstrNickName.c_str());
		UILua::EndLuaFunction(L, 1);
	}

	void UICtrl_SetObservedUserTextColor_Death(i3UIScene* scene)
	{
		LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserTextColor_Death");
		UILua::EndLuaFunction(L, 0);
	}

	void UICtrl_SetObservedUserClan(const INT32 idx, i3UIImageBoxEx* clanmarkCtrl, i3UIScene* scene)
	{
		const SLOT_INFO* slotInfo = BattleSlotContext::i()->getSlotInfo(idx);

		if (slotInfo->_clanMark == (UINT32)-1) 
		{
			UICtrl_EnableObservedUserClan(false, scene);
		}
		else
		{
			UICtrl_EnableObservedUserClan(true, scene);

			LuaState* L = UILua::CallLuaFunction(scene, "UICtrl_SetObservedUserClanName");
			i3Lua::PushStringUTF16To8(L, slotInfo->_ClanName);
			UILua::EndLuaFunction(L, 1);

			GameUI::SetClanTexture(clanmarkCtrl, CLAN_MARK_MEDIUM, slotInfo->_clanMark);
			clanmarkCtrl->SetShapeSize(42.0f, 42.0f);
		}
	}

	void UICtrl_RefreshObservedUserInfo(const INT32 idx, i3UIImageBoxEx* clanmarkCtrl, i3UIScene* scene)
	{
		if (BattleSlotContext::i()->IsGMObserverForClient(idx)) return;

		CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx(idx);
		const CHARACTER_TEAM_TYPE teamType = BattleSlotContext::i()->getTeam(idx);

		INT32 currHP = 0, maxHP = 0;
		hu::get_chara_hp(player, currHP, maxHP);

		UICtrl_SetObservedUserHP(teamType, currHP, maxHP, scene);
		UICtrl_SetObservedUserOrder(teamType, BattleSlotContext::i()->CheckPlayerOrder(idx), scene);
		UICtrl_SetObservedUserKillCount(ScoreContext::i()->getIndividualScore(idx)->m_KillCount, scene);
		UICtrl_SetObservedUserAssistCount(ScoreContext::i()->getIndividualScore(idx)->m_AssistCount, scene);
		UICtrl_SetObservedUserDeathCount(ScoreContext::i()->getIndividualScore(idx)->m_DeathCount, scene);

		INT32 currBulletCnt = -1, maxBulletCnt = -1;
		if (hu::get_bullet_cnt(player, currBulletCnt, maxBulletCnt))
			UICtrl_SetObservedUserBulletCount(currBulletCnt, maxBulletCnt, scene);

		i3::rc_wstring wstrWeaponName = hu::get_weapon_name(player);
		UICtrl_SetObservedUserWeaponName(wstrWeaponName.c_str(), scene);

		UICtrl_SetObservedUserClan(idx, clanmarkCtrl, scene);
		UICtrl_SetObservedUserNickname(BattleSlotContext::i()->getNickForSlot(idx), scene);
		if (player)
		{
			if (player->getEquipContext())
			{
				UICtrl_EnableHelmetProtectionImg(player->getEquipContext()->getHelmetProtect() != true, scene);
			}
			else
			{
				I3PRINTLOG(I3LOG_NOTICE, "Player EquipContext Is NULL");
			}
		}
		else
		{
			I3PRINTLOG(I3LOG_NOTICE, "Player Is NULL");
		}

		
		
	}


	View::View(i3UIScene* scene) : m_scene(scene) 
	{
		for (INT32 i=0; i<EVT_MAX_COUNT; i++)
			m_funcList[i] = i3::bind(&View::None, this, i3::_1);
	}
	View::~View() {}

	void View::OnNotify(const EVENT_TYPE evt, const INT32 arg)
	{
		if (0 <= arg && arg < SLOT_MAX_COUNT)
		{
			const fun& f = m_funcList[evt];
			f(arg);
		}
	}

	View_ActiveUserInfo::View_ActiveUserInfo(i3UIScene* scene) : View(scene), m_origposY(0.0f), m_currUserIdx(-1)
	{
		m_funcList[EVT_ENABLE]				= i3::bind(&View_ActiveUserInfo::TurnOn, this, i3::_1);
		m_funcList[EVT_TARGET_CHANGED]		= i3::bind(&View_ActiveUserInfo::SelectUser, this, i3::_1);
		m_funcList[EVT_GIVEUP_BATTLE]		= i3::bind(&View_ActiveUserInfo::Rearrange, this, i3::_1);
		m_funcList[EVT_CHATBOX_APPEARED]	= i3::bind(&View_ActiveUserInfo::Reposition, this, i3::_1);
		m_funcList[EVT_WEAPON_CHANGED]		= i3::bind(&View_ActiveUserInfo::ChangeWeaponName, this, i3::_1);
		m_funcList[EVT_HP_CHANGED]			= i3::bind(&View_ActiveUserInfo::ChangeHPGauge, this, i3::_1);
		m_funcList[EVT_KILL]				= i3::bind(&View_ActiveUserInfo::ChangeKillCount, this, i3::_1);
		m_funcList[EVT_DEATH]				= i3::bind(&View_ActiveUserInfo::ChangeOnDead, this, i3::_1);

		m_frameWnd_R = static_cast<i3UIFrameWnd*>(scene->FindChildByName("i3UIFrameWnd_RedSlotList"));
		//m_frameWnd_R->EnableCtrl(false);

		m_frameWnd_B = static_cast<i3UIFrameWnd*>(scene->FindChildByName("i3UIFrameWnd_BlueSlotList"));
		//m_frameWnd_B->EnableCtrl(false);

		m_origposY = m_frameWnd_R->getPos()->y;
	}

	View_ActiveUserInfo::~View_ActiveUserInfo() {}

	void View_ActiveUserInfo::TurnOn(const INT32 arg)
	{
		const bool enable = (arg == 1) ? true : false;

		if (enable == true)
		{
			UICtrl_TurnOnTeamUserInfo(true, BattleSlotContext::i()->getMyTeam(), m_scene);

			if (observer::IsPreparedToObserveEnemyHP())
				UICtrl_TurnOnTeamUserInfo(true, BattleSlotContext::i()->getEnemyTeamType(), m_scene);
			else
				UICtrl_TurnOnTeamUserInfo(false, BattleSlotContext::i()->getEnemyTeamType(), m_scene);

			if (m_currUserIdx != -1)
				SelectUser(m_currUserIdx);

			Reposition(g_pFramework->GetStageBattle()->GetChatEnable());
		}
		else
		{
			UICtrl_EnableTeamUserInfo(false, CHARACTER_TEAM_RED, m_scene);
			UICtrl_EnableTeamUserInfo(false, CHARACTER_TEAM_BLUE, m_scene);
			m_currUserIdx = -1;
		}
	}

	void View_ActiveUserInfo::SelectUser(const INT32 arg)
	{
		const INT32 idx = arg;
		if (BattleSlotContext::i()->IsGMObserverForClient(idx)) return;
		if (!ub::is_live_character(idx)) return;

		if (!GetFrameWnd(BattleSlotContext::i()->getTeam(idx))->isEnable())
		{
			m_currUserIdx = idx;
			return;
		}

		if (m_currUserIdx != -1)	// 기존 셀렉션 해제.
			UICtrl_SelectUserInfo(false, BattleSlotContext::i()->getTeam(m_currUserIdx), BattleSlotContext::i()->CheckPlayerOrder(m_currUserIdx), m_scene);

		// 셀렉션 추가.
		UICtrl_SelectUserInfo(true, BattleSlotContext::i()->getTeam(idx), BattleSlotContext::i()->CheckPlayerOrder(idx), m_scene);
		m_currUserIdx = idx;
	}

	void View_ActiveUserInfo::Rearrange(const INT32 arg)
	{
		const INT32 giveupUserIdx = arg;
		if (!GetFrameWnd(BattleSlotContext::i()->getTeam(giveupUserIdx))->isEnable()) return;
		if (BattleSlotContext::i()->IsGMObserverForClient(giveupUserIdx)) return;

		const CHARACTER_TEAM_TYPE teamType = BattleSlotContext::i()->getTeam(giveupUserIdx);

		UICtrl_EnableTeamUserInfo(false, teamType, m_scene);
		UICtrl_TurnOnTeamUserInfo(true, teamType, m_scene);

		if ((m_currUserIdx != -1) && (teamType == BattleSlotContext::i()->getTeam(m_currUserIdx)))
			UICtrl_SelectUserInfo(true, teamType, BattleSlotContext::i()->CheckPlayerOrder(m_currUserIdx), m_scene);
	}

	void View_ActiveUserInfo::Reposition(const INT32 arg)
	{
		const bool chatboxAppeared = (arg == 1) ? true : false;

		if (GetFrameWnd(CHARACTER_TEAM_RED)->isEnable())
			MoveFrameWnd(chatboxAppeared, CHARACTER_TEAM_RED);

		if (GetFrameWnd(CHARACTER_TEAM_BLUE)->isEnable())
			MoveFrameWnd(chatboxAppeared, CHARACTER_TEAM_BLUE);
	}

	void View_ActiveUserInfo::ChangeWeaponName(const INT32 arg)
	{
		const INT32 idx = arg;
		const CHARACTER_TEAM_TYPE teamType = BattleSlotContext::i()->getTeam(idx);
		
		if (!GetFrameWnd(teamType)->isEnable()) return;
		if (BattleSlotContext::i()->IsGMObserverForClient(idx)) return;

		CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx(idx);
		if (player)
		{
			UICtrl_SetUserWeaponName(teamType, BattleSlotContext::i()->CheckPlayerOrder(idx), hu::get_weapon_name(player), ub::is_live_character(idx), m_scene);
			UICtrl_SetAliveUserCount(teamType, BattleSlotContext::i()->CheckTotalNumOfAlive(teamType), m_scene);
		}
	}

	void View_ActiveUserInfo::ChangeHPGauge(const INT32 arg)
	{
		const INT32 idx = arg;
		const CHARACTER_TEAM_TYPE teamType = BattleSlotContext::i()->getTeam(idx);

		if (!GetFrameWnd(teamType)->isEnable()) return;
		if (BattleSlotContext::i()->IsGMObserverForClient(idx)) return;

		UICtrl_SetUserHPGauge(teamType, BattleSlotContext::i()->CheckPlayerOrder(idx), 
			hu::get_chara_hp_ratio(g_pCharaManager->getCharaByNetIdx(idx)), m_scene);
	}

	void View_ActiveUserInfo::ChangeOnDead(const INT32 arg)
	{
		const INT32 idx = arg;
		const CHARACTER_TEAM_TYPE teamType = BattleSlotContext::i()->getTeam(idx);

		if (!GetFrameWnd(teamType)->isEnable()) return;
		if (BattleSlotContext::i()->IsGMObserverForClient(idx)) return;

		const INT32 order = BattleSlotContext::i()->CheckPlayerOrder(idx);
		UICtrl_SetUserNickNameColor_Death(teamType, order, m_scene);
		UICtrl_SetUserWeaponNameColor_Death(teamType, order, m_scene);
		UICtrl_SetUserKillCountColor_Death(teamType, order, m_scene);
		UICtrl_SetUserHPGauge(teamType, order, hu::get_chara_hp_ratio(g_pCharaManager->getCharaByNetIdx(idx)), m_scene);
		UICtrl_SetAliveUserCount(teamType, BattleSlotContext::i()->CheckTotalNumOfAlive(teamType), m_scene);
	}

	void View_ActiveUserInfo::ChangeKillCount(const INT32 arg)
	{
		const INT32 idx = arg;
		const CHARACTER_TEAM_TYPE teamType = BattleSlotContext::i()->getTeam(idx);

		if (!GetFrameWnd(teamType)->isEnable()) return;
		if (BattleSlotContext::i()->IsGMObserverForClient(idx)) return;

		UICtrl_SetUserKillCount(teamType, BattleSlotContext::i()->CheckPlayerOrder(idx), 
			ScoreContext::i()->getIndividualScore(idx)->m_KillCount, ub::is_live_character(idx), m_scene);
	}

	i3UIFrameWnd* View_ActiveUserInfo::GetFrameWnd(const CHARACTER_TEAM_TYPE teamType) const
	{
		if (teamType == CHARACTER_TEAM_RED)
			return m_frameWnd_R;

		return m_frameWnd_B;
	}

	void View_ActiveUserInfo::MoveFrameWnd(const bool chatboxAppeared, const CHARACTER_TEAM_TYPE teamType)
	{
		i3UIFrameWnd* frameWnd = GetFrameWnd(teamType);

		if (chatboxAppeared)
		{
			const REAL32 gap = 3.0f;
			const REAL32 slotHeight = 36.0f;
			const REAL32 activeSlotCountBoxHeight = 32.0f;

			const INT32 activeSlotCount_R = BattleSlotContext::i()->CheckTotalNumOfPlayers(CHARACTER_TEAM_RED);
			const INT32 activeSlotCount_B = BattleSlotContext::i()->CheckTotalNumOfPlayers(CHARACTER_TEAM_BLUE);

			const INT32 count =
				(activeSlotCount_R > activeSlotCount_B) ? activeSlotCount_R : activeSlotCount_B;

			const REAL32 bottomY = 
				(frameWnd->getPos()->y + activeSlotCountBoxHeight + ((slotHeight + gap) * count)) + gap;

			const REAL32 chatboxY = UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING)->GetFirstFrameWnd()->getPos()->y;
			if (bottomY > chatboxY)
			{
				const REAL32 diffY = bottomY - chatboxY;

				VEC3D* currPos = nullptr;

				currPos = frameWnd->getPos();
				frameWnd->setPos(currPos->x, currPos->y - diffY, currPos->z);
			}
		}
		else
		{
			VEC3D* currPos = nullptr;

			currPos = frameWnd->getPos();
			frameWnd->setPos(currPos->x, m_origposY, currPos->z);
		}
	}

	View_ObservedUserInfo::View_ObservedUserInfo(i3UIScene* scene) : View(scene) , m_currUserIdx(-1)
	{
		m_funcList[EVT_ENABLE]				= i3::bind(&View_ObservedUserInfo::TurnOn, this, i3::_1);
		m_funcList[EVT_TARGET_CHANGED]		= i3::bind(&View_ObservedUserInfo::Refresh, this, i3::_1);
		m_funcList[EVT_WEAPON_CHANGED]		= i3::bind(&View_ObservedUserInfo::ChangeWeaponInfoText, this, i3::_1);
		m_funcList[EVT_BULLETCOUNT_CHANGED] = i3::bind(&View_ObservedUserInfo::ChangeBulletCountText, this, i3::_1);
		m_funcList[EVT_HP_CHANGED]			= i3::bind(&View_ObservedUserInfo::ChangeHPGauge, this, i3::_1);
		m_funcList[EVT_DEATH]				= i3::bind(&View_ObservedUserInfo::ChangeOnDead, this, i3::_1);
		m_funcList[EVT_KILL]				= i3::bind(&View_ObservedUserInfo::ChangeKillCount, this, i3::_1);
		m_funcList[EVT_HELMETPROTECTION]	= i3::bind(&View_ObservedUserInfo::HideHelmetProtectionImg, this, i3::_1);
		m_funcList[EVT_ASSIST]				= i3::bind(&View_ObservedUserInfo::ChangeAssistCount, this, i3::_1);
		m_funcList[EVT_HAZEMODE]			= i3::bind(&View_ObservedUserInfo::HazeMode, this, i3::_1);

		m_frameWnd = static_cast<i3UIFrameWnd*>(scene->FindChildByName("i3UIFrameWnd_CurrTargetInfo"));
		//m_frameWnd->EnableCtrl(false);

		m_clanmarkCtrl = static_cast<i3UIImageBoxEx*>(scene->FindChildByName("ClanMarkImagebox"));
	}

	View_ObservedUserInfo::~View_ObservedUserInfo() {}

	void View_ObservedUserInfo::HazeMode(const INT32 arg)
	{
		LuaState* L = UILua::CallLuaFunction(m_scene, "UICtrl_EnableHazeMode");
		i3Lua::PushBoolean(L, arg > 0 ? true : false);
		UILua::EndLuaFunction(L, 1);
	}

	void View_ObservedUserInfo::TurnOn(const INT32 arg)
	{
		const bool enable = arg != 0;
		if (m_frameWnd->isEnable() == enable) return;

		if (enable)
		{
			if (m_currUserIdx != -1)
				Refresh(m_currUserIdx);
		}

		if (m_currUserIdx == -1)
			UICtrl_EnableObservedUserInfo(enable == 1, m_scene);
		else
			UICtrl_EnableObservedUserInfo(enable == 1, BattleSlotContext::i()->getTeam(m_currUserIdx), m_scene);
	}

	void View_ObservedUserInfo::Refresh(const INT32 arg)
	{
		const INT32 idx = arg;

		UICtrl_EnableObservedUserInfo(true, BattleSlotContext::i()->getTeam(idx), m_scene);
		UICtrl_RefreshObservedUserInfo(idx, m_clanmarkCtrl, m_scene);

		m_currUserIdx = idx;
	}

	void View_ObservedUserInfo::ChangeWeaponInfoText(const INT32 arg)
	{
		if (!m_frameWnd->isEnable()) return;

		const INT32 idx = arg;
		if (m_currUserIdx != idx) return;

		UICtrl_SetObservedUserWeaponName(hu::get_weapon_name(g_pCharaManager->getCharaByNetIdx(idx)), m_scene);
		ChangeBulletCountText(idx);
	}

	void View_ObservedUserInfo::ChangeBulletCountText(const INT32 arg)
	{
		if (!m_frameWnd->isEnable()) return;

		const INT32 idx = arg;
		if (m_currUserIdx != idx) return;

		INT32 currBulletCnt = 0, maxBulletCnt = 0;
		if (hu::get_bullet_cnt(g_pCharaManager->getCharaByNetIdx(idx), currBulletCnt, maxBulletCnt))
			UICtrl_SetObservedUserBulletCount(currBulletCnt, maxBulletCnt, m_scene);
	}

	void View_ObservedUserInfo::ChangeHPGauge(const INT32 arg)
	{
		if (!m_frameWnd->isEnable()) return;
		
		const INT32 idx = arg;
		if (m_currUserIdx != idx) return;

		INT32 currHP = 0, maxHP = 0;
		hu::get_chara_hp(g_pCharaManager->getCharaByNetIdx(idx), currHP, maxHP);
		
		UICtrl_SetObservedUserHP(BattleSlotContext::i()->getTeam(idx), currHP, maxHP, m_scene);
	}

	void View_ObservedUserInfo::ChangeOnDead(const INT32 arg)
	{
		if (!m_frameWnd->isEnable()) return;

		const INT32 idx = arg;
		if (m_currUserIdx != idx) return;

		INT32 currHP = 0, maxHP = 0;
		hu::get_chara_hp(g_pCharaManager->getCharaByNetIdx(idx), currHP, maxHP);

		UICtrl_SetObservedUserHP(BattleSlotContext::i()->getTeam(idx), currHP, maxHP, m_scene);
		UICtrl_SetObservedUserDeathCount(ScoreContext::i()->getIndividualScore(idx)->m_DeathCount, m_scene);
		UICtrl_SetObservedUserTextColor_Death(m_scene);
	}

	void View_ObservedUserInfo::ChangeKillCount(const INT32 arg)
	{
		if (!m_frameWnd->isEnable()) return;

		const INT32 idx = arg;
		if (m_currUserIdx != idx) return;

		UICtrl_SetObservedUserKillCount(ScoreContext::i()->getIndividualScore(idx)->m_KillCount, m_scene);
	}

	void View_ObservedUserInfo::ChangeAssistCount(const INT32 arg)
	{
		if (!m_frameWnd->isEnable()) return;

		const INT32 idx = arg;
		if (m_currUserIdx != idx) return;

		UICtrl_SetObservedUserAssistCount(ScoreContext::i()->getIndividualScore(idx)->m_AssistCount, m_scene);
	}

	void View_ObservedUserInfo::HideHelmetProtectionImg(const INT32 arg)
	{
		if (!m_frameWnd->isEnable()) return;

		const INT32 idx = arg;
		if (m_currUserIdx != idx) return;

		UICtrl_EnableHelmetProtectionImg(false, m_scene);
	}

	Context::Context() {}
	Context::~Context()
	{
		Clear();
	}

	void Context::Register(View* view)
	{
		m_viewList.push_back(view);
	}

	void Context::Clear()
	{
		i3::cu::for_each_delete_clear(m_viewList);
	}

	void Context::Notify(const EVENT_TYPE evt, const INT32 arg)
	{
		if (!UIBattleFrame::i() || !UIBattleFrame::i()->GetHUD(UIHUD_OBSERVER)) return;
		if (!UIBattleFrame::i()->GetHUD(UIHUD_OBSERVER)->isEnable()) return;

		for (size_t i=0; i<m_viewList.size(); i++)
			m_viewList[i]->OnNotify(evt, arg);
	}

	bool Context::SameOfUserIdx(INT32 Idx)
	{
		if (!UIBattleFrame::i() || !UIBattleFrame::i()->GetHUD(UIHUD_OBSERVER)) return false;
		if (!UIBattleFrame::i()->GetHUD(UIHUD_OBSERVER)->isEnable()) return false;
		if (m_viewList.size() <= 0) return false;

		return m_viewList[1]->SameOfUserIdx(Idx);
	}
}

using namespace observer_hud;

I3_CLASS_INSTANCE(UIHUDObserver);

UIHUDObserver::UIHUDObserver()
{
	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		m_hpList[i] = -1;
		m_bulletCountList[i] = -1;
	}
}

UIHUDObserver::~UIHUDObserver() {}

void UIHUDObserver::_InitializeAtLoad(i3UIScene* scene)
{
	UIHUDBase::_InitializeAtLoad(scene);

	View_ActiveUserInfo* viewUserInfo = new View_ActiveUserInfo(scene);
	MEMDUMP_NEW(viewUserInfo, sizeof(View_ActiveUserInfo));
	Context::i()->Register(viewUserInfo);

	View_ObservedUserInfo* viewObserveInfo = new View_ObservedUserInfo(scene);
	MEMDUMP_NEW(viewUserInfo, sizeof(View_ObservedUserInfo));
	Context::i()->Register(viewObserveInfo);
}

void UIHUDObserver::_ClearAtUnload(i3UIScene* scene)
{
	Context::i()->Clear();

	UIHUDBase::_ClearAtUnload(scene);
}

void UIHUDObserver::OnCreate(i3GameNode* parent)
{
	if (!CStageInfoUtil::IsObserverModeStage(*MyRoomInfoContext::i()->getStage())) return;

	UIHUDBase::OnCreate(parent);

	AddScene("Scene/HUD/PBRe_GhostMode.i3UIs", nullptr);
}

bool UIHUDObserver::OnExitStart()
{
	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		m_hpList[i] = -1;
		m_bulletCountList[i] = -1;
	}

	return UIHUDBase::OnExitStart();
}

void UIHUDObserver::OnUpdate(REAL32 tm)
{
	if (!CStageInfoUtil::IsObserverModeStage(*MyRoomInfoContext::i()->getStage())) return;
	if (!isVisible()) return;

	INT32 idx = _CheckSomeoneHPChanged();
	if (idx != -1)
		Context::i()->Notify(EVT_HP_CHANGED, idx);

	idx = _CheckBulletCountChanged();
	if (idx != -1)
		Context::i()->Notify(EVT_BULLETCOUNT_CHANGED, idx);
}

INT32 UIHUDObserver::_CheckSomeoneHPChanged()
{
	INT32 idx = -1;

	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (!UserContext::i()->IsPlayingSlot(i)) continue;
		if (!g_pCharaManager->getCharaByNetIdx(i)) continue;

		INT32 currHP = 0, maxHP = 0;
		hu::get_chara_hp(g_pCharaManager->getCharaByNetIdx(i), currHP, maxHP);
		if (currHP != m_hpList[i])
		{
			idx = i;
			m_hpList[i] = currHP;
			break;
		}
	}

	return idx;
}

INT32 UIHUDObserver::_CheckBulletCountChanged()
{
	INT32 idx = -1;

	INT32 bullet = -1, maxBullet = -1;
	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (!UserContext::i()->IsPlayingSlot(i)) continue;
		if (!g_pCharaManager->getCharaByNetIdx(i)) continue;
		if (!hu::get_bullet_cnt(g_pCharaManager->getCharaByNetIdx(i), bullet, maxBullet)) continue;

		if (m_bulletCountList[i] != bullet)
		{
			idx = i;
			m_bulletCountList[i] = bullet;
			break;
		}
	}

	return idx;
}