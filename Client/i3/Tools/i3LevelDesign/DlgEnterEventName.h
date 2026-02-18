#pragma once
#include "afxwin.h"


// CDlgEnterEventName dialog

class CDlgEnterEventName : public CDialog
{
	DECLARE_DYNAMIC(CDlgEnterEventName)

protected:
	char	szEventName[MAX_PATH];

public:
	CDlgEnterEventName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEnterEventName();

// Dialog Data
	enum { IDD = IDD_DLG_ENTEREVENTNAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	char	*	getEventName(void)	{	return szEventName;}
	CEdit m_editEnterEventName;
	afx_msg void OnBnClickedOk();
};
