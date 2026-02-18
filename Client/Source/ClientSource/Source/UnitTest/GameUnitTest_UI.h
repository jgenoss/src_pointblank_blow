#pragma once
#if ENABLE_UNIT_TEST

#include "UI/UIDefine.h"
#include "GameUnitTestInfo.h"

// 유닛테스트 페이지별 스텝.
enum UNITTEST_UI_STEP
{
	UT_UI_STEP_START = 0,
	UT_UI_STEP0,
	UT_UI_STEP1,
	UT_UI_STEP2,
	UT_UI_STEP3,
	UT_UI_STEP4,

	UT_UI_STEP_MAX
};

class UIMainFrame;
class UIFramework;

class CGameUnitTest_UI : public i3ElementBase
{
	I3_CLASS_DEFINE(CGameUnitTest_UI, i3ElementBase);

private:
	REAL32				m_rChangeStepTime;
	REAL32				m_rEventSendTime;

	bool				m_bRandomEvent;
	bool				m_bEnterSubPhase;

	UNITTEST_UI_STEP	m_eCurrentStep;		
	UNITTEST_UI_STEP	m_eOldStep;			// 서브페이지로 이동시 이전 스텝 저장용

	INT32				m_i32SubPhaseLoopCnt;

	bool				m_bBattleProcess;

	//PopupList per Phase
	GameUnitTest::PopupList m_PhasePopupList[UIPHASE_MAX];

	//PopupList per SubPhase
	GameUnitTest::PopupList m_SubPhasePopupList[USP_MAX];

	void _RegisterPopupList();
	void _SetStage();


protected:
	void _ChangeUIPhase(void);	
	void _SendRandomEvent(void);

protected:
	//////// UI Phase Test에 사용되는 함수 //////
	void _OnUpdate_PhaseTest(REAL32 rDeltaSeconds);
	void _SendGameEvent(void);

	void _ChangeNextStep(void);
	void _ChangeNextStep_PhaseServerList(void);
	void _ChangeNextStep_PhaseCreateChara(void);
	void _ChangeNextStep_PhaseLobby(void);
	void _ChangeNextStep_PhaseClanLobby(void);
	void _ChangeNextStep_PhaseReadyRoom(void);
	void _ChangeNextStep_SubPhaseInven(void);
	void _ChangeNextStep_SubPhaseShop(void);

	void _StartSubPhaseTest(void);
	void _ChangSubPhaseTest(void);
	/////////////////////////////////////////////

public:
	CGameUnitTest_UI(void);
	virtual ~CGameUnitTest_UI(void);

	void OnUpdate(REAL32 rDeltaSeconds);
	void JumpToReady(bool bStartPhase);	

	bool IsBattleProcess(void) const	{ return m_bBattleProcess; }
	void SetBattleProcess(bool bFlag)	{ m_bBattleProcess = bFlag; }
};

#endif //end of ENABLE_UNIT_TEST