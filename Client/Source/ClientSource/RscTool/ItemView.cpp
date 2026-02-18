// ItemView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "ItemView.h"
#include "ItemListView.h"
#include "Category1stView.h"
#include "Category2ndView.h"

// ItemView

IMPLEMENT_DYNCREATE(ItemView, CView)

ItemView::ItemView() : m_category_1st_view(NULL), m_category_2nd_view(NULL)
{

}

ItemView::~ItemView()
{
}

BEGIN_MESSAGE_MAP(ItemView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// ItemView 그리기입니다.

void ItemView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// ItemView 진단입니다.

#ifdef _DEBUG
void ItemView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void ItemView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// ItemView 메시지 처리기입니다.

int ItemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	
	CCreateContext cc;
	memset(&cc, 0, sizeof(CCreateContext));
	
	m_splitter.CreateStatic(this, 1, 2);
	m_splitter_col0.CreateStatic(&m_splitter, 2, 1, WS_CHILD|WS_VISIBLE, 
		m_splitter.IdFromRowCol(0,0) );
	// 

//	if (!m_splitter.CreateView(0, 0, RUNTIME_CLASS(PathView), CSize(0,0), &cc))
//		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(ItemListView);
	if (!m_splitter.CreateView(0, 1, RUNTIME_CLASS(ItemListView), CSize(0,0), &cc))
		return FALSE;
	
	cc.m_pNewViewClass = RUNTIME_CLASS(Category1stView);
	
	if (!m_splitter_col0.CreateView(0,0, RUNTIME_CLASS(Category1stView), CSize(0,0), &cc))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(Category2ndView);

	if (!m_splitter_col0.CreateView(1,0, RUNTIME_CLASS(Category2ndView), CSize(0,0), &cc))
		return FALSE;

//	cc.m_pNewViewClass = RUNTIME_CLASS(PathView);



	m_splitter.SetRedraw(FALSE);	// SetRedraw는 그리지 않는 블록구간을 설정하는 함수임..
	// (창최대화된 상태로 생성시..OnInitialUpdate()이전에 창이 렌더링되는 MFC버그에 기인함 )

	m_category_1st_view = static_cast<Category1stView*>(m_splitter_col0.GetPane(0,0));
	m_category_2nd_view = static_cast<Category2ndView*>(m_splitter_col0.GetPane(1,0));
	m_ItemListView = static_cast<ItemListView*>(m_splitter.GetPane(0, 1));
	
	m_category_1st_view->SetItemView(this);
	m_category_2nd_view->SetItemView(this);


	return 0;
}

void	ItemView::UpdateAllListCtrls()
{
// TODO : category_1st_view에 대한 셀렉션 0번 처리를 해줄수도 있고 아니면 함수 자체를 삭제할것임..
//	m_category_2nd_view->UpdateListCtrl();	
}


void	ItemView::DeleteAllListCtrls()
{
	m_category_1st_view->ResetSelection();
	m_category_2nd_view->DeleteAllItems();
	m_ItemListView->DeleteAllItems();
}

void ItemView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	m_splitter.SetRedraw(TRUE);	// SetRedraw는 그리지 않는 블록구간을 설정하는 함수임..
	// (창최대화된 상태로 생성시..OnInitialUpdate()이전에 창이 렌더링되는 MFC버그에 기인함 )


	CRect cr;
	GetClientRect(&cr);

	m_splitter.SetRowInfo(0, cr.Height(), 0);
	m_splitter.SetColumnInfo(0, cr.Width() / 8, 0);
	m_splitter_col0.SetRowInfo(0, cr.Height()/2, 0);

}

void ItemView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_splitter.MoveWindow(0, 0, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL ItemView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}
