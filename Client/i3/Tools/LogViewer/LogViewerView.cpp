
// LogViewerView.cpp : CLogViewerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "LogViewer.h"
#endif

#include "LogViewerDoc.h"
#include "LogViewerView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogViewerView

IMPLEMENT_DYNCREATE(CLogViewerView, CView)

BEGIN_MESSAGE_MAP(CLogViewerView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CLogViewerView 생성/소멸

CLogViewerView::CLogViewerView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_sLogName = "Cast";
	m_sWorkingDiretory = "D:\\PBLog";
}

CLogViewerView::~CLogViewerView()
{
}

BOOL CLogViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CLogViewerView 그리기

void CLogViewerView::OnDraw(CDC* /*pDC*/)
{
	CLogViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CLogViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLogViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CLogViewerView 진단

#ifdef _DEBUG
void CLogViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CLogViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLogViewerDoc* CLogViewerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLogViewerDoc)));
	return (CLogViewerDoc*)m_pDocument;
}
#endif //_DEBUG


int CLogViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(1, 100, NULL);

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CRect rc;
	GetClientRect(rc);

	rc.top = 0;
	rc.left = 0;
	rc.right = 500;
	rc.bottom = 500;

	HMODULE hmod = LoadLibrary("SciLexer.dll");

	if (hmod == NULL)
	{
		MessageBox("The Scintilla DLL could not be loaded.", "Error loading Scintilla", MB_OK | MB_ICONERROR);
	}

	BOOL bRes = m_ScriptWnd.CreateEx(0, "Scintilla", "AAA", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN, rc, this, 0);

	if (bRes)
		m_ScriptWnd.Initialize();

	//m_Control.Init("Control");
	//m_Trans.Init("Trans");

	m_LogFile.Init(m_sWorkingDiretory, m_sLogName);

	return 0;
}

void CLogViewerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 	std::string valuse = m_Control.GetInOut();
	// 	m_sData.clear();
	// 	if (valuse.size() > 0)
	// 	{
	// 		m_ScriptWnd.AddText((char*)valuse.c_str());
	// 	}
	// 	valuse = m_Trans.GetInOut();
	// 	if (valuse.size() > 0)
	// 	{
	// 		m_ScriptWnd.AddText((char*)valuse.c_str());
	// 	}

	i3::string valuse = m_LogFile.GetInOut();
	if (valuse.size() > 0)
	{
		m_ScriptWnd.AddText((char*)valuse.c_str());
	}

	CView::OnTimer(nIDEvent);
}

void CLogViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (::IsWindow(m_ScriptWnd.m_hWnd))
	{
		CRect rc;
		GetClientRect(rc);

		m_ScriptWnd.SetWindowPos(NULL, rc.left, rc.top, rc.right, rc.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CLogViewerView::Search(INT32 nBeginDate, INT32 nEndDate)
{
	m_ScriptWnd.New();

	m_LogFile.SetSearchRange(nBeginDate, nEndDate);
	m_LogFile.ExtractLog();
}

void CLogViewerView::SetWorkingDirectory(i3::string Dir)
{
	m_ScriptWnd.New();
	m_sWorkingDiretory = Dir;
	m_LogFile.Init(m_sWorkingDiretory, m_sLogName);
	m_LogFile.ExtractLog();
}

void CLogViewerView::SetLogName(i3::string sName)
{ 
	m_ScriptWnd.New();
	m_sLogName = sName; 
	m_LogFile.Init(m_sWorkingDiretory, m_sLogName);
	m_LogFile.ExtractLog();

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetStateText(m_sLogName.c_str());
}