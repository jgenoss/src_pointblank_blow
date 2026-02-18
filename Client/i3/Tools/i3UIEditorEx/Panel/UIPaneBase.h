#pragma once

class CUIPaneBase : public CDockablePane
{
	DECLARE_DYNAMIC( CUIPaneBase)

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

};
