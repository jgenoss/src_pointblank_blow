#include "pch.h"


#include "UIHUDTabMinimapDomination.h"
#include "Minimap.h"
#include "Unit.h"
#include "MinimapUtil.h"
#include "BattleHUD_Define.h"
#include "../Mode/Domination/Domination_Defines.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UIHudManager.h"
#include "../BattleHUD_Minimap.h"

#include "ClientStageInfoUtil.h"

// 기획 문서 참조 : http://tiki.zepetto.biz/tiki-download_wiki_attachment.php?attId=2269

using namespace minimap_new;

I3_CLASS_INSTANCE(UIHUDTabMinimapDomination);

UIHUDTabMinimapDomination::UIHUDTabMinimapDomination() : m_bActivate(false),
m_bMinimapUpdatable(false),
m_bUserInfoUpdatable(false)
{
}

UIHUDTabMinimapDomination::~UIHUDTabMinimapDomination()
{
	for (size_t i=0; i<m_units.size(); i++)
	{
		I3_SAFE_DELETE( m_units[i]);
	}
	m_units.clear();

	m_ranking.clear();
}

void UIHUDTabMinimapDomination::_InitializeAtLoad(i3UIScene* scene)
{
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)) return;

	UIHUDBase::_InitializeAtLoad(scene);

	m_bUserInfoUpdatable = true;

	_CreateMinimap(scene);
	_CreateUnits();
	_InitSlotControls(scene);
}

void UIHUDTabMinimapDomination::_CreateMinimap(i3UIScene* scene)
{
	if (Minimap::GetInstance())
	{
		if (MyRoomInfoContext::i()->getStage()->IsHudOff() == false)
		{
			i3UIFrameWnd* frame = reinterpret_cast<i3UIFrameWnd*>(scene->FindChildByName("MinimapImageFrame"));
			m_bMinimapUpdatable = Minimap::GetInstance()->Create(frame);
		}
	}
}

void UIHUDTabMinimapDomination::_CreateUnits()
{
	if (!_IsMinimapUpdatable()) return;

	INT32 slotIdx[maxSlotNum] = {0, 2, 4, 6};

	for (INT32 i=0; i<maxSlotNum; i++)
	{
		if (BattleSlotContext::i()->getMySlotIdx() == slotIdx[i])
		{
			UnitCharaMe * p = new UnitCharaMe(slotIdx[i]);
			MEMDUMP_NEW( p, sizeof( UnitCharaMe));
			m_units.push_back( p);
		}
		else 
		{
			UnitCharaFellow * p = new UnitCharaFellow(slotIdx[i]);
			MEMDUMP_NEW( p, sizeof( UnitCharaFellow));
			m_units.push_back( p);
		}

		m_ranking.push_back(slotIdx[i]);
	}

	for (INT32 d=0; d<MAX_COUNT_DOMI_DINO; d++)
	{
		UnitDomiDino * p = new UnitDomiDino(d);
		MEMDUMP_NEW( p, sizeof( UnitDomiDino));
		m_units.push_back( p);
	}
}

void UIHUDTabMinimapDomination::_InitSlotControls(i3UIScene* scene)
{
	char name[30] = {0,};
	for (INT32 i=0; i<maxSlotNum; i++)
	{
		i3::snprintf(name, 30, "Slot%d", i);
		m_slot[i].slot				= static_cast<i3UIFrameWnd*>(scene->FindChildByName(name));			::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "BackImage%d", i);
		m_slot[i].backImage			= static_cast<i3UIImageBox*>(scene->FindChildByName(name));			::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "DeadImage%d", i);
		m_slot[i].deadImage			= static_cast<i3UIImageBox*>(scene->FindChildByName(name));			::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "Rank%d", i);
		m_slot[i].rank				= static_cast<i3UIButtonImageSet*>(scene->FindChildByName(name));	::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "NickName%d", i);
		m_slot[i].name				= static_cast<i3UIStaticText*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "HPTitle%d", i);
		m_slot[i].hpTitle			= static_cast<i3UIStaticText*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "HPBar%d", i);
		m_slot[i].hpBar				= static_cast<i3UIProgressBar*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "HPBarBackImage%d", i);
		m_slot[i].hpBarBackImage	= static_cast<i3UIImageBox*>(scene->FindChildByName(name));			::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "SPTitle%d", i);
		m_slot[i].spTitle			= static_cast<i3UIStaticText*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "SPBar%d", i);
		m_slot[i].spBar				= static_cast<i3UIProgressBar*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "SPBarBackImage%d", i);
		m_slot[i].spBarBackImage	= static_cast<i3UIImageBox*>(scene->FindChildByName(name));			::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "ScoreTitle%d", i);
		m_slot[i].scoreTitle		= static_cast<i3UIStaticText*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "Score%d", i);
		m_slot[i].score				= static_cast<i3UIStaticText*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "WeaponNameTitle%d", i);
		m_slot[i].weaponTitle		= static_cast<i3UIStaticText*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
		i3::snprintf(name, 30, "WeaponName%d", i);
		m_slot[i].weapon			= static_cast<i3UIStaticText*>(scene->FindChildByName(name));		::memset(name, 0, sizeof(char)*30);
	}

	m_stageInfo.serverChannelRoom	= static_cast<i3UIStaticText*>(scene->FindChildByName("ServerChannelRoom"));
	m_stageInfo.dayWaveDesc			= static_cast<i3UIStaticText*>(scene->FindChildByName("DayWaveDesc"));
	m_stageInfo.mapName				= static_cast<i3UIStaticText*>(scene->FindChildByName("MapName"));
	m_stageInfo.wave				= static_cast<i3UIStaticText*>(scene->FindChildByName("Wave"));
	m_stageInfo.curWave				= static_cast<i3UIStaticText*>(scene->FindChildByName("CurWave"));
	m_stageInfo.maxWave				= static_cast<i3UIStaticText*>(scene->FindChildByName("MaxWave"));
	m_stageInfo.missionDesc			= static_cast<i3UIStaticText*>(scene->FindChildByName("MissionDesc"));
}

void UIHUDTabMinimapDomination::OnCreate(i3GameNode* nodeParent)
{
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)) return;

	UIHUDBase::OnCreate(nodeParent);

	AddScene("Scene/Domination/PBRe_Domi_Tap.i3UIs", nullptr, true, false);
}

void UIHUDTabMinimapDomination::OnUpdate(REAL32 tm)
{
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)) return;

	if (_IsActivated())
	{
		UIHUDBase::OnUpdate(tm);

		if (_IsMinimapUpdatable())
		{
			Minimap::GetInstance()->Update(tm);

			for (size_t i= 0; i<m_units.size(); i++)
				m_units[i]->Update(tm);
		}

		UpdateInfo();
	}
}	

void UIHUDTabMinimapDomination::UpdateInfo()
{
	if (_IsUserInfoUpdatable())
	{
		_SetStageInfo();
		_SetUserInfo();
	}
}

bool UIHUDTabMinimapDomination::OnEntranceStart(void* arg1, void* arg2)
{
	if (!_IsUserInfoUpdatable() || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)) return false;

	if (UIHUDBase::OnEntranceStart(arg1, arg2) == false)
		return false;

	m_bActivate = true;

	return true;
}

void UIHUDTabMinimapDomination::_SetStageInfo()
{
	if( domi_state_info::i() == nullptr)
		return;

	COLOR red; red.r = 255; red.g = 0; red.b = 0; red.a = 255;

	ROOM_INFO_BASIC roomInfo; MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

	i3::stack_wstring wstr;
	i3::wstring tmp = L"";

	i3::sprintf(wstr, GAME_RCSTRING("STR_POPUP_WHAT_SERVER"), g_pServerInfo->GetCurGameServerIndex()+1);
	tmp += L" ";	tmp	+= wstr;

	i3::sprintf(wstr, GAME_RCSTRING("STR_POPUP_WHAT_CHANNEL"), g_pServerInfo->GetCurChannelNumber()+1);
	tmp += L" ";	tmp	+= wstr;

	i3::sprintf(wstr, GAME_RCSTRING("STR_POPUP_WHAT_ROOM"), roomInfo._Idx+1);
	tmp += L" ";	tmp	+= wstr;

	m_stageInfo.serverChannelRoom->SetText(tmp);


	
	tmp = GAME_RCSTRING("STR_DOMI_ADD_LIST_4");


	i3::sprintf(wstr, L" %d / %d", domi_state_info::i()->wave_count, domi_state_info::i()->max_wave);
	tmp += wstr;
	m_stageInfo.dayWaveDesc->SetTextColor(&red);
	m_stageInfo.dayWaveDesc->SetText(tmp);

	i3::rc_wstring stage_name = CStageInfoUtil::GetMapData(roomInfo._StageID)->getDisplayName();
	m_stageInfo.mapName->SetText( stage_name );			// TODO : 서버 협업후 반드시 변경해야함. (2014.07.18.수빈)

	m_stageInfo.wave->SetText(GAME_RCSTRING("STR_DOMI_ADD_LIST_4"));
	
	i3::sprintf(wstr, L"%d ", domi_state_info::i()->wave_count);

	m_stageInfo.curWave->SetTextColor(&red);
	m_stageInfo.curWave->SetText(wstr);


	i3::sprintf(wstr, L"/ %d", domi_state_info::i()->max_wave);

	m_stageInfo.maxWave->SetTextColor(&red);
	m_stageInfo.maxWave->SetText(wstr);
	m_stageInfo.missionDesc->SetText(L"");

}

void UIHUDTabMinimapDomination::_SetUserInfo()
{	
	struct fn
	{
		bool operator()(const INT32& l_slotIdx, const INT32& r_slotIdx) const
		{
			UINT32 l_score = domi_state_info::i()->total_score[l_slotIdx/2];
			UINT32 r_score = domi_state_info::i()->total_score[r_slotIdx/2];

			if (l_score > r_score) return true;
			return false;
		}
	};
	std::sort(m_ranking.begin(), m_ranking.end(), fn());

	_HideAllSlot();
	INT32 slotNum = 0;
	for (size_t i=0; i<m_ranking.size(); i++)
	{
		INT32 userSlotIdx = m_ranking[i];
		if (tu::CanBeShown(userSlotIdx))
		{
			_ShowSlot(slotNum, userSlotIdx);
			slotNum++;
		}
	}
}

void UIHUDTabMinimapDomination::_ShowSlot(INT32 num, INT32 slotIdx)
{
	m_slot[num].backImage->SetVisible(true);

	if (tu::IsDead(slotIdx))
		m_slot[num].deadImage->EnableCtrl(true);

	m_slot[num].slot->SetVisible(true);
	if (slotIdx == BattleSlotContext::i()->getMySlotIdx())
		m_slot[num].slot->SetTemplateByName("FrameWnd_Domi_Tap_box02");
	else
		m_slot[num].slot->SetTemplateByName("FrameWnd_Domi_Tap_box03");

	m_slot[num].rank->SetVisible(true);
	m_slot[num].rank->SetShapeIdx(BattleSlotContext::i()->getSlotInfo(slotIdx)->_Rank);

	m_slot[num].name->SetVisible(true);
	m_slot[num].name->SetText(BattleSlotContext::i()->getNickForSlot(slotIdx));

	m_slot[num].hpTitle->SetVisible(true);
	m_slot[num].hpTitle->SetText("HP");
	m_slot[num].hpBar->SetVisible(true);
	m_slot[num].hpBar->setProgress(tu::GetHpRatio(slotIdx));
	m_slot[num].hpBar->setModifyState(true);
	m_slot[num].hpBarBackImage->SetVisible(true);
	m_slot[num].hpBarBackImage->setShapeVisible(0, true);

	m_slot[num].spTitle->SetVisible(true);
	m_slot[num].spTitle->SetText("SP");
	m_slot[num].spBar->SetVisible(true);
	REAL32 sp = static_cast<REAL32>(domi_state_info::i()->sp.sp[slotIdx/2]);
	REAL32 spRatio = sp / 10000.0f;
	m_slot[num].spBar->setProgress((spRatio > 1.0f) ? 1.0f : spRatio);
	m_slot[num].spBar->setModifyState(true);
	m_slot[num].spBarBackImage->SetVisible(true);
	m_slot[num].spBarBackImage->setShapeVisible(0, true);

	m_slot[num].scoreTitle->SetVisible(true);
	m_slot[num].scoreTitle->SetText(GAME_RCSTRING("STR_DOMI_ADD_LIST_6"));
	m_slot[num].score->SetVisible(true);

	i3::stack_wstring wstr_score; i3::sprintf(wstr_score, L"%d", domi_state_info::i()->total_score[slotIdx/2]);
	m_slot[num].score->SetText(wstr_score);

	m_slot[num].weaponTitle->SetVisible(true);
	m_slot[num].weaponTitle->SetText(GAME_RCSTRING("STR_TBL_BATTLEGUI_PRIMARY_WEAPON"));
	m_slot[num].weapon->SetVisible(true);

	i3::rc_wstring wstrWeaponName;
	tu::GetWeaponName(slotIdx, WEAPON_SLOT_PRIMARY, wstrWeaponName);
	m_slot[num].weapon->SetText(wstrWeaponName);
}

void UIHUDTabMinimapDomination::_HideAllSlot()
{
	for (INT32 i=0; i<maxSlotNum; i++)
		_HideSlot(i);
}

void UIHUDTabMinimapDomination::_HideSlot(INT32 num)
{
	m_slot[num].backImage->SetVisible(true);
	m_slot[num].deadImage->EnableCtrl(false);	// SetVisible로 했더니 안먹힌다.
	m_slot[num].rank->SetVisible(false);
	m_slot[num].name->SetVisible(false);
	m_slot[num].hpTitle->SetVisible(false);
	m_slot[num].hpBar->SetVisible(false);
	m_slot[num].hpBarBackImage->SetVisible(false);
	m_slot[num].spTitle->SetVisible(false);
	m_slot[num].spBar->SetVisible(false);
	m_slot[num].spBarBackImage->SetVisible(false);
	m_slot[num].scoreTitle->SetVisible(false);
	m_slot[num].score->SetVisible(false);
	m_slot[num].weaponTitle->SetVisible(false);
	m_slot[num].weapon->SetVisible(false);
}

void UIHUDTabMinimapDomination::Show(bool activate)
{
	if (!_IsUserInfoUpdatable() || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)) return;

	if (_IsActivated() != activate)
	{
		SetVisible(activate);

		UpdateInfo();

		UIHudManager* hudMgr 
			= static_cast<UIHudManager*>(UIBattleFrame::i()->GetHUD(UIHUD_MANAGER));
		if (hudMgr)
		{
			CBattleHUD_MiniMap::instance()->SetEnableMiniMap(!activate);
			hudMgr->EnableHud(HUD::MINIMAP, !activate);
		}

		m_bActivate = !m_bActivate;
	}
}
