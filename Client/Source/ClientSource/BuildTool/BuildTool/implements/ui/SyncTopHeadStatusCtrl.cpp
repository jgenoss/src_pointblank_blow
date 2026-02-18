#include "stdafx.h"
#include "SyncTopHeadStatusCtrl.h"
#include "SyncPlayerBar.h"
#include "UIDefines.h"

namespace ui
{
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	const int g_totalNumOfCol = 1;
	const int g_totalNumOfRow = 3;

	BEGIN_MESSAGE_MAP(SyncTopHeadStatusCtrl, CUGCtrl)
	END_MESSAGE_MAP()

	SyncTopHeadStatusCtrl::SyncTopHeadStatusCtrl(SyncPlayerBar* owner) : m_owner(owner)
	{}

	SyncTopHeadStatusCtrl::~SyncTopHeadStatusCtrl()
	{}

	void SyncTopHeadStatusCtrl::OnSetup()
	{
		AddFont(L"Arial", -16, 500);	// 0, current sync contents name.
		AddFont(L"Arial", -12, 500);	// 1, country name.

		SetDefFont(GetFont(0));

		m_penIvoryBlackColor.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);

		SetNumberCols(g_totalNumOfCol);
		SetNumberRows(g_totalNumOfRow);
		SetDoubleBufferMode(TRUE);
		SetVS_Width(0);
		SetHS_Height(0);
		SetTH_Height(0);
		SetSH_Width(0);

		CUGCell cell;

		for (int col = 0; col < g_totalNumOfCol; col++)
		{
			for (int row = 0; row < g_totalNumOfRow; row++)
			{
				GetCell(col, row, &cell);
				cell.SetBackColor(COLOR_IVORY_BLACK);
				cell.SetTextColor(COLOR_SILVER);
				cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
				cell.SetBorderColor(&m_penIvoryBlackColor);
				cell.SetTextColor(COLOR_SILVER);
				cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
				cell.SetReadOnly(TRUE);
				SetCell(col, row, &cell);
			}
		}

		SetRowHeight(0, 15);
		SetRowHeight(1, 15);
		JoinCells(0, 0, 0, 1);

		GetCell(0, 0, &cell);
		cell.SetFont(GetFont(0));
		SetCell(0, 0, &cell);

		GetCell(0, 2, &cell);
		cell.SetFont(GetFont(1));
		SetCell(0, 2, &cell);
	}

	COLORREF SyncTopHeadStatusCtrl::OnGetDefBackColor(int section)
	{
		return COLOR_IVORY_BLACK;
	}

	void SyncTopHeadStatusCtrl::OnDrawFocusRect(CDC *dc, RECT *rect)
	{}

	void SyncTopHeadStatusCtrl::AdjustLayout(const RECT& rt)
	{
		MoveWindow(&rt);

		const int cx = rt.right - rt.left;
		SetColWidth(0, cx);
	}

	void SyncTopHeadStatusCtrl::SetContentsName(const std::string& strContentsName)
	{
		std::wstring wstrContentsName(strContentsName.begin(), strContentsName.end());
		QuickSetText(0, 0, wstrContentsName.c_str());
		RedrawCell(0, 0);
	}

	void SyncTopHeadStatusCtrl::SetCountryName(const std::string& strCountryName)
	{
		std::wstring wstrCountryName(strCountryName.begin(), strCountryName.end());
		QuickSetText(0, 2, wstrCountryName.c_str());
		RedrawCell(0, 2);
	}
}