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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_WorkDir;
	afx_msg void OnBnClickedBtnWorkdir();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedOk();
	CEdit m_ED_Except;
};
