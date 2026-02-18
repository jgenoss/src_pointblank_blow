// PackScriptFileExplorerListView.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptFileExplorerListView.h"


#include "PackScriptFileTaskPane.h"

// PackScriptFileExplorerListView

namespace
{
	struct PackScriptExplorerListCallback : i3MFCListCtrlCallback
	{
		virtual void OnDragEnd(i3MFCListCtrl* Ctrl, const i3::vector<int>& DragIndexList, CWnd* targetWnd, const CPoint& ptScreen) 
		{
			CWnd* parent_of_target = targetWnd->GetParent();

			if ( parent_of_target->GetRuntimeClass() 
				!= RUNTIME_CLASS(PackScriptFileTaskPane) )
				return;
			
			i3ShellListCtrl* shell_list = static_cast<i3ShellListCtrl*>(Ctrl);
			i3::vector<i3::wstring>	fullPathList;
			CString fullPath;

			const size_t num_drag = DragIndexList.size();

			for (size_t i = 0 ; i < num_drag; ++i )
			{
				int nIdx = DragIndexList[i];
				shell_list->GetItemPath(fullPath, nIdx);
				fullPathList.push_back( i3::wstring(LPCWSTR(fullPath), fullPath.GetLength() ) );
			}
			
			//
			// 주 타겟은 일단 해당 콘트롤 부모가 PackScriptFileTaskPane 인것으로..
			// 확인되면 그쪽 함수를 호출하도록 연결한다...
			PackScriptFileTaskPane* pane = static_cast<PackScriptFileTaskPane*>(parent_of_target);
			pane->OnDragEndFromExplorer( targetWnd, ptScreen, fullPathList);					
			
		}

		virtual void OnDragMove(i3MFCListCtrl* Ctrl, const i3::vector<int>& DragIndexList, CWnd* targetWnd, const CPoint& ptScreen) 
		{
			CWnd* parent_of_target = targetWnd->GetParent();
			if (parent_of_target && parent_of_target->GetRuntimeClass() 
				!= RUNTIME_CLASS(PackScriptFileTaskPane) )
				return;
			
			PackScriptFileTaskPane* pane = static_cast<PackScriptFileTaskPane*>(parent_of_target);
			pane->OnDragMoveFromExplorer( targetWnd, ptScreen);		
			
		}
	};
}


IMPLEMENT_DYNCREATE(PackScriptFileExplorerListView, CView)

PackScriptFileExplorerListView::PackScriptFileExplorerListView()
{
	m_wndList.SetCallback(new PackScriptExplorerListCallback);
}

PackScriptFileExplorerListView::~PackScriptFileExplorerListView()
{
	delete m_wndList.GetCallback();
}

BEGIN_MESSAGE_MAP(PackScriptFileExplorerListView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// PackScriptFileExplorerListView drawing

void PackScriptFileExplorerListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// PackScriptFileExplorerListView diagnostics

#ifdef _DEBUG
void PackScriptFileExplorerListView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void PackScriptFileExplorerListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// PackScriptFileExplorerListView message handlers

int PackScriptFileExplorerListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	
	m_wndList.Create(WS_CHILD | WS_VISIBLE|LVS_REPORT /*|LVS_SHOWSELALWAYS*/
		, CRect(0,0,0,0), this, 1);
	m_wndList.SetExtendedStyle(LVS_EX_FULLROWSELECT| LVS_EX_DOUBLEBUFFER);

	::SetWindowTheme(m_wndList.GetSafeHwnd(), L"explorer", 0);

	return 0;
}

void PackScriptFileExplorerListView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_wndList.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}



BOOL PackScriptFileExplorerListView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}
