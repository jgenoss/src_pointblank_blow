#pragma once

class CPaneLogView : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneLogView)

public:

public:
	CPaneLogView();
	virtual ~CPaneLogView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point);
};

