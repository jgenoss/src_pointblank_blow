// category1stView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "category1stView.h"
#include "eItemInfo.h"

#include "ItemView.h"
#include "Category2ndView.h"
#include "ItemListView.h"


namespace
{
	struct Category1stViewListCallback : i3MFCListCtrlCallback
	{
		Category1stView* owner;

		Category1stViewListCallback(Category1stView* owner) : owner(owner) {}

		virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
		{
			// 선택된 경우, 두번째 목록창 갱신으로 처리해준다...
			owner->GetItemView()->GetCategory2ndView()->UpdateListCtrl((eItemInfo)index);
			owner->GetItemView()->GetItemListView()->DeleteAllItems();
		}
	};

}



// Category1stView

IMPLEMENT_DYNCREATE(Category1stView, CView)

Category1stView::Category1stView() : m_ItemView(NULL)
{
	m_listCtrl.SetCallback(new Category1stViewListCallback(this));
}

Category1stView::~Category1stView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(Category1stView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Category1stView 그리기입니다.

void Category1stView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// Category1stView 진단입니다.

#ifdef _DEBUG
void Category1stView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void Category1stView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// Category1stView 메시지 처리기입니다.

int Category1stView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;


	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER|LVS_SINGLESEL);

	m_listCtrl.InsertColumn(0, L"Main Category");

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	
	m_listCtrl.InsertItem(0, L"Weapon");
	m_listCtrl.SetItemData(0, ITEMINFO_WEAPON);
	
	m_listCtrl.InsertItem(1, L"Equipment");
	m_listCtrl.SetItemData(1, ITEMINFO_EQUIPMENT);

	m_listCtrl.InsertItem(2, L"Character");
	m_listCtrl.SetItemData(2, ITEMINFO_CHARACTER);

	m_listCtrl.InsertItem(3, L"World");
	m_listCtrl.SetItemData(3, ITEMINFO_WORLD);

	return 0;
}

void Category1stView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_listCtrl.SetSize(cx, cy);
	m_listCtrl.SetColumnWidth(0, cx);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void	Category1stView::ResetSelection()
{
	m_listCtrl.UnselectAll();
}

