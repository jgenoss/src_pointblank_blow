// LocaleStringFrameWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LocaleStringFrameWnd.h"

#include "MainFrm.h"
#include "LocaleStringDiff.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
// LocaleStringFrameWnd
#include "PefTxtDiffInputInfo.h"


IMPLEMENT_DYNCREATE(LocaleStringFrameWnd, CFrameWndEx)

LocaleStringFrameWnd::LocaleStringFrameWnd() 
{

}

LocaleStringFrameWnd::~LocaleStringFrameWnd()
{

}


BEGIN_MESSAGE_MAP(LocaleStringFrameWnd, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// LocaleStringFrameWnd 메시지 처리기입니다.

int LocaleStringFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER);
	m_listCtrl.InsertColumn(0, L"Key Name");
	m_listCtrl.InsertColumn(1, L"Diff Type");
	m_listCtrl.InsertColumn(2, L"Latest Revision Value");
	m_listCtrl.InsertColumn(3, L"Local File Value");	
	

	return 0;
}

void LocaleStringFrameWnd::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetMainFrame()->DestroyPopupWndFromList(this);

	CFrameWndEx::PostNcDestroy();
}

BOOL LocaleStringFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	int fullscreen_w = ::GetSystemMetrics(SM_CXSCREEN);
	int fullscreen_h = ::GetSystemMetrics(SM_CYSCREEN);

	cs.cx = fullscreen_w * 9 / 10;
	cs.cy = fullscreen_h * 5 / 6;

	cs.y = (fullscreen_h - cs.cy) / 2;
	cs.x = (fullscreen_w - cs.cx) / 2;

	return TRUE;
}

BOOL LocaleStringFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//return CFrameWndEx::OnEraseBkgnd(pDC);
	return TRUE;
}

void LocaleStringFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_listCtrl.GetSafeHwnd() == NULL)
		return;

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

	m_listCtrl.SetColumnWidth( 0, cx / 3);	cx -= cx/3;
	m_listCtrl.SetColumnWidth( 1, 80);		cx -= 80;
	
	cx = (cx < 300) ? 300 : cx;

	m_listCtrl.SetColumnWidth(2, cx / 2);
	m_listCtrl.SetColumnWidth(3, cx / 2);


}

namespace
{
	struct LocaleStringFrameWnd_Visitor : LocaleStringDiff_Visitor
	{
		LocaleStringFrameWnd* owner;	
		i3MFCListCtrl*	 list_ctrl;
		LocaleStringFrameWnd_Visitor(LocaleStringFrameWnd* owner) : owner(owner)
		{
			list_ctrl = owner->GetListCtrl();
		}

		virtual void Modified( const LocaleString* lhs, 
								const LocaleString* rhs ) 
		{
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, lhs->m_key.c_str());
			list_ctrl->SetItemText(idx, 1, L"MODIFIED");
			list_ctrl->SetItemText(idx, 2, lhs->m_val.c_str());
			list_ctrl->SetItemText(idx, 3, rhs->m_val.c_str());
		}

		virtual void Added( const LocaleString* rhs ) 
		{
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, rhs->m_key.c_str());
			list_ctrl->SetItemText(idx, 1, L"ADDED");
			list_ctrl->SetItemText(idx, 2, L"");
			list_ctrl->SetItemText(idx, 3, rhs->m_val.c_str());
		}

		virtual void Removed( const LocaleString* lhs) 
		{
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, lhs->m_key.c_str());
			list_ctrl->SetItemText(idx, 1, L"REMOVED");
			list_ctrl->SetItemText(idx, 2, lhs->m_val.c_str());
			list_ctrl->SetItemText(idx, 3, L"");		
		}
		
	};
		
}



void LocaleStringFrameWnd::SetInfo(const PefTxtDiffInputInfo& input_info, const LocaleStringSetPtr& exp, const LocaleStringSetPtr& local)
{

	m_exp_string_set = exp;
	m_local_string_set = local;
	
	i3::const_char_range str_mode[3] = { i3::as_literal("MEDIA_PATH"), 
		i3::as_literal("QA_PATH"), i3::as_literal("LIVE_PATH") };

	i3::string str_rev[2];

	if (input_info.revision[0] != -1)
		str_rev[0] = i3::format_string("r%d", input_info.revision[0]);
	else
		str_rev[0] = "Local";

	if (input_info.revision[1] != -1)
		str_rev[1] = i3::format_string("r%d", input_info.revision[1]);
	else
		str_rev[1] = "Local";

	LVCOLUMNW col_info;
	::memset(&col_info, 0, sizeof(LVCOLUMNW));
	col_info.mask = LVCF_TEXT;

	i3::string str = i3::format_string("%s : %s of %s", input_info.rel_path[0], str_rev[0], 
		str_mode[input_info.mode[0]]);

	i3::wstring wstr;		i3::mb_to_utf16(str, wstr);

	col_info.pszText = const_cast<wchar_t*>(wstr.c_str());
	m_listCtrl.SetColumn(2, &col_info);
	
	str = i3::format_string("%s : %s of %s", input_info.rel_path[1], str_rev[1], 
		str_mode[input_info.mode[1]] );

	i3::mb_to_utf16(str, wstr);

	col_info.pszText = const_cast<wchar_t*>(wstr.c_str());
	m_listCtrl.SetColumn(3, &col_info);

	LocaleStringDiffSet set;
	FindLocaleStringDiff(*m_exp_string_set, *m_local_string_set, set);
	
	if (set.m_list.empty() )
	{
		INT32 idx = m_listCtrl.GetItemCount();
		m_listCtrl.InsertItem(idx, L"No Different Text Data");
	}
	else
	{
		LocaleStringFrameWnd_Visitor v(this);
		VisitLocaleStringDiff(set, &v);
	}
		
}
