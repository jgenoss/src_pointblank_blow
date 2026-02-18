
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "TextureViewer.h"
#include "PropertyPane.h"
#include "ImageListPane.h"
#include "PieceListPane.h"
#include "TimeLinePane.h"
#include "AnimationViewPane.h"
#include "AnimationListPane.h"

class CMainFrame : public CFrameWndEx
{
	CPropertyPane* m_pPropertyPane;
	CImageListPane* m_pImageListPane;
	CTextureViewer* m_pTextureViewer;
	CPieceListPane* m_pPieceListPane;
	CTimeLinePane*	m_pTimeLinePane;
	CAnimationViewPane*	m_pAnimationViewPane;
	CAnimationListPane* m_pAnimationListPane;
	i3Viewer*		m_pViewer;

protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// 특성입니다.
public:
	CRect	m_ViewWindowRect;
	CRect	m_MainWindowRect;
	// 작업입니다.
public:

	// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL) override;

	// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;


	// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	bool CreateRenderer();

public:
	CPropertyPane* GetPropertys() { return m_pPropertyPane; }
	CTextureViewer* GetTextureView() { return m_pTextureViewer; }
	CPieceListPane* GetPieceListPane() { return m_pPieceListPane; }
	CImageListPane* GetImageListPane() { return m_pImageListPane; }
	CAnimationListPane* GetAnimationListPane() { return m_pAnimationListPane; }
	CAnimationViewPane* GetAnimationViewPane() { return m_pAnimationViewPane; }
	CTimeLinePane* GetTimeLinePane() {return m_pTimeLinePane;}

};


