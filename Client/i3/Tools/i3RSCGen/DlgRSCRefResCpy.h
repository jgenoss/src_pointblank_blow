#pragma once
#include "afxwin.h"

#include "resource.h"

class CDlgRSCRefResCpy : public CDialog
{
	DECLARE_DYNAMIC(CDlgRSCRefResCpy)

public:
	CDlgRSCRefResCpy(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRSCRefResCpy();

	enum { IDD = IDD_RSCREFRESCPY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog() override;

protected:
	virtual void OnOK() override;
	afx_msg void OnBtnCpy();

	CEdit							m_ebWorkDir;
	CEdit							m_ebSrcRSC;
	CEdit							m_ebCopyTo;	

	CButton							m_btGenRSCRefStatFile;
	CButton							m_btCpyRSCRefRes;

public:
	char							m_ownerWorkDir[MAX_PATH];
	char							m_filePath[MAX_PATH];
};

