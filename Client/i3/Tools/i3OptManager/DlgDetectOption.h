#pragma once
#include "afxcmn.h"


// CDlgDetectOption dialog

class CDlgDetectOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgDetectOption)

public:
	CDlgDetectOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDetectOption();

// Dialog Data
	enum { IDD = IDD_DLG_DETECT_OPTION };

protected:
	i3List *		m_pWarnList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LST_Warn;
	virtual BOOL OnInitDialog();

	bool	Execute( i3List * pWarnList);
protected:
	virtual void OnOK();
};
