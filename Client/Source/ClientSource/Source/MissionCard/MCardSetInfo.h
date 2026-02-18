#pragma once

#include "MCardPrizeInfo.h"

struct MCardInfo;
typedef i3::scoped_ptr<MCardInfo>	MCardInfoPtr;

struct MCardSetInfo : public i3::class_object_pool<MCardSetInfo>
{
	INT32			setID = -1;
	i3::rc_wstring	wstrName;
	i3::rc_wstring	wstrTitle;
	i3::rc_wstring	wstrFileName;
	i3::rc_wstring  wstrUITemplateName;
	INT32			UITemplateIndex = -1;

	INT32			totExp = 0;
	INT32			totPoint = 0;
	INT32			bonusBadgeMaster = 0;
	i3::rc_wstring	wstrEventPeriod;				// 이벤트 기간..이벤트 아이템에만 해당될것임..
	
	MCardPrizeInfo_Mqf	prizeCompletion;			// 모든 카드 완료시 최종 보상... (vecBonusItem 대체용도)
	UINT32			goodsID = 0;						// 이것은 서버로부터 로긴시 시스템패킷을 받은뒤에라야 셋팅이 된다.

	MCardInfoPtr mCardInfo[ MAX_CARD_PER_CARDSET ];
};

void InitMCardSetInfo(MCardSetInfo* cardSet);

