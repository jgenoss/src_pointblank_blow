// FileCheck_MD5FrameWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_MD5FrameWnd.h"

#include "MainFrm.h"

#include "FileCheck_OnlyFilesView.h"
#include "FileCheck_ProgressBarView.h"
#include "FileCheck_ProgressMD5View.h"
#include "FileCheck_MD5ModifiedView.h"
#include "FileCheck_NoFileView.h"

#include "FileCheck_PackScriptMgr.h"

#include "PackMode.h"

// FileCheck_MD5FrameWnd

IMPLEMENT_DYNCREATE(FileCheck_MD5FrameWnd, CFrameWndEx)

FileCheck_MD5FrameWnd::FileCheck_MD5FrameWnd()
{

}

FileCheck_MD5FrameWnd::~FileCheck_MD5FrameWnd()
{

}


BEGIN_MESSAGE_MAP(FileCheck_MD5FrameWnd, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_MD5FrameWnd 메시지 처리기입니다.

int FileCheck_MD5FrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CCreateContext cc;
	memset(&cc, 0, sizeof(CCreateContext));

	if (!m_mainSplitter.CreateStatic(this, 1, 3, WS_CHILD | WS_VISIBLE))
		return FALSE;
	

	m_rowSplitter[0].CreateStatic(&m_mainSplitter, 3, 1, WS_CHILD|WS_VISIBLE, m_mainSplitter.IdFromRowCol(0,0));
	m_rowSplitter[1].CreateStatic(&m_mainSplitter, 2, 1, WS_CHILD|WS_VISIBLE, m_mainSplitter.IdFromRowCol(0,1));


	
	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_OnlyFilesView);
	if (!m_rowSplitter[0].CreateView(0, 0, cc.m_pNewViewClass, CSize(0,0), &cc))
		return FALSE;
	if (!m_rowSplitter[0].CreateView(1, 0, cc.m_pNewViewClass, CSize(0,0), &cc))
		return FALSE;	
	
	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_NoFileView);
	if (!m_rowSplitter[0].CreateView(2, 0, cc.m_pNewViewClass, CSize(0,0), &cc))
		return FALSE;	

	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_ProgressBarView);
	if (!m_rowSplitter[1].CreateView(0, 0, cc.m_pNewViewClass, CSize(0,0), &cc))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_ProgressMD5View);

	if (!m_rowSplitter[1].CreateView(1, 0, cc.m_pNewViewClass, CSize(0,0), &cc))
		return FALSE;
		
	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_MD5ModifiedView);
	m_mainSplitter.CreateView(0, 2, cc.m_pNewViewClass, CSize(0,0), &cc);


	int ScreenWidth = lpCreateStruct->cx;
	int ScreenHeight = lpCreateStruct->cy - GetSystemMetrics(SM_CYCAPTION);

	for (INT32 i = 0 ; i < 3 ; ++i)
		m_mainSplitter.SetColumnInfo(i, ScreenWidth / 3, 0);

	m_rowSplitter[0].SetRowInfo(0, ScreenHeight/3, 0);
	m_rowSplitter[0].SetRowInfo(1, ScreenHeight/3, 0);

	m_rowSplitter[1].SetRowInfo(0, ScreenHeight/20, 0);

	PackMode curr_pm =	FileCheck_PackScriptMgr::i()->GetCurrPackMode();

	if (curr_pm == PACKMODE_MEDIA_TO_QA_COPY)
	{
		FileCheck_OnlyFilesView* view;
		view = (FileCheck_OnlyFilesView*) m_rowSplitter[0].GetPane(0,0);
		view->InitCaption(L"MEDIA Only (Comparing MEDIA-QA)");
		view = (FileCheck_OnlyFilesView*) m_rowSplitter[0].GetPane(1,0);
		view->InitCaption(L"QA Only (Comparing MEDIA-QA)");
	}
	else
	{
		FileCheck_OnlyFilesView* view;
		view = (FileCheck_OnlyFilesView*) m_rowSplitter[0].GetPane(0,0);
		view->InitCaption(L"QA Only (Comparing QA-LIVE)");
		view = (FileCheck_OnlyFilesView*) m_rowSplitter[0].GetPane(1,0);
		view->InitCaption(L"Live Only (Comparing QA-LIVE)");
	}
	
	FileCheck_NoFileView* noFileView = (FileCheck_NoFileView*)m_rowSplitter[0].GetPane(2,0);
	noFileView->InitCaption(L"No Exist Files");

	FileCheck_ProgressMD5View* MD5view = (FileCheck_ProgressMD5View*) m_rowSplitter[1].GetPane(1,0);
	FileCheck_ProgressBarView* BarView = (FileCheck_ProgressBarView*) m_rowSplitter[1].GetPane(0,0);

	MD5view->SetProgressBarView(BarView);
	BarView->SetProgressMD5View(MD5view);

	FileCheck_MD5ModifiedView* modified_view = (FileCheck_MD5ModifiedView*)m_mainSplitter.GetPane(0, 2);
	
	MD5view->SetModifiedView(modified_view);

	if (curr_pm == PACKMODE_MEDIA_TO_QA_COPY)
	{
		MD5view->InitCaption(L"MEDIA-QA Common Files");
		modified_view->InitCaption(L"MEDIA-QA Modified FileList");
	}
	else
	{
		MD5view->InitCaption(L"QA-LIVE Common Files");
		modified_view->InitCaption(L"QA-LIVE Modified FileList");
	}
	

	return 0;
}

void FileCheck_MD5FrameWnd::SetInfo(const i3::vector<RSCFileResultPtr>& rsc_list)
{
	m_rsc_list = rsc_list;

	const size_t num_rsc = rsc_list.size();

	for ( size_t i = 0 ; i < num_rsc ; ++i )
	{
		const RSCFileResult* res = rsc_list[i].get();

		RSCFileContentsResult* contents_left = res->contents[0].get();
		RSCFileContentsResult* contents_right = res->contents[1].get();

		FindDiffPackScriptContentsResult(contents_left, contents_right, m_diff_result_LR);
	}

	FileCheck_MD5FrameWnd::SetInfoImpl();
}


void FileCheck_MD5FrameWnd::SetInfo(const i3::vector<PackScriptResultPtr>& psr_list)
{
	// 자료 세분화가 필요함..

	m_psr_list = psr_list;

	const size_t num_psr = psr_list.size();

	for (size_t i = 0 ; i < num_psr ; ++i)
	{
		const PackScriptResult* res = psr_list[i].get();
		
		PackScriptContentsResult* contents_Left = res->contents[0].get();
		PackScriptContentsResult* contents_Right    = res->contents[1].get();
	
		// Media-QA 비교 및 누적 ..
		FindDiffPackScriptContentsResult(contents_Left, contents_Right, m_diff_result_LR);
	
	}

	FileCheck_MD5FrameWnd::SetInfoImpl();
	
}	

void FileCheck_MD5FrameWnd::SetInfoImpl()
{
	{
		FileCheck_OnlyFilesView* view;
		view = (FileCheck_OnlyFilesView*) m_rowSplitter[0].GetPane(0,0);
		view->InitList(m_diff_result_LR, 0);
		view = (FileCheck_OnlyFilesView*) m_rowSplitter[0].GetPane(1,0);
		view->InitList(m_diff_result_LR, 1);
	}

	{
		FileCheck_NoFileView* view;
		view = (FileCheck_NoFileView*) m_rowSplitter[0].GetPane(2,0);
		view->InitList(m_diff_result_LR);
	}


	{
		FileCheck_ProgressMD5View* view;
		view = (FileCheck_ProgressMD5View*) m_rowSplitter[1].GetPane(1,0);
		view->InitList(m_diff_result_LR);
	}

	{
		FileCheck_ProgressBarView* view;
		view = (FileCheck_ProgressBarView*) m_rowSplitter[1].GetPane(0,0);
		view->InitProgressBar(m_diff_result_LR);
	}

	{
		FileCheck_MD5ModifiedView* modified_view = (FileCheck_MD5ModifiedView*)m_mainSplitter.GetPane(0, 2);
		modified_view->InitDiffResult(m_diff_result_LR);
	}

	
}



void FileCheck_MD5FrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);
	
	//m_mainSplitter.MoveWindow(0,0,cx,cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL FileCheck_MD5FrameWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// return CFrameWndEx::OnEraseBkgnd(pDC);
	return TRUE;
}

void FileCheck_MD5FrameWnd::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetMainFrame()->DestroyPopupWndFromList(this);

	CFrameWndEx::PostNcDestroy();
}

BOOL FileCheck_MD5FrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	int fullscreen_w = ::GetSystemMetrics(SM_CXSCREEN);
	int fullscreen_h = ::GetSystemMetrics(SM_CYSCREEN);

	cs.cx = fullscreen_w * 9 / 10;
	cs.cy = fullscreen_h * 5 / 6;

	cs.y = (fullscreen_h - cs.cy) / 2;
	cs.x = (fullscreen_w - cs.cx) / 2;

	return TRUE;
}
