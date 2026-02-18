#pragma once
#include "afxwin.h"


// CSpecActionDynamicState dialog

class CSpecActionDynamicState : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecActionDynamicState)
protected:
	i3ActionDynamicState *	m_pAction;

public:
	CSpecActionDynamicState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecActionDynamicState();

	virtual void	SetObject( i3ElementBase * pObj) override;

// Dialog Data
	enum { IDD = IDD_SPEC_ACTION_DYNAMIC_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;
	virtual void OnCancel() override;

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_DynamicStateCtrl;
	afx_msg void OnCbnSelchangeCbDynamicState();
};
