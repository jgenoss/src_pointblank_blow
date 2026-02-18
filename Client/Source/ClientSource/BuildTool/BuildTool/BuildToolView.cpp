
// BuildToolView.cpp : CBuildToolView 클래스의 구현
//

#include "stdafx.h"
#include "BuildTool.h"

#include "BuildToolDoc.h"
#include "BuildToolView.h"

#include "MainFrm.h"
#include "ui/ContentsView.h"
#include "ui/FileView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	struct TabViewCallback : i3MFCTabViewCtrlCallback
	{
		CBuildToolView*	owner;
		TabViewCallback(CBuildToolView* owner) : owner(owner) {}

		// 반복되는 비슷한 유형의 함수호출이 많아서 작성함
	protected:
		template<typename T>
		bool SetPaneState(T& t, BOOL bShow= TRUE, BOOL bActivate= TRUE)
		{
			if (t.GetSafeHwnd())
			{
				t.ShowPane(bShow, FALSE, bActivate);
				return true;
			}
			return false;
		}

	public:
		virtual void OnActivateView(i3MFCTabViewCtrl* ctrl, 
			CView* oldView, CView* newView, 
			int oldTabNum, int newTabNum) 
		{
			//
			// 탭은 수시로 일어나는 것이고, 로딩은 최초에 1번만 발생해야한다..
			// 구분 방법을 알아야 함..
			// 가장 무난한 것은.. PathMgr쪽 입력과 관련되서 처리하는 것이다..
			//

			CMainFrame* pMainFrame = GetMainFrame();

			if (NULL == pMainFrame)
				return;

			switch(newTabNum)
			{
			case 0:	// ContentsView
			{
				SetPaneState(pMainFrame->GetContentsDescBar(), TRUE, TRUE);
				SetPaneState(pMainFrame->GetMQContentsStatusBar(), TRUE, TRUE);
				SetPaneState(pMainFrame->GetQLContentsStatusBar(), TRUE, TRUE);		
				if (true == SetPaneState(pMainFrame->GetPackInnerFileSearchBar(), FALSE, FALSE))
					pMainFrame->GetPackInnerFileSearchBar().AdjustDockingLayout();
				if (true == SetPaneState(pMainFrame->Geti3PackToolBar(), FALSE, FALSE))
					pMainFrame->Geti3PackToolBar().AdjustDockingLayout();
				SetPaneState(pMainFrame->GetSyncPlayerBar(), TRUE, FALSE);

			}break;

			case 1:	// FileView
			{
				SetPaneState(pMainFrame->GetContentsDescBar(), FALSE, TRUE);
				SetPaneState(pMainFrame->GetMQContentsStatusBar(), FALSE, TRUE);
				SetPaneState(pMainFrame->GetQLContentsStatusBar(), FALSE, TRUE);				
				ui::i3PackToolBar& i3PackToolBar = pMainFrame->Geti3PackToolBar();
				if (true == SetPaneState(i3PackToolBar, TRUE, FALSE))
				{		
					i3PackToolBar.EnableDocking(CBRS_ALIGN_ANY);
					pMainFrame->DockPane(&i3PackToolBar);	// bottom.
					i3PackToolBar.AdjustDockingLayout();
				}
				// should be docked below PackToolBar (docking order).
				ui::PackedFileSearchBar& packedFileSearchBar = pMainFrame->GetPackInnerFileSearchBar();
				if (true == SetPaneState(packedFileSearchBar, TRUE, FALSE))
				{
					packedFileSearchBar.EnableDocking(CBRS_ALIGN_ANY);
					pMainFrame->DockPane(&packedFileSearchBar);	// bottom.
					packedFileSearchBar.AdjustDockingLayout();
				}

				static_cast<ui::FileView*>(newView)->CreateExplorer();

			}break;

			default:
			{
			}break;
			}
		}
	};

}

// CBuildToolView

IMPLEMENT_DYNCREATE(CBuildToolView, CView)

BEGIN_MESSAGE_MAP(CBuildToolView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CBuildToolView 생성/소멸

CBuildToolView::CBuildToolView()
{
	m_tabViewCtrl.SetCallback(new TabViewCallback(this));
}

CBuildToolView::~CBuildToolView()
{
	delete m_tabViewCtrl.GetCallback();
}

BOOL CBuildToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CBuildToolView 그리기

void CBuildToolView::OnDraw(CDC* /*pDC*/)
{
	CBuildToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CBuildToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CBuildToolView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CBuildToolView 진단

#ifdef _DEBUG
void CBuildToolView::AssertValid() const
{
	CView::AssertValid();
}

void CBuildToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBuildToolDoc* CBuildToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBuildToolDoc)));
	return (CBuildToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CBuildToolView 메시지 처리기
int CBuildToolView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_tabViewCtrl.Create(this);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(ui::ContentsView), _T("Contents"), 0);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(ui::FileView), _T("Files"), 1);

	return 0;
}

void CBuildToolView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_tabViewCtrl.Embed_OnSize(cx, cy);
}

void CBuildToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	GetMainFrame()->SetViews((ui::ContentsView*)m_tabViewCtrl.GetView(0), (ui::FileView*)m_tabViewCtrl.GetView(1));
}

BOOL CBuildToolView::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}