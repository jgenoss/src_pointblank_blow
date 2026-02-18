#include "stdafx.h"
#include "LoadingProgressTextCtrl.h"
#include "UIDefines.h"

namespace ui
{
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	BEGIN_MESSAGE_MAP(LoadingProgressTextCtrl, CUGCtrl)
	END_MESSAGE_MAP()

	LoadingProgressTextCtrl::LoadingProgressTextCtrl()
	{}

	LoadingProgressTextCtrl::~LoadingProgressTextCtrl()
	{}

	void LoadingProgressTextCtrl::OnSetup()
	{
		AddFont(L"Arial", -12, 500);
		SetDefFont(GetFont(0));

		m_cellBorderColorPen.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);
	
		SetNumberCols(1);
		SetNumberRows(1);

		SetDoubleBufferMode(TRUE);
		SetUserSizingMode(2);
		SetMultiSelectMode(FALSE);

		SetSH_Width(0);
		SetTH_Height(0);

		CUGCell cell;
		GetCell(0, 0, &cell);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_WHITE);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_cellBorderColorPen);
		SetCell(0, 0, &cell);
	}

	COLORREF LoadingProgressTextCtrl::OnGetDefBackColor(int section)
	{
		return COLOR_IVORY_BLACK;
	}

	void LoadingProgressTextCtrl::OnDrawFocusRect(CDC *dc, RECT *rect)
	{}

	void LoadingProgressTextCtrl::SetText(const std::wstring& wstrText)
	{
		QuickSetText(0, 0, wstrText.c_str());
		RedrawCell(0, 0);
	}

	void LoadingProgressTextCtrl::AdjustLayout(const RECT& rect)
	{
		MoveWindow(&rect);

		const int cx = rect.right - rect.left;
		SetColWidth(0, cx);

		const int cy = rect.bottom - rect.top;
		SetRowHeight(0, cy);
	}
}