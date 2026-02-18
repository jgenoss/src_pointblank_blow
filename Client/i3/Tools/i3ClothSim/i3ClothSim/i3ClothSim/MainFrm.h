// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "PaneEnvironment.h"
#include "PaneParam.h"
#include "PaneViewer.h"
#include "PaneObject.h"
#include "PaneSpring.h"

class CMainFrame : public i3TDKDockFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성
public:
	CPaneEnvironment		m_PaneEnv;
	CPaneParam				m_PaneParam;
	CPaneViewer				m_PaneViewer;
	PaneObject				m_PaneObject;
	CPaneSpring				m_PaneSpring;

	CToolBar				m_wndToolBar;

// 작업
public:

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

protected:  // 컨트롤 모음이 포함된 멤버입니다.

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDebugDump();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdateSelMaskParticle(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSelMaskSpring( CCmdUI * pCmdUI);
	afx_msg void OnSelMaskParticle(void);
	afx_msg void OnSelMaskSpring(void);
};


