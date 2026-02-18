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
	enum { IDD = IDD_OPTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CEdit m_EditCtrl_WorkMediaPath;

public:
	afx_msg void OnBnClickedPath();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedOk();
};
