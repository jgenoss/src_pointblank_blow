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
	Panel_Spec				m_Panel_Spec;

	i3TDKRibbonColor *		m_pCOL_ClearColor;
	i3TDKRibbonColor *		m_pCOL_AmbientColor;
	i3TDKRibbonColor *		m_pCOL_LightColor;

	CMFCRibbonButton *		m_pBTN_ReflectMap;

	CMFCRibbonComboBox *	m_pCB_ShadowType;
	CMFCRibbonComboBox *	m_pCB_ShadowSize;

	CMFCRibbonCheckBox *	m_pCHK_PaneSg;
	CMFCRibbonCheckBox *	m_pCHK_PaneSpec;

// 작업입니다.
public:

	void			RefreshRibbon(void);

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

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

	afx_msg void OnToolsProjectconfig();
	afx_msg void OnUpdateToolsProjectconfig(CCmdUI *pCmdUI);

	afx_msg void OnToolsPhysics();

	afx_msg void OnClicked_ClearColor();
	afx_msg void OnClicked_AmbientColor();
	afx_msg void OnClicked_LightColor();

	afx_msg void OnChanged_LightIntensity();
	afx_msg void OnUpdate_LightIntensity( CCmdUI * pCmdUI);

	afx_msg void OnClicked_ReflectMap();
	afx_msg void OnUpdate_ReflectMap( CCmdUI * pCmdUI);

	afx_msg void OnCBChange_ShadowSize();
	afx_msg void OnUpdate_ShadowSize( CCmdUI * pCmdUI);

	// Panes
	afx_msg void OnClicked_PaneSg();
	afx_msg void OnUpdate_PaneSg( CCmdUI * pCmdUI);

	afx_msg void OnClicked_PaneSpec();
	afx_msg void OnUpdate_PaneSpec( CCmdUI * pCmdUI);
};


