#pragma once
#include "afxwin.h"


// SpecActionDisable dialog

class SpecActionDisable : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(SpecActionDisable)
protected:
	i3ActionDisable *		m_pAction;

public:
	virtual void			SetObject( i3ElementBase * pObj) override;

public:
	SpecActionDisable(CWnd* pParent = NULL);   // standard constructor
	virtual ~SpecActionDisable();

// Dialog Data
	enum { IDD = IDD_SPEC_ACTION_DISABLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;
	virtual void OnCancel() override;

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Enable;
	CEdit m_ED_FadeTime;
	afx_msg void OnBnClickedChkEnable();
	afx_msg void OnEnKillfocusEdFadetime();
};
