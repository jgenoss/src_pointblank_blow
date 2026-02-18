#include "pch.h"
#include "MCardSetInfo.h"




void InitMCardSetInfo(MCardSetInfo* cardSet) 
{
	cardSet->setID = -1;
	cardSet->totExp = 0;
	cardSet->totPoint = 0;
	cardSet->bonusBadgeMaster = 0;
	InitMCardPrizeInfo_Mqf(cardSet->prizeCompletion);
	cardSet->goodsID = 0;				// 0 인 경우 상품이 아닌 무료카드셋으로 간주한다..
}
