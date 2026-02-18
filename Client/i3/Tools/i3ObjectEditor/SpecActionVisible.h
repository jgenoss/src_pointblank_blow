#pragma once
#include "afxwin.h"


// CSpecActionVisible dialog

class CSpecActionVisible : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecActionVisible)
protected:
	i3ActionVisible *		m_pAction;

public:
	virtual void			SetObject( i3ElementBase * pObj) override;

public:
	CSpecActionVisible(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecActionVisible();

// Dialog Data
	enum { IDD = IDD_SPEC_ACTION_VISIBLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;
	virtual void OnCancel() override;

	DECLARE_MESSAGE_MAP()
public:
	CButton m_EnableCtrl;
	CEdit	m_EditAlphaTimeCtrl;
	afx_msg void OnBnClickedChkEnable();
	afx_msg void OnEnChangeEditAlphatime();
};
