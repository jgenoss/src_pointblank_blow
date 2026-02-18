#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_Bool dialog

class i3TDKSpec_Attr_Bool : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_Bool)
protected:
	i3AbstractBoolAttr *	m_pAttr = nullptr;


public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_Bool(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_Bool::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_Bool();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_BOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Enable;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedTdkChkEnable();
};
