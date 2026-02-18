// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "MainRenderView.h"
#include "DockSceneGraphWnd.h"
#include "DockAttributeWnd.h"
#include "DockPropertyEditWnd.h"
#include "DlgFileLoader.h"
#include "DlgWeaponEditor.h"

class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame();
protected: // serialization에서만 만들어집니다.
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

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

	CMFCRibbonStatusBar m_wndStatusBar;

	cDockSceneGraphWnd		m_wndSceneGraph;
	cDockAttributeWnd		m_wndAttribute;
	cDockPropertyEditWnd	m_wndPropertyEdit;

	cMainRenderView			m_wndView;
	INT32					m_nSelectedTabIdx;
	
	cDlgFileLoader*			m_pDlgFileLoader;
	cDlgWeaponEditor*		m_pDlgWeapon;

protected:
	bool InitDockWnd(cDockBaseWnd* wndDock,CString strName, DWORD dwPosition, UINT resID, UINT resToolBarID);

	bool _LoadPefFiles(void);
	bool _LoadPef(LPCSTR pFilePath, const ePefType& pef = E_PEF_NULL);
	bool _LoadImageList(LPCSTR pFilePath, const ePefType& pef = E_PEF_NULL);

	void _SetCurrentType(ePefType type);


public:
	void OnCreateLater();

	cMainRenderView&	GetMainRenderView(void)	{ return m_wndView;		}

	void InitStatusBar();
	void ShowStatusBar();
	void HideStatusBar();

	void ResizeStatusBar(int cxWidth);

	void ResetStatusText1();
	void ResetStatusText2();
	void ResetStatusText3();

	void SetStatusText1(const CString & Text);
	void SetStatusText2(const CString & Text);
	void SetStatusText3(const CString & Text);

	cDlgFileLoader*		GetFileLoader() { return m_pDlgFileLoader; }
	cDlgWeaponEditor*	GetDlgWeaponEditor() { return m_pDlgWeapon; }


	// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewWeaponTab();
	afx_msg void OnUpdateViewWeaponTab(CCmdUI *pCmdUI);


public:
	afx_msg void OnViewGoodsTab();
	afx_msg void OnUpdateViewGoodsTab(CCmdUI *pCmdUI);
	afx_msg void OnViewEquipmentTab();
	afx_msg void OnUpdateViewEquipmentTab(CCmdUI *pCmdUI);
	afx_msg void OnViewCharacterTab();
	afx_msg void OnUpdateViewCharacterTab(CCmdUI *pCmdUI);
	afx_msg void OnViewAbilityTab();
	afx_msg void OnUpdateViewAbilityTab(CCmdUI *pCmdUI);
	afx_msg void OnBtnOpenWeaponEditor();
	afx_msg void OnUpdateOpenWeaponEditor(CCmdUI *pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnPath();
	afx_msg void OnUpdatePath(CCmdUI *pCmdUI);
	afx_msg void OnEnSetfocusEditPath();
	afx_msg void OnPathButton();
	afx_msg void OnUpdatePathButton(CCmdUI *pCmdUI);
	afx_msg void OnUtilityCategoryCombo();
	afx_msg void OnUpdateUtilityCategoryCombo(CCmdUI *pCmdUI);
	afx_msg void OnUtilityCategoryRotateBtn();
	afx_msg void OnUpdateUtilityCategoryRotateBtn(CCmdUI *pCmdUI);

};


CMainFrame *	AfxGetMainFrame();