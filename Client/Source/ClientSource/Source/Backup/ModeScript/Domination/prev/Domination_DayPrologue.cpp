#include "pch.h"

#ifdef	DOMI_FLOW

#include "Domination_DayPrologue.h"
#include "..\..\UI\HUD\UIHUD_Domination_Manager.h"
#include "../../UI/UIBattleFrame.h"

CDominationDayPrologue::CDominationDayPrologue( gmode_domination* pmode) : 
	CDominationFlowBase( pmode)
{
}

CDominationDayPrologue::~CDominationDayPrologue(void)
{
}


void CDominationDayPrologue::ProcessEventRoundStart(void)
{
	UIBattleFrame::i()->SetIntermissionStart();
#ifdef DOMI_HUD
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_MESSAGE, HUD_DMN_EVT_MESSAGE_DAY, TRUE );
#endif
}

void CDominationDayPrologue::ProcessEventRoundEnd()
{
	UIBattleFrame::i()->SetIntermissionEnd();
#ifdef DOMI_HUD
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_MESSAGE, HUD_DMN_EVT_MESSAGE_DISABLE, UIHUDDominationMessage::MSG_DAY );
#endif
}

#endif