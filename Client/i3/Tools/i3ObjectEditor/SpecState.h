#pragma once
#include "afxwin.h"
#include "ObjectState.h"

// CSpecState dialog

class CSpecState : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecState)
protected:
	CObjectState *	m_pState;

public:
	CSpecState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecState();

	virtual void	SetObject( i3ElementBase * pObj) override;

// Dialog Data
	enum { IDD = IDD_SPEC_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_HPCtrl;
	CEdit m_SgCtrl;

protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;
public:
	afx_msg void OnEnKillfocusEdHp();
	afx_msg void OnBnClickedBtnSg();
	CButton m_CHK_ShadowCaster;
	afx_msg void OnBnClickedChkShadowCaster();
};
