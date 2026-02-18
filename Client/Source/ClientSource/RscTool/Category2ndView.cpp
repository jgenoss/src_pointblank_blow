// Category2ndView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "Category2ndView.h"

#include "SubItemInfo.h"
#include "ItemInfoMgr.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "ItemListView.h"
#include "ItemView.h"

#include "ItemInfo.h"


namespace
{
	struct Category2ndViewCallback : i3MFCListCtrlCallback
	{
		Category2ndViewCallback(Category2ndView* owner) : owner(owner) {}
		Category2ndView*	owner;

		virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
		{
			SubItemInfo* cat = (index == -1) ? NULL :
								  reinterpret_cast<SubItemInfo*>(Ctrl->GetItemData(index));
			
			ItemListView* pItemListView = owner->GetItemView()->GetItemListView();

			pItemListView->SetWeaponCategory(cat);
			
		}
	
	};
		
}


// Category2ndView

IMPLEMENT_DYNCREATE(Category2ndView, CView)

Category2ndView::Category2ndView() : m_ItemView(NULL)
{
	m_listCtrl.SetCallback( new Category2ndViewCallback(this) );
}

Category2ndView::~Category2ndView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(Category2ndView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Category2ndView 그리기입니다.

void Category2ndView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// Category2ndView 진단입니다.

#ifdef _DEBUG
void Category2ndView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void Category2ndView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// Category2ndView 메시지 처리기입니다.

int Category2ndView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER|LVS_SINGLESEL);

	m_listCtrl.InsertColumn(0, L"Sub Category");

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

//	m_listCtrl.InsertItem(0, L"Weapon");
//	m_listCtrl.SetItemData(0, ITEM_MAIN_CATEGORY_WEAPON);
//	Category2ndView::UpdateListCtrl();

	return 0;
}

void	Category2ndView::UpdateListCtrl(eItemInfo e)
{
	m_listCtrl.DeleteAllItems();

	ItemInfo* info = ItemInfoMgr::i()->GetItemInfo(e);

	const int num_category =  info->GetNumSubItemInfo();

	i3::wstring strTemp;

	for (int i = 0 ; i < num_category ; ++i)
	{
		SubItemInfo* cat = info->GetSubItemInfo(i);
		i3::mb_to_utf16(cat->GetName(), strTemp);
		m_listCtrl.InsertItem(i, strTemp.c_str() );
		m_listCtrl.SetItemData(i, DWORD_PTR(cat));
	}
}

void	Category2ndView::DeleteAllItems()
{
	m_listCtrl.DeleteAllItems();
	m_listCtrl.RedrawWindow();
}


void Category2ndView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_listCtrl.SetSize(cx, cy);
	m_listCtrl.SetColumnWidth(0, cx);
}
