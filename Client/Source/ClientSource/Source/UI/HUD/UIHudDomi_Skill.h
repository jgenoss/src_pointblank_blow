#pragma once

#include "UIHUD_Domination_Base.h"
#include "..\..\GaugeSyncTimer.h"
#include "CoolTimeGuage.h"

#define MAX_SP_GAUGE_DEFAULT 10000

class CUIHudDomi_Skill : public UIHUDDominationBase
{
	I3_CLASS_DEFINE( CUIHudDomi_Skill, UIHUDDominationBase);

public:
	CUIHudDomi_Skill();
	virtual ~CUIHudDomi_Skill();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scene) override;
	virtual void OnUpdate(REAL32 tm) override;

	void WarningNoUse(const i3::wliteral_range& reason);
	void WarningNoUse(const i3::wliteral_range& reason, const INT32 cost);
	void WarningNoUse(const i3::wliteral_range& str1, const i3::wliteral_range& str2);
	void AllowTheUse(const i3::wliteral_range& type, INT16 cost = -1);
	void HideNotice(bool force = false);
	
	void VisibleGauge( bool visible);
	void SetSPGauge(UINT16 sp);

	void  SetSPMax(UINT16 max);
	INT32 GetSPMax() const	{ return m_spMax; }

private:
	bool m_isVisibleWarningNoUse;
	bool m_isVisibleAllowTheUse;
	
	UINT16 m_spMax;
	UINT16 m_spCurr;

	i3UIFrameWnd* m_frame_gauge;
};
