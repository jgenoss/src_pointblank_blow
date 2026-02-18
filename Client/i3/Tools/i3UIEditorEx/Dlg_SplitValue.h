#pragma once
#include "afxwin.h"
#include "resource.h"


// CDlg_SpreadValue dialog

class CDlg_SplitValue : public CDialog
{
	DECLARE_DYNAMIC(CDlg_SplitValue)

public:
	CDlg_SplitValue(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_SplitValue();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	INT32	m_Col;
	INT32	m_Margin;

	CEdit m_edColumnCount;
	CEdit m_edMargin;
protected:
	void _UpdateInfo();
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnEnKillfocusEditColumnCount();
	afx_msg void OnEnKillfocusEditMarginValue();
	virtual BOOL OnInitDialog() override;
};
