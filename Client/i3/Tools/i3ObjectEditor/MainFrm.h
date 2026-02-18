// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "PanelState.h"
#include "PanelSceneGraph.h"
#include "PanelViewer.h"
#include "PanelSpec.h"
#include "PanelTimeEvent.h"

#include "ChildView.h"
class CMainFrame : public i3TDKDockFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성
public:
	CPanelState			m_PanelState;
	CPanelViewer		m_PanelViewer;
	CPanelSpec			m_PanelSpec;
	CPanelTimeEvent		m_PanelTimeEvent;

	char				m_szFileName[ MAX_PATH];

// 작업
public:
	void		OnNewObject(void);
	void		OnLoadObject( const char * pszPath);
	void		OnSaveObject( const char * pszPath);

	void		OnStartPlay(void);
	void		OnStartPause(void);
	void		OnChangeTime( REAL32 tm);

	INT32		getSelectedState(void)							{ return m_PanelState._GetSelectedState(); }

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

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnToolsOption();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileExport();
	afx_msg void OnToolsReferenceobjects();
};


