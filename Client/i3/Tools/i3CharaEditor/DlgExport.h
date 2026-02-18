#pragma once
#include "afxwin.h"


// CDlgExport dialog

class CDlgExport : public CDialog
{
	DECLARE_DYNAMIC(CDlgExport)
protected:
	char		m_szI3SName[ MAX_PATH];

public:
	CDlgExport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgExport();

	bool		Execute( const char * pszFile);

	const char *	getI3SName(void)			{ return m_szI3SName; }

// Dialog Data
	enum { IDD = IDD_DLG_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_ExportI3SCtrl;
	CButton m_checkRemoveSameImage;
	CButton m_ExportI3CHRCtrl;
	CButton m_GenCCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CButton m_RotateAxisCtrl;
	CButton m_GenMipmapCtrl;
	CButton m_ConvertDXTCtrl;
	CButton m_OptionDXTAlphaCtrl;
	CButton m_OptAnim;
	CButton m_GenAnimPack;
	CButton m_ExternTexture;
	CEdit m_ED_I3SName;
};
