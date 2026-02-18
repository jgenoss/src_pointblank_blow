// ItemListView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "ItemListView.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "ItemData.h"
#include "SubItemInfo.h"

#include "MainFrm.h"

// ItemListView

namespace
{
	struct ListCallback : i3MFCListCtrlCallback
	{
		ItemListView* m_owner;

		ListCallback(ItemListView* owner) : m_owner(owner) {}

		virtual void OnDblClk(i3MFCListCtrl* Ctrl, INT32 idx) 
		{
			///
			if (idx == -1)	return;
			///
			
			ItemData* wd = reinterpret_cast<ItemData*>(Ctrl->GetItemData(idx));
			GetMainFrame()->CreateItemDetailFrame( wd->CreateRscFileSet() );
		}


	};

}



IMPLEMENT_DYNCREATE(ItemListView, CView)

ItemListView::ItemListView()
{
	m_listCtrl.SetCallback(new ListCallback(this) );
}

ItemListView::~ItemListView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(ItemListView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ItemListView 그리기입니다.

void ItemListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// ItemListView 진단입니다.

#ifdef _DEBUG
void ItemListView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void ItemListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// ItemListView 메시지 처리기입니다.

int ItemListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER);

	m_listCtrl.InsertColumn(0, L"Item");

	
	
	return 0;
}

void ItemListView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_listCtrl.SetSize(cx, cy);
	m_listCtrl.SetColumnWidth(0, cx );

	
}


void	ItemListView::SetWeaponCategory(SubItemInfo* cat)
{
	m_listCtrl.DeleteAllItems();

	if (cat == NULL) return;
	
	const size_t num =	cat->GetNumItemData();
	i3::wstring strTemp;

	for (size_t i = 0 ; i < num ; ++i )
	{
		ItemData* data = cat->GetItemData(i);
		
		i3::mb_to_utf16(data->GetName(), strTemp);

		m_listCtrl.InsertItem(i, strTemp.c_str());
		
		m_listCtrl.SetItemData(i, DWORD_PTR(data));
	}

}

void	ItemListView::DeleteAllItems()
{
	m_listCtrl.DeleteAllItems();
	m_listCtrl.RedrawWindow();
}
