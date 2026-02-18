#include "pch.h"
#include "Cheatkey_LimitPred.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "../UI/UIMainFrame.h"

namespace Cheatkey
{
	//----------------------------------------------
	// 사용 제한 없음
	//----------------------------------------------
	bool LPred_Anywhere::operator()() { return true; }

	//----------------------------------------------
	// 인게임 전용
	//----------------------------------------------
	bool LPred_IngameOnly::operator()()
	{
		ISTEST_S
		{
			i3Framework* frmWork = i3Framework::getCurrentFramework();
			i3StageInfo* startStageInfo = frmWork->GetStartStageInfo(); // 바로가기로 맵로딩테스트로 실행된상태입니다.

			if(startStageInfo)
				return true;
		}

		return	(BattleSlotContext::i()->getSlotInfo(BattleSlotContext::i()->getMySlotIdx())->_State == SLOT_STATE_BATTLE) ? true : false;
	}	

	//----------------------------------------------
	// 아웃게임 전용 : 로비, 레디룸, 샵 등등...
	//----------------------------------------------
	bool LPred_OutgameOnly::operator()()
	{
		UINT8 state = BattleSlotContext::i()->getSlotInfo(BattleSlotContext::i()->getMySlotIdx())->_State;

		if (SLOT_STATE_LOAD == state)			return false;
		if (SLOT_STATE_RENDEZVOUS == state)		return false;
		if (SLOT_STATE_PRESTART == state)		return false;
		if (SLOT_STATE_BATTLE_LOADOK == state)	return false;
		if (SLOT_STATE_BATTLE_READY == state)	return false;
		if (SLOT_STATE_BATTLE == state)			return false;

		return true;
	}

	//----------------------------------------------
	// 로비 전용
	//----------------------------------------------
	bool LPred_LobbyOnly::operator()()
	{
		UIMainFrame* stage = g_pFramework->GetUIMainframe();
		if (stage)
		{
			if (stage->GetCurrentPhaseType() == UIPHASE_LOBBY) return true;
		}

		return false;
	}

	//----------------------------------------------
	// 클랜 로비 전용
	//----------------------------------------------
	bool LPred_ClanLobbyOnly::operator()()
	{
		UIMainFrame* stage = g_pFramework->GetUIMainframe();
		if (stage)
		{
			if (stage->GetCurrentPhaseType() == UIPHASE_CLANLOBBY) return true;
		}

		return false;
	}

	//----------------------------------------------
	// 데코레이터
	//----------------------------------------------
	ILimitPredDecorator::ILimitPredDecorator(ILimitPred* p) : m_pred(p) {}
	ILimitPredDecorator::~ILimitPredDecorator()
	{
		I3_SAFE_DELETE(m_pred);
	}

	bool ILimitPredDecorator::operator()()
	{
		 return m_pred->operator()();
	}
	//----------------------------------------------
	// GM 전용
	//----------------------------------------------
	bool LPred_GMOnly::operator()()
	{
		return ILimitPredDecorator::operator()() && UserInfoContext::i()->IsGM_Observer();
	}

	//----------------------------------------------
	// 치트 사용 가능 계정 전용
	//----------------------------------------------
	bool LPred_PermittedAccountOnly::operator()()
	{
#if !defined(I3_DEBUG) && !defined(BUILD_RELEASE_QA_VERSION)
		return ILimitPredDecorator::operator()() && UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT);
#else
		return ILimitPredDecorator::operator()();
#endif
	}
}