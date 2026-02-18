#include "pch.h"
#include "UIHudCrossCounterVS.h"

#include "UIHudUtil.h"
#include "UIHudManager.h"

#include "../../CrosshairMgr.h"
#include "../../Crosshair.h"

#include "../StageBattle/ScoreContext.h"
#include "../StageBattle/IntrudeContext.h"
#include "BattleSlotContext.h"

namespace
{
	void InitControls(CCModeVSControls* control, i3UIScene* scene)
	{
		control->roundNameImg		= static_cast<i3UIImageBox*>(scene->FindChildByName("RoundNameBackImg"));
		control->roundText			= static_cast<i3UIStaticText*>(scene->FindChildByName("RoundNameText"));
		control->leftTeamImg		= static_cast<i3UIButtonImageSet*>(scene->FindChildByName("LeftTeamImg"));
		control->leftTeamNameImg	= static_cast<i3UIButtonImageSet*>(scene->FindChildByName("LeftTeamNameImg"));
		control->rightTeamImg		= static_cast<i3UIButtonImageSet*>(scene->FindChildByName("RightTeamImg"));
		control->rightTeamNameImg	= static_cast<i3UIButtonImageSet*>(scene->FindChildByName("RightTeamNameImg"));
	}

	void SetRoundInfo(CCModeVSControls* control)
	{
		control->roundNameImg->EnableCtrl(true);

		char str[50] = {0,};
	#if defined( LOCALE_ITALY )	  // 59276 이태리 퍼블리셔측 요청에 의한 라운드 메세지 수정(ROUND 숫자 -> FASE 숫자)
		::sprintf_s(str, sizeof(str), "FASE %1d", ScoreContext::i()->getCurrentRound() + 1);
	#else
		::sprintf_s(str, 50, "ROUND %1d", ScoreContext::i()->getCurrentRound()+1);
	#endif
		control->roundText->EnableCtrl(true);
		control->roundText->SetText(str);

		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

		INT32 red = -1, blue = -1;
		if( myPlayer != nullptr) hu::get_team_mark(myPlayer, red, blue);

		if( red == -1 )
		{
			control->leftTeamImg->EnableCtrl(false);	control->leftTeamNameImg->EnableCtrl(false);
		}
		else
		{
			control->leftTeamImg->EnableCtrl(true);	control->leftTeamNameImg->EnableCtrl(true);

			control->leftTeamImg->SetShapeIdx(red);	control->leftTeamNameImg->SetShapeIdx(red);
		}

		if( blue == -1 )
		{
			control->rightTeamImg->EnableCtrl(false);	control->rightTeamNameImg->EnableCtrl(false);
		}
		else
		{
			control->rightTeamImg->EnableCtrl(true);	control->rightTeamNameImg->EnableCtrl(true);

			control->rightTeamImg->SetShapeIdx(blue);	control->rightTeamNameImg->SetShapeIdx(blue);
		}
	}
}

UIHudCrossCounterVS::UIHudCrossCounterVS(UIHudManager* p) : iHudBase(p)
{
}

UIHudCrossCounterVS::~UIHudCrossCounterVS()
{
}

/*********************************/
void UIHudCrossCounterVS::InitializeAtLoad( i3UIScene * pScene)
{
	iHudBase::AddFrameWnd(pScene, "CCmode");

	InitControls(&m_controls, pScene);
}

void	UIHudCrossCounterVS::Update( REAL32 rDeltaSeconds)
{
}

/*********************************/
//_Process_EVENT_START_BATTLE 에서 HUDMgr이 true됨
void UIHudCrossCounterVS::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetVisible(false);
}

/*virtual*/ void UIHudCrossCounterVS::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	if( IntrudeContext::i()->IsIntrusionMessage()) return;

	bool cc_mode = hu_ut::is_cross_count_mode();
	iHudBase::SetEnable( cc_mode );
	if( !cc_mode )	return;

	SetRoundInfo(&m_controls);
	g_pSndMng->StartAnnounceMessage(GTA_TRAINING_LEVELUP);
	CrosshairMgr::i()->getCrosshair()->SetEnable(false);
}

/*virtual*/ void UIHudCrossCounterVS::event_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetVisible(false);
	CrosshairMgr::i()->getCrosshair()->SetEnable(true);
}