#pragma once

#include "MCardPrizeInfo.h"

struct MCardQuestInfo;

typedef i3::scoped_ptr<MCardQuestInfo>	MCardQuestInfoPtr;

struct MCardInfo
{
	MCardQuestInfoPtr	aQuestInfo[MAX_QUEST_PER_CARD_NEW];
	MCardPrizeInfo_Mqf	mCardPrize;
};

void InitMCardInfo(MCardInfo* card);
