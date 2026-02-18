#pragma once
#include "afxwin.h"


// CCodePageSelectDlg dialog

class CCodePageSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCodePageSelectDlg)

public:
	CCodePageSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCodePageSelectDlg();

// Dialog Data
	enum { IDD = IDD_CODEPAGE_SELECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()
	CComboBox m_CodePage;
public:	
	afx_msg void OnCbnSelchangeComboExportCp();

	INT32 GetCP(void)		{ return m_nCP;	}

protected:
	UINT32 m_nCP;
};
