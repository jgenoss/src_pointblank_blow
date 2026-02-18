#pragma once
#include "afxwin.h"


// CSpecAI dialog

class CSpecAI : public i3TDKDialogBase
//class CSpecAI : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecAI)

public:
	CSpecAI(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecAI();

	virtual void	SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_SPEC_AI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_NameCtrl;
	CButton m_EnableCtrl;
	CComboBox m_InitStateCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	CEdit m_DriverMetaCtrl;
	afx_msg void OnBnClickedChkEnable();
	afx_msg void OnCbnSelchangeCbInitstate();
	afx_msg void OnEnKillfocusEdDrivermeta();
};
