// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "PanelGroup.h"
#include "PanelSceneGraph.h"
#include "PanelViewer.h"
#include "PanelToolbar.h"
#include "PanelSpec.h"

class CMainFrame : public i3TDKDockFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성
public:
	CPanelGroup			m_PanelGroup;
	CPanelSceneGraph	m_PanelSceneGraph;
	CPanelViewer		m_PanelViewer;
	CPanelToolbar		m_PanelToolbar;
	CPanelSpec			m_PanelSpec;


// 작업
public:
	void	NewProject(void);
	void	OpenProject( const char * pszPath);
	void	SaveProject( const char * pszPath);

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnToolsTerrainauto();
	afx_msg void OnToolsVolumelightassign();
};


