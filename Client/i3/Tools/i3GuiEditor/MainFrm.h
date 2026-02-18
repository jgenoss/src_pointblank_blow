// MainFrm.h : CMainFrame 클래스의 인터페이스
//
#pragma once
#include "PanePreviewer.h"
#include "PaneTreeView.h"
#include "PaneLayOut.h"
#include "PaneUVEdit.h"
#include "PaneProperty.h"
#include "PaneResList.h"
#include "PaneEventnAction.h"
#include "PaneControlShape.h"
#include "PaneGuiTreeView.h"
#include "i3GuiToolResManager.h"

class CMainFrame : public i3TDKDockFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame();

// 특성
public:
	//pane
	CPanePreviewer		m_PanePreviewer;
//	CPaneTreeView		m_PaneSceneTreeView;
	CPaneGuiTreeView	m_PaneGuiTreeView;
	CPaneUVEdit			m_PaneUVEdit;
	CPaneLayOut			m_PaneLayOut;
	CPaneProperty		m_PaneProperty;
	CPaneResList		m_PaneResList;
	CPaneEventnAction	m_PaneEventnAction;
	CPaneControlShape	m_PaneControlShape;

	//guiroot
	i3Node					*	m_pSkyNode;
	i3Node					*	m_pWorldNode;
	i3AttrSet				*	m_pBackGroundRoot;
	i3GuiRoot				*	m_pRoot;

	INT32				m_nPenColor;
// 작업
public:


// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void	setRoot( i3GameNode * pNode);
	i3GameNode * GetRoot()	{	return m_pRoot;}

	void	Reset();
// 컨트롤 모음이 포함된 멤버입니다.
protected:  

	BOOL	SaveProject( const char * szFileName );

public:
	BOOL	LoadProject( const char * szFileName );

	void	_Rec_BuildScene( i3GuiObjBase * pObj);
// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileNew();
	afx_msg void OnFileSaveProject();
	afx_msg void OnFileLoadProject();
	afx_msg void OnLoadbgAsSky();
	afx_msg void OnLoadbgAsWorld();
	afx_msg void OnExportFile();
	afx_msg void OnMenuSetworkingfolder();
	afx_msg void OnFileTemplateExport();
	afx_msg void OnFileTemplateImport();
	afx_msg void OnMenuSetlinecolorWhite();
	afx_msg void OnMenuSetlinecolorBlack();
	afx_msg void OnEraseshapeEmptyForoffset();
};


