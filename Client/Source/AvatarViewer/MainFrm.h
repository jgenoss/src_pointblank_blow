// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ChildView.h"
#include "Panel_Avatar.h"
#include "Panel_Anim.h"
#include "Panel_Part.h"
#include "Panel_SceneGraph.h"
#include "Panel_CamSet.h"

class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	CPanel_Avatar		m_Panel_Avatar;
	CPanel_Anim			m_Panel_Anim;
	Panel_SceneGraph	m_Panel_Sg;
	CPanel_Part			m_Panel_Part;

	i3TDKRibbonColor *		m_pCOL_ClearColor;
	i3TDKRibbonColor *		m_pCOL_AmbientColor;
	i3TDKRibbonColor *		m_pCOL_LightColor;

	CMFCRibbonButton *		m_pBTN_ReflectMap;

	CMFCRibbonComboBox *	m_pCB_ShadowType;
	CMFCRibbonComboBox *	m_pCB_ShadowSize;

	CMFCRibbonCheckBox *	m_pCHK_Pane_Avatar;
	CMFCRibbonCheckBox *	m_pCHK_Pane_Anim;
	CMFCRibbonCheckBox *	m_pCHK_Pane_Part;

	CPanel_CamSet		m_Panel_CamSet;
	CMFCRibbonButton *	m_pBtn_Pane_CamSet;

	bool				m_bCam1PVMode;
	INT32				m_AvatarPreLOD;
	MATRIX				m_mtxSaved3PVCam;

protected:
	void				CreatePanels(void);
	void				InitRibbon_Home(void);

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CChildView    m_wndView;

	TEAM_TYPE	  m_eTeamType;

	CMFCRibbonComboBox *	m_pCB_WeaponList;
	i3::vector_map<i3::string, UINT128> m_weaponList;
	
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	void InitializeRibbon();
	void RefreshRibbon(void);

public:
	afx_msg void	OnCameraSetBtn();
	afx_msg void	OnUpdateCameraSetBtn(CCmdUI * pCmdUI);
	afx_msg void	OnCameraSaveBtn();
	afx_msg void	OnUpdateCameraSaveBtn(CCmdUI * pCmdUI);
	afx_msg void	OnCameraLoadBtn();
	afx_msg void	OnUpdateCameraLoadBtn(CCmdUI * pCmdUI);
	afx_msg void	OnCamera1PVBtn();
	afx_msg void	OnUpdateCamera1PVBtn(CCmdUI * pCmdUI);

	afx_msg void 	OnViewShadow();
	afx_msg void 	OnUpdateViewShadow(CCmdUI *pCmdUI);

	afx_msg void 	OnViewGrid();
	afx_msg void 	OnUpdateViewGrid(CCmdUI *pCmdUI);
	
	afx_msg void 	OnChanged_LOD();
	afx_msg void 	OnUpdate_LOD( CCmdUI * pCmdUI);

	afx_msg void 	OnToolsGeometry();
	afx_msg void 	OnUpdateToolsGeometry(CCmdUI *pCmdUI);

	afx_msg void 	OnToolsTextures();
	afx_msg void 	OnUpdateToolsTextures(CCmdUI *pCmdUI);

	afx_msg void 	OnToolsProjectconfig();
	afx_msg void 	OnUpdateToolsProjectconfig(CCmdUI *pCmdUI);

	afx_msg void 	OnClicked_ClearColor();
	afx_msg void 	OnClicked_AmbientColor();
	afx_msg void 	OnClicked_LightColor();

	afx_msg void 	OnChanged_LightIntensity();
	afx_msg void 	OnUpdate_LightIntensity( CCmdUI * pCmdUI);

	afx_msg void 	OnUpdate_LightVertical(CCmdUI * pCmdUI);
	afx_msg void 	OnUpdate_LightHorizon(CCmdUI * pCmdUI);

	afx_msg void 	OnClicked_ReflectMap();
	afx_msg void 	OnUpdate_ReflectMap( CCmdUI * pCmdUI);

	afx_msg void 	OnCBChange_ShadowSize();
	afx_msg void 	OnUpdate_ShadowSize( CCmdUI * pCmdUI);

	// Panes
	afx_msg void 	OnClicked_ViewAvatar();
	afx_msg void 	OnUpdate_ViewAvatar( CCmdUI * pCmdUI);

	afx_msg void 	OnClicked_ViewAnim();
	afx_msg void 	OnUpdate_ViewAnim( CCmdUI * pCmdUI);

	afx_msg void 	OnClicked_ViewPart();
	afx_msg void 	OnUpdate_ViewPart( CCmdUI * pCmdUI);

	afx_msg void 	OnFileSave();
	afx_msg void 	OnFileLoad();
	afx_msg void 	OnDropFiles(HDROP hDropInfo);

	afx_msg void 	OnSelectBlueTeam();
	afx_msg void 	OnSelectRedTeam();
	afx_msg void 	OnUpdateSelectTeamBlue(CCmdUI *pCmdUI);
	afx_msg void 	OnUpdateSelectTeamRed(CCmdUI *pCmdUI);
	afx_msg void 	OnSelectTeamColorOff();
	afx_msg void 	OnUpdateSelectTeamColorOff(CCmdUI * pCmdUI);

	// attachment weapon
	afx_msg void 	OnClicked_AttachWeaponRightHand();
	afx_msg void 	OnUpdate_AttachWeaponRightHand( CCmdUI * pCmdUI);
	afx_msg void 	OnClicked_AttachWeaponBackpack();
	afx_msg void 	OnUpdate_AttachWeaponBackpack( CCmdUI * pCmdUI);
	afx_msg void 	OnClicked_ChangeAttachWeapon();
	afx_msg void 	OnUpdate_ChangeAttachWeapon( CCmdUI * pCmdUI);
	afx_msg void	OnChanged_WeaponSelect();
	afx_msg void	OnUpdate_WeaponSelect( CCmdUI * pCmdUI);
	
	// make
	afx_msg void	OnSaveMergedTexture();
	afx_msg void	OnUpdateSaveMergedTexture( CCmdUI * pCmdUI);

	afx_msg void	OnConvertSGbyMacroTexture();
	afx_msg void	OnUpdateConvertSGbyMacroTexture( CCmdUI * pCmdUI);

	afx_msg void	OnConvertBlendIndex();
	afx_msg void	OnUpdateConvertBlendIndex( CCmdUI * pCmdUI);


public:
	void ChangePart( EQUIP::ePART ePart, const char * pszPartName, const char * pszFullPath );

	void SetTeamType(TEAM_TYPE val)				{ m_eTeamType = val; }
	TEAM_TYPE	GetTeamType( void) const		{ return m_eTeamType; }

};


