#pragma once

#include "iHudBase.h"
#include "BattleHUD_Define.h"
#include "UIHudDefine.h"

class UIHudManager;

class UIHudC4Indicator : public iHudBase
{
public:
	UIHudC4Indicator(UIHudManager * p);
	~UIHudC4Indicator();

	
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;

	void SetEnableAvailablePositionImage(bool isEnable);
	void SetEnableInstalledImage(bool isEnable);
	void SetEnableC4Progress(bool isEnable);
	void SetC4TimerColor(COLOR color);

	i3UIStaticText *	GetC4Timer()					{	return m_pC4Count; }
	i3UIProgressBar *	GetC4ProgressBar()				{	return m_pC4Progress; }
	i3UIImageBox *		GetC4AvailablePosImage()		{	return m_pC4AvailablePosImage; }

private:
	i3UIImageBox * m_pC4AvailablePosImage;
	i3UIImageBox * m_pC4InstalledImage;
	i3UIStaticText * m_pC4Count;
	i3UIProgressBar * m_pC4Progress;
	i3UIFrameWnd * m_pC4ProgressBG;

};