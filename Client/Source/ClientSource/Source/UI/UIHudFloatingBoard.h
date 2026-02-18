#pragma once

#include "iHudBase.h"

#include "UISlide.h"

class UIHudManager;

class UIHudFloatingBoard : public iHudBase
{
public:
	UIHudFloatingBoard(UIHudManager* p);
	virtual ~UIHudFloatingBoard();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void ClearAtUnload( i3UIScene * pScene) override;

	virtual void Update( REAL32 rDeltaSeconds) override;

	virtual void SetParameter(INT32 arg1, INT32 arg2 = -1) override;

private:
	void OnRevive( i3RenderContext * pCtx);
	void event_start_battle(INT32 arg, const i3::user_data& UserData);

	void slide_proc();
	UI::TCallBack<UIHudFloatingBoard, &UIHudFloatingBoard::slide_proc> m_slide_call;

private:
	i3UIStaticText* m_StaticText;
	UI::tmSlide m_Slide;
};
