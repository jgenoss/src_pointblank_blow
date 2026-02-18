// PackScriptFileExplorerTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptFileExplorerTreeView.h"

#include "PackScriptFileExplorerListView.h"

#include "PackScriptFileTaskPane.h"

#include "PathMgr.h"

// PackScriptFileExplorerTreeView
namespace
{
	struct PackScriptExplorerTreeCallback : i3MFCTreeCtrlCallback
	{
		virtual void OnDragEnd(i3MFCTreeCtrl* thisCtrl, 
								HTREEITEM hItem, CWnd* targetWnd, const CPoint& ptScreen) 
		{
			i3ShellTreeCtrl* shell_tree = static_cast<i3ShellTreeCtrl*>(thisCtrl);
			
			CString fullPath;
			shell_tree->GetItemPath(fullPath, hItem);
			
		
			//
			// 주 타겟은 일단 해당 콘트롤 부모가 PackScriptFileTaskPane 인것으로..
			// 확인되면 그쪽 함수를 호출하도록 연결한다...
			//
			CWnd* parent_of_target = targetWnd->GetParent();

			if ( parent_of_target->GetRuntimeClass() 
					== RUNTIME_CLASS(PackScriptFileTaskPane) )
			{
				PackScriptFileTaskPane* pane = static_cast<PackScriptFileTaskPane*>(parent_of_target);
				
				i3::vector<i3::wstring> vecFullPath(1);
				vecFullPath[0].assign(LPCWSTR(fullPath), fullPath.GetLength() );

				pane->OnDragEndFromExplorer( targetWnd, ptScreen, vecFullPath);					
			}
		}

		virtual void OnDragMove(i3MFCTreeCtrl* thisCtrl, 
								HTREEITEM hItem, CWnd* targetWnd, const CPoint& ptScreen) 
		{

			CWnd* parent_of_target = targetWnd->GetParent();
			
			if (parent_of_target && parent_of_target->GetRuntimeClass() 
					== RUNTIME_CLASS(PackScriptFileTaskPane) )
			{
				PackScriptFileTaskPane* pane = static_cast<PackScriptFileTaskPane*>(parent_of_target);
				pane->OnDragMoveFromExplorer( targetWnd, ptScreen);		
			}
		}
	};
}

IMPLEMENT_DYNCREATE(PackScriptFileExplorerTreeView, CView)

PackScriptFileExplorerTreeView::PackScriptFileExplorerTreeView()
{
	m_wndShellTree.SetCallback(new PackScriptExplorerTreeCallback);
}

PackScriptFileExplorerTreeView::~PackScriptFileExplorerTreeView()
{
	delete m_wndShellTree.GetCallback();
}

BEGIN_MESSAGE_MAP(PackScriptFileExplorerTreeView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// PackScriptFileExplorerTreeView drawing



void PackScriptFileExplorerTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// PackScriptFileExplorerTreeView diagnostics

#ifdef _DEBUG
void PackScriptFileExplorerTreeView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void PackScriptFileExplorerTreeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// PackScriptFileExplorerTreeView message handlers

int PackScriptFileExplorerTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndShellTree.SetRootPath( PathMgr::i()->GetMainMediaPath().c_str() );

	CRect rectDummy (0, 0, 0, 0);

	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_TRACKSELECT | //TVS_HASLINES |
			TVS_HASBUTTONS| TVS_SHOWSELALWAYS| TVS_FULLROWSELECT |TVS_EDITLABELS;		// TVS_SHOWSELALWAYS 일단 추가해봄.(나름 필요한 옵션임)
		
	m_wndShellTree.Create(dwViewStyle, rectDummy, this, 1);

	::SetWindowTheme(m_wndShellTree.GetSafeHwnd(), L"explorer", 0);
	m_wndShellTree.SetExtendedStyle( 
		TVS_EX_FADEINOUTEXPANDOS | TVS_EX_DOUBLEBUFFER  
		, //TVS_EX_AUTOHSCROLL | 
		TVS_EX_FADEINOUTEXPANDOS | TVS_EX_DOUBLEBUFFER );



//	pFont->GetLogFont(&lf);
//	lf.lfQuality = CLEARTYPE_QUALITY;
	
//	CFont font;
//	font.CreateFontIndirect(&lf);
//	m_wndShellTree.SetFont(&font);


	// TODO:  Add your specialized creation code here
	m_wndShellTree.SelectRoot();

	return 0;
}

void PackScriptFileExplorerTreeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_wndShellTree.SetWindowPos(NULL,-1, -1,cx, cy, SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	// TODO: Add your message handler code here
}

void PackScriptFileExplorerTreeView::SetRelation(class PackScriptFileExplorerListView* pListView)
{
	m_wndShellTree.SetRelatedList(pListView->GetListCtrl());
}

BOOL PackScriptFileExplorerTreeView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}
