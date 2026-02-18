#pragma once
#include "afxwin.h"


// CSpecActionEmitEffect dialog

class CSpecActionEmitEffect : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecActionEmitEffect)
protected:
	i3ActionEmitEffect *		m_pAction;

public:

	virtual void	SetObject( i3ElementBase * pObj) override;

public:
	CSpecActionEmitEffect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecActionEmitEffect();

// Dialog Data
	enum { IDD = IDD_SPEC_ACTION_EMITEFFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EffectNameCtrl;
	CButton m_AttachEnableCtrl;
	CComboBox m_BoneCBCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnBnClickedBtnEffect();
	CEdit m_ED_OffsetX;
	CEdit m_ED_OffsetY;
	CEdit m_ED_OffsetZ;
	CEdit m_ED_Theta;
	CEdit m_ED_Rho;
	afx_msg void OnEnKillfocusEdOffsetX();
	afx_msg void OnEnKillfocusEdOffsetY();
	afx_msg void OnEnKillfocusEdOffsetZ();
	afx_msg void OnEnKillfocusEdit2();
	afx_msg void OnEnKillfocusEdit3();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnSelchangeCbBonelist();
	afx_msg void OnBnClickedChkAttach();
	CButton m_CHK_EmitToWorld;
	CButton m_CHK_Repeat;
	CEdit m_ED_RepeatLifeTime;
	CEdit m_ED_RepeatInterval;
	afx_msg void OnBnClickedChkEmittoworld();
	afx_msg void OnBnClickedChkRepeat2();
	CEdit m_ED_Scale;
	afx_msg void OnEnKillfocusEdScale();
};
