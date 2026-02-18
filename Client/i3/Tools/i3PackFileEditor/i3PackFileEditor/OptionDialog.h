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
	enum { IDD = IDD_OPTIONDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_MediaFolderCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnFoldersel();
protected:
	virtual void OnOK();
public:
	CEdit m_TexPathCtrl;
	afx_msg void OnBnClickedBtnTexsel();
	CEdit m_OutPathCtrl;
	afx_msg void OnBnClickedBtnOutdir();
	afx_msg void OnBnClickedCheckPspSwizzled();
	BOOL m_bSwizzled;
};
