#pragma once
#include "afxwin.h"


// CPropLight dialog

class CPropLight : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPropLight)

protected:
	i3LevelLight	*	m_pLight;

public:
	CPropLight(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropLight();

	virtual void	SetObject( i3ElementBase * pObj) override;

// Dialog Data
	enum { IDD = IDD_PROP_LIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void	SetLight( i3LevelLight * pLight);

	void	Update();

	CStatic m_stAmbient;
	CStatic m_stDiffuse;
	CStatic m_stSpecular;

	CButton m_btnSelAmbient;
	CButton m_btnSelDiffuse;
	CButton m_btnSelSpecular;

	COLORREAL	m_ColAmbient;
	COLORREAL	m_ColDiffuse;
	COLORREAL	m_ColSpecular;

	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnSelAmbient();
	afx_msg void OnBnClickedBtnSelDiffuse();
	afx_msg void OnBnClickedBtnSelSpecular();

protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	CComboBox m_cbLightType;
	CEdit m_edAttenuation_X;
	CEdit m_edAttenuation_Y;
	CEdit m_edAttenuation_Z;
	CEdit m_edCutoffAngle;
	CEdit m_edFalloffRate;
	CEdit m_edRange;
	CEdit m_edSpotlight_Angle;
	afx_msg void OnCbnSelchangeCbLightType();
	afx_msg void OnEnKillfocusEdAttenuationX();
	afx_msg void OnEnKillfocusEdAttenuationY();
	afx_msg void OnEnKillfocusEdAttenuationZ();
	afx_msg void OnEnKillfocusEdCutoffangle();
	afx_msg void OnEnKillfocusEdFalloffrate();
	afx_msg void OnEnKillfocusEdRange();
	afx_msg void OnEnKillfocusSpotlightAngle();
	virtual BOOL OnInitDialog() override;

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
