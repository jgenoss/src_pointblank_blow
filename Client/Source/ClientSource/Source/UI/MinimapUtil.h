#ifndef _MINIMAP_UTIL_H_
#define _MINIMAP_UTIL_H_

#include "UIBattleFrame.h"
#include "UIHUDTabMinimap.h"
#include "GameCharaActionContext.h"
#include "GameCharaWeaponContext.h"
#include "TabMinimap_RoomInfo.h"
#include "../../../Common/CommonSource/CommonDediCli.h"
#include "UIHUDTabMinimapDomination.h"

#include "../StageBattle/DinoModeContext.h"
#include "../StageBattle/ScoreContext.h"
#include "../StageBattle/UserContext.h"
#include "UIHudGameResult.h"
#include "BattleHud.h"
#include "UIHudManager.h"


namespace tu
{
	template<typename T, UIHUD hud> class TabMinimapTmplUtil
	{
	public:
		void ProcessInput() const;
		void ProcessRoundEnd() const;
		void ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData) const;
		T	 Tabminimap() const;
	};

	template<typename T, UIHUD hud>
	inline void TabMinimapTmplUtil<T, hud>::ProcessInput() const
	{
		T tabminimap = Tabminimap();

		if (tabminimap)
		{
			UIHudGameResult* gameResult = (UIHudGameResult*)HUD::instance()->GetHud(HUD::GAME_RESULT);
			if (gameResult->IsVisible()) return;

			if (tabminimap->isEnable() == false)
				UIBattleFrame::i()->EnableHUD(hud, true);

			if ((g_pFramework->getKeyPress() & GAME_KEY_CMD_SHOWSCORE) || (g_pFramework->getSysKeyPress() & GAME_KEY_TAB))
				tabminimap->Show(true);
			else
				tabminimap->Show(false);
		}
	}

	template<typename T, UIHUD hud>
	inline void TabMinimapTmplUtil<T, hud>::ProcessRoundEnd() const
	{
		T tabminimap = Tabminimap();
		if (tabminimap)
			tabminimap->Show(false);
	}

	template<typename T, UIHUD hud>
	inline void TabMinimapTmplUtil<T, hud>::ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData) const
	{
		T tabminimap = Tabminimap();
		if (tabminimap)
			if (tabminimap->isEnable())
				tabminimap->ProcessGameEvent(event, arg, UserData);
	}

	template<typename T, UIHUD hud>
	inline T TabMinimapTmplUtil<T, hud>::Tabminimap() const
	{
		UIBattleFrame* frame = UIBattleFrame::i();
		if (!frame) return nullptr;

		return static_cast<T>(frame->GetHUD(hud));
	}



	static TabMinimapTmplUtil<UIHUDTabMinimap*, UIHUD_TABMINIMAP> tabminimap_default;

	static TabMinimapTmplUtil<UIHUDTabMinimapDomination*, UIHUD_TABMINIMAP> tabminimap_domi;


	void ProcessInput();
	void ProcessRoundEnd();
	void ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData);
	UINT16 GetTeamScore(TEAM_TYPE team);
	INT32 GetRestTeamScore(TEAM_TYPE team);
	
	TEAM_TYPE GetMyTeamType();	
	
	static TEAM_TYPE GetTeamType(INT32 slotIdx)
	{
		TEAM_TYPE teamType = CHARA::CharaTeamType2TeamType(g_pCharaManager->getCharaInfo( slotIdx)->GetTeam());
		return teamType;
	}

	static TEAM_TYPE GetEnemyTeamType()
	{
		if (GetMyTeamType() == TEAM_RED) return TEAM_BLUE;
		return TEAM_RED;
	};

	static INT32 GetKillCount(INT32 slotIdx)
	{
		INT32 killCount = -1;

		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);

		if (chara != nullptr)
			killCount = ScoreContext::i()->getIndividualScore(slotIdx)->m_KillCount; 

		return killCount;
	}

	static INT32 GetAssistCount(INT32 slotIdx)
	{
		INT32 assistCount = -1;

		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);

		if (chara != nullptr)
			assistCount = ScoreContext::i()->getIndividualScore(slotIdx)->m_AssistCount;

		return assistCount;
	}

	static INT32 GetDeathCount(INT32 slotIdx)
	{
		INT32 deathCount = -1;

		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);

		if (chara != nullptr)
			deathCount = ScoreContext::i()->getIndividualScore(slotIdx)->m_DeathCount; 

		return deathCount;
	}

	static bool IsDead(INT32 slotIdx)
	{
		bool dead = false;

		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);
		if (chara != nullptr)
		{
			chara->isCharaStateMask(CHARA_STATE_DEATH) == true ? dead = true : dead = false; 
		}
		return dead;
	}

	bool IsInBattleField(INT32 slotIdx);
	bool IsObserver(INT32 slotIdx);

	bool IsNPC(INT32 slotIdx);
	bool CanBeShown(INT32 slotIdx);
	bool IsRoomOwner(INT32 slotIdx);
	
	UINT8 GetPing(INT32 slotIdx);
	
	static REAL32 GetHP(INT32 slotIdx)
	{
		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);
		if (chara)
			return static_cast<REAL32>(chara->getCurHP_Percent());

		return 0.0f;
	}

	static REAL32 GetHpRatio(INT32 slotIdx)
	{
		REAL32 hp = GetHP(slotIdx);
		REAL32 maxHP = 100.0f;

		if (hp == 0.0f) return 0.0f;

		return hp/maxHP;
	}

	static void GetWeaponName(INT32 slotIdx, WEAPON_SLOT_TYPE weaponSlot, i3::rc_wstring& out)
	{
		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);
		if (chara == nullptr)
		{
			out = L"NONE"; return;
		}

		WeaponBase* wp = chara->getWeaponContext()->getWeapon(weaponSlot);
		if (wp == nullptr) // 무기를 버려서 없을 경우
		{
			out = L"NONE"; return;
		}

		if (chara->getWeaponContext()->getWeapon(weaponSlot))
			chara->getWeaponContext()->getWeapon(weaponSlot)->getWeaponInfo()->GetWeaponName(out);
	}
}


#endif