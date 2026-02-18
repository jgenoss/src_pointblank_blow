#pragma once

#include "i3Math.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ColorKeyEditDialog.h"

// CParticleSpecColor dialog

class CParticleSpecColor : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CParticleSpecColor)
protected:
	CColorKeyEditDialog		m_ColorDlg;

public:
	CParticleSpecColor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleSpecColor();

// Dialog Data
	enum { IDD = IDD_PARTICLESPEC_COLOR };

protected:
	i3ParticleInfo *	m_pInfo;

public:
	void	SetParticle( i3ParticleInfo * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedColorInterpolation();
	afx_msg void OnBnClickedChkUseColorRandom();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCbColorRgbType();
	afx_msg void OnEnChangeEdRgbConstG();
	afx_msg void OnEnChangeEdRgbConstB();
	afx_msg void OnCbnSelchangeCbColorAlphaType2();
	afx_msg void OnEnChangeEdRgbConstAlpha();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	CComboBox m_AlphaFuncCtrl;
	afx_msg void OnBnClickedBtnRgbTableAlpha();
	afx_msg void OnEnKillFocus();
	CButton m_InterpCtrl;
	CButton m_UseRandomCtrl;
	CComboBox m_RGBFuncCtrl;
	CButton m_ColorKeyEditCtrl;
	CEdit m_InitRCtrl;
	CEdit m_InitGCtrl;
	CEdit m_InitBCtrl;
	CEdit m_InitACtrl;
	CButton m_AlphaKeyEditCtrl;
	afx_msg void OnBnClickedBtnEditkey();
	afx_msg void OnBnClickedBtnEditalphakey();
};
