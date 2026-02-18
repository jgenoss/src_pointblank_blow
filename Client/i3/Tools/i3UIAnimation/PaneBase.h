#pragma once

class CPaneBase : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneBase)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	CPaneBase();
	virtual ~CPaneBase();
};
