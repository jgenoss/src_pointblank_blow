// FileCheckView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheckView.h"

#include "FileCheck_PSTreeView.h"
#include "FileCheck_PSListView.h"
#include "FileCheck_RSCListView.h"
#include "FileCheck_ResultView.h"
#include "FileCheck_PackScriptMgr.h"
#include "PackMode.h"
#include "PathMode.h"

// FileCheckView

IMPLEMENT_DYNCREATE(FileCheckView, CView)

FileCheckView::FileCheckView() : m_PSTreeView(NULL), m_RSCListView(NULL)
{
	::memset(m_ResultView, 0, sizeof(FileCheck_ResultView*)*2);
}

FileCheckView::~FileCheckView()
{
}

BEGIN_MESSAGE_MAP(FileCheckView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheckView 그리기입니다.

void FileCheckView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheckView 진단입니다.

#ifdef _DEBUG
void FileCheckView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheckView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheckView 메시지 처리기입니다.

int FileCheckView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CCreateContext cc;
	memset(&cc, 0, sizeof(CCreateContext));

	m_splitter.CreateStatic(this, 1, 4);

	m_col0_splitter.CreateStatic(&m_splitter, 2, 1, WS_CHILD|WS_VISIBLE, m_splitter.IdFromRowCol(0,0));


	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_PSTreeView);
	if (!m_col0_splitter.CreateView(0, 0, RUNTIME_CLASS(FileCheck_PSTreeView), CSize(0,0), &cc))
		return FALSE;
	
	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_RSCListView);
	if (!m_col0_splitter.CreateView(1, 0, RUNTIME_CLASS(FileCheck_RSCListView), CSize(0,0), &cc))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_PSListView);
	if (!m_splitter.CreateView(0, 1, RUNTIME_CLASS(FileCheck_PSListView), CSize(0,0), &cc))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_ResultView);
	if (!m_splitter.CreateView(0, 2, RUNTIME_CLASS(FileCheck_ResultView), CSize(0,0), &cc))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(FileCheck_ResultView);
	if (!m_splitter.CreateView(0, 3, RUNTIME_CLASS(FileCheck_ResultView), CSize(0,0), &cc))
		return FALSE;
	
	m_PSTreeView = (FileCheck_PSTreeView*)m_col0_splitter.GetPane(0,0);
	m_PSListView = (FileCheck_PSListView*)m_splitter.GetPane(0,1);

	m_PSTreeView->SetPSListView(m_PSListView);
	m_PSListView->SetPSTreeView(m_PSTreeView);

	m_ResultView[0] = (FileCheck_ResultView*)m_splitter.GetPane(0, 2);
	m_ResultView[1] = (FileCheck_ResultView*)m_splitter.GetPane(0, 3);

	m_RSCListView = (FileCheck_RSCListView*)m_col0_splitter.GetPane(1, 0);

	return 0;
}

void	FileCheckView::InitCaption(PackMode pm)
{
	if (pm == PACKMODE_MEDIA_TO_QA_COPY)
	{
	
		m_ResultView[0]->InitPathMode(PATHMODE_MEDIA, 0, L"MEDIA Path Result Files");
		m_ResultView[1]->InitPathMode(PATHMODE_QA, 1, L"QA Path Result Files");

	}
	else
	{
		m_ResultView[0]->InitPathMode(PATHMODE_QA, 0, L"QA Path Result Files");
		m_ResultView[1]->InitPathMode(PATHMODE_LIVE, 1, L"LIVE Path Result Files");
	}
	
}



void FileCheckView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_splitter.MoveWindow(0,0, cx, cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL FileCheckView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void FileCheckView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	
	m_splitter.SetRedraw(TRUE);

	CRect cr;
	GetClientRect(&cr);

	m_splitter.SetRowInfo(0, cr.Height(), 0);

	m_splitter.SetColumnInfo(0, cr.Width() / 4, 0);
	m_splitter.SetColumnInfo(1, cr.Width() / 4, 0);
	m_splitter.SetColumnInfo(2, cr.Width() / 4, 0);
	m_splitter.SetColumnInfo(3, cr.Width() / 4, 0);

	m_col0_splitter.SetRowInfo(0, cr.Height()/2, 0);

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void	FileCheckView::Update_OnActivateView()
{
	m_PSTreeView->UpdatePackScriptFolder();	// 매회 처리한다.
	
//	m_RSCListView->UpdateRSCList();

	m_ResultView[0]->ClearList();
	m_ResultView[1]->ClearList();
	
}

void	FileCheckView::Update_AfterActivateView()
{
	m_RSCListView->UpdateRSCList();
}


void	FileCheckView::Update_OnDeactivateView()
{
	m_ResultView[0]->ClearList();
	m_ResultView[1]->ClearList();

	m_RSCListView->ClearList();
}

void	FileCheckView::Update_ResultView(const PackScriptFile* f)
{
	const PackScriptResultPtr res = FileCheck_PackScriptMgr::i()->FindPackScriptFileResult(f);
	
	m_ResultView[0]->UpdateList(res.get());
	m_ResultView[1]->UpdateList(res.get());
		
}

void	FileCheckView::Update_ResultView(const RSCFileResult* r)
{
	m_ResultView[0]->UpdateList(r);
	m_ResultView[1]->UpdateList(r);
}



