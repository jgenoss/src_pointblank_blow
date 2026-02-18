#include "pch.h"
#include "UIHUDEscape.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIMath.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( UIHUDEscape); //, i3GameObjBase);

UIHUDEscape::UIHUDEscape(  )
{

}

UIHUDEscape::~UIHUDEscape( void )
{

}

void UIHUDEscape::OnCreate( i3GameNode * pParent )
{
	UIBattlePopupBase::OnCreate( pParent);

	AddScene( "Scene/HUD/PBRe_HUD_DinoEscape.i3UIs" );
}


void UIHUDEscape::SetInfo()
{
	INT32 iImg, iteam = 0;
	STAGE_UID StageUID = MyRoomInfoContext::i()->getStage()->GetStageUID();
	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	iteam = BattleSlotContext::i()->getMyTeam();

	iImg = g_pFramework->GetEscapeMapShapeIdx(StageUID);

	i3::rc_wstring name, skill1, skill2;
	if (pMyChara->IsDino())
	{
		if (i3::kind_of<CGameCharaDinoSting*>(pMyChara)) //->IsSting() || pMyChara->IsCCSting())
		{
			if (i3::same_of<CGameCharaDinoStingMars*>(pMyChara))
				name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_MARS");
			else
				name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_STING");

			skill1 = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_PRICKLE");
			skill2 = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_GAS");
		}
		else if (i3::kind_of<CGameCharaDinoRaptor*>(pMyChara)) // (pMyChara->IsRaptor() || pMyChara->IsCCRaptor() || pMyChara->IsRaptorBlue() || pMyChara->IsRaptorGreen())
		{
			if (i3::same_of<CGameCharaDinoRaptorMercury*>(pMyChara))
				name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_MERCURY");
			else
				name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_RAPTOR");

			skill1 = GAME_RCSTRING("STR_TBL_INGAME_HUD_RAPTER_ATTACK_A");
			skill2 = GAME_RCSTRING("STR_TBL_INGAME_HUD_RAPTER_ATTACK_B");
		}
		else if (i3::kind_of<CGameCharaDinoTank*>(pMyChara)) // (pMyChara->IsTank())
		{
			name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_TANK");
			skill1 = GAME_RCSTRING("STR_TBL_INGAME_HUD_TANK_ATTACK_A");
			skill2 = GAME_RCSTRING("STR_TBL_INGAME_HUD_TANK_ATTACK_B");

		}
		else if (i3::kind_of<CGameCharaDinoAcid*>(pMyChara)) // (pMyChara->IsAcid() || pMyChara->IsCCAcid())							)
		{
			if (i3::same_of<CGameCharaDinoAcidVulcan*>(pMyChara))
				name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_VULCAN");
			else
				name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_ACID");

			skill1 = GAME_RCSTRING("STR_TBL_INGAME_HUD_ACID_ATTACK_A");
			skill2 = GAME_RCSTRING("STR_TBL_INGAME_HUD_ACID_ATTACK_B");

		}
		else if (i3::kind_of<CGameCharaDinoElite*>(pMyChara)) // (pMyChara->IsElite())
		{
			name = GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_ELITE");
			skill1 = GAME_RCSTRING("STR_TBL_INGAME_HUD_ELITE_ATTACK_A");
			skill2 = GAME_RCSTRING("STR_TBL_INGAME_HUD_ELITE_ATTACK_B");

		}
	}

	LuaState * L = 0;
	L = _CallLuaFunction("SetInfo");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, iImg);
		i3Lua::PushInteger(L, iteam);

		i3Lua::PushStringUTF16To8(L, name);
		i3Lua::PushStringUTF16To8(L, skill1);
		i3Lua::PushStringUTF16To8(L, skill2);
		_EndLuaFunction(L, 5);
	}
}

void UIHUDEscape::OnUpdate( REAL32 rDeltaSeconds )
{
	UIHudUsurpation::OnUpdate(rDeltaSeconds);
}

