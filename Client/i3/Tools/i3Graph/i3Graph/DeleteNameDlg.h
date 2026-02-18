#pragma once
#include "afxcmn.h"


// CDeleteNameDlg dialog

class CDeleteNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeleteNameDlg)	
    i3List		m_NodeList; 		
public:
	CDeleteNameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDeleteNameDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DELNAME };
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	i3List * GetList(void)			{ return & m_NodeList; }
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_NodeListCtrl;
};
