#pragma once

// CPanelLog dialog

class CPanelLog : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelLog)
public:
	i3TDKLogCtrl	m_LogCtrl;


public:
	CPanelLog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelLog();

// Dialog Data
	enum { IDD = IDD_PANEL_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
