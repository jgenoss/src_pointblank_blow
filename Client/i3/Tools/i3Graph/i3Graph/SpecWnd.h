#pragma once
// CSpecWnd dialog

#include "BaseSpecDialog.h"

class CSpecWnd : public CDialog
{
	DECLARE_DYNAMIC(CSpecWnd)

public:
	CSpecWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecWnd();

protected:
	CBaseSpecDialog *	m_pDialog; 

// Dialog Data
	enum { IDD = IDD_DIALOG_SPEC };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void DeleteDialog(void)			{ m_pDialog = NULL; }
	BOOL AttachDialog( CBaseSpecDialog * pDialog); 
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
