#pragma once

#include "UIPopupBase.h"

class UIPopupClanSeason_IndivisualRanking : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupClanSeason_IndivisualRanking, UIPopupBase);

public:
	UIPopupClanSeason_IndivisualRanking();
	virtual ~UIPopupClanSeason_IndivisualRanking();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual bool OnEntranceStart(void* pArg1 = nullptr, void* pArg2 = nullptr) override;
};