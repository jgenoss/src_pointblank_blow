// MainFrm.h : CMainFrame 클래스의 인터페이스
//
#pragma once
#include "i3RSGToolbar.h"
class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

protected:
	// 특성

// 작업
public:

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar	m_wndToolBar;
	i3RSGToolbar m_wndRSGToolBar;

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnViewOption();
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;
public:
	afx_msg void OnDebugDumpgdi();
	afx_msg void OnRSCRefResCpy();

	i3RSGToolbar* GetRSGToolBar() {return &m_wndRSGToolBar;}
};


CMainFrame *	AfxGetMainFrame();