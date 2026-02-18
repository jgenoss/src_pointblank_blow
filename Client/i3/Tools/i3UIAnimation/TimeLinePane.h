#pragma once
#include "PaneBase.h"
#include "TimeLineCtrl.h"
class CTimeLinePane : public CPaneBase
{
	DECLARE_DYNAMIC(CTimeLinePane)
	DECLARE_MESSAGE_MAP()

protected:
	CTimeLineCtrl		m_TimeCtrl;

public:
	CTimeLinePane();
	virtual ~CTimeLinePane();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void AdjustLayout();

	void AddSequence(i3UIAnimation2DSprite* pAnimation);
};

