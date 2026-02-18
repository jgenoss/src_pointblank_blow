#pragma once

#include "UIFlowEditBox.h"
#include "UIPhaseClanMember.h"

class UIClanSlot
{
public:
	UIClanSlot();
	~UIClanSlot();

	bool Init(i3UIFrameWnd * pWnd, INT32 idx);
	void SetSlotData(SORT_CLAN_INFO * pInfo, INT32 i32SelSlot);

	void SetSlotEnable(bool bEnable, INT32 I32SelSlot);
	void SelectSlot(INT32 i32SelSlot);

	void OnUpdate(REAL32 rDeltaSeconds);

	const bool isSameClanIdx(const INT32 clan_idx) const { return (m_i32ClanIdx == clan_idx); }

protected:
	i3UIFrameWnd*		m_pFrameWnd;
	i3UIImageBoxEx *	m_pClanMark;
	i3UIStaticText *	m_pClanName;
	i3UIButton *		m_pVisitBtn;

	UIFlowEditBox		m_FlowEditBox;

	INT32				m_i32ClanIdx;
	INT32				m_nClanMarkSize;
};