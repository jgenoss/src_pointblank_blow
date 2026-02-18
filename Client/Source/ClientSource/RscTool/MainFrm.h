
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once



class CMainFrame;
CMainFrame* GetMainFrame();

class PackScriptView;
class ItemView;

typedef i3::shared_ptr<struct AssocFileSet>	AssocFileSetPtr;
class ExecutePackFrameWnd;

typedef i3::shared_ptr<struct LocaleStringSet>	LocaleStringSetPtr;

struct PefTxtDiffInputInfo;
typedef i3::shared_ptr<struct PackScriptResult>			PackScriptResultPtr;
typedef i3::shared_ptr<struct RSCFileResult>			RSCFileResultPtr;


class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	
	void		CreatePackScriptFileFrame(class PackScriptFile* psf);
	
	void		CreateItemDetailFrame(const AssocFileSetPtr& ptr);
	
	void		CreatePefDiffFrame(const PefTxtDiffInputInfo& input_info, i3RegKey* exp_key, i3RegKey* local_key);
	void		CreateLocaleStringDiffFrame(const PefTxtDiffInputInfo& input_info , const LocaleStringSetPtr& exp, const LocaleStringSetPtr& local);

	void		CreateFileCheck_MD5Frame(const i3::vector<PackScriptResultPtr>& psr_list);
	void		CreateFileCheck_MD5Frame(const i3::vector<RSCFileResultPtr>& rsc_list);

	ExecutePackFrameWnd*	GetExecutePackFrameWnd();

	static void	DestroyPopupWndFromList(CFrameWndEx* wnd);
	static void DestroyExecuteFrameWndFromList(ExecutePackFrameWnd* wnd);

// 특성입니다.
public:

// 작업입니다.
public:
	void		SetViews(PackScriptView* view1, ItemView* view2);

private:



// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

	
	// 도킹창 닫힘 통지는 도킹클래스의 가상함수로 통제하기 힘들다.. ( 현재로서는 CFrameWndEx 파생클래스에서 처리하는게 가장 깔끔하다..)
//	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* /*pWnd*/) ;
//	virtual BOOL OnCloseDockingPane(CDockablePane* /*pWnd*/) ;
	PackScriptView*	GetPackScriptView() const { return m_packscript_view; }
	ItemView*		GetItemView() const { return m_item_view; }


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

	PackScriptView*	  m_packscript_view;
	ItemView*		  m_item_view;
	
	ExecutePackFrameWnd*	m_ExecutePackFrameWnd;

private:


	static void		DestroyAllPopupWnds();

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);

	afx_msg void OnFileNewResourceSolution();

	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();

};

inline CMainFrame* GetMainFrame() {	return static_cast<CMainFrame*>(AfxGetMainWnd()); }
