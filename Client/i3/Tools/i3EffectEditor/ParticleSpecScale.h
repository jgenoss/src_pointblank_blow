#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "GraphViewCtrl.h"

// CParticleSpecScale dialog

class CParticleSpecScale : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CParticleSpecScale)

public:
	CParticleSpecScale(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleSpecScale();

// Dialog Data
	enum { IDD = IDD_PARTICLESPEC_SCALE };

protected:
	i3ParticleInfo *	m_pInfo;
	CGraphViewCtrl		m_GraphViewCtrl;

public:
	void SetParticle( i3ParticleInfo * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Arg0ConstCtrl;
	CSpinButtonCtrl m_Arg0ConstSpinCtrl;
	CEdit m_Arg0FactorCtrl;
	CSpinButtonCtrl m_Arg0FactorSpinCtrl;
	CEdit m_Arg1ConstCtrl;
	CSpinButtonCtrl m_Arg1ConstSpinCtrl;
	CEdit m_Arg1FactorCtrl;
	CSpinButtonCtrl m_Arg1FactorSpinCtrl;
	virtual BOOL OnInitDialog();

	CEdit m_MinCtrl;
	CEdit m_MaxCtrl;
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	CButton m_InterpolateCtrl;
	CButton m_ClampCtrl;
	CComboBox m_Arg0FuncCtrl;
	CComboBox m_Arg1FuncCtrl;
	CButton m_Arg0GraphCtrl;
	CButton m_Arg1GraphCtrl;
	CEdit m_FactorXCtrl;
	CEdit m_FactorYCtrl;
	CEdit m_FactorZCtrl;
	CEdit m_UpdateIntervalCtrl;
	CStatic m_UpdateFrameCtrl;
	CEdit m_InitialRandomFactorCtrl;
	CEdit m_BillboardScaleFactorCtrl;

	afx_msg void OnEnKillFocus();
	afx_msg void OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnUpdateUpdateInterval();
	afx_msg void OnBnClickedPosClamp();
	afx_msg void OnCbnSelchangeCbScaleArg0Type2();
	afx_msg void OnCbnSelchangeCbScaleArg1Type2();
	afx_msg void OnBnClickedBtnScaleArg0Table2();
	afx_msg void OnBnClickedBtnScaleArg1Table2();

private:
	void	InitSpinCtrl();
};
