// FileCheck_PSTreeView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_PSTreeView.h"
#include "PackScriptTree.h"
#include "PackScriptMgr.h"

#include "FileCheck_PSListView.h"

namespace
{
	struct TreeCallback : i3MFCTreeCtrlCallback
	{
		TreeCallback(FileCheck_PSTreeView* owner) : owner(owner) {}		FileCheck_PSTreeView*	owner;

		virtual void OnSelChanged(i3MFCTreeCtrl* Ctrl, HTREEITEM hItem, void* ItemData)
		{
			FileCheck_PSListView* pListView = owner->GetPSListView();

			pListView->UpdateListCtrl(reinterpret_cast<PackScriptFolder*>(ItemData) );
		}

	};

}


// FileCheck_PSTreeView

IMPLEMENT_DYNCREATE(FileCheck_PSTreeView, CView)

FileCheck_PSTreeView::FileCheck_PSTreeView() : m_pPSListView(NULL)
{
	m_treeCtrl.SetCallback(new TreeCallback(this));
}

FileCheck_PSTreeView::~FileCheck_PSTreeView()
{
	delete m_treeCtrl.GetCallback();
}


BEGIN_MESSAGE_MAP(FileCheck_PSTreeView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_PSTreeView 그리기입니다.

void FileCheck_PSTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_PSTreeView 진단입니다.

#ifdef _DEBUG
void FileCheck_PSTreeView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_PSTreeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_PSTreeView 메시지 처리기입니다.
namespace
{
	void	AddItem_PackScriptFolder_Recursive(i3MFCTreeCtrl* treectrl, PackScriptFolder* parent, HTREEITEM parentItem)
	{
		const i3::vector<PackScriptFolder*>& child_list =	parent->GetChildList();
		const std::size_t num_child = child_list.size();
		for (std::size_t i = 0 ; i < num_child; ++i )
		{
			PackScriptFolder* child = child_list[i];
			HTREEITEM myItem = treectrl->AddItem( parentItem, child->GetFolderName().c_str(), child); //, child->HasChildFolder() );
			AddItem_PackScriptFolder_Recursive(treectrl, child, myItem);
		}
	}
}

int FileCheck_PSTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	m_treeCtrl.CreateEx(this, 1);

	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_treeCtrl.GetFont(), FALSE);
	m_title.SetWindowText(L"PackScript Folders");

	PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();

	if (root)
	{
		HTREEITEM root_item = m_treeCtrl.AddRootItem( root->GetFolderName().c_str(), root); //, m_root->HasChildFolder() );

		AddItem_PackScriptFolder_Recursive(&m_treeCtrl, root, root_item);

		m_treeCtrl.Expand( m_treeCtrl.GetRootItem(), TVE_EXPAND);	// 루트는 미리 펼쳐둔다.
	}
	
	m_treeCtrl.RedrawWindow();

	return 0;
}

void	FileCheck_PSTreeView::UpdatePackScriptFolder()
{
	PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();

	if ( this->GetSafeHwnd() )
	{
		m_treeCtrl.SetRedraw(FALSE);
		m_treeCtrl.DeleteAllItems();
//		this->GetRightView()->UpdateListCtrl(NULL);	// 트리업데이트시 리스트쪽 리셋도 함께 한다.
		
		if (root)
		{
			HTREEITEM root_item = m_treeCtrl.AddRootItem( root->GetFolderName().c_str(), root); //, m_root->HasChildFolder() );
			AddItem_PackScriptFolder_Recursive(&m_treeCtrl, root, root_item);
			m_treeCtrl.Expand( m_treeCtrl.GetRootItem(), TVE_EXPAND);	// 루트는 미리 펼쳐둔다.

			m_treeCtrl.SelectItem(root_item);
		}

		m_treeCtrl.SetRedraw(TRUE);
		m_treeCtrl.RedrawWindow();
	}

}


void FileCheck_PSTreeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_title.MoveWindow(0,0, cx, GetSystemMetrics(SM_CYCAPTION) );
	m_treeCtrl.MoveWindow(0, GetSystemMetrics(SM_CYCAPTION), cx, cy-GetSystemMetrics(SM_CYCAPTION) );

}

BOOL FileCheck_PSTreeView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

namespace
{

	HTREEITEM FindItem(const CString& name, CTreeCtrl& tree, HTREEITEM hRoot)
	{
		// check whether the current item is the searched one
		CString text = tree.GetItemText(hRoot);
		if (text.Compare(name) == 0)
			return hRoot; 

		// get a handle to the first child item
		HTREEITEM hSub = tree.GetChildItem(hRoot);
		// iterate as long a new item is found
		while (hSub)
		{
			// check the children of the current item
			HTREEITEM hFound = FindItem(name, tree, hSub);
			if (hFound)
				return hFound; 

			// get the next sibling of the current item
			hSub = tree.GetNextSiblingItem(hSub);
		} 

		// return NULL if nothing was found
		return NULL;
	}

	HTREEITEM FindItem(const void* data, CTreeCtrl& tree, HTREEITEM hRoot)
	{
		// check whether the current item is the searched one
		void* item_data = (void*) tree.GetItemData(hRoot);
		
		if (item_data == data)
			return hRoot;

		// get a handle to the first child item
		HTREEITEM hSub = tree.GetChildItem(hRoot);
		// iterate as long a new item is found
		while (hSub)
		{
			// check the children of the current item
			HTREEITEM hFound = FindItem(data, tree, hSub);
			if (hFound)
				return hFound; 

			// get the next sibling of the current item
			hSub = tree.GetNextSiblingItem(hSub);
		} 

		// return NULL if nothing was found
		return NULL;
	}

}


void	FileCheck_PSTreeView::SelectFolder(const PackScriptFolder* folder)
{
	
	HTREEITEM hItem = FindItem(folder, m_treeCtrl, m_treeCtrl.GetRootItem());

	m_treeCtrl.SelectItem(hItem);
	
}
