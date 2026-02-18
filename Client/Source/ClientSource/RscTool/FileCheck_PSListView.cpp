// FileCheck_PSListView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_PSListView.h"
#include "PackScriptTree.h"
#include "IconInfo.h"

#include "FileCheckView.h"

#include "MainFrm.h"
#include "FileCheck_PackScriptMgr.h"

#include "FileCheck_PSTreeView.h"

namespace
{
	enum
	{
		COMMAND_OPEN_MD5CHECK_PSFILES = 1,
	};

	struct ListCallback : i3MFCListCtrlCallback
	{
		ListCallback(FileCheck_PSListView* owner) : owner(owner) {} FileCheck_PSListView* owner;

		virtual void OnClick(i3MFCListCtrl* Ctrl, int index, int subItem) 
		{
			if (index == -1)
				return;

			PSVariant* var = owner->GetListVariant(index);		
			
			struct type_vis : i3::static_visitor<>
			{
				type_vis(FileCheck_PSListView* owner) : owner(owner) {} FileCheck_PSListView* owner;
				void operator()(const PackScriptFile* f) const
				{
					FileCheckView* parent = (FileCheckView*) owner->GetParent()->GetParent();
					parent->Update_ResultView(f);
				}
			};
			
			i3::apply_visitor(type_vis(owner), *var);
		}

		virtual void OnDblClk(i3MFCListCtrl* Ctrl, INT32 idx) 
		{
			if (idx == -1)
				return;

			PSVariant* var = owner->GetListVariant(idx);		

			struct type_vis : i3::static_visitor<>
			{
				type_vis(FileCheck_PSListView* owner) : owner(owner) {} FileCheck_PSListView* owner;
				void operator()(const PackScriptFolder* folder) const
				{
					owner->GetPSTreeView()->SelectFolder(folder);
				//	owner->UpdateListCtrl(folder);
				//	FileCheckView* parent = (FileCheckView*) owner->GetParent()->GetParent();
				//	parent->Update_ResultView(folder);
				}
			};
			
			i3::apply_visitor(type_vis(owner), *var);

		}		// 일단 싱글을 염두에 두고..



		virtual void OnContextMenu(i3MFCListCtrl* Ctrl, const CPoint& pt) 
		{
			CPoint ptClient = pt;
			Ctrl->ScreenToClient(&ptClient);
			int target_idx = Ctrl->HitTest(ptClient);

			CMenu menu;
			menu.CreatePopupMenu();

			menu.AppendMenu(MF_STRING, COMMAND_OPEN_MD5CHECK_PSFILES, L"Open MD5-Check Window with PackScripts");		
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
			case COMMAND_OPEN_MD5CHECK_PSFILES:
				owner->OpenMD5CheckFrameWnd();
			break;

			default:
				break;
			}

		}
	};
}


// FileCheck_PSListView

IMPLEMENT_DYNCREATE(FileCheck_PSListView, CView)

FileCheck_PSListView::FileCheck_PSListView() : m_folder(NULL), m_pPSTreeView(NULL)
{
	m_listCtrl.SetCallback(new ListCallback(this));
}

FileCheck_PSListView::~FileCheck_PSListView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(FileCheck_PSListView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_PSListView 그리기입니다.

void FileCheck_PSListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_PSListView 진단입니다.

#ifdef _DEBUG
void FileCheck_PSListView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_PSListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_PSListView 메시지 처리기입니다.

int FileCheck_PSListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	m_listCtrl.Create(this, 1); 
	m_listCtrl.InsertColumn(0, L"Folders or Files");
//	m_listCtrl.HideHeader();

	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_listCtrl.GetFont(), FALSE);

	m_title.SetWindowText(L"PackScript Child Folders/Files");

	return 0;
}

void FileCheck_PSListView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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

}

BOOL FileCheck_PSListView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void	FileCheck_PSListView::UpdateListCtrl(PackScriptFolder* pData)
{
	m_listCtrl.SetRedraw(FALSE);
	m_listCtrl.DeleteAllItems();		// 일단 깨끄시 지우고 처리..
	m_vec_varPS.clear();

	m_folder = pData;

	if (pData == NULL)
		return;
	
	const i3::vector<PackScriptFolder*>& child_list = pData->GetChildList();

	const size_t num_child = child_list.size();

	int folder_icon_index = GetFolderItemIconIndex();

	for (size_t i = 0 ; i < num_child ; ++i )
	{
		PackScriptFolder* f = child_list[i];
		m_listCtrl.InsertItem(i, f->GetFolderName().c_str(), folder_icon_index);
		m_vec_varPS.push_back( i3::make_shared<PSVariant>(f) );
	}

	const i3::vector<PackScriptFile*>& file_list = pData->GetFileList();
	
	const std::size_t num_file = file_list.size();
	
	for (std::size_t i = 0 ; i < num_file ; ++i )
	{
		PackScriptFile* f = file_list[i];

		m_listCtrl.AddItem(f->GetFileTitle().c_str(), NULL);
		m_vec_varPS.push_back(i3::make_shared<PSVariant>(f));
	}

	
	CRect rc;	GetClientRect(&rc);

	// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
	rc.right -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);

	m_listCtrl.SetColumnWidth(0, rc.right);	

	m_listCtrl.SetRedraw(TRUE);	

	m_listCtrl.RedrawWindow();

}

void	FileCheck_PSListView::OpenMD5CheckFrameWnd()
{
	i3::vector<int> sel_list;
	m_listCtrl.GetAllSelectedItems(sel_list);
	
	const size_t num_sel = sel_list.size();

	i3::vector<PSVariant*>	sel_var_list(num_sel);
	
	for ( size_t i = 0 ; i < num_sel ; ++i)
		sel_var_list[i] = m_vec_varPS[ sel_list[i] ].get();

	struct type_vis : i3::static_visitor<>
	{
		type_vis(i3::vector<PackScriptResultPtr>& psr_list) : psr_list(psr_list) {} i3::vector<PackScriptResultPtr>& psr_list;
		void operator()(const PackScriptFile* f) const
		{
			PackScriptResultPtr res = FileCheck_PackScriptMgr::i()->FindPackScriptFileResult(f);
			if (res)
				psr_list.push_back(res);
		}

		void operator()(const PackScriptFolder* f) const
		{
			i3::vector<PackScriptFile*> psf_list;
			CollectPackScriptFiles(f, psf_list);
			
			const size_t num_psf = psf_list.size();

			for (size_t i = 0 ; i < num_psf ; ++i )
			{
				PackScriptResultPtr res = FileCheck_PackScriptMgr::i()->FindPackScriptFileResult(psf_list[i]);
				if (res)
					psr_list.push_back(res);
			}
		}
	};
	
	i3::vector<PackScriptResultPtr> psr_list;
	
	const size_t num_var = sel_var_list.size();
	for (size_t i = 0 ; i < num_var ; ++i)
		i3::apply_visitor(type_vis(psr_list), *sel_var_list[i]);
	
	GetMainFrame()->CreateFileCheck_MD5Frame(psr_list);
}
