#pragma once
#include "afxwin.h"


// CGetStringDlg dialog

class CGetStringDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetStringDlg)

public:
	CGetStringDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGetStringDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_GETSTRING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

public:

protected:

	DECLARE_MESSAGE_MAP()
	virtual void OnOK() override;
public:
	CEdit m_edString;
	virtual BOOL OnInitDialog() override;
	CButton m_chkSearchExactlySame;

	char m_str[MAX_PATH];
	bool m_bSearchExactlySame;

};
