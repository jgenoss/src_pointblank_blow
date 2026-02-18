#pragma once
#include "afxwin.h"


// CSpecChara dialog

class CSpecChara : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecChara)

public:
	CSpecChara(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecChara();

	virtual void	SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_SPEC_CHARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_NameCtrl;
	CEdit m_SlopeCtrl;
	CEdit m_ConstCtrl;
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEnKillfocusEdConst();
	afx_msg void OnEnKillfocusEdName();
	afx_msg void OnEnKillfocusEdSlope();
};
