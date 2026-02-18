#pragma once
#include "afxwin.h"

#include "resource.h"

class CDlgRefResCpy : public CDialog
{
	DECLARE_DYNAMIC(CDlgRefResCpy)

public:
	CDlgRefResCpy(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRefResCpy();

	enum { IDD = IDD_DLG_REFRESCPY};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
	afx_msg void OnBtnCpy();

	CEdit							m_ebWorkDir;
	CEdit							m_ebFile;
	CEdit							m_ebCopyToDir;	

public:
	char							m_ownerWorkDir[MAX_PATH];
	char							m_filePath[MAX_PATH];
};

