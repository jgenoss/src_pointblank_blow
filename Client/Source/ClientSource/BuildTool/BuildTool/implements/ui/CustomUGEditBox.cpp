#include "stdafx.h"
#include "CustomUGEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	CustomUGEditBox::CustomUGEditBox() : m_callback(new CustomUGEditBoxCallback)
	{
	}

	CustomUGEditBox::~CustomUGEditBox()
	{
	}

	void CustomUGEditBox::SetText(const std::string& strText)
	{
		if (strText.empty())
		{
			QuickSetText(0, 0, L"");
			RedrawCell(0, 0);
			return;
		}

		std::wstring wstr;
		i3::mb_to_utf16(strText, wstr);
		QuickSetText(0, 0, wstr.c_str());
		RedrawCell(0, 0);
	}

	void CustomUGEditBox::SetText(const std::wstring& wstrText)
	{
		QuickSetText(0, 0, wstrText.c_str());
		RedrawCell(0, 0);
	}

	void CustomUGEditBox::AdjustLayout(const RECT& rt)
	{
		MoveWindow(&rt);

		const int cx = rt.right - rt.left - 17;
		SetColWidth(0, cx);

		const int cy = rt.bottom - rt.top;
		SetRowHeight(0, cy);
	}

	void CustomUGEditBox::OnSetup()
	{
		AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
		AddFont(L"Arial", -12, 500);		// 1, cell

		SetNumberCols(1);
		SetSH_Width(0);
		SetTH_Height(0);

		SetDoubleBufferMode(TRUE);
		SetUserSizingMode(2);
		SetVScrollMode(UG_SCROLLTRACKING);
		SetMultiSelectMode(2);

		SetDefFont(GetFont(1));

		m_penBorder.CreatePen(PS_SOLID, 1, m_borderColor);

		SetNumberRows(1);

		CUGCell cell;
		GetCell(0, 0, &cell);
		cell.SetCellTypeEx(UGCT_NORMALMULTILINE);
		cell.SetBackColor(m_backColor);
		cell.SetTextColor(m_textColor);
		cell.SetAlignment(m_alignment);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penBorder);
		SetCell(0, 0, &cell);
	}
}