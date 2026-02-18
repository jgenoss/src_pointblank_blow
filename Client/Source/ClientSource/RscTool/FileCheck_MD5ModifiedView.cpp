// FileCheck_MD5ModifiedView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_MD5ModifiedView.h"

#include "FileCheck_PackScriptMgr.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/extract_filename.h"

// FileCheck_MD5ModifiedView

IMPLEMENT_DYNCREATE(FileCheck_MD5ModifiedView, CView)

FileCheck_MD5ModifiedView::FileCheck_MD5ModifiedView() : m_diff_result(NULL)
{

}

FileCheck_MD5ModifiedView::~FileCheck_MD5ModifiedView()
{
}

BEGIN_MESSAGE_MAP(FileCheck_MD5ModifiedView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_MD5ModifiedView 그리기입니다.

void FileCheck_MD5ModifiedView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_MD5ModifiedView 진단입니다.

#ifdef _DEBUG
void FileCheck_MD5ModifiedView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_MD5ModifiedView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_MD5ModifiedView 메시지 처리기입니다.

int FileCheck_MD5ModifiedView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);
	m_listCtrl.InsertColumn(0, L"File");
	m_listCtrl.InsertColumn(1, L"PACKSCRIPT", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(2, L"RSC_FILE", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(3, L"PACKSCRIPT", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(4, L"RSC_FILE", LVCFMT_LEFT, 200);

	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_listCtrl.GetFont(), FALSE);

	return 0;
}

void FileCheck_MD5ModifiedView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_title.MoveWindow(0,0, cx, GetSystemMetrics(SM_CYCAPTION) );
	m_listCtrl.MoveWindow(0, GetSystemMetrics(SM_CYCAPTION), cx, cy-GetSystemMetrics(SM_CYCAPTION) );

//	m_listCtrl.SetSize(cx, cy);

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);
	}

	m_listCtrl.SetColumnWidth(0, cx - 300);

	m_listCtrl.RedrawWindow();
}

BOOL FileCheck_MD5ModifiedView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}



void	FileCheck_MD5ModifiedView::InitCaption(const wchar_t* caption)
{
	m_title.SetWindowText(caption);
}

void	FileCheck_MD5ModifiedView::InitDiffResult(const DiffResult_PackScriptResult& diff_result)
{
	m_diff_result = &diff_result;
}

void	FileCheck_MD5ModifiedView::InsertModifiedResult(size_t idx)
{
	//
	m_mod_result.in_out_result[0].push_back(m_diff_result->in_out_alist[DIFFRESULT_PS_FOUND_LEFT][idx]);
	m_mod_result.in_out_result[1].push_back(m_diff_result->in_out_alist[DIFFRESULT_PS_FOUND_RIGHT][idx]);
	//
	
	size_t num_mod_res = m_mod_result.in_out_result[0].size();
	
//	const i3::vector<const PackScriptUnitResult*>& v_unit_res = m_diff_result->in_out_found_lhs_res;
//	const i3::vector<size_t>& v_string_idx = m_diff_result->in_out_found_lhs_string_idx;

	const PSUnitResult_MapResFile* l_res = m_diff_result->in_out_alist[DIFFRESULT_PS_FOUND_LEFT][idx].get();
	const PSUnitResult_MapResFile* r_res = m_diff_result->in_out_alist[DIFFRESULT_PS_FOUND_RIGHT][idx].get();


	i3::wstring wstr;
	const i3::string& strFile = l_res->unit->result_file_list[ l_res->string_idx ]; //	v_unit_res[idx]->result_file_list[v_string_idx[idx] ];
	i3::mb_to_utf16(strFile, wstr);

	INT32 curr_index = num_mod_res - 1;
	m_listCtrl.InsertItem( curr_index, wstr.c_str());
	
	if ( l_res->unit->parent)
	{
		const i3::wstring& ps_path = l_res->unit->parent->parent->packscript_path;
		i3::extract_filename(ps_path, wstr);
		m_listCtrl.SetItemText(curr_index, 1, wstr.c_str());					
	}

	const RSCFileResultMapElement* elem = FileCheck_PackScriptMgr::i()->FindRSCFileMapElement(strFile);

	if (elem && !elem->contents_result_list_by_path[0].empty() )
	{
		const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[0][0];

		const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

		i3::mb_to_utf16(rscFileName, wstr);
		m_listCtrl.SetItemText(curr_index, 2, wstr.c_str());
	}

	if ( r_res->unit->parent)
	{
		const i3::wstring& ps_path = r_res->unit->parent->parent->packscript_path;
		i3::extract_filename(ps_path, wstr);
		m_listCtrl.SetItemText(curr_index, 3, wstr.c_str());					
	}

	if (elem && !elem->contents_result_list_by_path[1].empty() )
	{
		const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[1][0];

		const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

		i3::mb_to_utf16(rscFileName, wstr);
		m_listCtrl.SetItemText(curr_index, 4, wstr.c_str());
	}


}

void	FileCheck_MD5ModifiedView::DeleteAllResult()
{
	m_listCtrl.DeleteAllItems();
	
	m_mod_result.in_out_result[0].clear();
	m_mod_result.in_out_result[1].clear();
}

void	FileCheck_MD5ModifiedView::SetRedrawListCtrl(BOOL bTrue)
{
	m_listCtrl.SetRedraw(bTrue);

	if (bTrue)
	{
		m_listCtrl.RedrawWindow();
	}
}

