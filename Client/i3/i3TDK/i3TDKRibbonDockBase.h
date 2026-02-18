#pragma once


// i3TDKRibbonDockBase

class I3_EXPORT_TDK i3TDKRibbonDockBase : public CDockablePane
{
	DECLARE_DYNCREATE(i3TDKRibbonDockBase)

public:
	i3TDKRibbonDockBase() {}
	virtual ~i3TDKRibbonDockBase() {}

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};


