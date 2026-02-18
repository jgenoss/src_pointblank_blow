#pragma once
#include "afxwin.h"


// OptionDialog dialog

class OptionDialog : public CDialog
{
	DECLARE_DYNAMIC(OptionDialog)

public:
	OptionDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionDialog();

// Dialog Data
	enum { IDD = IDD_OPTION };

	CEdit m_WorkDirEdit;
	CEdit m_EditRSC;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOptionBtnFolder();
protected:
	virtual void OnOK() override;
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnEnChangeOptionEditWork();	
	afx_msg void OnBnClickedOptionBtnRsc();
	
};
