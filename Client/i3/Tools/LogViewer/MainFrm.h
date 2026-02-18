
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
class CLogViewerView;
class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCRibbonBar	  m_wndRibbonBar;
	CMFCRibbonStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	COleDateTime	  m_tmBegin;
	COleDateTime	  m_tmEnd;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	CLogViewerView* GetView();
public :
	void			CreateRibbon();

	void			OnMenuCalender();
	void			OnMenuConfigDir();

	void			OnLogControl();
	void			OnLogCast();
	void			OnLogClan();
	void			OnLogTrans();
	void			OnLogMessenger();
	void			OnLogGame();
	void			OnLogDedi();
	void			OnLogDediHack();
	void			OnLogSIA();
	void			OnLogSIA_SERVICE();
	void			OnLogZLog();
	void			OnLogZLog_SERVICE();

	afx_msg void	OnUpdateLogControl(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogCast(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogClan(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogTrans(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogMessenger(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogGame(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogDedi(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogDediHACK(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogSIA(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogSIA_SERVICE(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogZLog(CCmdUI *pCmdUI);
	afx_msg void	OnUpdateLogZLog_SERVICE(CCmdUI *pCmdUI);

	void			SetStateText(const char* pchText);
	void			SetPrograssBar(INT32 nPos);
	void			SetPrograssRange(INT32 nMin, INT32 nMax);
};


