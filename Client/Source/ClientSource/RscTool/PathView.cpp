// PathView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PathView.h"
#include "IconInfo.h"

#include "PathDlg.h"
#include "PackPath.h"
#include "PathMgr.h"

#include "i3Base/itl/range/algorithm/find.h"
#include "i3Base/itl/range/algorithm/find_if.h"
#include "i3Base/string/compare/generic_is_equal.h"

#include "PackScriptLeftView.h"

#include "MainFrm.h"
#include "ItemView.h"

#include "ExecuteTSVN.h"

// PathView
namespace
{
	enum
	{
		COMMAND_ADD_PATH = 1,
		COMMAND_EDIT_PATH,
		COMMAND_DELETE_PATH,
		COMMAND_CLONE_PATH,

		COMMAND_TSVN_UPDATE_MEDIA,
		COMMAND_TSVN_UPDATE_QA,
		COMMAND_TSVN_UPDATE_LIVE,
		COMMAND_TSVN_UPDATE_ALL,
	};


struct PathViewListCallback : i3MFCListCtrlCallback
{
	PathView* m_owner;
	PathViewListCallback(PathView* owner) : m_owner(owner) {}

	virtual void OnDblClk(i3MFCListCtrl* Ctrl, INT32 idx) 
	{
		// 클릭한 놈 기준으로 PathMgr 갱신하고...
		// 팩스크립트쪽 왼쪽,오른쪽 창 갱신한다...
		PathMgr::i()->SelectPathName( idx );
						
		m_owner->GetLeftView()->UpdatePackScriptFolder();

		if (idx != -1)
			AfxGetMainWnd()->SetWindowText(PathMgr::i()->GetPathName().c_str());
		else
			AfxGetMainWnd()->SetWindowText(L"RscTool");

	}		// 일단 싱글을 염두에 두고..

	virtual void OnDragEnd(i3MFCListCtrl* Ctrl, const i3::vector<int>& DragIndexList, CWnd* targetWnd, const CPoint& ptScreen) 
	{
		if (Ctrl != targetWnd)
			return;

		CPoint ptClient = ptScreen;
		Ctrl->ScreenToClient(&ptClient);
		int target_idx = Ctrl->HitTest(ptClient);
		
		if (target_idx == -1)
		{
			CRect rc;	Ctrl->GetClientRect(rc);
			target_idx = ( ptClient.y > rc.bottom / 2 ) ? -1 :  0;
		}
		
		i3::vector<PackPathUnitPtr>& unit_list = PathMgr::i()->GetPackPathFile()->unit_list;
		
		i3::vector<PackPathUnitPtr> mod_list;
		const size_t num_drag_idx = DragIndexList.size();

		for (size_t i = 0; i < num_drag_idx ; ++i )
		{
			int move_idx = DragIndexList[i] ;
			
			if (move_idx == target_idx)				// 이동인덱스와 목표인덱스가 같으면 목표인덱스를 증가시킴..
				++target_idx;	
			PackPathUnitPtr unit = unit_list[ move_idx ];
			mod_list.push_back(unit);
		}

		if ( target_idx >= int(unit_list.size()) )
			target_idx = -1;
		
		PackPathUnitPtr target_unit;
		if ( target_idx != -1 ) 
			target_unit = unit_list[target_idx];		//삽입목록과 겹칠수 있음 유의할것.

		
		for ( size_t i = 0 ; i < num_drag_idx ; ++i)
		{
			int del_idx = DragIndexList[num_drag_idx - 1 - i];
			Ctrl->DeleteItem( del_idx );						//뒤에서부터 지운다.
			unit_list.erase( unit_list.begin() + del_idx);		
		}
		
		// 새로 인서트....
		i3::vector<PackPathUnitPtr>::iterator it = (target_idx == -1) 
			? unit_list.end() : i3::range::find(unit_list,  target_unit);
					
		int insert_idx = i3::distance(unit_list.begin(), it);

		for (size_t i = 0; i < num_drag_idx; ++i , ++insert_idx )
		{
			Ctrl->InsertItem(insert_idx, mod_list[i]->PathName.c_str(), ::GetFolderItemIconIndex() );
			Ctrl->SetItemData(insert_idx,  (DWORD_PTR) mod_list[i].get());
		}
		
		unit_list.insert( it, mod_list.begin(), mod_list.end() );

		Ctrl->Sort(0, TRUE);

		PathMgr::i()->SavePathFile();
	}


	virtual void OnDragMove(i3MFCListCtrl* Ctrl, const i3::vector<int>& DragIndexList, CWnd* targetWnd, const CPoint& ptScreen) 
	{
		
		
	}

	virtual void OnClick(i3MFCListCtrl* Ctrl) {}
	virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) {}

	virtual void OnContextMenu(i3MFCListCtrl* Ctrl, const CPoint& pt) 
	{
		CPoint ptClient = pt;
		Ctrl->ScreenToClient(&ptClient);
		int target_idx = Ctrl->HitTest(ptClient);
		//			Ctrl->SelectItem(htarget);

		CMenu menu;
		menu.CreatePopupMenu();
		
		menu.AppendMenu(MF_STRING, COMMAND_ADD_PATH, L"New Path");

		if (target_idx != -1)
		{
			menu.AppendMenu(MF_STRING, COMMAND_EDIT_PATH, L"Edit Path");
			menu.AppendMenu(MF_STRING, COMMAND_DELETE_PATH, L"Delete Path");
			menu.AppendMenu(MF_STRING, COMMAND_CLONE_PATH, L"Clone Path");
		}
		
		bool bMediaPath = PathMgr::i()->IsMediaPathExist();
		bool bQAPath = PathMgr::i()->IsQAPathExist();
		bool bLivePath = PathMgr::i()->IsLivePathExist(); 

		bool bSeparationNeed = false;

		if (bMediaPath || bQAPath || bLivePath)
		{
			menu.AppendMenu(MF_SEPARATOR);
		}
		
		if (bMediaPath)
		{
			menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_MEDIA, L"TortoiseSVN Update : Media Path");
			bSeparationNeed = true;
		}
		if (bQAPath)
		{
			menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_QA, L"TortoiseSVN Update : QA Path");
			bSeparationNeed = true;
		}
		if (bLivePath)
		{
			menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_LIVE, L"TortoiseSVN Update : Live Path");
			bSeparationNeed = true;
		}

		if (bMediaPath && bQAPath && bLivePath)
		{
			menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_ALL, L"TortoiseSVN Update : ALL 3 Paths");
			bSeparationNeed = true;
		}

		long result_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, Ctrl);
		menu.DestroyMenu();

		switch( result_cmd )
		{
		case COMMAND_ADD_PATH:
			// 다이알로그 생성...
			{
				PathDlg dlg;
				if ( dlg.DoModal() == IDOK )
				{
					PackPathUnitPtr new_unit = i3::make_shared<PackPathUnit>( *dlg.GetResult() );
										
					i3::vector<PackPathUnitPtr>& unit_list = PathMgr::i()->GetPackPathFile()->unit_list;
					
					if (target_idx == -1)
					{
						target_idx = unit_list.size();
					}

					Ctrl->InsertItem(target_idx, new_unit->PathName.c_str(), ::GetFolderItemIconIndex());
					Ctrl->SetItemData(target_idx, DWORD_PTR(new_unit.get()) );
					unit_list.insert(unit_list.begin() + target_idx, new_unit);
					
					// 
					Ctrl->Sort(0, TRUE);		// LVS_ICON모드에서 신기하게도 이 함수가 잘 작동한다. (외어야 될듯)
					//

					PathMgr::i()->SavePathFile();
				}
			}
		break;
		case COMMAND_CLONE_PATH:
			{
				i3::vector<PackPathUnitPtr>& unit_list = PathMgr::i()->GetPackPathFile()->unit_list;
				
				PackPathUnit copy_unit = *unit_list[target_idx];
				copy_unit.PathName.clear();				// 패쓰이름을 지우고 그것으로 다이알로그를 연다..
				
				PathDlg dlg;			dlg.SetUnit(copy_unit);

				if (dlg.DoModal() == IDOK)
				{
					// 새 생성된 것은 맨 뒤에 일단 놓는다..
					PackPathUnitPtr new_unit = i3::make_shared<PackPathUnit>(*dlg.GetResult());
					int insert_index = Ctrl->GetItemCount();
					Ctrl->InsertItem(insert_index, new_unit->PathName.c_str(), ::GetFolderItemIconIndex());
					Ctrl->SetItemData(insert_index, DWORD_PTR(new_unit.get()));
					unit_list.insert(unit_list.begin() + insert_index, new_unit);
					//
					Ctrl->Sort(0, TRUE);
					//

					PathMgr::i()->SavePathFile();
				}
			}
		break;
		case COMMAND_EDIT_PATH:
			{
				i3::vector<PackPathUnitPtr>& unit_list = PathMgr::i()->GetPackPathFile()->unit_list;

				PathDlg dlg;

				dlg.SetUnit( *unit_list[target_idx] );

				if ( dlg.DoModal() == IDOK )
				{
					bool update_need = false;
					if (PathMgr::i()->IsCurrPathName(target_idx))
						update_need = true;

					*unit_list[target_idx] = *dlg.GetResult();		// 카운팅 아닌 복사처리해야함..
					Ctrl->SetItemText(target_idx, 0, unit_list[target_idx]->PathName.c_str());

					if (update_need && !PathMgr::i()->IsEqualToCurrPathUnit(target_idx) )
					{
						PathMgr::i()->SelectPathName( target_idx );
						m_owner->GetLeftView()->UpdatePackScriptFolder();
						AfxGetMainWnd()->SetWindowText( unit_list[target_idx]->PathName.c_str());
					}
					
					PathMgr::i()->SavePathFile();
				}
			}
		break;

		case COMMAND_DELETE_PATH:
			{
				if (target_idx == -1)
					return;
				
				i3::vector<int> sel_list;
				Ctrl->GetAllSelectedItems(sel_list);

				if (sel_list.empty())
					sel_list.push_back(target_idx);
				
				i3::vector<PackPathUnitPtr>& unit_list = PathMgr::i()->GetPackPathFile()->unit_list;

				for (size_t i = 0 ; i < sel_list.size() ; ++i )
				{
					int idx_del = sel_list[sel_list.size() - 1 - i];
					
					if (PathMgr::i()->IsCurrPathName(idx_del))
					{
						PathMgr::i()->SelectPathName( -1 );	// 지워질것이므로 -1..
						m_owner->GetLeftView()->UpdatePackScriptFolder();
					}

					Ctrl->DeleteItem(idx_del);
					unit_list.erase(unit_list.begin() + idx_del );		
				}
								
				// 
				Ctrl->Sort(0, TRUE);		// LVS_ICON모드에서 신기하게도 이 함수가 잘 작동한다. (외어야 될듯)
				//

				PathMgr::i()->SavePathFile();
			}
		break;

		case COMMAND_TSVN_UPDATE_MEDIA:
			{

				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetMainMediaPath()));


			}
		break;

		case COMMAND_TSVN_UPDATE_QA:
			{

				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetQAPath()));
			}
		break;

		case COMMAND_TSVN_UPDATE_LIVE:
			{

				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetLivePath()));
			}
		break;		
		
		case COMMAND_TSVN_UPDATE_ALL:
			{
				HANDLE ah[3];
				tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetMainMediaPath()), ah[0]);
				tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetQAPath()), ah[1]);
				tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetLivePath()), ah[2]);
				tsvn::WaitExecuteEnd(ah, 3);
			}
		break;
		default:
		break;
		}

	}

	virtual void OnEndLabelEdit(i3MFCListCtrl* Ctrl, int index, 
		int sub_index, const wchar_t* strText, bool& apply) 
	{
		if (strText == NULL || strText[0] == 0)
		{
			apply = false; return;
		}

		i3::vector<PackPathUnitPtr>& unit_list = PathMgr::i()->GetPackPathFile()->unit_list;

		if (PathMgr::i()->IsCurrPathName(index))
		{
			i3::generic_string_copy(unit_list[index]->PathName, strText);
			PathMgr::i()->SelectPathName( index );
			AfxGetMainWnd()->SetWindowText(strText);
		}
		else
		{
			i3::generic_string_copy(unit_list[index]->PathName, strText);
		}

		apply = true;

		PathMgr::i()->SavePathFile();
	}

	virtual ~PathViewListCallback() {}
};

}

IMPLEMENT_DYNCREATE(PathView, CView)

PathView::PathView() : m_pLeftView(NULL)
{
	m_listCtrl.SetCallback(new PathViewListCallback(this));
}

PathView::~PathView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(PathView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// PathView 그리기입니다.

void PathView::OnDraw(CDC* pDC)
{
	//CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// PathView 진단입니다.

#ifdef _DEBUG
void PathView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void PathView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// PathView 메시지 처리기입니다.

int PathView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_EDITLABELS);
	
	m_listCtrl.ModifyStyle(LVS_TYPEMASK, LVS_ICON); //|  LVS_ALIGNTOP | LVS_AUTOARRANGE);
//	m_listCtrl.ModifyStyleEx(0, LVS_EX_AUTOAUTOARRANGE | LVS_EX_MULTIWORKAREAS);

	m_listCtrl.InsertColumn(0, L"");
	
	i3::vector<PackPathUnitPtr>& unit_list = PathMgr::i()->GetPackPathFile()->unit_list;
	
	const size_t num_unit = unit_list.size();

	for (size_t i = 0 ; i < num_unit ; ++i)
	{
		PackPathUnit* unit = unit_list[i].get();
		m_listCtrl.InsertItem(i, unit->PathName.c_str(), ::GetFolderItemIconIndex());
		m_listCtrl.SetItemData(i, DWORD_PTR(unit));
	}

	return 0;
}

void PathView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	m_listCtrl.SetSize(cx, cy);
	m_listCtrl.Sort(0, TRUE);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

// 현재 PathView::OnInitialUpdate에 레지스트리로 현재 패쓰 이름을 저장하고 읽음..

void PathView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CWinApp* App = AfxGetApp();

	CString strRegPathName = App->GetProfileString(L"Path", L"PathName");

	if ( strRegPathName.IsEmpty() )	
	{
		App->Unregister();	
	}
	else
	{
		PackPathFile* pf = PathMgr::i()->GetPackPathFile();

		struct FindPred
		{
			i3::const_wchar_range rng;
			FindPred(const i3::const_wchar_range& rng) : rng(rng) {}
			bool operator()(const PackPathUnitPtr& ptr) const
			{
				return i3::generic_is_equal(ptr->PathName, rng);
			}
		};

		i3::vector<PackPathUnitPtr>::iterator it = i3::range::find_if(pf->unit_list, 
			FindPred( i3::const_wchar_range(LPCWSTR(strRegPathName), LPCWSTR(strRegPathName) + strRegPathName.GetLength() ) ));

		if ( it == pf->unit_list.end() )
		{
			App->Unregister();
		}
		else
		{

			size_t idx = i3::distance(pf->unit_list.begin(), it);

			m_listCtrl.SetItemState(idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			
			PathMgr::i()->SelectPathName( idx );
			m_pLeftView->UpdatePackScriptFolder();

			if (idx != -1)
				AfxGetMainWnd()->SetWindowText(PathMgr::i()->GetPathName().c_str());
			else
				AfxGetMainWnd()->SetWindowText(L"RscTool");		
		}

	}

}
