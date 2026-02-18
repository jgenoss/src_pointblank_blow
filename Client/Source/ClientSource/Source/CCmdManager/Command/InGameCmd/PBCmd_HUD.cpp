#include "pch.h"
#include "PBCmd_HUD.h"
#include "../../../Cheatkey_LimitPred.h"

#include "../../../MyRoomInfoContext.h"
#include "../../../BattleSlotContext.h"
#include "../../../BattleHUD.h"
#include "../../../UI/UIHudManager.h"
#include "../../../UI/UIHUDNoticeShop.h"
#include "../../../UI/HUD/UIHUD_Domination_Manager.h"

#include "../../../StageBattle/ScoreContext.h"

namespace Cheatkey
{
	//--------------------------------------------------
	// 2D UI enable/disable
	// ex) "/ui-toggle"
	//--------------------------------------------------
	bool UIToggle::_Execute()
	{
		if( g_pFramework->getHudLayer()->IsFlag( I3_NODEFLAG_DISABLE))
		{
			g_pFramework->getHudLayer()->RemoveFlag( I3_NODEFLAG_DISABLE);
			HUD::instance()->Show(); //hud manager
			UIHUDNoticeShop::instance()->SetVisible(true); //야전 상점 아이콘
			g_pFramework->VisiblePrintInfoText(true); //디버그 스트링
		}
		else
		{
			g_pFramework->getHudLayer()->AddFlag( I3_NODEFLAG_DISABLE);
			HUD::instance()->Hide();
			UIHUDNoticeShop::instance()->SetVisible(false);
			g_pFramework->VisiblePrintInfoText(false);
		}

		return true;
	}

	ICommand* UIToggle::Creator()
	{
		return new UIToggle(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//
	// SlotInfo
	//

	bool SlotInfo::_ParseParams(const i3::vector<i3::string>& params)
	{
		//if(params.size()> 2) return false;

		//for( UINT32 i = 0; i< params.size(); i++)
		//	m_iValue[i] = ::atoi(params.at(i).c_str());
		return true;
	}

	const wchar_t* SlotInfo::SlotStateToStr(SLOT_STATE state)
	{
		switch(state)
		{
		case SLOT_STATE_EMPTY:		return L"EMPTY";
		case SLOT_STATE_CLOSE:		return L"CLOSE";
		case SLOT_STATE_SHOP:		return L"SHOP";
		case SLOT_STATE_INFO:		return L"INFO";
		case SLOT_STATE_CLAN:		return L"CLAN";
		case SLOT_STATE_INVENTORY:	return L"INVENTORY";
		case SLOT_STATE_GACHA:		return L"GACHA";
		case SLOT_STATE_GIFTSHOP:	return L"GIFTSHOP";
		case SLOT_STATE_NORMAL:		return L"NORMAL";
		case SLOT_STATE_READY:		return L"READY";
		case SLOT_STATE_LOAD:		return L"LOAD";
		case SLOT_STATE_RENDEZVOUS:	return L"RENDEZVOUS";
		case SLOT_STATE_PRESTART:	return L"PRESTART";
		case SLOT_STATE_BATTLE_LOADOK:	return L"BATTLE_LOADOK";
		case SLOT_STATE_BATTLE_READY:	return L"BATTLE_READY";
		case SLOT_STATE_BATTLE:		return L"BATTLE";
		default:
			break;
		}

		return L"unknown";
	}

	const wchar_t* SlotInfo::SlotNickNameToStr(i3::rc_wstring& nickStr)
	{
		if(nickStr.empty()) return L"(null)";

		return nickStr.c_str();
	}

	bool SlotInfo::_Execute()
	{		
		int mySlot = BattleSlotContext::i()->getMySlotIdx();

		SLOT_INFO slotInfo[USER_SLOT_COUNT];
		i3::rc_wstring slotNick[USER_SLOT_COUNT];

		BattleSlotContext::i()->__ConsoleDump__(slotInfo, slotNick);

		i3UIManager::CLog(L"*****************[SlotInfo Dump Start]**********************");
		i3UIManager::CLog(L"MySlotIdx: %d , TeamChange(ScoreCtx:%s) ", 
			mySlot, ScoreContext::i()->IsAutoTeamChangeExecuted() ? L"True" : L"False" );
		i3UIManager::CLog(L"  ");
		i3UIManager::CLog(L"RawDump:");
		i3UIManager::CLog(L"Slot   |  SlotState   #    SlotNick    @   Team(Lobby)   |  Team(Battle)  # CharaMan(TeamChange/Team) ");

		for(INT32 i=0; i<USER_SLOT_COUNT;i++)
		{
			const wchar_t* teamBattleStr = L"TeamUnknown";

			const GLOBAL_CHARA_INFO* cinfo = g_pCharaManager->getCharaInfo(i);
			if(cinfo)
			{
				CHARACTER_TEAM_TYPE teamType = cinfo->GetTeam();

				if(teamType == CHARACTER_TEAM_RED)			teamBattleStr = L"TeamRed";
				else if(teamType == CHARACTER_TEAM_BLUE)	teamBattleStr = L"TeamBlue";
				else if(teamType == CHARACTER_TEAM_NONE)	teamBattleStr = L"TeamNone";
			}

			bool cmRst = false;
			bool cmTeamChanged = false;
			const GLOBAL_CHARA_INFO* cmGCI=nullptr;
			cmRst = g_pCharaManager->__ConsoleDump_getCharaData(i, false, cmTeamChanged, &cmGCI);

			const wchar_t* charManTeamChange = L"Unknown";
			const wchar_t* charManTeam = L"Unknown";

			if(cmRst)
			{
				charManTeamChange = cmTeamChanged ? L"true" : L"false";

				if(cmGCI->GetTeam() == CHARACTER_TEAM_RED)
					charManTeam = L"Red";
				else if(cmGCI->GetTeam() == CHARACTER_TEAM_BLUE)
					charManTeam = L"Blue";
			}

			i3UIManager::CLog(L"Slot %02d   |    %s     #      %s      @    %s     |  %s      #  %s / %s   ", 
				i, 
				SlotStateToStr((SLOT_STATE)slotInfo[i]._State),
				SlotNickNameToStr(slotNick[i]),
				(i % 2) ? L"Blue" : L"Red",
				teamBattleStr,
				charManTeamChange, charManTeam
			);
		}
		i3UIManager::CLog(L"*****************[SlotInfo Dump End]**********************");

		return true;
	}

	ICommand* SlotInfo::Creator()
	{
		return new SlotInfo(new LPred_PermittedAccountOnly(new LPred_Anywhere));
	}


	void ViewPosition::_Execute(const REAL32 tm)
	{
		i3::stack_wstring wstrPos;
		CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		VEC3D *pos = pMyChara->GetPos();
		i3::sprintf( wstrPos, L"Player position: X(%.2f), Y(%.2f), Z(%.2f)", 
			getX(pos), getY(pos), getZ(pos));

		CHud::i()->CreateInGameMessage(wstrPos);
	}

	IRunnableCommand* ViewPosition::Creator()
	{
		return new ViewPosition(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
CMD_RESULT_FLAG PBCmd_ViewPosition::ExecuteCommand( i3::vector<i3::string> &params)
{
	return CMD_RESULT_OK;
}

CMD_RESULT_FLAG PBCmd_ViewPosition::RunCommand(REAL32 DeltaSec)
{
	i3::stack_wstring wstrPos;
	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	VEC3D *pos = pMyChara->GetPos();
	i3::sprintf( wstrPos, L"Player position: X(%.2f), Y(%.2f), Z(%.2f)", 
		getX(pos), getY(pos), getZ(pos));

	CHud::i()->CreateInGameMessage(wstrPos);
	PBConsoleManager::getInstance()->AddLogStr( wstrPos.c_str());

	g_pFramework->getCommandSet()->ToogleShowPos();

	return CMD_RESULT_OK;
}