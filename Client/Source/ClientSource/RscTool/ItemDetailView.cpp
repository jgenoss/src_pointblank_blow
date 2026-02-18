// ItemDetailView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "ItemDetailView.h"

#include "AssocFile.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "PackScriptTree.h"

#include "ItemDetailFrame.h"
#include "LoadRSCFile.h"


namespace
{
	struct ItemDetailViewListCallback : i3MFCListCtrlCallback
	{
		ItemDetailView* owner;
		ItemDetailViewListCallback(ItemDetailView* owner) : owner(owner) {}


		virtual void		OnClick(i3MFCListCtrl* Ctrl, int index, int subItem)
		{
			ItemDetailFrame* frame = (ItemDetailFrame*)owner->GetParent();

			i3::vector<int> all_sels;
			Ctrl->GetAllSelectedItems(all_sels);

			const size_t num = all_sels.size();

			i3::vector<const AssocFile*> files;

			for (size_t i = 0 ; i < num ; ++i )
			{
				const AssocFile* f = (const AssocFile*)Ctrl->GetItemData(all_sels[i]);
				files.push_back(f);	
			}

			frame->GetPefItemPackScriptPane()->SelectAssocResultFiles(files);
		}
	
	};

}


// ItemDetailView

IMPLEMENT_DYNCREATE(ItemDetailView, CView)

ItemDetailView::ItemDetailView() 
{
	m_listCtrl.SetCallback( new ItemDetailViewListCallback(this));
}

ItemDetailView::~ItemDetailView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(ItemDetailView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ItemDetailView 그리기입니다.

void ItemDetailView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// ItemDetailView 진단입니다.

#ifdef _DEBUG
void ItemDetailView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void ItemDetailView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// ItemDetailView 메시지 처리기입니다.

int ItemDetailView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER);
	m_listCtrl.InsertColumn(0, L"RegName / ParentRsc");
	m_listCtrl.InsertColumn(1, L"RegValue / NodeName");
	m_listCtrl.InsertColumn(2, L"FileExist", LVCFMT_CENTER, 60);
	m_listCtrl.InsertColumn(3, L"FilePath");
	m_listCtrl.InsertColumn(4, L"MiddlePack");
	m_listCtrl.InsertColumn(5, L"PackFile");
	m_listCtrl.InsertColumn(6, L"RSCFile");
	return 0;
}

void ItemDetailView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_listCtrl.GetSafeHwnd())
	{
		m_listCtrl.SetSize(cx, cy);
		m_listCtrl.SetColumnWidth(0, cx/5);
		m_listCtrl.SetColumnWidth(1, cx/10);
		m_listCtrl.SetColumnWidth(3, cx/5);
		m_listCtrl.SetColumnWidth(4, cx*3/10);
		m_listCtrl.SetColumnWidth(5, cx*3/10);
		m_listCtrl.SetColumnWidth(6, cx*3/10);
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void ItemDetailView::SetAssocFileSet(const AssocFileSetPtr& ptr)
{
	m_AssocFileSet = ptr;
	const i3::vector<AssocFilePtr>& list = m_AssocFileSet->assoc_file_list;

	const size_t num = list.size();

	i3::wstring wstrTemp;

	m_listCtrl.SetRedraw(FALSE);

	for (size_t i = 0 ; i < num ; ++i )
	{
		const AssocFile* f = list[i].get();

		i3::mb_to_utf16(f->reg_name, wstrTemp);
		m_listCtrl.InsertItem(i, wstrTemp.c_str());
			

		m_listCtrl.SetItemData(i, DWORD_PTR(f));

		i3::mb_to_utf16(f->reg_value, wstrTemp);
		m_listCtrl.SetItemText(i, 1, wstrTemp.c_str());

		i3::mb_to_utf16(f->file_path, wstrTemp);
		m_listCtrl.SetItemText(i, 3, wstrTemp.c_str());

		i3::mb_to_utf16(f->midpack_path, wstrTemp);
		m_listCtrl.SetItemText(i, 4, wstrTemp.c_str());

		const size_t num_res = f->vec_result_file_index.size();
	
		const i3::vector<AssocResultFileBasePtr>& file_list = m_AssocFileSet->result_file_list;
			
		i3::vector<const PackScriptFile*>	ps_list;
		i3::vector<i3::rc_string>			rsc_path_list;
		
		bool								file_exist = false;

		for (size_t j = 0; j < num_res ; ++j )
		{
			const AssocResultFileBasePtr& ptr = file_list[ f->vec_result_file_index[j] ]; 

			if ( ptr->is_packscript )
			{
				ps_list.push_back( static_cast<AssocResultFile_PackScript*>( ptr.get() )->packscript_file);		
				file_exist = true;
			}
			else
			if (ptr->is_rsc_file )
			{
				rsc_path_list.push_back(static_cast<AssocResultFile_RscFile*>(ptr.get())->rsc_file_path);
			}
			else
			{
				file_exist = true;
			}
		}
			
	
		const size_t num_ps = ps_list.size();
		if (num_ps)
		{
			i3::stack_wstring wstrTemp2;

			size_t j = 0;
			for (; j < num_ps - 1 ; ++j )
			{
				// 최소한 1개는 있을 거심..
				const PackScriptFile* f = ps_list[j];
				wstrTemp2 += f->GetFileTitle();
				wstrTemp2 += L".i3PackScript ; ";
			}

			const PackScriptFile* f = ps_list[j];
			wstrTemp2 += f->GetFileTitle();
			wstrTemp2 += L".i3PackScript";
			m_listCtrl.SetItemText(i, 5, wstrTemp2.c_str() );
		}
			
		const size_t num_rsc_path = rsc_path_list.size();
		if (num_rsc_path)
		{
			i3::stack_string	strTemp;
			size_t j = 0;
			for ( ; j < num_rsc_path - 1; ++j)
			{
				const i3::rc_string& str = rsc_path_list[j];
				strTemp += str;
				strTemp += " ; ";
			}
			
			const i3::rc_string& str = rsc_path_list[j];
			strTemp += str;
			
			i3::mb_to_utf16(strTemp, wstrTemp);
			m_listCtrl.SetItemText(i, 6, wstrTemp.c_str() );		
		}

		m_listCtrl.SetItemText(i, 2, (file_exist) ? L"O" : L"X");

	}

	m_listCtrl.SetRedraw(TRUE);
	m_listCtrl.RedrawWindow();

}

void ItemDetailView::SelectAssocFiles(const i3::vector<const AssocResultFileBase*>&  base_list)
{
	m_listCtrl.UnselectAll();

	const size_t num_base = base_list.size();
	
	int max_idx = -1;


	for (size_t i = 0; i < num_base ; ++i)
	{
		const AssocResultFileBase* base = base_list[i];

		const i3::vector<size_t>& index_list = base->m_assoc_fileset_index_list;

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
