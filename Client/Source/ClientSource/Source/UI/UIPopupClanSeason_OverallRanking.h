#pragma once

#include "UIPopupBase.h"

class UIPopupClanSeason_OverallRanking : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupClanSeason_OverallRanking, UIPopupBase);

public:
	UIPopupClanSeason_OverallRanking();
	virtual ~UIPopupClanSeason_OverallRanking();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual bool OnEntranceStart(void* pArg1 = nullptr, void* pArg2 = nullptr) override;
};