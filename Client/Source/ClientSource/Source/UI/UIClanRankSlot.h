#pragma once

#include "UIClanSlot.h"
#include "UIFlowEditBox.h"
#include "UIPhaseClanMember.h"

enum CLAN_SOCORE_VIEW
{
	CSV_RECORD = 0,
	CSV_CUR_CLANPOINT,
	CSV_PRE_CLANPOINT,	
};

class UIClanRankSlot : public UIClanSlot
{
public:
	UIClanRankSlot();
	~UIClanRankSlot();

	bool Init(i3UIFrameWnd * pWnd, INT32 idx);

	void SetSlotData(SORT_CLAN_INFO * pInfo, INT32 i32SelSlot, INT32 i32Rank, CLAN_SOCORE_VIEW view_type);
	void SetSlotEnable(bool bEnable, INT32 I32SelSlot);

	i3::rc_wstring GetScoreString(SORT_CLAN_INFO * pInfo, CLAN_SOCORE_VIEW view_type);


protected:

	i3UIStaticText *	m_pClanRank;
	i3UIStaticText *	m_pClanMenberNum;
	i3UIStaticText *	m_pClanScore;

};