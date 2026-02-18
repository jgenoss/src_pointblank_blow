#pragma once
#include "afxcmn.h"


// CSelectNodeTypeDialog dialog

class CSelectNodeTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelectNodeTypeDialog)

public:
	CSelectNodeTypeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectNodeTypeDialog();

// Dialog Data
	enum { IDD = IDD_SELECTNODETYPEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_TypeListCtrl;
	i3List	m_MetaList;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
