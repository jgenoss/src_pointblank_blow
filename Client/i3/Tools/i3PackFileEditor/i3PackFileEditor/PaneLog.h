#pragma once
#include "afxcmn.h"


// CPaneLog dialog

class CPaneLog : public CDialog
{
	DECLARE_DYNAMIC(CPaneLog)

public:
	CPaneLog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneLog();

	void	Log( const char * pszFile, const char * pszMessage);
	void	ClearLog(void);

// Dialog Data
	enum { IDD = IDD_PANELOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListCtrl m_LogListCtrl;
	virtual BOOL OnInitDialog();
};
