#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlg_SelTemplateType dialog

class CDlg_SelTemplateType : public CDialog
{
	DECLARE_DYNAMIC(CDlg_SelTemplateType)

public:
	CDlg_SelTemplateType(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_SelTemplateType();

// Dialog Data
	enum { IDD = IDD_DLG_SELECTMETA_TEMPLATE };

	i3ClassMeta	*	m_pBaseMeta;
	char			m_szSelectedMeta[MAX_PATH];

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
public:
	CListCtrl m_ListCtrl;
	afx_msg void OnDestroy();
	afx_msg void OnNMDblclkTemplateList(NMHDR *pNMHDR, LRESULT *pResult);
};
