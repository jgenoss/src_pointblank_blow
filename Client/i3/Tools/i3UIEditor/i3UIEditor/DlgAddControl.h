#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgAddControl dialog

class CDlgAddControl : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddControl)

protected:
	i3UITemplate *		m_pTemplate;
	i3UIControl *		m_pCtrl;

protected:

	void				_UpdateTemplate( HTREEITEM hItem, i3UITemplate * pTemplate, i3UITemplate * pParentTemplate = NULL, INT32 idxChild = 0);
	void				_SetTemplate( i3UITemplate * pMeta, i3UITemplate * pParentTemplate, INT32 idxChild, HTREEITEM hParent = TVI_ROOT);

public:
	CDlgAddControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddControl();

// Dialog Data
	enum { IDD = IDD_DLG_SELECT_ADDCONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CB_Type;
	CTreeCtrl m_TREE_Template;
	CListCtrl m_LV_Candidates;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnCbnSelchangeCbType();
	CButton m_BTN_Ok;
	afx_msg void OnTvnSelchangedTreeTemplate(NMHDR *pNMHDR, LRESULT *pResult);
};
