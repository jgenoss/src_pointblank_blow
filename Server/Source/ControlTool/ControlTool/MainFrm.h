
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "OutputWnd.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	COutputWnd        m_wndOutput;
	CMFCCaptionBar    m_wndCaptionBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();
public:
	BOOL				m_bAdmin;
	BOOL				m_bAdmin_Update;
	void				SetCaption( TCHAR* strText );
	void				AddString( INT32 i32Idx, TCHAR* strText );
	void				UpdateHackList(UINT32 * pHackList); 
	afx_msg void		OnClose();
	afx_msg void		OnUpdateAllServerStart(CCmdUI *pCmdUI);
	afx_msg void		OnUpdateAllServerStop(CCmdUI *pCmdUI);
	afx_msg void		OnUpdateAllUserkick(CCmdUI *pCmdUI);
	afx_msg void		OnUpdateSetMd5key(CCmdUI *pCmdUI);
	afx_msg void		OnUpdateAllWarningClear(CCmdUI *pCmdUI);
	afx_msg void		OnUpdateUpdateServer(CCmdUI *pCmdUI);

	void				SetGraph(INT32 ccuData);
	
};


