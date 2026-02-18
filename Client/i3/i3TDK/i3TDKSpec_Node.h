#pragma once
#include "afxwin.h"


// i3TDKSpec_Node dialog
typedef void (*TDK_CB_BOUND_MAT)(i3Node *p1);

class i3TDKSpec_Node : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Node)
public:
	TDK_CB_BOUND_MAT	m_pCB_ResetBoundMat;
protected:
	i3Node *		m_pNode = nullptr;
	
public:
	i3TDKSpec_Node(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Node::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Node();

	virtual void			SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_NODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Name;
	CEdit m_ED_Flags;
	CEdit m_ED_Bound;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnEnKillfocusTdkEdName();
	afx_msg void OnBnClickedTdkBtnFlags();
	afx_msg void OnEnKillfocusTdkEdBound();
	CButton m_BTN_Flags;
	afx_msg void OnEnChangeTdkEdName();
	CButton m_BTN_Bound;
	afx_msg void OnBnClickedTdkBtnBound();
};
