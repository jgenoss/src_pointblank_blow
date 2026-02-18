#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"


// CSelectSceneDlg dialog

class CSelectSceneDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectSceneDlg)

public:
	CSelectSceneDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectSceneDlg();

// Dialog Data
	enum { IDD = IDD_DLG_SELECTSCENE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
			void _UpdateState(void);

	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	CListCtrl m_lvScene;
	CButton m_btnSetActive;
	CButton m_btnShowHide;
	CButton m_btnCloseScene;
	afx_msg void OnBnClickedBtnClosescene();
	afx_msg void OnBnClickedBtnSetactive();
	afx_msg void OnBnClickedBtnShowhide();
	afx_msg void OnNMDblclkLvScenelist(NMHDR *pNMHDR, LRESULT *pResult);
};
