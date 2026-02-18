// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "CSearchToolBar.h"


class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성
public:
	CComboBox m_combo;

// 작업
public:
	CSearchToolBar* GetSearchToolBar() { return &m_wndSearchToolBar; }

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

public:
	// pText가 NULL이면 "Noname"로 채웁니다.
	void	SetWindowTextEx(LPCTSTR pText = NULL);

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CSearchToolBar	m_wndSearchToolBar;

// 메시지 맵 함수를 생성했습니다.
protected:
	void DockControlBarLeftOf(CControlBar* pBar, CControlBar* pLeftOf);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();

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
};


CMainFrame *	AfxGetMainFrame();