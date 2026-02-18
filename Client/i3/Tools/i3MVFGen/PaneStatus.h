#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MVFNode.h"

// CPaneStatus dialog

class CPaneStatus : public CDialog
{
	DECLARE_DYNAMIC(CPaneStatus)

public:
	CPaneStatus(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneStatus();

	void	ClearLog(void);
	void	Log( INT32 type, MVFNode * pNode, char * pszMsg);
	void	SetProgress( INT32 vmax);
	void	AddProgress( char * pszFile);

// Dialog Data
	enum { IDD = IDD_PANE_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_FileCtrl;
	CProgressCtrl m_ProgCtrl;
	CListCtrl m_LogListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
