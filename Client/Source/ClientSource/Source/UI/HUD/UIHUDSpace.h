#pragma once

#include "UI/UIBattlePopupBase.h"

class UIHudSpace : public UIBattlePopupBase
{
	I3_CLASS_DEFINE(UIHudSpace, UIBattlePopupBase);
public:
	UIHudSpace();
	virtual ~UIHudSpace(void);

public:
	virtual void	OnCreate(i3GameNode * pParent) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart(void) override;

	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;
	virtual bool	OnKey_Escape() override;

public:
	void SetInfo();


protected:
	i3UIProgressBar* pProgressbar;

	REAL32	open_time;
	REAL32	target_time;

	bool close_popup;
	bool enable_progressbar;
};