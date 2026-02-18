#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "i3TDK.h"

// CParticleSpecParticle dialog

class CParticleSpecParticle : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CParticleSpecParticle)

public:
	CParticleSpecParticle(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleSpecParticle();

// Dialog Data
	enum { IDD = IDD_PARTICLESPEC_PARTICLE };

protected:
	i3ParticleInfo *	m_pInfo;
	i3Particle *		m_pNode;
	i3TDKImageViewCtrl		m_ImageViewCtrl;

public:
	void	SetParticle( i3Particle * pNode);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_IntervalCtrl;
	CSpinButtonCtrl m_IntervalSpinCtrl;
	CEdit m_LifetimeCtrl;
	CSpinButtonCtrl m_LifetimeSpinCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCbParticleType();
	afx_msg void OnBnClickedChkVolatile();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedBtnTexture();
	CComboBox m_TypeCtrl;
	CButton m_TextureBtn;
	afx_msg void OnCbnSelendokCbParticleType();
	CStatic m_ResTextCtrl;
	CStatic m_SeqTextCtrl;
	CButton m_BtnSeqCtrl;
	afx_msg void OnBnClickedBtnSequence();
	CComboBox m_BlendModeCtrl;
	CComboBox m_BlendOpCtrl;
	afx_msg void OnCbnSelchangeCbBlendMode();
	afx_msg void OnCbnSelchangeCbBlendOp();
	CStatic m_ImageViewPlaceHolder;
	CButton m_RemoveSeqCtrl;
	afx_msg void OnBnClickedBtnRemoveSequence();
	CButton m_VolatileCtrl;
	CEdit m_TextureNameCtrl;
	afx_msg void OnEnKillFocus();
	afx_msg void OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult);
	CComboBox m_TexTypeCtrl;
	CEdit m_DepthBiasSlopeCtrl;
	CEdit m_DepthBiasBiasCtrl;
	CButton m_BillboardingCtrl;
	CButton m_LightingCtrl;
	afx_msg void OnCbnSelchangeCbTextype();
	afx_msg void OnBnClickedChkFacetocamera();
	afx_msg void OnBnClickedChkLighting();
	CButton m_CHK_Decal;
	afx_msg void OnBnClickedChkDecal();
	CButton m_CHK_ZWrite;
	afx_msg void OnBnClickedChkDepthwrite();
	CButton m_CHK_ZTest;
	afx_msg void OnBnClickedChkDepthTest();

private:
	void InitSpinCtrl();
};
