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

#include "PanelProperty.h"
#include "PanelTransform.h"
#include "PanelResource.h"
#include "PanelNavMesh.h"
#include "DlgLayer.h"
#include "PanelHitMap.h"
#include "HitMapManager.h"

class i3MFCRibbonButton;
class CHitMapManager;


class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

	CPanelProperty			m_PanelProperty;
	CPanelHitMap			m_PanelHitMap;
	CPanelTransform			m_PanelTransform;
	CPanelResource			m_PanelRes;
	CPanelNavMesh			m_PanelNavMesh;

	i3TDKRibbonDockBase		m_DockProperty;
	i3TDKRibbonDockBase		m_DockHitMap;
	i3TDKRibbonDockBase		m_DockTransform;
	i3TDKRibbonDockBase		m_DockRes;
	i3TDKRibbonDockBase		m_DockNavMesh;

	CMFCRibbonEdit *		m_pEdit_BloomThreshold;
	CMFCRibbonEdit *		m_pEdit_BloomIntensity;
	CMFCRibbonEdit *		m_pEdit_BlurIteration;
	CMFCRibbonEdit *		m_pEdit_BlurTheta;

	CMFCRibbonSlider *		m_pSlider_BloomThreshold;
	CMFCRibbonSlider *		m_pSlider_BloomIntensity;
	CMFCRibbonSlider *		m_pSlider_BlurTheta;

	CMFCRibbonCheckBox *	m_pCHK_BloomEnable;
	CHitMapManager*			m_pHitMapMng;

	CDlgLayer		m_DlgLayer;

public:
	i3LevelFramework*		m_pFramework;

// Operations
public:

	bool		DropObject( i3LevelRes * pRes, POINT scrPt);
	bool		DropRes( const char * pszResPath);

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	void		_SetPath( const char * pszPath);
	bool		_IsInside( CWnd * pWnd, POINT pt);

	void		CreateDockingWindows(void);

	void		InitRibbon_Home(void);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

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
	CMFCToolBarImages	m_RibbonHome_EnableImg;		// 다음의 2개멤버추가.이 클래스가 동적할당된 경우엔, 자동삭제가 안된다..
	CMFCToolBarImages	m_RibbonHome_DisableImg;	// (2012.09.12.수빈)

	CMFCRibbonComboBox *	m_pCB_SpaceMode;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonCtrlMode( CCmdUI* pCmdUI );

	afx_msg void OnUpdateRibbonUndo( CCmdUI* pCmdUI );
	afx_msg void OnUpdateRibbonRedo( CCmdUI* pCmdUI );
	afx_msg void OnUpdateRibbonUndoListClear( CCmdUI* pCmdUI );

	DECLARE_MESSAGE_MAP()

	void InitializeRibbon();
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnToolsExportpath();
	afx_msg void OnEditAddlight();
	afx_msg void OnToolsOptions();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDebugScene();
	afx_msg void OnBtnElmSelection();
	afx_msg void OnBtnElmTranslation();
	afx_msg void OnBtnElmRotation();
	afx_msg void OnBtnElmScale();
	afx_msg void OnBtnElmPathedit();
	afx_msg void OnBtnElmObjbrush();
	afx_msg void OnBtnElmList();
	afx_msg void OnBtnElmExport();
	afx_msg void OnBtnCollisionEdit();
	afx_msg void OnBtnLayer();
	afx_msg void OnBtnOption();
	afx_msg void OnBtnElmUndo();
	afx_msg void OnBtnElmRedo();
	afx_msg void OnBtnElmUndoListClear();
	afx_msg void OnBtnChangeMode();

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

	afx_msg void OnBtnEditEscape();
	afx_msg void OnBtn_LoadHitMap();

protected:
//	virtual void PostNcDestroy();			// 제거함...(2012.09.12.수빈. 이 함수의 기본클래스함수호출을 막으면 소멸자호출이 안됨)
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditReassignResName();

	afx_msg void OnSpaceMode();
	afx_msg void OnUpdate_SpaceMode( CCmdUI * pCmd);
};


