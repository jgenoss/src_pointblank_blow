#pragma once
#include "afxwin.h"


// CTreeAddNode dialog

class CTreeAddNode : public CDialog
{
	DECLARE_DYNAMIC(CTreeAddNode)

public:
	CTreeAddNode(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTreeAddNode();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDNODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ListContorl;
	CString m_SelectedNode;
};
