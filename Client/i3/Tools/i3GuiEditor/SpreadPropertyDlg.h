#pragma once
#include "afxwin.h"


// CSpreadPropertyDlg dialog

class CSpreadPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpreadPropertyDlg)

public:
	CSpreadPropertyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpreadPropertyDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	INT32	m_nColCount;
	REAL32	m_nSpreadWidth;
	REAL32	m_nSpreadHeight;

	CEdit m_SpreadColCount;
	CEdit m_SpreadWidth;
	CEdit m_SpreadHeight;

	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};
