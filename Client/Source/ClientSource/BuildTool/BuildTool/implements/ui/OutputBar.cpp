#include "stdafx.h"
#include "OutputBar.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	const int nBorderSize = 1;

	/////////////////////////////////////////////////////////////////////////////
	// COutputBar

	BEGIN_MESSAGE_MAP(OutputBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_CTLCOLOR_REFLECT()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// COutputBar construction/destruction

	OutputBar::OutputBar()
	{
		// TODO: add one-time construction code here

	}

	OutputBar::~OutputBar()
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	// COutputBar message handlers

	int OutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		CRect rectDummy;
		rectDummy.SetRectEmpty();

		// Create tabs window:
		if (!m_tabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
		{
			TRACE0("Failed to create output tab window\n");
			return -1;      // fail to create
		}

		m_tabs.SetFlatFrame(FALSE, FALSE);

		// Create list windows.
		// TODO: create your own window here:
		const DWORD dwViewStyle =
			LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_REPORT;

		if (!m_outputList.Create(dwViewStyle, rectDummy, &m_tabs, 2))
		{
			TRACE0("Failed to create output view\n");
			return -1;      // fail to create
		}

		LV_COLUMN col;
		col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.iSubItem = 0;
		col.cx = rectDummy.Width();
		col.pszText = L"Log";
		m_outputList.InsertColumn(0, &col);
		
		// Attach list windows to tab:
		m_tabs.AddTab(&m_outputList, _T("Output"), (UINT)-1);

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		return 0;
	}

	void OutputBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		// Tab control should cover a whole client area:
		m_tabs.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

		m_outputList.SetColumnWidth(0, cx);
	}

	void OutputBar::Log(const std::string& message)
	{
		const int itemCount = m_outputList.GetItemCount();

		std::wstring wMessage(L"1> ");
		wMessage.append(message.begin(), message.end());
		m_outputList.InsertItem(itemCount, wMessage.c_str());

		CRect rt;
		m_outputList.GetItemRect(0, rt, LVIR_BOUNDS);

		CSize size(0, rt.Height());

		m_outputList.Scroll(size);

		m_outputList.UpdateWindow();
	}

	void OutputBar::ClearLog()
	{
		m_outputList.DeleteAllItems();
	}

	HBRUSH OutputBar::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		UNREFERENCED_PARAMETER(nCtlColor);

		pDC->SetBkColor(COLOR_IVORY_BLACK);

		return m_bkGndBrush;
	}

	BOOL OutputBar::OnEraseBkgnd(CDC* pDC)
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillRect(&rect, &m_bkGndBrush);

		return 1;
	}
}
