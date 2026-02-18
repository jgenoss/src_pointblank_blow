#pragma once
#include "afxwin.h"


// CParticleSpecRotate dialog

class CParticleSpecRotate : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CParticleSpecRotate)

public:
	CParticleSpecRotate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleSpecRotate();

// Dialog Data
	enum { IDD = IDD_PARTICLESPEC_ROTATE };

protected:
	i3ParticleInfo *	m_pInfo;

public:
	void	SetParticle( i3ParticleInfo * pNode);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_FuncCtrl;
	CEdit m_InitCtrl;
	CEdit m_FactorCtrl;
	CSpinButtonCtrl m_Arg0ConstSpinCtrl;
	CSpinButtonCtrl m_Arg0FactorSpinCtrl;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnEnKillFocus();
	afx_msg void OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeCbRotArg0Type();
	CButton m_GraphCtrl;
	afx_msg void OnBnClickedBtnRotArg0Table();
	CEdit m_RandomFactorCtrl;

private:
	void InitSpinCtrl();

};
