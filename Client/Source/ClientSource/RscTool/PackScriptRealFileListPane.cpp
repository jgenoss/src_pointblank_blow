// PackScriptRealFileListPane.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptRealFileListPane.h"

#include "PathMgr.h"

#include "PackScriptContents.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/itl/algorithm/replace.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/is_matched_wildcard.h"

#include "PackScriptFileFrameWnd.h"

// PackScriptRealFileListPane

struct PackScriptRealFileListCallback : i3MFCListCtrlCallback
{
	PackScriptRealFileListCallback(PackScriptRealFileListPane* owner) : m_owner(owner) {}
	
	PackScriptRealFileListPane*	m_owner;

	virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
	{
		// 귀차니즘 쩐다...ㅋ..
		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptFileExplorerListView* explorer = framewnd->GetExplorerListView();
		i3ShellListCtrl* target_ctrl = explorer->GetListCtrl();
		target_ctrl->UnselectAll();
		//

		const i3::vector<i3::string>& file_list = m_owner->GetResultFileList();

		const i3::string& str_relPath = file_list[index];
		
		i3::wstring full_path( PathMgr::i()->GetMainMediaPath() );
		full_path += L'/';

		i3::wstring wstr_relPath;
		i3::mb_to_utf16(str_relPath, wstr_relPath);
		full_path += wstr_relPath;
		i3::replace(full_path.begin(), full_path.end(), L'/', L'\\');
		//
	
		//
		i3::wstring wstr_dir, wstr_filename;
		i3::extract_directoryname_filename(full_path, wstr_dir, wstr_filename);

		HRESULT hr = target_ctrl->DisplayFolder(wstr_dir.c_str());
		//
		if (hr == S_OK)
		{
			LVFINDINFO info;
			info.flags = LVFI_STRING;
			info.psz = wstr_filename.c_str();
			int itemIdx = target_ctrl->FindItem(&info);

			if (itemIdx != -1)
			{
				target_ctrl->SetItemState(itemIdx, LVIS_SELECTED| LVIS_FOCUSED, LVIS_SELECTED| LVIS_FOCUSED);
				target_ctrl->EnsureVisible(itemIdx, FALSE);
			}
		}

	}

	
};


IMPLEMENT_DYNAMIC(PackScriptRealFileListPane, CDockablePane)

PackScriptRealFileListPane::PackScriptRealFileListPane() : m_unit(NULL)
{
	m_listCtrl.SetCallback(new PackScriptRealFileListCallback(this));
}

PackScriptRealFileListPane::~PackScriptRealFileListPane()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(PackScriptRealFileListPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// PackScriptRealFileListPane message handlers



int PackScriptRealFileListPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);
	
	m_listCtrl.InsertColumn(0, L"Target File Path");
	m_listCtrl.InsertColumn(1, L"Encrypted", LVCFMT_RIGHT);
	
	return 0;
}

void PackScriptRealFileListPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_listCtrl.SetSize(cx, cy);

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL);
	}

	m_listCtrl.SetColumnWidth(0, cx - 100);
	m_listCtrl.SetColumnWidth(1, 100);
	
	this->RedrawWindow();
}

// 셋팅시 대부분의 목록 초기화를 처리한다.
void PackScriptRealFileListPane::SetPackScriptUnit(PackScriptUnit* unit)
{
	m_unit = unit;

	m_resultAddList.clear();
	m_resultRemoveList.clear();
	m_resultEncList.clear();
	m_resultFileList.clear();
	m_encMappingList.clear();

	if (m_unit == NULL)
	{
		m_listCtrl.SetRedraw(FALSE);
		m_listCtrl.RemoveAllItems();
		m_listCtrl.SetRedraw(TRUE);
		return;
	}

	const PackScriptExceptionFilterList& filter_list = PathMgr::i()->GetExceptionList();

	{
		const i3::wstring& wstrMedia = PathMgr::i()->GetMainMediaPath();
		const i3::vector<i3::string>& nation_folder_filter_list = PathMgr::i()->GetNationFolderFilterList();

		i3::string strMedia;
		i3::utf16_to_mb(wstrMedia, strMedia);
	
		ImportResultFilesFromPackScriptUnit(strMedia, 
										*m_unit, filter_list.qa_filter_list, nation_folder_filter_list, m_resultFileList, m_encMappingList,
										m_resultAddList, m_resultRemoveList, m_resultEncList);
	}

	if (!filter_list.qa_force_copy_list.empty())
	{
		i3::vector<i3::string> resultFileList;
		i3::vector<bool>		new_encMappingList;

		i3::vector<i3::string> qa_force_copy_file_list;

		const size_t num_result_file = m_resultFileList.size();
		const size_t num_qa_force_copy = filter_list.qa_force_copy_list.size();

		for (size_t i = 0; i < num_result_file; ++i )
		{
			bool bFiltered = false;
			for (size_t  j = 0; j < num_qa_force_copy ; ++j)
			{
				if ( i3::is_matched_wildcard(m_resultFileList[i], filter_list.qa_force_copy_list[j]) )
				{
					bFiltered = true;
					break;
				}
			}
			
			if (!bFiltered)
			{
				resultFileList.push_back(m_resultFileList[i]);
				new_encMappingList.push_back( m_encMappingList[i]);
			}
			else
			{
				qa_force_copy_file_list.push_back(m_resultFileList[i]);
			}
		}
		
		i3::swap(m_resultFileList, resultFileList);
		i3::swap(m_encMappingList, new_encMappingList);

		if (!qa_force_copy_file_list.empty() )
		{
			PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)this->GetParent();
			PackScriptForceCopyToQAPane* forcecopy_pane = framewnd->GetPackScriptForceCopyToQAPane();
			
			forcecopy_pane->SetForceCopyFileList(qa_force_copy_file_list);
		}
	}
		

	//
	// ListCtrl에 컨텐츠 채우기..
	// 

	const size_t numFile = m_resultFileList.size();
	
	i3::wstring wstr;

	m_listCtrl.SetRedraw(FALSE);

	m_listCtrl.DeleteAllItems();
	m_listCtrl.SetItemCount(numFile);		// reserve개념?

	COLORREF defaultClr = m_listCtrl.GetTextColor();
	COLORREF redColor = RGB(255, 0, 0);

	for (size_t i = 0 ; i < numFile; ++i )
	{
		const i3::string& strData = m_resultFileList[i];
		i3::mb_to_utf16(strData, wstr);
		m_listCtrl.AddItem(wstr.c_str(), NULL);

		if (m_encMappingList[i])
		{
			m_listCtrl.SetItemText(i, 1, L"ENC");
		}
	}

	m_listCtrl.SetRedraw(TRUE);
	
//	m_listCtrl.Invalidate(FALSE);
//	m_listCtrl.RedrawWindow();
	
}

// 복잡하게 생각하지 말고.....셋팅과 동일하게처리...
// 일단, 병목이 있는지 충분이 상황을 기다려보고, 병목이 심하면 개선해야한다..
// 
//
// 실제로 병목이 발견되었는데..
// 1. CListCtrl의 병목 ( 수천개단위에서 InsertItem과 DeleteAllItems함수자체가 병목이 됨)
// --> SetItemCount / SetRedraw(FALSE)~(TRUE)블록을 걸어도 그닥 개선되어 보이지 않음..
// 2.  ImportResultFilesFromPackScriptUnit 함수 일괄 처리의 병목..
// --> CListCtrl과의 연동을 한다면 최적화를 생각하기가 좀 어려워보인다..
// 

// 대안 -> virtual list ctrl 방식으로 CListCtrl 운용방식을 변경해야함..
//         LVS_OWNERDATA 옵션을 걸고 쓰는 방식인데...
//         매우 자주 쓰는 함수 몇개를 포기하고 커스텀 구현도 많이 해둬야하는 부담..
//

//void PackScriptRealFileListPane::UpdateUnitData()
//{
//	PackScriptRealFileListPane::SetPackScriptUnit(m_unit);
//}


BOOL PackScriptRealFileListPane::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CDockablePane::OnEraseBkgnd(pDC);
	return TRUE;
}
