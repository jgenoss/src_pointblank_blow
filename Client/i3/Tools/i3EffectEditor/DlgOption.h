#pragma once
#include "afxwin.h"


// CDlgOption dialog

class CDlgOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgOption)

public:
	CDlgOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOption();

// Dialog Data
	enum { IDD = IDD_DLG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_WorkFolderCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBtnWorkfolder();
};
