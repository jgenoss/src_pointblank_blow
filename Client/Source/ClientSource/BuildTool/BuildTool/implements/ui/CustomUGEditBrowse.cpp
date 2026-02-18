#include "stdafx.h"
#include "CustomUGEditBrowse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	CustomUGEditBrowseCtrl::CustomUGEditBrowseCtrl() : m_callback(new CustomUGEditBrowseCallback)
	{
	}

	CustomUGEditBrowseCtrl::~CustomUGEditBrowseCtrl()
	{
	}

	void CustomUGEditBrowseCtrl::AdjustLayout(const RECT& rt)
	{	
		MoveWindow(&rt);

		const int cx = rt.right - rt.left;
		SetColWidth(0, cx);
	}

	void CustomUGEditBrowseCtrl::OnSetup()
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

		m_penIvoryBlack.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);
		m_penIvoryBlackLight.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);

		CUGCell cell;
		GetHeadingDefault(&cell);
		SetFont(GetFont(0));
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetBackColor(COLOR_BLACK);
		SetTextColor(COLOR_TAN3_PERU);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlack);
		SetHeadingDefault(&cell);

		SetNumberRows(1);
		AddCellType(&m_ellipType);

		GetCell(0, 0, &cell);
		cell.SetCellType(m_ellipType.GetID());
		cell.SetCellTypeEx(UGCT_NORMALMULTILINE);
		cell.SetBackColor(m_backColor);
		cell.SetTextColor(m_textColor);
		cell.SetAlignment(UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlackLight);
		SetCell(0, 0, &cell);
	}

	int	CustomUGEditBrowseCtrl::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
	{
		if (ID == m_ellipType.GetID() && msg == UGCT_ELLIPSISBUTTONCLICK)
		{
			CFolderPickerDialog dlg(m_wstrExplain.c_str(), 0, NULL, 0);
			if (IDOK == dlg.DoModal())
			{
				std::wstring wstrPath(dlg.GetPathName().GetString());
				QuickSetText(0, 0, wstrPath.c_str());
				m_callback->OnSelectOK(wstrPath);
				return TRUE;
			}
		}

		return FALSE;
	}
}