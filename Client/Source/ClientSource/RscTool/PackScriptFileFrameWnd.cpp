// PackScriptFileFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptFileFrameWnd.h"
#include "PackScriptTree.h"

#include "i3Base/itl/vector_util.h"


#include "PackScriptFileExplorerTreeView.h"
#include "PackScriptFileExplorerListView.h"

#include "PathMgr.h"

#include "MainFrm.h"

// PackScriptFileFrameWnd




IMPLEMENT_DYNCREATE(PackScriptFileFrameWnd, CFrameWndEx)

PackScriptFileFrameWnd::PackScriptFileFrameWnd() : m_psf(NULL), m_explorer_listview(NULL)
{

}

PackScriptFileFrameWnd::~PackScriptFileFrameWnd()
{
	
}


BEGIN_MESSAGE_MAP(PackScriptFileFrameWnd, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// PackScriptFileFrameWnd message handlers

int PackScriptFileFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

//
	int ScreenWidth = lpCreateStruct->cx;
	int ScreenHeight = lpCreateStruct->cy;

	CCreateContext cc;
	memset(&cc, 0, sizeof(CCreateContext));
	  
	if (!m_splitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(PackScriptFileExplorerTreeView);
	if (!m_splitter.CreateView(0, 0, RUNTIME_CLASS(PackScriptFileExplorerTreeView), CSize( 0, 0), &cc))
		return FALSE;
	  
	cc.m_pNewViewClass = RUNTIME_CLASS(PackScriptFileExplorerListView);
	if (!m_splitter.CreateView(0, 1, RUNTIME_CLASS(PackScriptFileExplorerListView), CSize(0, 0), &cc))
		return FALSE;

	m_splitter.SetColumnInfo(0, ScreenWidth / 6, 0);

	PackScriptFileExplorerTreeView* pLeftView = (PackScriptFileExplorerTreeView*)m_splitter.GetPane(0,0);
	PackScriptFileExplorerListView* pRightView = (PackScriptFileExplorerListView*)m_splitter.GetPane(0,1);

	pLeftView->SetRelation(pRightView);

	m_explorer_listview = pRightView;
//
	

	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT);

	m_taskpane.Create(L"Task Pane", this, CRect(0,0,ScreenWidth/4,ScreenHeight), TRUE, 1234, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_RIGHT, 
		AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle);
			
	m_contentspane.Create(L"Contents Pane", this, CRect(0,0,ScreenWidth/4,ScreenHeight), TRUE, 1235,  WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_RIGHT, 
		AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle);

	m_forcecopypane.Create(L"QA Copy Only", this, CRect(0,0,ScreenWidth/4, ScreenHeight/3), TRUE, 1236,  
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_RIGHT|CBRS_BOTTOM, 
		AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle);

	m_taskpane.EnableDocking(CBRS_ALIGN_RIGHT );
	m_contentspane.EnableDocking(CBRS_ALIGN_RIGHT);	
	m_forcecopypane.EnableDocking(CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
//	m_taskpane.RemoveCaptionButtons();

	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	this->DockPane(&m_contentspane);
	this->DockPane(&m_taskpane);
	
	m_forcecopypane.DockToWindow(&m_contentspane, CBRS_ALIGN_BOTTOM);
	
	m_forcecopypane.ShowWindow(SW_HIDE);

	return 0;
}

void	PackScriptFileFrameWnd::SetPackScriptFile(PackScriptFile* psf) 
{ 
	m_psf = psf; 
	
	psf->MakeAbsPath( PathMgr::i()->GetPackScriptPath(), m_load_save_path);
	m_load_save_path += L".i3PackScript";

/*
	m_load_save_path  = PathMgr::i()->GetPackScriptPath();
	m_load_save_path += L'/';
	m_load_save_path.append(psf->GetParentPath().begin(), psf->GetParentPath().end());
	m_load_save_path += L'/';
	m_load_save_path += psf->GetFileTitle();
*/

	m_taskpane.LoadPackScriptFile(m_load_save_path);
	
	
}

void PackScriptFileFrameWnd::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	
	GetMainFrame()->DestroyPopupWndFromList(this);


	CFrameWndEx::PostNcDestroy();
}

//
// 큼직하게 띄울목적으로 사이즈를 변경..
//
BOOL PackScriptFileFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class


	if (!CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	// Size the window to 1/3 screen size and center it

	int fullscreen_w = ::GetSystemMetrics(SM_CXSCREEN);
	int fullscreen_h = ::GetSystemMetrics(SM_CYSCREEN);

	cs.cx = fullscreen_w * 9 / 10;
	cs.cy = fullscreen_h * 5 / 6;

	cs.y = (fullscreen_h - cs.cy) / 2;
	cs.x = (fullscreen_w - cs.cx) / 2;

	return TRUE;
}

BOOL PackScriptFileFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return TRUE;
//	return CFrameWndEx::OnEraseBkgnd(pDC);
}

void PackScriptFileFrameWnd::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	// 창닫는 일 하기 전에 메시지 박스를 출력하고...저장할건지 말건지를 수행하고닫음...
	//

	if ( m_taskpane.IsContentsModified() )
	{

		i3::wstring wstrWndName;
		wstrWndName.resize( this->GetWindowTextLength() );
  		this->GetWindowText(&wstrWndName[0], this->GetWindowTextLength()+1);

		i3::wstring strMsg;
		i3::sprintf(strMsg, L"You Must Choose Save %s Or Not...", wstrWndName);

		int res = MessageBoxW(strMsg.c_str(), wstrWndName.c_str(), MB_YESNOCANCEL);

		if ( res == IDCANCEL)
			return;					// 기본클래스 함수 호출 회피..
	
		if ( res == IDYES)
		{
			// 저좡...

			bool res = ExportPackScriptFile( 
				m_taskpane.GetPackScriptContents(), m_load_save_path);

		}
	}
		
	CFrameWndEx::OnClose();
}
