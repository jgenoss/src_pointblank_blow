// PackScriptView.cpp : implementation file
//

#include "stdafx.h"
#include "BuildTool.h"
#include "MainFrm.h"
#include "ContentsView.h"
#include <tuple>
#include "UIDefines.h"
#include <boost/format.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	namespace
	{
		rsc::ContentsDataPtr g_nullContentsData(new rsc::ContentsData);
	}

	struct ContentsViewCallback_Null : IContentsViewCallback
	{

	};

#ifdef _DEBUG
	void ContentsView::AssertValid() const
	{
		CView::AssertValid();
	}

#ifndef _WIN32_WCE
	void ContentsView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}
#endif
#endif //_DEBUG

	IMPLEMENT_DYNCREATE(ContentsView, CView)

	BEGIN_MESSAGE_MAP(ContentsView, CView)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_MESSAGE(WMU_ADD_CONTENTS_VIEWLIST, &ContentsView::OnAddContents)
		ON_MESSAGE(WMU_CLEAR_CONTENTS_VIEWLIST, &ContentsView::OnClearContents)
		ON_MESSAGE(WMU_UPDATE_CONTENTS_VIEWLIST, &ContentsView::OnUpdateContents)
	END_MESSAGE_MAP()

	ContentsView::ContentsView() : m_callback(new ContentsViewCallback_Null)
	{
	}

	ContentsView::~ContentsView()
	{

	}

	void ContentsView::AddContents(const std::vector<rsc::ContentsDataPtr>& contentsDataList)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		std::copy(contentsDataList.begin(), contentsDataList.end(), std::back_inserter(m_contentsDataAddQ));

		PostMessage(WMU_ADD_CONTENTS_VIEWLIST);
	}

	const rsc::ContentsDataPtr& ContentsView::GetContents(const long row) const
	{
		if (row < 0 || long(m_contentsDataList.size()) <= row)
			return g_nullContentsData;

		return m_contentsDataList[row];
	}

	void ContentsView::ClearContents()
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		PostMessage(WMU_CLEAR_CONTENTS_VIEWLIST);
	}

	void ContentsView::UpdateContents(const std::vector<rsc::ContentsDataPtr>& contentsDataList)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		std::copy(contentsDataList.begin(), contentsDataList.end(), std::back_inserter(m_contentsDataUpdateQ));

		PostMessage(WMU_UPDATE_CONTENTS_VIEWLIST);
	}

	void ContentsView::SetToolbarCountryName(const std::wstring& wstrCountryName)
	{
		std::wstring wstrTopic = 
			boost::io::str(boost::wformat(L"All Contents In %s") % wstrCountryName);
		m_toolbar.SetTopicText(wstrTopic);

		m_wstrCountryName = wstrCountryName;
	}

	void ContentsView::OnDraw(CDC* pDC)
	{
	}

	int ContentsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CView::OnCreate(lpCreateStruct) == -1)
			return -1;
		
		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		RECT rect;
		GetClientRect(&rect);

		// 컨텐츠 리스트 생성.
		m_contentsListCtrl = std::make_unique<ContentsViewList>(*this);
		// 콜백 시그널은 CMainFrame::SetViews 함수에서 설정. 생성 순서 문제 때문에.

		// 툴바 생성.
		m_toolbar.Create(IDD_CONTENTSVIEW_TOOLBAR, this);

		CenterWindow();

		return 0;
	}

	void ContentsView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);

		RECT rect;
		GetClientRect(&rect);

		const LONG cy_toolbar = 80;
		int size = 0;

		// 툴바.
		RECT rectToolbar;
		rectToolbar.top = rect.top;		rectToolbar.bottom = cy_toolbar;
		rectToolbar.left = rect.left;	rectToolbar.right = rect.right;
		m_toolbar.MoveWindow(&rectToolbar);

		// 컨텐츠 리스트.
		RECT rectListCtrl;
		rectListCtrl.top = cy_toolbar;	rectListCtrl.bottom = rect.bottom;
		rectListCtrl.left = rect.left;	rectListCtrl.right = rect.right;
		m_contentsListCtrl->AdjustLayout(rectListCtrl);
	}

	BOOL ContentsView::OnEraseBkgnd(CDC* pDC)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	LRESULT ContentsView::OnAddContents(WPARAM wparam, LPARAM lparam)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		std::copy(m_contentsDataAddQ.begin(), m_contentsDataAddQ.end(), std::back_inserter(m_contentsDataList));
		m_contentsListCtrl->AddContents(m_contentsDataAddQ);

		m_contentsDataAddQ.clear();

		return 0;
	}

	LRESULT ContentsView::OnClearContents(WPARAM wparam, LPARAM lparam)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		m_contentsDataList.clear();
		m_contentsListCtrl->ClearContents();

		m_contentsDataList.clear();

		return 0;
	}

	LRESULT ContentsView::OnUpdateContents(WPARAM wparam, LPARAM lparam)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		struct fn : std::binary_function<rsc::ContentsDataPtr, rsc::ContentsDataPtr, bool>
		{
			bool operator()(const rsc::ContentsDataPtr& lhs, const rsc::ContentsDataPtr& rhs) const
			{
				return lhs.get() == rhs.get();
			}
		};

		CUGCell cell;
		CString str;
		std::wstring wstr;

		for (auto &currContentsData : m_contentsDataUpdateQ)
		{
			auto it = std::find_if(m_contentsDataList.begin(), m_contentsDataList.end(), std::bind2nd(fn(), currContentsData));
			if (it == m_contentsDataList.end())
				return TRUE;

			const size_t row = std::distance(m_contentsDataList.begin(), it);
			m_contentsListCtrl->UpdateContents(row, currContentsData);
		}

		m_contentsDataUpdateQ.clear();

		return 0;
	}
}