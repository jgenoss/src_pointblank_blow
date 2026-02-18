#pragma once
class i3RSGToolbar : public CToolBar
{
	DECLARE_DYNAMIC(i3RSGToolbar)

	INT									m_RSCBtnWidth;
	CButton								m_RSCBtn;
	CRect								m_RSCBtnRect;

public:
	i3RSGToolbar();
	virtual ~i3RSGToolbar();

	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_TOOLBAR) override;
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, CRect rcBorders = CRect(0, 0, 0, 0),
		UINT nID = AFX_IDW_TOOLBAR) override;

	BOOL CreateAdditional(INT nID, INT comboWidth = 300);

	bool	IsRSCSave();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

