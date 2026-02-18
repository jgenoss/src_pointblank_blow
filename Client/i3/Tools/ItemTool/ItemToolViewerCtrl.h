#pragma once

class cItemToolViewerCtrl : public i3TDKViewerCtrl
{
	DECLARE_DYNAMIC(cItemToolViewerCtrl)
	DECLARE_MESSAGE_MAP()
public:
	cItemToolViewerCtrl(void);
	virtual ~cItemToolViewerCtrl(void);

private:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
};
