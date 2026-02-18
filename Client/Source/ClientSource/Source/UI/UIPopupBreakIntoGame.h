#pragma once

#include "UIPopupBase.h"

class UIPopupBreakIntoGame : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupBreakIntoGame, UIPopupBase);

public:
	void clicked(int idx) const;

public:
	UIPopupBreakIntoGame();
	virtual ~UIPopupBreakIntoGame();

	virtual void OnCreate( i3GameNode * pParent) override;

	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual	void OnLoadAllScenes() override;

	virtual bool OnKey_Enter( void) override;
};