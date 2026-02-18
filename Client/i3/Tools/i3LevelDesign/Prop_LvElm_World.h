#pragma once
#include "afxwin.h"


// CProp_LvElm_World dialog

class CProp_LvElm_World : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_World)
protected:
	i3TDKColorCtrl		m_COL_WorldFog;
	i3TDKColorCtrl		m_COL_SkyFog;

	i3LevelWorld *		m_pWorldElm;
	i3LevelResWorld *	m_pWorldRes;
	i3World *			m_pWorld;

	void	_SetWorldFogColor( COLOR * pColor);
	void	_SetSkyFogColor( COLOR * pColor);
	void	_UpdateInfo( void);

	bool	GetSgFile( CString & strFile);
public:

public:
	CProp_LvElm_World(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvElm_World();

	virtual void	SetObject( i3ElementBase * pObj) override;

// Dialog Data
	enum { IDD = IDD_PROP_WORLD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_WorldPathCtrl;
	CButton m_WorldFogEnableCtrl;
	CStatic m_WorldFogColorCtrl;
	CEdit m_WorldFogDensityCtrl;
	CComboBox m_WorldFogModeCtrl;
	CEdit m_WorldFogNearCtrl;
	CEdit m_WorldFogFarCtrl;
	CEdit m_SkyPathCtrl;
	CButton m_SkyFogEnableCtrl;
	CStatic m_SkyFogColorCtrl;
	CEdit m_SkyFogDensityCtrl;
	CComboBox m_SkyFogModeCtrl;
	CEdit m_SkyFogNearCtrl;
	CEdit m_SkyFogFarCtrl;
	CEdit m_WorldCollCtrl;
	CEdit m_HitCollCtrl;
	CEdit m_LightCollCtrl;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnWorldSel();
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnBnClickedChkEnfogworld();
	afx_msg void OnBnClickedBtnSelAmbient();
	afx_msg void OnEnKillfocusEditWfdenskty();
	afx_msg void OnCbnSelchangeComboSfmode();
	afx_msg void OnEnKillfocusEditWfnear();
	afx_msg void OnEnKillfocusEditWffar();
	afx_msg void OnBnClickedBtnSelskypath();
	afx_msg void OnBnClickedChkEnfogSky();
	afx_msg void OnBnClickedBtnSelAmbient2();
	afx_msg void OnEnKillfocusEditSfdensity();
	afx_msg void OnCbnSelchangeComboWfmode();
	afx_msg void OnEnKillfocusEditSfnear();
	afx_msg void OnEnKillfocusEditSffar();
	afx_msg void OnBnClickedBtnWorldcolSel();
	afx_msg void OnBnClickedBtnHitcolSel();
	afx_msg void OnBnClickedBtnLightvolume();
	afx_msg void OnBnClickedBtnSectionedit();
	afx_msg void OnBnClickedBtnSectoredit();
};
