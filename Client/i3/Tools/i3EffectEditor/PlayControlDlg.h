#pragma once
#include "afxwin.h"
#include "i3EditCtrl.h"

// CPlayControlDlg dialog

class CPlayControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlayControlDlg)

public:
	CPlayControlDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlayControlDlg();

// Dialog Data
	enum { IDD = IDD_PLAYPANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_BtnPlay;
	CButton m_BtnStop;
	CButton m_BtnPause;
	i3EditCtrl	m_FPSCtrl;
	CToolTipCtrl m_TooltipCtrl;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnPause();
	CStatic m_FPSPlaceHolder;
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
