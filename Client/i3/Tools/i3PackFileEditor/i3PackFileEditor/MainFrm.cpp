// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3PackFileEditor.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "i3PackFileEditorView.h"
#include ".\mainfrm.h"
#include "i3PackFileEditorDoc.h"
#include "GlobalVariable.h"
#include "OptionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TOOL_OPTION, OnToolOption)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	m_pContext = NULL;
}

CMainFrame::~CMainFrame()
{
	I3_SAFE_RELEASE( m_pContext);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles( TRUE);
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("대화 상자 모음을 만들지 못했습니다.\n");
		return -1;		// 만들지 못했습니다.
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("크기 조정 막대를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	// TODO: 도구 설명을 표시하지 않으려면 이 줄을 제거하십시오.
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	{
		i3VideoInfo info;

		m_pContext = i3RenderContext::NewObject();

		info.m_bFullScreen = FALSE;
		info.m_bInterlaced = FALSE;
		info.m_FrameBufferCount = 1;
		info.m_Width = 10;
		info.m_Height = 10;
		info.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBA_8888;
		
		m_pContext->Create( &info, (void *) m_hWnd);
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// 분할 창을 만듭니다.
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(Ci3PackFileEditorView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.x = g_pProfile->m_MainWndX;
	cs.y = g_pProfile->m_MainWndY;
	cs.cx = g_pProfile->m_MainWndCX;
	cs.cy = g_pProfile->m_MainWndCY;

	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1];
	Ci3PackFileEditorDoc * pDoc;

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	pDoc = (Ci3PackFileEditorDoc *) GetActiveDocument();
	if( pDoc == NULL)
	{
		DragFinish( hDropInfo);
		CFrameWnd::OnDropFiles(hDropInfo);
		return;
	}

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		pDoc->AddFile( TempName);
	}

	DragFinish( hDropInfo);
}

void CMainFrame::OnDestroy()
{
	I3_SAFE_RELEASE( m_pContext);

	{
		CRect rect;

		GetWindowRect( &rect);

		g_pProfile->m_MainWndX = rect.left;
		g_pProfile->m_MainWndY = rect.top;
		g_pProfile->m_MainWndCX = rect.Width();
		g_pProfile->m_MainWndCY = rect.Height();
	}

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnToolOption()
{
	COptionDialog dlg;

	dlg.DoModal();
}
