
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "ui/TasksBar.h"
#include "ui/SyncPlayerBar.h"
#include "ui/OutputBar.h"
#include <queue>
#include "ui/LoadingProgressMsgBox.h"
#include "ui/ContentsDescriptionBar.h"
#include "ui/MQContentsStatusBar.h"
#include "ui/QLContentsStatusBar.h"
#include "ui/i3PackToolBar.h"
#include "ui/PackedFileSearchBar.h"
#include "ui/DuplicatedPackFilesDlg.h"

class CMainFrame;
CMainFrame* GetMainFrame();

namespace ui { class ContentsView; class FileView; }

class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:
	void SetViews(ui::ContentsView* view1, ui::FileView* view2);

	ui::ContentsView&			GetContentsView() const		{ return *m_contentsView;	}
	ui::FileView&				GetFileView() const			{ return *m_fileView; }
	ui::SyncPlayerBar&			GetSyncPlayerBar()			{ return m_syncPlayerBar;	}
	ui::ContentsDescriptionBar& GetContentsDescBar()  		{ return m_contentsDescBar; }
	ui::MQContentsStatusBar&	GetMQContentsStatusBar() 	{ return m_MQContentsStatusBar; }
	ui::QLContentsStatusBar&	GetQLContentsStatusBar()	{ return m_QLContentsStatusBar; }
	ui::TasksBar&				GetTasksBar()				{ return m_taskBar;  }
	ui::i3PackToolBar&			Geti3PackToolBar()			{ return m_i3PackToolBar; }
	ui::PackedFileSearchBar&	GetPackInnerFileSearchBar() { return m_packInnerFileSearchBar; }
	ui::DuplicatedPackFilesDlg& GetDuplicatedPackFileReport() { return m_duplicatedPackFilesReport; }

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
	//CMFCMenuBar			m_wndMenuBar;
	//CMFCToolBar			m_wndToolBar;
	//CMFCStatusBar		m_wndStatusBar;
	CMFCToolBarImages	m_UserImages;
	ui::ContentsView*	m_contentsView = nullptr;
	ui::FileView*		m_fileView = nullptr;
	ui::TasksBar		m_taskBar;
	ui::SyncPlayerBar	m_syncPlayerBar;
	//ui::OutputBar		m_outputBar;
	ui::ContentsDescriptionBar m_contentsDescBar;
	ui::MQContentsStatusBar m_MQContentsStatusBar;
	ui::QLContentsStatusBar m_QLContentsStatusBar;
	ui::i3PackToolBar m_i3PackToolBar;
	ui::PackedFileSearchBar	m_packInnerFileSearchBar;
	ui::DuplicatedPackFilesDlg m_duplicatedPackFilesReport;

private:
	static void		DestroyAllPopupWnds();

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnCountryTaskClicked();
	afx_msg LRESULT OnViewListCtrlLClicked(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

	LRESULT OnInsertContents(WPARAM wparam, LPARAM lparam);
};

//inline CMainFrame* GetMainFrame() {	return static_cast<CMainFrame*>(AfxGetMainWnd()); }	// 스레드에서 사용하면 안 됨.
inline CMainFrame* GetMainFrame() { return static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd()); }

