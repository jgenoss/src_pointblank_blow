#pragma once
#include "afxcmn.h"

// i3TDKSelectAttrTypeDialog dialog

class I3_EXPORT_TDK i3TDKSelectAttrTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(i3TDKSelectAttrTypeDialog)

public:
	i3TDKSelectAttrTypeDialog(CWnd* pParent = nullptr) : CDialog(IDD_SELECTATTRTYPEDIALOG, pParent) {}   // standard constructor
	virtual ~i3TDKSelectAttrTypeDialog() {}

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
