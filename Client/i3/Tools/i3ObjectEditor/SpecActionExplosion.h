#pragma once
#include "afxwin.h"


// CSpecActionExplosion dialog

class CSpecActionExplosion : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecActionExplosion)
protected:
	i3ActionExplosion *		m_pAction;

public:
	virtual void	SetObject( i3ElementBase * pObj) override;

public:
	CSpecActionExplosion(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecActionExplosion();

// Dialog Data
	enum { IDD = IDD_SPEC_ACTION_EXPLOTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_AttachEnableCtrl;
	CComboBox m_BoneCBCtrl;

	CEdit m_RageCtrl;
	CEdit m_DamageCtrl;
	CEdit m_ForceCtrl;
	CEdit m_SlopeCtrl;
	afx_msg void OnEnKillfocusEdRange();
	afx_msg void OnEnKillfocusEdDamage();
	afx_msg void OnEnKillfocusEdForce();
	afx_msg void OnEnKillfocusEdSlope();
	afx_msg void OnCbnSelchangeCbBonelist();
	afx_msg void OnBnClickedChkAttach();
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};
