#if !defined( _UIHUDDOMI_ROUND_RESULT_H_)
#define _UIHUDDOMI_ROUND_RESULT_H_

//#ifdef	DOMI_ROUND_RESULT

#include "UIHUD_Domination_Base.h"

class DigitNumber;

class CUIHudDomi_RoundResult : public UIHUDDominationBase
{
	I3_CLASS_DEFINE(CUIHudDomi_RoundResult, UIHUDDominationBase);

public:
	CUIHudDomi_RoundResult();
	virtual ~CUIHudDomi_RoundResult();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scene) override;
	virtual void OnUpdate(REAL32 tm) override;
	virtual bool OnEntranceStart(void* pArg1 = nullptr, void* pArg2 = nullptr) override;
	virtual void OnEntranceEnd() override;
	virtual bool OnExitStart() override;

private:
	void	_SetupContainer();
	void	_SetFlowInfo();
	void	_SetPlayResult();
	void	_SetScores();
	void	_SetRankUp();
	INT32	_GetExpMax(INT32 rank);
	void	_SetRewardItem();

private:
	i3::vector<DomiDefs::ROUND_RESULT_INFO> m_results;
	i3UIFrameWnd*	m_scoreWindow;
	i3UIFrameWnd*	m_slotBorderLine;
	DigitNumber*	m_totalScore;
};

//#endif

#endif