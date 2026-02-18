#pragma once
#include "afxwin.h"


// CDlgMipmapOption dialog

enum ODT_TYPE
{
	ODT_EXTRACT_MIPMAP = 0,
	ODT_GENERATE_MIPMAP,
	ODY_RELOAD_MIPMAP
};

class CDlgMipmapOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgMipmapOption)

protected:
	ODT_TYPE		m_Type;

public:
	bool			Execute( ODT_TYPE type);

public:
	CDlgMipmapOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMipmapOption();

// Dialog Data
	enum { IDD = IDD_DLG_MIPMAP_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_ExportPath;
	CEdit m_ED_MinCX;
	CEdit m_ED_MinCY;
	CComboBox m_CB_Filter;
	CButton m_CHK_Dither;
	CButton m_CHK_SRGB_In;
	CButton m_CHK_SRGB_Out;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
public:
	afx_msg void OnBnClickedBtnExportpath();
	CButton m_CHK_ExportI3I;
};
