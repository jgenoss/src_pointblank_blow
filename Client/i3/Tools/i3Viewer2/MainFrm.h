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

// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "Panel_SceneGraph.h"
#include "Panel_Spec.h"


class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	Panel_SceneGraph		m_Panel_Sg;

	i3TDKRibbonDockBase		m_Dock_Spec;
	

	i3TDKRibbonColor *		m_pCOL_ClearColor;
	i3TDKRibbonColor *		m_pCOL_AmbientColor;
	i3TDKRibbonColor *		m_pCOL_LightColor;

	CMFCRibbonSlider *		m_pSlider_LightIntensity;
	CMFCRibbonEdit *		m_pEdit_LightIntensity;

	CMFCRibbonEdit *		m_pEdit_LightPosRho;
	CMFCRibbonEdit *		m_pEdit_LightPosTheta;

	CMFCRibbonSlider *		m_pSlider_LightPosRho;
	CMFCRibbonSlider *		m_pSlider_LightPosTheta;

	CMFCRibbonCheckBox *	m_pCHK_LightPosVisible;

	CMFCRibbonEdit *		m_pEdit_BloomThreshold;
	CMFCRibbonEdit *		m_pEdit_BloomIntensity;
	CMFCRibbonEdit *		m_pEdit_BlurIteration;
	CMFCRibbonEdit *		m_pEdit_BlurTheta;

	CMFCRibbonSlider *		m_pSlider_BloomThreshold;
	CMFCRibbonSlider *		m_pSlider_BloomIntensity;
	CMFCRibbonSlider *		m_pSlider_BlurTheta;

	CMFCRibbonCheckBox *	m_pCHK_BloomEnable;

	CMFCRibbonButton *		m_pBTN_ReflectMap;

	CMFCRibbonComboBox *	m_pCB_ShadowType;
	CMFCRibbonComboBox *	m_pCB_ShadowSize;

	CMFCRibbonCheckBox *	m_pCHK_PaneSg;
	CMFCRibbonCheckBox *	m_pCHK_PaneSpec;

	CMFCRibbonEdit *		m_pEdit_FOV;

	CMFCRibbonButton *		m_pBtnOpt1;
	CMFCRibbonButton *		m_pBtnOpt2;
	CMFCRibbonButton *		m_pBtnOpt3;
	CMFCRibbonButton *		m_pBtnOpt4;

// 작업입니다.
public:

	void			RefreshRibbon(void);
	Panel_Spec*		GetPanel_Spec() { return m_pPanel_Spec; }
	
	i3TDKDlgChangeTex*	GetTDKDlgChangeTex() const { return m_dlgChangeTex; }

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void PostNcDestroy() override;

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;

	i3TDKDlgChangeTex*	m_dlgChangeTex;
	Panel_Spec*				m_pPanel_Spec;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	void InitializeRibbon();
	void InitRibbon_Home();

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnViewNormal();
	afx_msg void OnUpdateViewNormal(CCmdUI *pCmdUI);
	afx_msg void OnViewMesh();
	afx_msg void OnUpdateViewMesh(CCmdUI *pCmdUI);
	afx_msg void OnViewCollider();
	afx_msg void OnUpdateViewCollider(CCmdUI *pCmdUI);
	afx_msg void OnViewWire();
	afx_msg void OnUpdateViewWire(CCmdUI *pCmdUI);
	afx_msg void OnViewSelonly();
	afx_msg void OnUpdateViewSelonly(CCmdUI *pCmdUI);
	afx_msg void OnViewShadow();
	afx_msg void OnUpdateViewShadow(CCmdUI *pCmdUI);
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewGrid(CCmdUI *pCmdUI);
	afx_msg void OnToolsGeometry();
	afx_msg void OnUpdateToolsGeometry(CCmdUI *pCmdUI);

	afx_msg void OnToolsTextures();
	afx_msg void OnUpdateToolsTextures(CCmdUI *pCmdUI);

	afx_msg void OnToolsChangeTex();
	afx_msg void OnUpdateToolsChangeTex(CCmdUI* pCmdUI);

	afx_msg void OnToolsProjectconfig();
	afx_msg void OnUpdateToolsProjectconfig(CCmdUI *pCmdUI);

	afx_msg void OnToolsBatchRenAttrOpti();
	afx_msg void OnUpdateToolsBatchRenAttrOpti(CCmdUI *pCmdUI);

	afx_msg void OnToolsTotalBlend();
	afx_msg void OnUpdateTotalBlend(CCmdUI * pCmdUI);

	afx_msg void OnToolsTotalColor();
	afx_msg void OnUpdateTotalColor(CCmdUI * pCmdUI);

	afx_msg void OnToolsPhysics();

	afx_msg void OnClicked_ClearColor();
	afx_msg void OnClicked_AmbientColor();
	afx_msg void OnClicked_LightColor();

	afx_msg void OnChanged_LightIntensity();
	afx_msg void OnUpdate_LightIntensity( CCmdUI * pCmdUI);

	afx_msg void OnUpdate_LightPosRho( CCmdUI * pCmdUI);
	afx_msg void OnUpdate_LightPosTheta( CCmdUI * pCmdUI);

	afx_msg void OnClicked_LightPosVisible();
	afx_msg void OnUpdate_LightPosVisible( CCmdUI * pCmdUI);

	afx_msg void OnChanged_BloomIntensity();
	afx_msg void OnUpdate_BloomIntensity(CCmdUI * pCmdUI);

	afx_msg void OnChanged_BloomThreshold();
	afx_msg void OnUpdate_BloomThreshold(CCmdUI * pCmdUI);

	afx_msg void OnClicked_BloomEnable();
	afx_msg void OnUpdate_BloomEnable(CCmdUI * pCmdUI);

	afx_msg void OnChanged_BlurIteration();
	afx_msg void OnUpdate_BlurIteration(CCmdUI * pCmdUI);

	afx_msg void OnChanged_BlurTheta();
	afx_msg void OnUpdate_BlurTheta(CCmdUI * pCmdUI);

	afx_msg void OnClicked_ReflectMap();
	afx_msg void OnUpdate_ReflectMap( CCmdUI * pCmdUI);

	afx_msg void OnCBChange_ShadowSize();
	afx_msg void OnUpdate_ShadowSize( CCmdUI * pCmdUI);

	// Panes
	afx_msg void OnClicked_PaneSg();
	afx_msg void OnUpdate_PaneSg( CCmdUI * pCmdUI);

	afx_msg void OnClicked_PaneSpec();
	afx_msg void OnUpdate_PaneSpec( CCmdUI * pCmdUI);

	afx_msg void OnChange_CameraFOV(void);

	afx_msg void OnClicked_Opt1(void);
	afx_msg void OnClicked_Opt2(void);
	afx_msg void OnClicked_Opt3(void);
	afx_msg void OnClicked_Opt4(void);

	afx_msg void OnClicked_Optimizer(i3ClassMeta * pOpt);
};


