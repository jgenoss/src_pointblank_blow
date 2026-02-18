#ifndef _UIHUDDOMI_WAVE_RESULT_H_
#define _UIHUDDOMI_WAVE_RESULT_H_

#include "UIHUD_Domination_Base.h"

class CUIHudDomi_WaveResult : public UIHUDDominationBase
{
	I3_CLASS_DEFINE(CUIHudDomi_WaveResult, UIHUDDominationBase);

public:
	CUIHudDomi_WaveResult();
	virtual ~CUIHudDomi_WaveResult();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scene) override;
	virtual void OnUpdate(REAL32 tm) override;

	virtual bool OnEntranceStart(void* arg1, void* arg2) override;
	virtual void OnEntranceEnd() override;
};

#endif