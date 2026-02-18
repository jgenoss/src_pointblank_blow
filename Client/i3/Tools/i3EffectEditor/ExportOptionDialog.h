#pragma once
#include "afxwin.h"


// CExportOptionDialog dialog

class CExportOptionDialog : public CDialog
{
	DECLARE_DYNAMIC(CExportOptionDialog)

public:
	BOOL		m_bExportTex;
	char		m_szPath[MAX_PATH];

public:
	CExportOptionDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExportOptionDialog();

	BOOL	Execute(void);

// Dialog Data
	enum { IDD = IDD_EXPORTOPTIONDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_ExportTextureCtrl;
	CEdit m_PathCtrl;
protected:
	virtual void OnOK();
};
