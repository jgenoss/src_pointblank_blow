#pragma once
#include "afxwin.h"

/*
// CPropWorld dialog
class CPropWorld : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPropWorld)

protected:
	i3LevelWorld	*	m_pWorld;

public:
	CPropWorld(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropWorld();

	virtual void	SetObject( i3ElementBase * pObj);

	void	UpdateWorldData();

// Dialog Data
	enum { IDD = IDD_PROP_WORLD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_bShowRenderableMesh;
	CEdit m_edWorldCol;
	CEdit m_edHeroCol;
	CEdit m_edHitCol;
	CEdit m_edLightVolume;
	
	CButton m_btnWorldColSel;
	CButton m_btnHeroColSel;
	CButton m_btnHitColSel;
	CButton m_btnLightVolume;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedBtnWorldcolSel();
	afx_msg void OnBnClickedBtnHerocolSel();
	afx_msg void OnBnClickedBtnHitcolSel();
	afx_msg void OnBnClickedChkShowrenderablemesh();
	CButton m_btnReset;
	afx_msg void OnBnClickedColReset();

	//Check Button
	CButton m_bEnableFog_World;
	CButton m_bEnableFog_Sky;

	//Select Color
	CButton m_btnSelAmbient;
	CButton m_btnSelAmbient_Sky;

	//Color Display
	CStatic m_stAmbient;
	CStatic m_stAmbient_Sky;

	//Fog Denskty
	CEdit m_edWfDensity;
	CEdit m_edSfDensity;

	//Fog Mode
	CComboBox m_cmbWFMode;
	CComboBox m_cmbSFMode;

	//Fog Near
	CEdit m_edWFNear;
	CEdit m_edSFNear;

	//Fog Far
	CEdit m_edWfFar;
	CEdit m_edSfFar;

	//Sky Path
	CEdit m_edSkyPath;
	CButton m_btnSelSkyPath;

	//Value
	float m_WfDensity;
	float m_SfDensity;
	float m_WfNear;
	float m_SfNear;
	float m_WfFar;
	float m_SfFar;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSelAmbient();
	afx_msg void OnBnClickedBtnSelAmbient2();
	afx_msg void OnBnClickedBtnSelskypath();
	afx_msg void OnBnClickedChkEnfogSky();
	afx_msg void OnBnClickedChkEnfogworld();
	afx_msg void OnCbnSelchangeComboSfmode();
	afx_msg void OnCbnSelchangeComboWfmode();
	afx_msg void OnEnChangeEditSfdensity();
	afx_msg void OnEnChangeEditSffar();
	afx_msg void OnEnChangeEditSfnear();
	afx_msg void OnEnChangeEditWfdenskty();
	afx_msg void OnEnChangeEditWffar();
	afx_msg void OnEnChangeEditWfnear();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnLightvolume();
};
*/