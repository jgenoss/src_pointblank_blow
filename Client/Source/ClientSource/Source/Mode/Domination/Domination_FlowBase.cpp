#include "pch.h"

#include "Domination_FlowBase.h"
#include "../../Source/ui/hud/UIHUD_Domination_Manager.h"

CDominationFlowBase::CDominationFlowBase( gmode_domination* pmode)
{
	m_pmode		= pmode; 
}

CDominationFlowBase::~CDominationFlowBase(void)
{
}

/*virtual*/ void CDominationFlowBase::StartFlow()
{
	//UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_SKILL, HUD_DMN_EVT_SKILL_SP_CHARGE, true);
}

/*virtual*/ void CDominationFlowBase::EndFlow()
{
	//UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_SKILL, HUD_DMN_EVT_SKILL_SP_CHARGE, false);
	//UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_CLEAR_INDICATIONS, -1);
}