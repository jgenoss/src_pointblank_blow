// FileCheck_OnlyFilesView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_OnlyFilesView.h"

#include "FileCheck_PackScriptMgr.h"
#include "DiffResult_PackScriptResult.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/extract_filename.h"


// FileCheck_OnlyFilesView

IMPLEMENT_DYNCREATE(FileCheck_OnlyFilesView, CView)

FileCheck_OnlyFilesView::FileCheck_OnlyFilesView() 
{
	
}

FileCheck_OnlyFilesView::~FileCheck_OnlyFilesView()
{
}

BEGIN_MESSAGE_MAP(FileCheck_OnlyFilesView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_OnlyFilesView 그리기입니다.

void FileCheck_OnlyFilesView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_OnlyFilesView 진단입니다.

#ifdef _DEBUG
void FileCheck_OnlyFilesView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_OnlyFilesView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_OnlyFilesView 메시지 처리기입니다.

int FileCheck_OnlyFilesView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);
	m_listCtrl.InsertColumn(0, L"File");
	m_listCtrl.InsertColumn(1, L"PACSCRIPT", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(2, L"RSC_FILE", LVCFMT_LEFT, 200);

	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_listCtrl.GetFont(), FALSE);

	return 0;
}

void FileCheck_OnlyFilesView::OnSize(UINT nType, int cx, int cy)
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

	m_listCtrl.SetColumnWidth(0, cx - 300);

	m_listCtrl.RedrawWindow();
}

BOOL FileCheck_OnlyFilesView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void	FileCheck_OnlyFilesView::InitCaption(const wchar_t* caption)
{
	m_title.SetWindowText(caption);
}

void	FileCheck_OnlyFilesView::InitList( const DiffResult_PackScriptResult& diff_result, int l_r)
{
//	const i3::vector<const PackScriptUnitResult*>*	pv_unit_res;
//	const i3::vector<size_t>*					pv_string_idx;

	const i3::vector<PSUnitResult_MapResFilePtr>*  pv_resFile;

	if (l_r == 0)   // lhs
	{
		pv_resFile = &diff_result.in_out_alist[DIFFRESULT_PS_LEFT_ONLY];
	}
	else			// rhs
	{
		pv_resFile = &diff_result.in_out_alist[DIFFRESULT_PS_RIGHT_ONLY];
	}

	const i3::vector<PSUnitResult_MapResFilePtr>&  v_resFile  = *pv_resFile;

//	const i3::vector<const PackScriptUnitResult*>&	v_unit_res = *pv_unit_res;
//	const i3::vector<size_t>&					v_string_idx = *pv_string_idx;

	const size_t num = v_resFile.size();

	i3::wstring wstr;
	
	m_listCtrl.SetRedraw(FALSE);

	for (size_t i = 0 ; i < num ; ++i)
	{
		const PSUnitResult_MapResFile* resFile = v_resFile[i].get();

		const i3::string& strFile = resFile->unit->result_file_list[resFile->string_idx];
		
		i3::mb_to_utf16(strFile, wstr);
		m_listCtrl.InsertItem(i, wstr.c_str());

		if (resFile->unit->parent)
		{
			const i3::wstring& ps_path = resFile->unit->parent->parent->packscript_path;
			i3::extract_filename(ps_path, wstr);
			m_listCtrl.SetItemText(i, 1, wstr.c_str());
		}

		const RSCFileResultMapElement* elem = FileCheck_PackScriptMgr::i()->FindRSCFileMapElement(strFile);
		
		if (elem && !elem->contents_result_list_by_path[l_r].empty() )
		{
			const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[l_r][0];

			const i3::rc_string& rscFileName = contents->parent->rsc_file_path;
			
			i3::mb_to_utf16(rscFileName, wstr);
			m_listCtrl.SetItemText(i, 2, wstr.c_str());
		}
		
	}

	m_listCtrl.SetRedraw(TRUE);
}
