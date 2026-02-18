// PackScriptFileTaskPane.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptFileTaskPane.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/make_unix_path.h"

#include "i3Base/itl/vector_util.h"

#include "i3MFCButton.h"

#include "PackScriptFileFrameWnd.h"

#include "i3Base/string/algorithm/replace_all.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/is_matched_wildcard.h"
#include "i3Base/itl/range/algorithm/find.h"

#include "i3Base/string/system/traverse_file.h"

#include "PathMgr.h"

#include "i3Base/string/ext/contain_char_rev.h"

// PackScriptFileTaskPane




struct PackFileListBoxCallback : i3MFCVSListBoxCallback
{
	PackScriptFileTaskPane* m_owner;
	PackFileListBoxCallback(PackScriptFileTaskPane* owner) : m_owner(owner) {}
	virtual void OnSelection(i3MFCVSListBox* Ctrl, INT32 idx, void* ItemData) 
	{
		m_owner->UpdatePackScriptUnit(static_cast<PackScriptUnit*>(ItemData) );

		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
		contents_pane->SetPackScriptUnit( m_owner->GetCurrentPackScriptUnit() );
	}

	virtual void OnDeselection(i3MFCVSListBox* Ctrl, INT32 old_idx, void* old_ItemData)
	{
//		m_owner->RemoveAddRemoveEncGroup();
		m_owner->UpdatePackScriptUnit(  m_owner->GetCopyScriptUnit() );

		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
		contents_pane->SetPackScriptUnit( m_owner->GetCopyScriptUnit() );
	}
	
	virtual void OnAdd(i3MFCVSListBox* Ctrl, int iItem)
	{
		PackScriptContents& contents = m_owner->GetPackScriptContents();
		
		// 중간에 비는게 없어야한다.. (이 부분은 assert체크하는게 원래 처리임)
		const size_t prev_num_unit = contents.unit_list.size();
		if ( iItem > int(prev_num_unit) )
		{
			Ctrl->RemoveItem(iItem);
			return;
		}
		
		CString cstr = Ctrl->GetItemText(iItem);
		i3::const_wchar_range src_rng( LPCTSTR(cstr), LPCTSTR(cstr) + cstr.GetLength() );
	
		int pos = i3::contain_char_rev(src_rng, L'.');
		if (pos == -1)
		{
			cstr += L".i3Pack";
			Ctrl->SetItemText(iItem, cstr);
			src_rng = i3::const_wchar_range( LPCTSTR(cstr), LPCTSTR(cstr) + cstr.GetLength() );
		}
		
		
		PackScriptUnitPtr new_unit = i3::make_shared<PackScriptUnit>();
		i3::vu::set_value_force(contents.unit_list, iItem, new_unit);
	
		i3::string& rc_target = new_unit->pack_filename;
		i3::utf16_to_mb(src_rng, rc_target);
		
		Ctrl->SetItemData( iItem, DWORD_PTR( new_unit.get()  ) );
				
		m_owner->UpdatePackScriptUnit(static_cast<PackScriptUnit*>( new_unit.get() ) );

		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
		contents_pane->SetPackScriptUnit( m_owner->GetCurrentPackScriptUnit() );
	}

	// 팩 파일 리네이밍의 경우 모델데이터 변경외엔 딱히 처리할게 없어보인다.
	virtual void OnRename(i3MFCVSListBox* Ctrl, int iItem)
	{
		PackScriptContents& contents = m_owner->GetPackScriptContents();

		// 중간에 비는게 없어야한다.. (이 부분은 assert체크하는게 원래 처리임)
		const size_t prev_num_unit = contents.unit_list.size();
		if ( iItem > int(prev_num_unit) )
		{
			Ctrl->RemoveItem(iItem);
			return;
		}

		CString cstr = Ctrl->GetItemText(iItem);
		i3::const_wchar_range src_rng( LPCTSTR(cstr), LPCTSTR(cstr) + cstr.GetLength() );

		int pos = i3::contain_char_rev(src_rng, L'.');
		if (pos == -1)
		{
			cstr += L".i3Pack";
			Ctrl->SetItemText(iItem, cstr);
			src_rng = i3::const_wchar_range( LPCTSTR(cstr), LPCTSTR(cstr) + cstr.GetLength() );
		}
		
		i3::string& rc_target = contents.unit_list[iItem]->pack_filename;
		i3::utf16_to_mb(src_rng, rc_target);

	}
		
	bool BeforeRemove(i3MFCVSListBox* Ctrl, int iItem) 
	{
		// 뭐 대체로 무조건 지우면 될것 같다....
		PackScriptContents& contents = m_owner->GetPackScriptContents();
		i3::vu::erase_index(contents.unit_list, iItem);
		//
		// ??? 그다음 처리가 필요한지 아닌지는 테스트 해보고 결정..? ( 별 필요없는듯??)
		return true;		
	}

	void OnMoveItemUp(i3MFCVSListBox* Ctrl, int iItem) 
	{
		// 뭐.... 벡터간 이동 처리로 끝날것 같다.. (스웝처리로도 될것 같고)
		// VSListCtrl의 ItemData의 경우 포인터로 연결된게 유실되지 않았을테니..
		// 별도 처리는 없어도 될것으로 보인다..

		PackScriptContents& contents = m_owner->GetPackScriptContents();
		i3::swap( contents.unit_list[iItem+1], contents.unit_list[iItem]);
	}

	void OnMoveItemDown(i3MFCVSListBox* Ctrl, int iItem) 
	{
		// 뭐.... 벡터간 이동처리로 끝날것 같다..(스웝처리로도 될것 같고)
		// VSListCtrl의 ItemData의 경우 포인터로 연결된게 유실되지 않았을테니..
		// 별도 처리는 없어도 될것으로 보인다..

		PackScriptContents& contents = m_owner->GetPackScriptContents();
		i3::swap( contents.unit_list[iItem-1], contents.unit_list[iItem]);
	}

};

//
// 간편하게 버튼 추가됨..
// 하지만 이 버튼의 행번호를 추적하는건 조금 까다로울것이다..
//

struct PackCmdAddOptButtonCallback : i3MFCButtonCallback
{
	PackCmdAddOptButtonCallback(PackScriptFileTaskPane* owner, int RowIdx) : 
		m_owner(owner), m_RowIdx(RowIdx) 
	{
		PackScriptUnit* unit = m_owner->GetCurrentPackScriptUnit();
		m_isSub = !unit->add_opt_list[RowIdx];		// 생성시 갱신될수 있게 반전시킨다..
	}
	
	PackScriptFileTaskPane* m_owner;
	int						m_RowIdx;		// 편집때 숫자가 바뀌므로 부지런이 바뀌어야한다..(제약)
	bool					m_isSub;		// 보존 목적...

	void	UpdateText(i3MFCButton* ctrl)
	{
		PackScriptUnit* unit = m_owner->GetCurrentPackScriptUnit();

		if (unit == NULL)		// 팩스크립트파일 리스트박스 셀렉션이 해제되는 일이 발생가능하여
			return;					// 널값이 될수 있다...일단 스킵처리로 오류제거..

		// 결과 반전시키고, 이걸 토대로 버튼 글자 변경..
		bool isCurSub = unit->add_opt_list[m_RowIdx];

		if (m_isSub != isCurSub )
		{
			if (isCurSub)
			{
				ctrl->SetTextColor(RGB(0,0,255));
				ctrl->SetWindowText(L"SUB");
			}
			else
			{
				ctrl->SetTextColor(RGB(255,0,0));
				ctrl->SetWindowText(L"NO SUB");
			}
			m_isSub = isCurSub;
		}
	}

	virtual void OnClick(i3MFCButton* ctrl)		// 
	{
		//
		PackScriptUnit* unit = m_owner->GetCurrentPackScriptUnit();
		// 결과 반전시키고, 이걸 토대로 버튼 글자 변경..
		unit->add_opt_list[m_RowIdx] = !unit->add_opt_list[m_RowIdx];
		UpdateText(ctrl);

		// 클릭포커스를 버튼에 두지 말고, 리스트콘트롤로 옮길수 있으면 그렇게 한다.
		i3MFCListCtrl_Custom* parent_ctrl = (i3MFCListCtrl_Custom*)ctrl->GetParent();
		parent_ctrl->UnselectAll();
		parent_ctrl->SetItemState(m_RowIdx, LVIS_SELECTED, LVIS_SELECTED);


		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
		contents_pane->SetPackScriptUnit( unit );
//		ctrl->Invalidate(FALSE);

	}

	virtual void OnLButtonDown(i3MFCButton* ctrl) 
	{

	}

	virtual void OnLButtonUp(i3MFCButton* ctrl) 
	{
		i3MFCListCtrl_Custom* parent_ctrl = (i3MFCListCtrl_Custom*)ctrl->GetParent();
		parent_ctrl->SetFocus();
	}

};

struct PackCmdAdd_CustomDrawCallback : i3MFCListCtrl_CustomDrawCallback
{
	PackCmdAdd_CustomDrawCallback(PackScriptFileTaskPane* owner) : m_owner(owner) {}

	virtual ~PackCmdAdd_CustomDrawCallback()
	{
		const size_t num_button = m_button_opt_list.size();
		for (size_t i = 0; i < num_button; ++i)
		{
			if (m_button_opt_list[i])
			{
				delete m_button_opt_list[i]->GetCallback();
			}
		}

		i3::cu::for_each_delete_clear(m_button_opt_list);		
	}


	// OnPaint는 화면밖으로 나간 콘트롤을 가리는 역할로 일단 쓴다..
	// OnPaint를 다시 재구성할것...
	// 0~TopIdx ~ LastIdx ~ numItem 까지 순회해서 콜백잡을 것.
	// 커스텀 전용 헬퍼함수 제공 해야함...( Show/Hide/MoveWindow들에 대한 처리 헬프)
	
	virtual void OnPaint(i3MFCListCtrl_Custom* pCtrl, int first, int last) 
	{
		const int num_vis_item = last - first;

		if ( num_vis_item > (int)m_button_opt_list.size() )
			m_button_opt_list.insert(m_button_opt_list.end(),  
			num_vis_item - (int)m_button_opt_list.size(), NULL);

		const size_t num_list = m_button_opt_list.size();
		
		int count_button = 0;

		for (int i = first ; i < last ; ++i, ++count_button)
		{
			i3MFCButton* button = m_button_opt_list[count_button];
			
			if (button)
			{
				CRect rc;
				button->GetWindowRect(&rc);
				pCtrl->ScreenToClient(&rc);		// button rect..
				CRect rc_new;
				pCtrl->GetSubItemRect(i, 1, LVIR_BOUNDS, rc_new);

				if ( rc != rc_new )					// 가시영역안에서의 위치 이동인 경우..
				{									// 이전 이미지 삭제가 안되는 경우가 있어 지우고 렌더링 해야함..
					if ( button->IsWindowVisible() )
						button->ShowWindow(SW_HIDE);

					button->MoveWindow(rc_new, FALSE); 
					button->ShowWindow(SW_SHOW);
				}
				else
				{
					button->ShowWindow(SW_SHOW);
				}

				PackCmdAddOptButtonCallback* cb = (PackCmdAddOptButtonCallback*)button->GetCallback();
				
				cb->m_RowIdx = i;
			}
			else
			{
				// 만든다..
				i3MFCButton* new_button = new i3MFCButton();

				CRect rc_new;
				pCtrl->GetSubItemRect(i, 1, LVIR_BOUNDS, rc_new);

				new_button->SetCallback(new PackCmdAddOptButtonCallback(m_owner, i) );
				new_button->Create(L"", WS_VISIBLE|WS_CHILD, rc_new, pCtrl, i);

				m_button_opt_list[count_button] = new_button;
				button = new_button;
			}

			PackCmdAddOptButtonCallback* cb = (PackCmdAddOptButtonCallback*)button->GetCallback();
			cb->UpdateText(button);
		}

		for (size_t i = count_button ; i < num_list ; ++i )
		{
			i3MFCButton* button = m_button_opt_list[i];
			if (button && button->IsWindowVisible() )
				button->ShowWindow(SW_HIDE);
		}
	}
	
	PackScriptFileTaskPane*			m_owner;
	i3::vector<i3MFCButton*>		m_button_opt_list;		// 그냥 대충 만듬..상관없음..
};														// Parent의 노티파이 귀찮으니..
														// i3MFCButton을 만들자..!

namespace 
{
	enum
	{
		COMMAND_DELETE_PACKSCRIPT_ITEM = 20000,

	};
}

struct PackCmdAddCallback : i3MFCListCtrlCallback
{

	PackCmdAddCallback(PackScriptFileTaskPane* owner, i3::vector<i3::string> (PackScriptUnit::*mem_ptr) ) : 
		m_owner(owner), m_mem_ptr(mem_ptr) {}


	// 탐색기창에도 해당 아이템 셀렉션이 될수 있게 조정한다.

	virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
	{
		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptFileExplorerListView* explorer = framewnd->GetExplorerListView();
		i3ShellListCtrl* target_ctrl = explorer->GetListCtrl();
		target_ctrl->UnselectAll();

		PackScriptUnit* unit =	m_owner->GetCurrentPackScriptUnit();		
		const i3::string& relPath = (unit->*m_mem_ptr)[index];			// add_list or rem_list or enc_list..
			// unit->add_list[index];
		
		i3::wstring wrelPath;
		i3::mb_to_utf16(relPath, wrelPath);

		const i3::rc_wstring&	abs_path = PathMgr::i()->GetMainMediaPath();
		
		i3::wstring wresultPath = abs_path; 
		wresultPath += L'/';
		wresultPath += wrelPath;
		i3::range::replace(wresultPath, L'\\', L'/');

		i3::wstring wstrDir, wstrFile;
		
		// TODO : 일단 확장자 구분 점이 없으면 폴더로 간주..( GetFileAttributesW로 교체해야될것 같다..나중에 다시 점검하게되면 그때 고친다..)
		if ( wresultPath.rfind( L'.' ) != i3::wstring::npos )  
		{
			i3::extract_directoryname_filename(wresultPath, wstrDir, wstrFile);
		}
		else
		{
			wstrDir = wresultPath;
		}
		
		i3::replace(wstrDir.begin(), wstrDir.end(), L'/', L'\\');
		HRESULT hr = target_ctrl->DisplayFolder(wstrDir.c_str());

		if (hr != S_OK)
		{
			return;			//
		}
		
		const PackScriptExceptionFilterList& filter_list = PathMgr::i()->GetExceptionList();
		const i3::vector<i3::string>& nation_folder_filter_list = PathMgr::i()->GetNationFolderFilterList();

		const size_t num_filter = filter_list.pack_filter_list.size();

		if ( wstrFile.empty() )		// 파일이 없으면 폴더전체를 셀렉션..
		{
			bool bSubDir = false;
			
			if ( &PackScriptUnit::add_list == m_mem_ptr)
			{
				bSubDir = unit->add_opt_list[index];
			}

			const int itemCount = target_ctrl->GetItemCount();
			
			i3::stack_string str_for_test;
			CString			 mfcstr_for_test;

			for (int i = 0 ; i < itemCount ; ++i )
			{
				bool is_folder = target_ctrl->IsItemFolder(i) != FALSE;

				mfcstr_for_test = target_ctrl->GetItemText(i,0);

				i3::utf16_to_mb( i3::const_wchar_range(LPCWSTR(mfcstr_for_test), LPCWSTR(mfcstr_for_test) + mfcstr_for_test.GetLength())
					, str_for_test);

				if ( is_folder )
				{
					if (!bSubDir )
						continue;
					
					const size_t num_nation_folder = nation_folder_filter_list.size();
					bool bFilterFound = false;
					for ( size_t j = 0 ; j < num_nation_folder ; ++j )
					{
						if ( i3::generic_is_iequal(nation_folder_filter_list[j], str_for_test) )
						{
							bFilterFound = true; break;
						}
					}

					if (bFilterFound)
						continue;
				}
								
				bool bFilterFound = false;

				if ( is_folder == false)
				{
					for (size_t j = 0 ; j < num_filter ; ++j)
					{
						if (i3::is_matched_wildcard( str_for_test, filter_list.pack_filter_list[j]) )
							{ bFilterFound = true; break; }					
					}
				}
				
				if (!bFilterFound)
					target_ctrl->SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
			}
			
			return;
		}

		//
		// 와일드 카드 여부 확인... 발견되면, 폴더 전체 뒤져야됨... 
		// 서브폴더 탐색의 경우는 일단 생략한다... 
		// (처리하려고한다면..탐색창이 더 필요하게 됨...)
		//
		
		i3::wstring::iterator p = i3::range::find_first_of( wstrFile, L"*?");

		if ( p != wstrFile.end() )		// 와일드카드 존재하면 현재 폴더의 전체를 뒤짐..
		{
			struct internal_callback : i3::traverse_file_callback_wc
			{
				const i3::wstring& wildcard;
				const i3::vector<i3::string>& filter_list;
				i3::vector<i3::wstring>	vecResult;

				internal_callback(const i3::wstring& wildcard, const i3::vector<i3::string>& filter_list) 
					: wildcard(wildcard), filter_list(filter_list) {}
				virtual bool on_dir(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) { return false; }
				virtual void on_file(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
				{
					if ( i3::is_matched_wildcard(fd.cFileName, wildcard) )
					{
						const size_t num_filter = filter_list.size();
						i3::stack_string str_for_test;
						i3::utf16_to_mb(fd.cFileName, str_for_test);

						for (size_t j = 0 ; j < num_filter ; ++j)
						{
							if (i3::is_matched_wildcard( str_for_test, filter_list[j]) )
								return;					
						}

						vecResult.push_back(fd.cFileName);

					}
				}
			};

			internal_callback cb(wstrFile, filter_list.pack_filter_list);
			traverse_file( wstrDir, &cb, false );
			
			const size_t num_res = cb.vecResult.size();

			LVFINDINFO info;
			info.flags = LVFI_STRING;

			for (size_t i = 0 ; i < num_res ; ++i )
			{
				info.psz = cb.vecResult[i].c_str();
				
				int itemIdx = target_ctrl->FindItem(&info);
		
				if (itemIdx != -1)
				{
					target_ctrl->SetItemState(itemIdx, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED| LVIS_FOCUSED);

					if ( i == num_res - 1)
						target_ctrl->EnsureVisible(itemIdx, FALSE);
				}
			}
		}
		else		// 단순 파일이름이라면 그냥 검색 셀렉션 하면 된다..
		{
			LVFINDINFO info;
			info.flags = LVFI_STRING;
			info.psz = wstrFile.c_str();
			int itemIdx = target_ctrl->FindItem( &info );

			if (itemIdx != -1)
			{

				const size_t num_filter = filter_list.pack_filter_list.size();
				i3::stack_string str_for_test;
				i3::utf16_to_mb(wstrFile, str_for_test);
				bool bFilterFound = false;

				for (size_t j = 0 ; j < num_filter ; ++j)
				{
					if (i3::is_matched_wildcard( str_for_test, filter_list.pack_filter_list[j]) )
					{
						bFilterFound = true;
						break;
					}
				}
				
				if (bFilterFound == false)
					target_ctrl->SetItemState(itemIdx, LVIS_SELECTED| LVIS_FOCUSED, LVIS_SELECTED| LVIS_FOCUSED);

				target_ctrl->EnsureVisible(itemIdx, FALSE);
			}

		}
		
	}

	virtual void OnEndLabelEdit(i3MFCListCtrl* Ctrl, int index, 
		int sub_index, const wchar_t* strText, bool& apply) 
	{
		if ( !strText )
			return;

		PackScriptUnit* unit =	m_owner->GetCurrentPackScriptUnit();	
		
		i3::vector<i3::string>& str_list = unit->*m_mem_ptr;
		
		i3::utf16_to_mb( strText, str_list[index]);

	//	Ctrl->SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
		// 그다지 좋지는 않지만..OnSelected의 구현 함수를 호출해준다..
		PackCmdAddCallback::OnSelected(Ctrl, index); 

		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
		contents_pane->SetPackScriptUnit(unit);
	}

	virtual void OnContextMenu(i3MFCListCtrl* pCtrl, const CPoint& pt) 
	{
		CPoint ptLocal(pt);
		pCtrl->ScreenToClient(&ptLocal);
		int target_idx = pCtrl->HitTest(ptLocal);
		pCtrl->SetFocus();

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, COMMAND_DELETE_PACKSCRIPT_ITEM, L"Delete");
		long result_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, pCtrl);
		menu.DestroyMenu();

		switch(result_cmd)
		{
		case COMMAND_DELETE_PACKSCRIPT_ITEM:
			m_owner->DeleteItemFromListCtrl(pCtrl, target_idx);		
			break;
		default:
			break;
		}
					
	}

	PackScriptFileTaskPane*						m_owner;
	i3::vector<i3::string>	(PackScriptUnit::*m_mem_ptr);
};



IMPLEMENT_DYNAMIC(PackScriptFileTaskPane, CMFCTasksPane)

PackScriptFileTaskPane::PackScriptFileTaskPane() : m_grp_idx_add(-1), 
	m_grp_idx_rem(-1), m_grp_idx_enc(-1)
{
	m_vslistctrl_pack.SetCallback( new PackFileListBoxCallback(this) );
	m_listctrl_add.SetCallback_CustomDraw( new PackCmdAdd_CustomDrawCallback(this) );

	// 생성초기화에 멤버포인터를 사용했음..
	m_listctrl_add.SetCallback(new PackCmdAddCallback(this, &PackScriptUnit::add_list));
	m_listctrl_rem.SetCallback(new PackCmdAddCallback(this, &PackScriptUnit::remove_list));
	m_listctrl_enc.SetCallback(new PackCmdAddCallback(this, &PackScriptUnit::enc_list));

}

PackScriptFileTaskPane::~PackScriptFileTaskPane()
{
	delete m_listctrl_enc.GetCallback();
	delete m_listctrl_rem.GetCallback();
	delete m_listctrl_add.GetCallback();
	delete m_listctrl_add.GetCallback_CustomDraw();
	delete m_vslistctrl_pack.GetCallback();	
}

BEGIN_MESSAGE_MAP(PackScriptFileTaskPane, CMFCTasksPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// PackScriptFileTaskPane message handlers


int PackScriptFileTaskPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCTasksPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	// EnableNavigationToolbar(FALSE, 0, 0, 0);


	EnableOffsetCustomControls (FALSE);
	
	int grp_idx_pack = this->AddGroup(L"PACK", FALSE, TRUE);

	m_listctrl_add.CreateEx(this, -1, LVS_EDITLABELS);
	m_listctrl_rem.CreateEx(this, -1, LVS_EDITLABELS);
	m_listctrl_enc.CreateEx(this, -1, LVS_EDITLABELS);

//	m_listctrl_add.SetOutlineColor(RGB(255,255,255));
//	m_listctrl_add.Set

//	m_listctrl_add.ModifyStyleEx(0, LVS_EX_CHECKBOXES, SWP_FRAMECHANGED);

	m_listctrl_add.InsertColumn(0, L"");
	m_listctrl_add.InsertColumn(1, L"", LVCFMT_CENTER, 20);

	m_listctrl_rem.InsertColumn(0, L"");
	m_listctrl_enc.InsertColumn(0, L"");
	
	m_listctrl_add.HideHeader();
	m_listctrl_rem.HideHeader();
	m_listctrl_enc.HideHeader();
	
	m_vslistctrl_pack.Create(L"", WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CRect(0,0,0,0), this);

	m_vslistctrl_pack.m_pWndList->ModifyStyleEx(WS_EX_CLIENTEDGE,  LVS_EX_DOUBLEBUFFER, SWP_FRAMECHANGED);

//	m_vslistctrl_pack.m_pWndList->ModifyStyle(0, LVS_SHOWSELALWAYS, SWP_FRAMECHANGED);

//	m_vslistctrl_pack.m_pWndList->ModifyStyle(0, LVS_SHAREIMAGELISTS);
//	m_vslistctrl_pack.m_pWndList->SetImageList(CImageList::FromHandle(GetShellImageList_Large()), LVSIL_NORMAL);
//	m_vslistctrl_pack.m_pWndList->SetImageList(CImageList::FromHandle(GetShellImageList_Small()), LVSIL_SMALL);


//	m_vslistctrl_pack.ModifyStyle(WS_VISIBLE, 0, SWP_FRAMECHANGED);



//	m_vslistctrl_pack.ModifyStyle(
	::SetWindowTheme(m_vslistctrl_pack.GetSafeHwnd(), L"explorer", 0);
	::SetWindowTheme(m_vslistctrl_pack.m_pWndList->GetSafeHwnd(), L"explorer", 0);
	
	m_vslistctrl_pack.SetStandardButtons();
	m_vslistctrl_pack.EnableBrowseButton();
	
//	m_vslistctrl_pack.ModifyStyle(WS_BORDER|WS_DLGFRAME|WS_THICKFRAME,0);

	AddWindow(grp_idx_pack, m_vslistctrl_pack.GetSafeHwnd(), 120);

	this->CollapseGroup(grp_idx_pack, FALSE);

//	this->AddGroup(L"COPY ONLY", FALSE, TRUE);

	return 0;
}

//
//
//  i3BaseDX.lib에서 wchar_t를 빌트인타입으로 하질 않았기 때문에 링크오류가 발생했었다.
//  빌드를 통과시키려면 wchar_t 빌트인 타입을 꺼야한다...-_-
// 


void	PackScriptFileTaskPane::LoadPackScriptFile(const i3::wstring& strFullPath)
{

	ImportPackScriptFile(strFullPath, m_contents);			// 간단하게 함수 한개로 일단 데이터 확보..
	
	CopyPackScriptContents(m_contents, m_contents_orig);	// 백업 (비교용)

	// 확보된걸 토대로 리스트업해야한다..

	i3::wstring wstr;		// 안타깝게도, 변환이 필요하다..
	CString mfcstr;
	const std::size_t num_unit = m_contents.unit_list.size();
	{
		const i3::vector<PackScriptUnitPtr>& v = m_contents.unit_list;

		for (size_t i = 0 ; i < num_unit; ++i )
		{
			i3::mb_to_utf16(v[i]->pack_filename, wstr);
			mfcstr.SetString(wstr.c_str(), wstr.length());

			m_vslistctrl_pack.AddItem( mfcstr, DWORD_PTR( v[i].get() ) );
		}
	}
	
	if (m_contents.copy_unit == NULL && num_unit > 0)
	{
		m_vslistctrl_pack.SelectItem(0);
		m_vslistctrl_pack.SetFocus();
	}
	else
	{
		m_vslistctrl_pack.SetFocus();
		m_vslistctrl_pack.ResetSelect();
		
		PackScriptFileTaskPane::UpdatePackScriptUnit(m_contents.copy_unit.get());

		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)GetParent();
		PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
		contents_pane->SetPackScriptUnit( GetCopyScriptUnit() );
		// copy스타일로..
	}

	// 강제로 셀렉션을 찍어내어...셀렉션 통지이후에 ADD/REMOVE/ENC를 출력하게끔 한다..
}

void	PackScriptFileTaskPane::RemoveAddRemoveEncGroup()
{
	if (m_grp_idx_add > -1 )
	{
		m_listctrl_add.DeleteAllItems();
		m_listctrl_rem.DeleteAllItems();
		m_listctrl_enc.DeleteAllItems();

		this->SetWindowHeight(m_grp_idx_enc, m_listctrl_enc.GetSafeHwnd(), 0);
		this->SetWindowHeight(m_grp_idx_rem, m_listctrl_rem.GetSafeHwnd(), 0);
		this->SetWindowHeight(m_grp_idx_add, m_listctrl_add.GetSafeHwnd(), 0);

		this->RemoveGroup(m_grp_idx_enc);
		this->RemoveGroup(m_grp_idx_rem);
		this->RemoveGroup(m_grp_idx_add);
		m_grp_idx_add = -1;
		m_grp_idx_rem = -1;
		m_grp_idx_enc = -1;

//		업데이트가 안되는데 강제 드로잉 해야함..
	}

}

void	PackScriptFileTaskPane::UpdatePackScriptUnit(PackScriptUnit* ItemData)
{

	if (m_grp_idx_add == -1)
	{
		m_grp_idx_add = this->AddGroup(L"");
		m_grp_idx_rem = this->AddGroup(L"");

		AddWindow(m_grp_idx_add, m_listctrl_add.GetSafeHwnd(), 0 );
		AddWindow(m_grp_idx_rem, m_listctrl_rem.GetSafeHwnd(), 0 );
	}
	
	bool copy_unit_mode = false;
	if ( ItemData == NULL || ItemData == m_contents.copy_unit.get() )
		copy_unit_mode = true;

	i3::wstring wstr;		// 안타깝게도, 변환이 필요하다..
	CRect rc;

	{
		const wchar_t* str_add = (copy_unit_mode) ? L"ADD (COPY ONLY)" : L"ADD"; 
		this->SetGroupName(m_grp_idx_add, str_add);

		m_listctrl_add.DeleteAllItems();
		size_t num_data = 0;
		if ( ItemData != NULL)
		{
			const i3::vector<i3::string>& v = ItemData->add_list;
			const i3::vector<bool>& check_list = ItemData->add_opt_list;

			const std::size_t num = v.size();

			for (size_t i = 0 ; i < num ; ++i )
			{
				i3::mb_to_utf16(v[i], wstr);
				m_listctrl_add.AddItem(wstr.c_str(), (void*)1);
			}
			num_data = num;
		}
	
		this->SetWindowHeight( m_grp_idx_add, m_listctrl_add.GetSafeHwnd(), 180); //120);


		m_listctrl_add.GetClientRect(&rc);
		m_listctrl_add.SetColumnWidth(0, rc.Width() - 80 );
		m_listctrl_add.SetColumnWidth(1, 80);
	//	m_listctrl_add.RedrawWindow();

		this->CollapseGroup(m_grp_idx_add, num_data == 0 );
	}
	
	{
		const wchar_t* str_remove = (copy_unit_mode) ? L"REMOVE (COPY ONLY)" : L"REMOVE"; 
		
		this->SetGroupName(m_grp_idx_rem, str_remove);

		m_listctrl_rem.DeleteAllItems();
		size_t num_data = 0;

		if ( ItemData )
		{
			const i3::vector<i3::string>& v = ItemData->remove_list;
			const std::size_t num = v.size();
			for (size_t i = 0 ; i < num ; ++i )
			{
				i3::mb_to_utf16(v[i], wstr);
				m_listctrl_rem.AddItem(wstr.c_str(), (void*)2);
			}

			num_data = num;
		}
	
		this->SetWindowHeight( m_grp_idx_rem, m_listctrl_rem.GetSafeHwnd(), 120);
	
		m_listctrl_rem.GetClientRect(&rc);
		m_listctrl_rem.SetColumnWidth(0, rc.Width());

		this->CollapseGroup(m_grp_idx_rem, num_data == 0 );

	//	m_listctrl_rem.RedrawWindow();
	}

	{
		m_listctrl_enc.DeleteAllItems();

		if ( copy_unit_mode )
		{
			if (m_grp_idx_enc > -1)
			{
				this->SetWindowHeight(m_grp_idx_enc, m_listctrl_enc.GetSafeHwnd(), 0);
				this->RemoveGroup(m_grp_idx_enc);
				m_grp_idx_enc = -1;
			}
		}
		else
		{
			if ( m_grp_idx_enc == -1)
			{
				m_grp_idx_enc = this->AddGroup(L"ENCRYPT");
				AddWindow(m_grp_idx_enc, m_listctrl_enc.GetSafeHwnd(), 0 );
			}

			const i3::vector<i3::string>& v = ItemData->enc_list;
			const std::size_t num = v.size();
			for (size_t i = 0 ; i < num ; ++i )
			{
				i3::mb_to_utf16(v[i], wstr);
				m_listctrl_enc.AddItem(wstr.c_str(), (void*)3);
			}

			this->SetWindowHeight( m_grp_idx_enc, m_listctrl_enc.GetSafeHwnd(), 120);

			m_listctrl_enc.GetClientRect(&rc);
			m_listctrl_enc.SetColumnWidth(0, rc.Width() );

			this->CollapseGroup(m_grp_idx_enc, num == 0 );
			//	m_listctrl_enc.RedrawWindow();
		}
	
	}
	

}


void PackScriptFileTaskPane::OnSize(UINT nType, int cx, int cy)
{
	CMFCTasksPane::OnSize(nType, cx, cy);
	
	CRect rc;
	m_listctrl_add.GetClientRect(&rc);
	m_listctrl_add.SetColumnWidth(0, rc.Width() );

	m_listctrl_rem.GetClientRect(&rc);
	m_listctrl_rem.SetColumnWidth(0, rc.Width());

	m_listctrl_enc.GetClientRect(&rc);
	m_listctrl_enc.SetColumnWidth(0, rc.Width() );
	


//	m_listctrl_add.SetSize(cx, cy);
//	m_listctrl_rem.SetSize(cx, cy);
//	m_listctrl_enc.SetSize(cx, cy);

	// TODO: Add your message handler code here
}


void	PackScriptFileTaskPane::OnDragEndFromExplorer(CWnd* targetWnd, const CPoint& ptScreen, 
								  const i3::vector<i3::wstring>& fullPathList)
{
	PackScriptUnit* unit = PackScriptFileTaskPane::GetCurrentPackScriptUnit();

//	if (unit == NULL)
//		return;

	i3::vector<i3::string>* data_plist;

	if		( targetWnd == &m_listctrl_add)
	{
		if (unit == NULL)
		{
			m_contents.copy_unit = i3::make_shared<PackScriptUnit>();
			unit = m_contents.copy_unit.get();
		}
		data_plist = &unit->add_list;
	}
	else if ( targetWnd == &m_listctrl_rem)	
	{
		if (unit == NULL)
		{
			m_contents.copy_unit = i3::make_shared<PackScriptUnit>();
			unit = m_contents.copy_unit.get();
		}

		data_plist = &unit->remove_list;
	}
	else if ( targetWnd == &m_listctrl_enc)
	{
		if (unit == NULL) return;
		data_plist = &unit->enc_list;
	}
	else return;

	i3MFCListCtrl* targetCtrl = static_cast<i3MFCListCtrl*>(targetWnd);
	i3::vector<i3::string>& data_list = *data_plist;

	
	int idx = targetCtrl->GetDropTarget();
	targetCtrl->SetDropTarget(-1);	// 리셋..

	if ( idx == -1)		// 몽땅 뒤에 붙임..
		idx = int(data_list.size());
	
		
	i3::vector<i3::wstring>	wresPathList;
	i3::vector<i3::string>	resPathList;

	const size_t num_fullPathList = fullPathList.size();

	const i3::rc_wstring&	abs_path = PathMgr::i()->GetMainMediaPath();

	for (size_t i = 0 ; i < num_fullPathList; ++i )
	{
		wresPathList.push_back(i3::wstring());
		i3::make_relative_path(abs_path, fullPathList[i], wresPathList.back());
		i3::make_unix_path(wresPathList.back() );
		resPathList.push_back(i3::string());
		i3::utf16_to_mb(wresPathList.back() , resPathList.back());		
	}

	data_list.insert(data_list.begin() + idx, resPathList.begin(), resPathList.end() );	

	if (targetWnd == &m_listctrl_add)
	{
		// 폴더인 경우, 파일인 경우 나눈다.. 
		// 폴더인 경우엔 디폴트로 SUB를 주고..
		// 파일인 경우 디폴트로 NOSUB를 준다..
		int insert_pos = idx;

		for (size_t i = 0 ; i < num_fullPathList; ++i )
		{
			const i3::string& str =	resPathList[i];
			size_t pos = str.find_last_of("/\\.");
			bool   bval = true;
			
			if ( pos != i3::string::npos && str[pos] == '.')  // 파일..
				bval = false;
			
			unit->add_opt_list.insert(unit->add_opt_list.begin() + insert_pos, bval);
			++insert_pos;
		}
	}
	

	targetCtrl->SetRedraw(FALSE);
	
	int insert_pos = idx;

	for (size_t i = 0 ; i < num_fullPathList ; ++i)
	{
		targetCtrl->InsertItem(insert_pos, wresPathList[i].c_str(), 0);
		++insert_pos;
	}

	targetCtrl->SetRedraw(TRUE);

	targetCtrl->RedrawWindow();
	
	// 드래그앤드롭 이후의 컨텐츠파일목록 갱신 요구됨...
	
	PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)GetParent();
	PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
	contents_pane->SetPackScriptUnit(unit);
	//
}

void	PackScriptFileTaskPane::OnDragMoveFromExplorer(CWnd* targetWnd, const CPoint& ptScreen)
{
	if (targetWnd != &m_listctrl_add && targetWnd != &m_listctrl_rem &&
		targetWnd != &m_listctrl_enc )
		return;

	i3MFCListCtrl* targetCtrl = static_cast<i3MFCListCtrl*>(targetWnd);

	CPoint ptLocal(ptScreen);
	targetCtrl->ScreenToClient(&ptLocal);

	int drop_idx = targetCtrl->HitTest(ptLocal);
		
	targetCtrl->SetFocus();
	targetCtrl->SetDropTarget(drop_idx);
	
}


PackScriptUnit*		PackScriptFileTaskPane::GetCurrentPackScriptUnit() const
{
	int packfile_sel = m_vslistctrl_pack.GetSelItem();
	if (packfile_sel == -1) 
	{
		return PackScriptFileTaskPane::GetCopyScriptUnit();
	//	return NULL;		
	}

	PackScriptUnit* unit = reinterpret_cast<PackScriptUnit*>(m_vslistctrl_pack.GetItemData(packfile_sel));
	return unit;
}

PackScriptUnit*		PackScriptFileTaskPane::GetCopyScriptUnit() const
{
	return m_contents.copy_unit.get();
}


//

void PackScriptFileTaskPane::DeleteItemFromListCtrl(i3MFCListCtrl* pCtrl, int target_idx)
{
	//	m_listctrl_add.GetItemData(target_idx);
	PackScriptUnit* pCurrUnit = PackScriptFileTaskPane::GetCurrentPackScriptUnit();
	if (!pCurrUnit)
		return;
	
	i3::vector<i3::string>*		data_plist;
	if		(pCtrl == &m_listctrl_add) data_plist = &pCurrUnit->add_list;
	else if (pCtrl == &m_listctrl_rem) data_plist = &pCurrUnit->remove_list;
	else if (pCtrl == &m_listctrl_enc) data_plist = &pCurrUnit->enc_list;
	else return;
	
	i3::vector<i3::string>& data_list = *data_plist;

	i3::vector<int>		vec_idx;
	pCtrl->GetAllSelectedItems(vec_idx);

	if (vec_idx.size() < 2 )
	{
		if (target_idx == -1)
			return;

		i3::vu::erase_index(data_list, target_idx);

		if (pCtrl == &m_listctrl_add)
			i3::vu::erase_index(pCurrUnit->add_opt_list, target_idx);

		pCtrl->DeleteItem(target_idx);
	}
	else
	{
		// 복수 삭제.. 
		while(!vec_idx.empty())
		{
			target_idx = vec_idx.back();
			
			i3::vu::erase_index(data_list, target_idx);

			if (pCtrl == &m_listctrl_add)
				i3::vu::erase_index(pCurrUnit->add_opt_list, target_idx);
			
			pCtrl->DeleteItem(target_idx);
			vec_idx.pop_back();
		}
	}


	// 삭제 처리 후 컨텐츠파일목록 갱신 요구됨...

	PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)GetParent();
	PackScriptRealFileListPane* contents_pane =	framewnd->GetPackScriptRealFileListPane();
	contents_pane->SetPackScriptUnit(pCurrUnit);
	//

}

BOOL PackScriptFileTaskPane::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return TRUE;
//	return CMFCTasksPane::OnEraseBkgnd(pDC);
}

bool	PackScriptFileTaskPane::IsContentsModified() const
{
	return m_contents != m_contents_orig;
}
