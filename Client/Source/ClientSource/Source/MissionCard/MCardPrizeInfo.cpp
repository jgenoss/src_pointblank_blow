#include "pch.h"
#include "MCardPrizeInfo.h"



void InitMCardPrizeInfo_Mqf(MCardPrizeInfo_Mqf& prizeInfo)
{
	prizeInfo.point = 0;
	prizeInfo.exp = 0;
	prizeInfo.prize = 0;
}


void ResetMCardPrizeInfo_Mqf(MCardPrizeInfo_Mqf& prizeInfo)
{
	InitMCardPrizeInfo_Mqf(prizeInfo);
	prizeInfo.vecPrizeItem.clear();
}

