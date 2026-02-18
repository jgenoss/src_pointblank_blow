// FileCheck_ResultView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_ResultView.h"
#include "PathMode.h"

#include "FileCheck_PackScriptMgr.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "DiffResult_PackScriptResult.h"

// FileCheck_ResultView

IMPLEMENT_DYNCREATE(FileCheck_ResultView, CView)

FileCheck_ResultView::FileCheck_ResultView() : m_path_mode(PATHMODE_MEDIA), 
m_l_r(0)
{

}

FileCheck_ResultView::~FileCheck_ResultView()
{
}

BEGIN_MESSAGE_MAP(FileCheck_ResultView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_ResultView 그리기입니다.

void FileCheck_ResultView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_ResultView 진단입니다.

#ifdef _DEBUG
void FileCheck_ResultView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_ResultView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_ResultView 메시지 처리기입니다.

int FileCheck_ResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);

	m_listCtrl.InsertColumn(0, L"File");
	m_listCtrl.InsertColumn(1, L"NO FILE", LVCFMT_CENTER, 50);

	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_listCtrl.GetFont(), FALSE);

	return 0;
}

void FileCheck_ResultView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_title.MoveWindow(0,0, cx, GetSystemMetrics(SM_CYCAPTION) );

//	m_listCtrl.SetSize(cx, cy );
	m_listCtrl.MoveWindow(0, GetSystemMetrics(SM_CYCAPTION), cx, cy-GetSystemMetrics(SM_CYCAPTION) );

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	cx -= GetSystemMetrics(SM_CXFRAME);

	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL) ;
	}

	m_listCtrl.SetColumnWidth(0, cx-50);

	this->RedrawWindow();
}

BOOL FileCheck_ResultView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}


void	FileCheck_ResultView::InitPathMode(PathMode m, int l_r, const wchar_t* caption)
{
	m_path_mode = m;
	m_l_r = l_r;

	m_title.SetWindowText(caption);
	

}

void	FileCheck_ResultView::UpdateList(const PackScriptResult* res)
{
	if (res == NULL)
	{
		m_listCtrl.DeleteAllItems();	return;
	}
	
	PackScriptContentsResult* contents_res = res->contents[m_l_r].get();
	
	if (contents_res == NULL)
	{
		m_listCtrl.DeleteAllItems();	return;
	}
	
	m_listCtrl.SetRedraw(FALSE);
	m_listCtrl.DeleteAllItems();

	const size_t num_list = contents_res->unit_list.size();
	
	INT32	item_count = 0;
	{
		for (size_t i = 0 ; i < num_list ; ++i )
		{
			PackScriptUnitResult* unit_res = contents_res->unit_list[i].get();

			item_count += unit_res->result_file_list.size();
		}

		PackScriptUnitResult* unit_res = contents_res->copy_unit.get();

		if (unit_res)
		{
			item_count += unit_res->result_file_list.size();
		}
	}

	m_listCtrl.SetItemCount(item_count);
	
	i3::wstring   wstrFile;
	item_count = 0;

	for (size_t i = 0 ; i < num_list ; ++i )
	{
		PackScriptUnitResult* unit_res = contents_res->unit_list[i].get();
		
		const size_t num_file = unit_res->result_file_list.size();

		for (size_t j = 0; j < num_file ; ++j, ++item_count)
		{
			const i3::string& strFile = unit_res->result_file_list[j];
			
			i3::mb_to_utf16(strFile, wstrFile);
			m_listCtrl.InsertItem(item_count, wstrFile.c_str() );
		}
	}
	
	PackScriptUnitResult* unit_res = contents_res->copy_unit.get();

	if (unit_res)
	{
		const size_t num_file = unit_res->result_file_list.size();
		for (size_t j = 0; j < num_file ; ++j, ++item_count)
		{
			const i3::string& strFile = unit_res->result_file_list[j];

			i3::mb_to_utf16(strFile, wstrFile);
			m_listCtrl.InsertItem(item_count, wstrFile.c_str() );
		}
	}

	m_listCtrl.SetRedraw(TRUE);
	m_listCtrl.RedrawWindow();

}

void	FileCheck_ResultView::UpdateList(const RSCFileResult* res)
{
	if (res == NULL) {	m_listCtrl.DeleteAllItems();	return;	}

	RSCFileContentsResult* contents = res->contents[m_l_r].get();

	if (contents == NULL) {	m_listCtrl.DeleteAllItems();	return;	}

	m_listCtrl.SetRedraw(FALSE);
	m_listCtrl.DeleteAllItems();
	
//	const i3::vector<const PackScriptUnitResult*>& res_list = contents->res_list;
//	const i3::vector<size_t>& string_idx_list =	contents->string_idx_list;
//	const i3::vector<size_t>& no_file_idx_list = contents->no_file_list;

	const i3::vector<PSUnitResult_MapResFilePtr>& resFile_list = contents->resFileList;
	
	const size_t num_list = resFile_list.size();
	
	i3::wstring wstrTemp;

	for (size_t i = 0 ; i < num_list ; ++i)
	{
		const PSUnitResult_MapResFile* resFile = resFile_list[i].get();

		const i3::string& strFilePath = resFile->unit->result_file_list[ resFile->string_idx ];

		i3::mb_to_utf16(strFilePath, wstrTemp);
		
		m_listCtrl.InsertItem(i, wstrTemp.c_str());
		
		if (resFile->file_exist == false)
			m_listCtrl.SetItemText(i, 1, L"X");
	}
	
	

	CRect rc;	GetClientRect(&rc);
	// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
	rc.right -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXFRAME);
	m_listCtrl.SetColumnWidth(0, rc.right - 50);	

	m_listCtrl.SetRedraw(TRUE);
	m_listCtrl.RedrawWindow();

}



void	FileCheck_ResultView::ClearList()
{
	m_listCtrl.SetRedraw(FALSE);
	m_listCtrl.DeleteAllItems();
	m_listCtrl.SetRedraw(TRUE);

	m_listCtrl.RedrawWindow();
}
