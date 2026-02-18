// PackScriptForceCopyToQAPane.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptForceCopyToQAPane.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "PackScriptFileFrameWnd.h"
#include "PathMgr.h"
#include "i3Base/itl/algorithm/replace.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"

// PackScriptForceCopyToQAPane
struct PackScriptForceCopyToQAPaneCallback : i3MFCListCtrlCallback
{
	PackScriptForceCopyToQAPaneCallback(PackScriptForceCopyToQAPane* owner) : m_owner(owner) {}

	PackScriptForceCopyToQAPane*	m_owner;

	virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
	{
		// 귀차니즘 쩐다...ㅋ..
		PackScriptFileFrameWnd* framewnd = (PackScriptFileFrameWnd*)m_owner->GetParent();
		PackScriptFileExplorerListView* explorer = framewnd->GetExplorerListView();
		i3ShellListCtrl* target_ctrl = explorer->GetListCtrl();
		target_ctrl->UnselectAll();
		//

		const i3::vector<i3::string>& file_list = m_owner->GetForceCopyFileList();

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

IMPLEMENT_DYNAMIC(PackScriptForceCopyToQAPane, CDockablePane)

PackScriptForceCopyToQAPane::PackScriptForceCopyToQAPane()
{
	m_listCtrl.SetCallback(new PackScriptForceCopyToQAPaneCallback(this));
}

PackScriptForceCopyToQAPane::~PackScriptForceCopyToQAPane()
{
	delete m_listCtrl.GetCallback();
}


BEGIN_MESSAGE_MAP(PackScriptForceCopyToQAPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// PackScriptForceCopyToQAPane 메시지 처리기입니다.



int PackScriptForceCopyToQAPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);
	m_listCtrl.InsertColumn(0, L"QA Copy-Only File");

	return 0;
}

void PackScriptForceCopyToQAPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	
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

	m_listCtrl.SetColumnWidth(0, cx );
	
	this->RedrawWindow();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL PackScriptForceCopyToQAPane::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return TRUE;
//	return CDockablePane::OnEraseBkgnd(pDC);
}

void	PackScriptForceCopyToQAPane::SetForceCopyFileList(const i3::vector<i3::string>& forcecopy_list)
{
	this->ShowWindow(SW_SHOW);

	m_forcecopy_list = forcecopy_list;

	m_listCtrl.SetRedraw(FALSE);
	
	m_listCtrl.DeleteAllItems();

	const size_t num_file = m_forcecopy_list.size();

	m_listCtrl.SetItemCount( num_file);
	i3::wstring wstr;

	for (size_t i = 0 ; i < num_file; ++i )
	{
		const i3::string& strData = m_forcecopy_list[i];
		i3::mb_to_utf16(strData, wstr);
		m_listCtrl.AddItem(wstr.c_str(), NULL);
	}

	m_listCtrl.SetRedraw(TRUE);	
}
