#include "stdafx.h"
#include "SyncTopHeadMenuCtrl.h"
#include "SyncPlayerBar.h"
#include "UIDefines.h"

namespace ui
{
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	const int g_totalNumOfCol = 2;
	const int g_totalNumOfRow = 2;

#define CELL_PARAM_CHECKBOX_FORCECOPY 1001

	BEGIN_MESSAGE_MAP(SyncTopHeadMenuCtrl, CUGCtrl)
	END_MESSAGE_MAP()

	SyncTopHeadMenuCtrl::SyncTopHeadMenuCtrl(SyncPlayerBar* owner) : m_owner(owner)
	{}

	SyncTopHeadMenuCtrl::~SyncTopHeadMenuCtrl()
	{}

	void SyncTopHeadMenuCtrl::OnSetup()
	{
		AddFont(L"Arial", -12, 500);

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
				cell.SetBackColor(COLOR_BLACK);
				cell.SetTextColor(COLOR_SILVER);
				cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
				cell.SetBorderColor(&m_penIvoryBlackColor);
				cell.SetTextColor(COLOR_SILVER);
				cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
				cell.SetReadOnly(TRUE);
				SetCell(col, row, &cell);
			}
		}

		for (int row = 0; row < g_totalNumOfRow; row++)
			SetRowHeight(row, 15);

		JoinCells(0, 0, 0, 1);
		JoinCells(1, 0, 1, 1);
		
		GetCell(0, 0, &cell);
		cell.SetText(L"Force Copy");
		cell.SetTextColor(COLOR_SKYBLUE);
		SetCell(0, 0, &cell);

		GetCell(1, 0, &cell);
		cell.SetText(L"Play All");
		cell.SetTextColor(COLOR_SKYBLUE);
		SetCell(1, 0, &cell);
	}

	void SyncTopHeadMenuCtrl::OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed)
	{
		if (updn == 1)	// button down.
			return;

		if (col == 0 && row == 0)
		{
			/*m_isForceCopyMode = !m_isForceCopyMode;
			const COLORREF color = m_isForceCopyMode ? COLOR_SKYBLUE : COLOR_SILVER;
			QuickSetTextColor(0, 0, color);
			RedrawCell(0, 0);

			m_owner->OnClickedForceCopyMode();*/
		}
		else if (col == 1 && row == 0)
		{
			m_isPlayAllMode = !m_isPlayAllMode;

			const COLORREF color = m_isPlayAllMode ? COLOR_SKYBLUE : COLOR_SILVER;
			
			QuickSetTextColor(1, 0, color);
			RedrawCell(1, 0);

			m_owner->OnTHMenuPlayAllClicked();
		}
	}

	COLORREF SyncTopHeadMenuCtrl::OnGetDefBackColor(int section)
	{
		return COLOR_BLACK;
	}

	void SyncTopHeadMenuCtrl::OnDrawFocusRect(CDC *dc, RECT *rect)
	{}

	int SyncTopHeadMenuCtrl::GetTotalNumOfCol()
	{
		return g_totalNumOfCol;
	}

	void SyncTopHeadMenuCtrl::AdjustLayout(const RECT& rt)
	{
		MoveWindow(&rt);

		const int cx = rt.right - rt.left;

		int cx_col = cx / 2;
		int cx_col_accum = 0;

		SetColWidth(0, cx_col);
		cx_col_accum += cx_col;

		SetColWidth(1, cx - cx_col_accum);
	}

	bool SyncTopHeadMenuCtrl::IsForceCopyMode() const
	{
		return m_isForceCopyMode;
	}

	bool SyncTopHeadMenuCtrl::IsPlayAllMode() const
	{
		return m_isPlayAllMode;
	}
}