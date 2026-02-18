#pragma once
#include "afxwin.h"


// CTreeAddAttr dialog

class CTreeAddAttr : public CDialog
{
	DECLARE_DYNAMIC(CTreeAddAttr)

public:
	CTreeAddAttr(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTreeAddAttr();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDATTR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void); 
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ListControl;
	CString m_SelectedAttr;
	afx_msg void OnCbnSelchangeCombo1();
};
