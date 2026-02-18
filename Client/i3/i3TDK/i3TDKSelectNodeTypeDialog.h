#pragma once
#include "afxcmn.h"

// i3TDKSelectNodeTypeDialog dialog

class I3_EXPORT_TDK i3TDKSelectNodeTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(i3TDKSelectNodeTypeDialog)

public:
	i3TDKSelectNodeTypeDialog(CWnd* pParent = nullptr) : CDialog(IDD_SELECTNODETYPEDIALOG, pParent) {}   // standard constructor
	virtual ~i3TDKSelectNodeTypeDialog() {}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_TypeListCtrl;
	i3::vector<i3ClassMeta*>	m_MetaList;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
