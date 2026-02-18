#pragma once
#include "afxwin.h"


// CDlgRename dialog

class CDlgRename : public CDialog
{
	DECLARE_DYNAMIC(CDlgRename)

public:
	CDlgRename(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRename();

// Dialog Data
	enum { IDD = IDD_DLG_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Name;

	char *	m_pszDest;
	INT32	m_Length;

	bool	Execute( char * pszName, INT32 len);
protected:
	virtual void OnOK() override;
};
