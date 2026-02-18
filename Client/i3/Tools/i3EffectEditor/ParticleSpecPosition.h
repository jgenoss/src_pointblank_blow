#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "GraphViewCtrl.h"

// CParticleSpecPosition dialog

class CParticleSpecPosition : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CParticleSpecPosition)

public:
	CParticleSpecPosition(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleSpecPosition();

// Dialog Data
	enum { IDD = IDD_PARTICLESPEC_POSITION };

protected:
	i3ParticleInfo *	m_pInfo;
	CGraphViewCtrl		m_GraphViewCtrl;

public:
	void	SetParticle( i3ParticleInfo * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bInterpolate;
	CEdit m_Arg0ConstCtrl;
	CSpinButtonCtrl m_Arg0ConstSpinCtrl;
	CEdit m_Arg0FactorCtrl;
	CSpinButtonCtrl m_Arg0FactorSpinCtrl;
	CEdit m_Arg1ConstCtrl;
	CSpinButtonCtrl m_Arg1ConstSpinCtrl;
	CEdit m_Arg1FactorCtrl;
	CSpinButtonCtrl m_Arg1FactorSpinCtrl;
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CEdit m_MinCtrl;
	CEdit m_MaxCtrl;
	BOOL m_bGravity;
	afx_msg void OnEnKillFocus();
	afx_msg void OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit m_RandomFactorX;
	CEdit m_RandomFactorY;
	CEdit m_RandomFactorZ;
	CEdit m_RandomInterval;
	afx_msg void OnBnClickedChkUseRandom();
	CButton m_UseRandomCtrl;
	CEdit m_UpdateIntervalCtrl;
	CStatic m_UpdateFrameCtrl;
//	afx_msg void OnEnChangeUpdateInterval();
	afx_msg void OnEnUpdateUpdateInterval();
	afx_msg void OnBnClickedPosClamp();
	CButton m_ClampCtrl;
	afx_msg void OnBnClickedBtnPosArg0Table();
	CComboBox m_Arg0FuncCtrl;
	CComboBox m_Arg1FuncCtrl;
	CButton m_Arg0GraphCtrl;
	CButton m_Arg1GraphCtrl;
	afx_msg void OnCbnSelchangeCbPosArg0Type();
	afx_msg void OnCbnSelchangeCbPosArg1Type();
	afx_msg void OnBnClickedBtnPosArg1Table();
	CEdit m_InitialRandomFactor;

private:
	void InitSpinCtrl();
};
