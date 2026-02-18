#pragma once

#include "UIPaneBase.h"

// CUIPaneTimeLine

class CUIPaneTimeLine : public CUIPaneBase
{
	DECLARE_DYNAMIC(CUIPaneTimeLine)

protected:
	i3TDKTimeEventCtrl		m_TimeEventCtrl;

public:
	CUIPaneTimeLine();
	virtual ~CUIPaneTimeLine();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


