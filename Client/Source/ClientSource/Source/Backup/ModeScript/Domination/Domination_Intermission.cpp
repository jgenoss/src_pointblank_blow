#include "pch.h"

#ifdef	DOMI_FLOW

#include "Domination_Intermission.h"
#include "..\..\UI\HUD\UIHUD_Domination_Manager.h"
#include "../../Ui/UIBattleFrame.h"
#include "../GameStateMgr.h"

CDominationIntermission::CDominationIntermission( gmode_domination* pmode) : 
	CDominationFlowBase( pmode),
	m_rOutputLabelTime( 0.0f), m_rInputIntervalTime( 0.0f)
{
}

CDominationIntermission::~CDominationIntermission(void)
{
}

//--------------------------------------
// 이벤트 처리 함수들

/*virtual*/ void CDominationIntermission::StartFlow()
{
	UIBattleFrame::i()->SetIntermissionStart();

	m_rOutputLabelTime		= 3.0f;
	m_rInputIntervalTime	= 0.0f;

	tinst<UIHUDDominationMgr>()->VisibleHUD( HUD_DOMI_SKILL, FALSE);
	tinst<UIHUDDominationMgr>()->EnableHUD(HUD_DOMI_MIDDLE_SCORE, FALSE);
}

/*virtual*/ void CDominationIntermission::EndFlow()
{
	UIBattleFrame::i()->SetIntermissionEnd();

	tinst<UIHUDDominationMgr>()->EnableHUD(HUD_DOMI_MIDDLE_SCORE, FALSE);
}

void CDominationIntermission::ProcessEventKey( INT32 arg)
{
	// 키보드 입력에 대해서 처리
	GAME_KEY_MAP	keymap	= g_pGameContext->GetKeyNetwork( arg);

	switch( keymap)
	{
	case	GAME_KEY_MAP_SPECIALACTION :
//#ifdef DOMI_HUD
//		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INTERMISSION, HUD_DMN_EVT_IM_SET_READY);
//#endif
		break;
	}
}


//--------------------------------------
// 업데이트 함수들
void CDominationIntermission::OnUpdate( REAL32 rDeltaSeconds)
{
	if( m_rOutputLabelTime > 0.0f)
	{
		m_rOutputLabelTime	-= rDeltaSeconds;

		if( m_rOutputLabelTime <= 0.0f)
		{
//#ifdef DOMI_HUD
//			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INTERMISSION, HUD_DMN_EVT_IM_DISABLELABEL);
//#endif
			m_rInputIntervalTime	= 0.0f;
		}
	}
	else
	{
		if( m_rInputIntervalTime > 0.0f) return;

		m_rInputIntervalTime	-= rDeltaSeconds;

		UINT64 keyStroke = g_pFramework->getKeyStroke();

		if( keyStroke & GAME_KEY_ACTION_SPECIAL )
		{
			g_pGameContext->PushKeyNetwork( GAME_KEY_MAP_SPECIALACTION);
			g_pGameContext->SetEvent( EVENT_KEY_PROCESS_NET );

			m_rInputIntervalTime	= 0.5f;	// 키연속으로 누르는 시간 0.5초
		}
	}
}

#endif