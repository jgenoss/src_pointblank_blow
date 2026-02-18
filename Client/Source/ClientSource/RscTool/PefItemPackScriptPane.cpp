// PefItemPackScriptPane.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PefItemPackScriptPane.h"

#include "AssocFile.h"
#include "PackScriptTree.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

// PefItemPackScriptPane
#include "ItemDetailFrame.h"
#include "ItemDetailView.h"

#include "PathMgr.h"
#include "MainFrm.h"
#include "ExecutePackFrameWnd.h"

#include "i3Base/string/ext/num_to_str.h"
#include "i3Base/itl/range/algorithm/find.h"

#include "LoadRSCFile.h"


namespace
{
	// 멀티셀렉션 동기화에서는 잘못된 재귀호출/중복호출을 피하기 쉬운 이유로..
	// OnSelected()보다는
	// OnClick이 더 유용할때가 많다..
	//
	enum 
	{
		COMMAND_MEDIA_TO_QA_COPY = 1,
		COMMAND_QA_TO_LIVE_PACK,
		COMMAND_MEDIA_TO_QA_TO_LIVE,
	};

	struct PefItemPackListCallback : i3MFCListCtrlCallback
	{
		PefItemPackScriptPane* owner;
		PefItemPackListCallback(PefItemPackScriptPane* owner) : owner(owner) {}

		virtual void		OnClick(i3MFCListCtrl* Ctrl, int index, int subItem)
		{
			ItemDetailFrame* frame = (ItemDetailFrame*)owner->GetParent();

			i3::vector<int> all_sels;
			Ctrl->GetAllSelectedItems(all_sels);

			const size_t num = all_sels.size();

			i3::vector<const AssocResultFileBase*> bases;

			for (size_t i = 0 ; i < num ; ++i )
			{
				AssocResultFileBase* base = (AssocResultFileBase*)Ctrl->GetItemData(all_sels[i]);
				bases.push_back(base);	
			}

			frame->GetDetailView()->SelectAssocFiles(bases);

		}

		virtual void OnContextMenu(i3MFCListCtrl* Ctrl, const CPoint& pt) 
		{
			CPoint ptClient = pt;
			Ctrl->ScreenToClient(&ptClient);

			int target_idx = Ctrl->HitTest(ptClient);
			
			CMenu menu;
			menu.CreatePopupMenu();

			bool bMediaPath = PathMgr::i()->IsMediaPathExist();
			bool bQAPath = PathMgr::i()->IsQAPathExist();
			bool bLivePath = PathMgr::i()->IsLivePathExist(); 

			if ( bMediaPath && bQAPath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_MEDIA_TO_QA_COPY, L"Copy : Media -> QA");
			}

			if ( bQAPath && bLivePath )
			{
				menu.AppendMenu(MF_STRING, COMMAND_QA_TO_LIVE_PACK, L"Pack : QA -> Live");
			}

			if ( bMediaPath && bQAPath && bLivePath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_MEDIA_TO_QA_TO_LIVE, L"Copy/Pack : Media -> QA -> Live");
			}


			long result_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, Ctrl);
			menu.DestroyMenu();

			if (Ctrl->GetFirstSelectedItem() == -1)
			{
				if (target_idx != -1)
					Ctrl->SetItemState(target_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			}

			switch( result_cmd )
			{

			case COMMAND_MEDIA_TO_QA_COPY:
				owner->DoPackScriptJobWithSelectedItems(PACKMODE_MEDIA_TO_QA_COPY);
				break;
			case COMMAND_QA_TO_LIVE_PACK:
				owner->DoPackScriptJobWithSelectedItems(PACKMODE_QA_TO_LIVE_PACK);
				break;
			case COMMAND_MEDIA_TO_QA_TO_LIVE:

				owner->DoPackScriptJobWithSelectedItems(PACKMODE_MEDIA_TO_QA_COPY);
				owner->DoPackScriptJobWithSelectedItems(PACKMODE_QA_TO_LIVE_PACK);

				break;

			default:
				break;



			}

		}
	};

}



IMPLEMENT_DYNAMIC(PefItemPackScriptPane, CDockablePane)

PefItemPackScriptPane::PefItemPackScriptPane() 
{
	m_listCtrl.SetCallback(new PefItemPackListCallback(this));
}

PefItemPackScriptPane::~PefItemPackScriptPane()
{
	delete m_listCtrl.GetCallback();
}


BEGIN_MESSAGE_MAP(PefItemPackScriptPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// PefItemPackScriptPane 메시지 처리기입니다.



int PefItemPackScriptPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER);
	m_listCtrl.InsertColumn(0, L"Result Filename");
	m_listCtrl.InsertColumn(1, L"Category");
	m_listCtrl.InsertColumn(2, L"NumItem", LVCFMT_CENTER, 60);
	return 0;
}

void PefItemPackScriptPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_listCtrl.SetSize(cx, cy);
	m_listCtrl.SetColumnWidth(0, cx/ 3);
	m_listCtrl.SetColumnWidth(1, cx/ 12);

}

void PefItemPackScriptPane::SetAssocFileSet(const AssocFileSetPtr& ptr)
{
	m_AssocFileSet = ptr;
	
	const i3::vector<AssocResultFileBasePtr>& list = m_AssocFileSet->result_file_list;

	const size_t num = list.size();

	i3::wstring wstrTemp;

	const wchar_t* str_packscript = L"PACKSCRIPT";
	const wchar_t* str_animpack   = L"ANIMPACK";
	const wchar_t* str_normalFile = L"NORMAL_FILE";
	const wchar_t* str_rscFile    = L"RSC_FILE";
	const wchar_t* str_err		  = L"ERROR";

	const wchar_t* file_category = str_err;
	i3::stack_wstring strNum;
	
	m_listCtrl.SetRedraw(FALSE);

	for (size_t i = 0 ; i < num ; ++i)
	{
		AssocResultFileBase* base = list[i].get();

		file_category = str_err;
		
		if (base->is_rsc_file)
		{
			AssocResultFile_RscFile*   p = static_cast<AssocResultFile_RscFile*>(base);
//			i3::mb_to_utf16(p->rsc_file_path, wstrTemp);
			i3::sprintf(wstrTemp, L"%s  [%s.i3PackScript]", p->rsc_file_path, p->packscript_file->GetFileTitle());
			file_category = str_rscFile;
		}
		else
		if (base->is_packscript)
		{
			AssocResultFile_PackScript* p = static_cast<AssocResultFile_PackScript*>(base);

			wstrTemp.clear();
			p->packscript_file->MakeRelPath(wstrTemp);
			wstrTemp += L".i3PackScript";
			
			file_category = str_packscript;
		}
		else
		if (base->is_midpack)
		{
			AssocResultFile_AnimPack* p = static_cast<AssocResultFile_AnimPack*>(base);
			i3::mb_to_utf16(p->midpack_path, wstrTemp);

			file_category = str_animpack;
		}
		else
		if (base->is_normal_file)
		{
			AssocResultFile_NormalFile* p = static_cast<AssocResultFile_NormalFile*>(base);
			i3::mb_to_utf16(p->file_path, wstrTemp);
			file_category = str_normalFile;
		}
				
		m_listCtrl.InsertItem(i, wstrTemp.c_str());
		m_listCtrl.SetItemText(i, 1, file_category);
		m_listCtrl.SetItemData(i, DWORD_PTR(base));

	
		i3::integral_to_str( base->m_assoc_fileset_index_list.size(), strNum);
		m_listCtrl.SetItemText(i, 2, strNum.c_str());
	}
		
	m_listCtrl.SetRedraw(TRUE);
	m_listCtrl.RedrawWindow();
}

void PefItemPackScriptPane::SelectAssocResultFiles(const i3::vector<const AssocFile*>&  file_list)
{
	m_listCtrl.UnselectAll();

	const size_t num_file = file_list.size();

	int max_idx = -1;
	
	for (size_t i = 0; i < num_file ; ++i)
	{
		const AssocFile* f = file_list[i];

		const i3::vector<size_t>& index_list = f->vec_result_file_index;

		const size_t num_idx = index_list.size();

		for (size_t j = 0 ; j < num_idx ; ++j)
		{
			int idx = int(index_list[j]);

			m_listCtrl.SetItemState(idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

			if (idx > max_idx)
				max_idx = idx;
		}
	}

	if (max_idx != -1)
		m_listCtrl.EnsureVisible(max_idx, FALSE);
	

}

void	PefItemPackScriptPane::DoPackScriptJobWithSelectedItems(PackMode mode)
{
	// 순서대로 처리한다..
	i3::vector<int>		vec_idx;
	m_listCtrl.GetAllSelectedItems(vec_idx);

	ExecutePackFrameWnd* packWnd = GetMainFrame()->GetExecutePackFrameWnd();

	i3::wstring wstrTemp;

	const size_t num = vec_idx.size();
	int target_idx = -1;
	
	i3::vector<i3::rc_string>  vec_animpack, vec_normalfile;
	
	i3::vector<const PackScriptFile*>	ps_list_for_rsc;
	
	for (size_t i = 0 ; i < num ; ++i )
	{
		target_idx = vec_idx[i];

		AssocResultFileBase* base = (AssocResultFileBase*)m_listCtrl.GetItemData(target_idx);
		
		if (base->is_rsc_file)
		{
			AssocResultFile_RscFile* p = static_cast<AssocResultFile_RscFile*>(base);

			if ( mode == PACKMODE_MEDIA_TO_QA_COPY)
			{
				i3::vector<i3::rc_string>	path_list;
				const size_t num_name = p->rscnode_rel_path_list.size();
				path_list.reserve(num_name);
				for (size_t i = 0 ; i < num_name ; ++i)
					path_list.push_back(  p->rscnode_rel_path_list[i]->m_Path);
						
				packWnd->AddRscTask( p->rsc_file_path, path_list);
			}
			else		// 그 이외의 모드는 중복 제거 후 패킹 수행으로 처리한다..
			{
				i3::vector<const PackScriptFile*>::iterator it = i3::range::find( ps_list_for_rsc, p->packscript_file);

				if (it == ps_list_for_rsc.end() )
				{
					wstrTemp.clear();
					p->packscript_file->MakeRelPath(wstrTemp);	
					wstrTemp += L".i3PackScript";
					
					packWnd->AddPackScriptFile(wstrTemp, mode);

					ps_list_for_rsc.push_back(p->packscript_file);
				}
			}
		}
		else
		if (base->is_packscript)
		{
			AssocResultFile_PackScript* p = static_cast<AssocResultFile_PackScript*>(base);

		
			if ( mode == PACKMODE_MEDIA_TO_QA_COPY)
			{
				wstrTemp = p->packscript_file->GetFileTitle();
				i3::string name;	i3::utf16_to_mb(wstrTemp, name);
				name += ".i3PackScript (PARTIAL COPY)";
				packWnd->AddNormalFileList(p->file_path_list, name, mode);				
			}
			else // 그 이외의 모드는 패킹 수행으로 처리한다..
			{
				wstrTemp.clear();
				p->packscript_file->MakeRelPath(wstrTemp);
				wstrTemp += L".i3PackScript";

				packWnd->AddPackScriptFile(wstrTemp, mode);
			}
			
		}
		else
		if (base->is_midpack)
		{
			AssocResultFile_AnimPack* p = static_cast<AssocResultFile_AnimPack*>(base);
			vec_animpack.push_back(p->midpack_path);

		}
		else
		if (base->is_normal_file)
		{
			AssocResultFile_NormalFile* p = static_cast<AssocResultFile_NormalFile*>(base);
			vec_normalfile.push_back(p->file_path);
		}
	}

	if (!vec_animpack.empty())
	{
		i3::string name = m_AssocFileSet->item_name;
		name += " (ANIMPACK COPY)";
		
		packWnd->AddNormalFileList(vec_animpack, name, mode);
	}
	
	if (!vec_normalfile.empty())
	{
		i3::string name = m_AssocFileSet->item_name;
		name += " (NORMALFILE COPY)";

		packWnd->AddNormalFileList(vec_normalfile, name, mode);
	}
	
}
