#pragma once
#include "afxwin.h"


// COptionDialog dialog

class COptionDialog : public CDialog
{
	DECLARE_DYNAMIC(COptionDialog)

public:
	COptionDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_WorkDirCtrl;
	CEdit m_ExceptRullCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBtnWorkdir();
	int m_nDefaultAlign;
};
