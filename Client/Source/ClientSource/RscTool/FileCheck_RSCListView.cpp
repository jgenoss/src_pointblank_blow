// FileCheck_RSCListView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_RSCListView.h"
#include "FileCheck_PackScriptMgr.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "FileCheckView.h"

#include "MainFrm.h"

namespace
{
	enum 
	{
		COMMAND_OPEN_MD5CHECK_RSCFILES = 1,
	};

	struct ListCallback : i3MFCListCtrlCallback
	{
		ListCallback(FileCheck_RSCListView* owner) : owner(owner) {} FileCheck_RSCListView* owner;

		virtual void OnClick(i3MFCListCtrl* Ctrl, int index, int subItem) 	
		{
			if (index == -1)
				return;

			const i3::vector<RSCFileResultPtr>& res_list = FileCheck_PackScriptMgr::i()->GetRSCFileResultList();
			
			FileCheckView* parent = (FileCheckView*) owner->GetParent()->GetParent()->GetParent();
			
			parent->Update_ResultView(res_list[index].get());
		}

		virtual void OnContextMenu(i3MFCListCtrl* Ctrl, const CPoint& pt) 
		{
			CPoint ptClient = pt;
			Ctrl->ScreenToClient(&ptClient);
			int target_idx = Ctrl->HitTest(ptClient);

			CMenu menu;
			menu.CreatePopupMenu();

			menu.AppendMenu(MF_STRING, COMMAND_OPEN_MD5CHECK_RSCFILES, L"Open MD5-Check Window with RSC Files");		
			long result_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, Ctrl);
			menu.DestroyMenu();

			// 아랫부분은 필이 수정해야한다..
			if (Ctrl->GetFirstSelectedItem() == -1)
			{
				if (target_idx != -1)
					Ctrl->SetItemState(target_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			}
			else
			{
				target_idx = Ctrl->GetFirstSelectedItem();
			}

			switch( result_cmd)
			{
			case COMMAND_OPEN_MD5CHECK_RSCFILES:
				owner->OpenMD5CheckFrameWnd();
				break;

			default:
				break;
			}

		}	

	};
}

// FileCheck_RSCListView

IMPLEMENT_DYNCREATE(FileCheck_RSCListView, CView)

FileCheck_RSCListView::FileCheck_RSCListView() 
{
	m_listCtrl.SetCallback(new ListCallback(this));
}

FileCheck_RSCListView::~FileCheck_RSCListView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(FileCheck_RSCListView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_RSCListView 그리기입니다.

void FileCheck_RSCListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_RSCListView 진단입니다.

#ifdef _DEBUG
void FileCheck_RSCListView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_RSCListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_RSCListView 메시지 처리기입니다.

int FileCheck_RSCListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);

	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_listCtrl.GetFont(), FALSE);

	m_title.SetWindowText(L"RSC FileList");

	m_listCtrl.InsertColumn(0, L"File");
//	m_listCtrl.HideHeader();

	return 0;
}

void FileCheck_RSCListView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_title.MoveWindow(0,0, cx, GetSystemMetrics(SM_CYCAPTION) );
	m_listCtrl.MoveWindow(0, GetSystemMetrics(SM_CYCAPTION), cx, cy-GetSystemMetrics(SM_CYCAPTION) );


	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);
	}
	
	m_listCtrl.SetColumnWidth(0, cx);
	
	this->RedrawWindow();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL FileCheck_RSCListView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void FileCheck_RSCListView::UpdateRSCList()
{
	m_listCtrl.SetRedraw(FALSE);
	m_listCtrl.DeleteAllItems();		// 일단 깨끄시 지우고 처리..

	const i3::vector<RSCFileResultPtr>& res_list = FileCheck_PackScriptMgr::i()->GetRSCFileResultList();

	const size_t num_list = res_list.size();
	
	i3::wstring wstrTemp;


	for (size_t i = 0 ; i < num_list ; ++i)
	{
		RSCFileResult* res = res_list[i].get();
		i3::mb_to_utf16(res->rsc_file_path, wstrTemp);
		m_listCtrl.InsertItem(i, wstrTemp.c_str() );
	}


	CRect rc;	GetClientRect(&rc);
	// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
	rc.right -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);

	m_listCtrl.SetColumnWidth(0, rc.right);	

	m_listCtrl.SetRedraw(TRUE);	
	m_listCtrl.RedrawWindow();	


}

void FileCheck_RSCListView::ClearList()
{
	m_listCtrl.SetRedraw(FALSE);
	m_listCtrl.DeleteAllItems();		// 일단 깨끄시 지우고 처리..
	m_listCtrl.SetRedraw(TRUE);

	m_listCtrl.RedrawWindow();
}

void	FileCheck_RSCListView::OpenMD5CheckFrameWnd()
{
	i3::vector<int> sel_list;
	m_listCtrl.GetAllSelectedItems(sel_list);

	const size_t num_sel = sel_list.size();

	const i3::vector<RSCFileResultPtr>& res_list = FileCheck_PackScriptMgr::i()->GetRSCFileResultList();
	
	i3::vector<RSCFileResultPtr> result_list(num_sel);

	for (size_t i = 0; i < num_sel ; ++i)
	{
		result_list[i] = res_list[ sel_list[i] ] ;
	}

	GetMainFrame()->CreateFileCheck_MD5Frame(result_list);
}
