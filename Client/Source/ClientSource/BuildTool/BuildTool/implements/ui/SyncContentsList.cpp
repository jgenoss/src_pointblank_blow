#include "stdafx.h"
#include "SyncContentsList.h"
#include <boost/lexical_cast.hpp>
#include "resource/DragdropDataContext.h"
#include <algorithm>
#include "UIDefines.h"
#include "SyncPlayerDlg.h"
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	enum LIST_COL_TYPE
	{
		LIST_COL_CONTENTS_ID,
		LIST_COL_CONTENTS_NAME,
		LIST_COL_CONTENTS_STATUS,
		LIST_COL_MAX_COUNT
	};

	struct SyncContentsListCallback : CustomUGCtrlCallback
	{
		SyncContentsList& owner;
		std::function<void(const std::vector<long>&)> funcRowDeleted;

		SyncContentsListCallback(SyncContentsList& own, const std::function<void(const std::vector<long>&)>& f) 
			: owner(own), funcRowDeleted(f)
		{}

		virtual ~SyncContentsListCallback()
		{}

		virtual void		OnSetup(CUGCtrl* ctrl) override
		{
			ctrl->AddFont(L"Arial", -12, 500);
			ctrl->SetDefFont(ctrl->GetFont(0));

			owner.m_penIvoryBlackLight.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);
			owner.m_penOrange.CreatePen(PS_SOLID, 1, COLOR_ORANGE);

			ctrl->SetNumberCols(LIST_COL_MAX_COUNT);
			ctrl->SetNumberRows(0);
			ctrl->EnableMenu(TRUE);
			ctrl->SetDoubleBufferMode(TRUE);
			ctrl->SetUserSizingMode(FALSE);
			ctrl->SetVScrollMode(UG_SCROLLTRACKING);
			ctrl->SetMultiSelectMode(2);

			ctrl->SetSH_Width(0);
			ctrl->SetTH_Height(0);

			CUGCell cell;
			ctrl->GetGridDefault(&cell);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			ctrl->SetGridDefault(&cell);
		}

		virtual void		OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum) override
		{
			if (blockNum == 1)
			{
				CUGCell cell;

				if (owner.m_currSelectionRow != -1)
				{
					for (int i = LIST_COL_CONTENTS_ID; i < LIST_COL_MAX_COUNT; i++)
					{
						ctrl->GetCell(i, owner.m_currSelectionRow, &cell);
						cell.SetBackColor(COLOR_IVORY_BLACK);
						cell.SetTextColor(COLOR_WHITE);
						cell.SetBorderColor(&owner.m_penIvoryBlackLight);
						ctrl->SetCell(i, owner.m_currSelectionRow, &cell);
					}
					ctrl->RedrawRow(owner.m_currSelectionRow);
				}

				for (int i = LIST_COL_CONTENTS_ID; i < LIST_COL_MAX_COUNT; i++)
				{
					ctrl->GetCell(i, endRow, &cell);
					if (i == endCol)
					{
						cell.SetTextColor(COLOR_WHITE);
						cell.SetBorderColor(&owner.m_penOrange);
					}
					else
					{
						cell.SetBackColor(COLOR_ORANGE);
						cell.SetTextColor(COLOR_IVORY_BLACK);
						cell.SetBorderColor(&owner.m_penOrange);
					}
					ctrl->SetCell(i, endRow, &cell);
				}

				ctrl->RedrawRow(endRow);
			}

			owner.m_currSelectionRow = endRow;
		}

		virtual void		OnMenuCommand(CUGCtrl* ctrl, int col, long row, int section, int item) override
		{
			switch (item)
			{
			case ID_MENU_DELETE_SYNCLIST:
				funcRowDeleted({ row });
				break;
			}
		}

		virtual int			OnMenuStart(CUGCtrl* ctrl, int col, long row, int section) override
		{
			ctrl->EmptyMenu();

			if (section == UG_GRID)
			{
				ctrl->AddMenuItem(ID_MENU_DELETE_SYNCLIST, L"Delete");
			}

			return TRUE;
		}
	};

	SyncContentsList::SyncContentsList(SyncPlayerDlg& owner)
	{
		std::unique_ptr<SyncContentsListCallback> cb =
			std::make_unique<SyncContentsListCallback>(*this, std::bind(&SyncPlayerDlg::DelContents, &owner, std::placeholders::_1));
		m_ctrl.SetCallback(std::move(cb));

		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), &owner, ID_SYNCPLAYER_CONTENTSLIST_CTRL);
	}

	SyncContentsList::~SyncContentsList()
	{
	}

	void SyncContentsList::AdjustLayout(const RECT& rt)
	{
		m_ctrl.MoveWindow(&rt);

		int cx = rt.right - rt.left;
		cx -= 17;	// 종 스크롤바 사이즈 만큼 빼줘야 횡 스크롤바가 나타나지 않는다.

		const int cx_col_id = static_cast<int>(cx * 0.15f);
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_ID, cx_col_id);

		const int cx_col_name = static_cast<int>(cx * 0.80f);
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_NAME, cx_col_name);

		const int cx_col_check = cx - (cx_col_id + cx_col_name);
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_STATUS, cx_col_check);
	}

	void SyncContentsList::SetContentsID(const long row, const std::wstring& wstrID)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;
		m_ctrl.GetCell(LIST_COL_CONTENTS_ID, row, &cell);
		cell.SetText(wstrID.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_ID, row, &cell);
	}

	void SyncContentsList::SetContentsName(const long row, const std::wstring& wstrName)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;
		m_ctrl.GetCell(LIST_COL_CONTENTS_NAME, row, &cell);
		cell.SetText(wstrName.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_NAME, row, &cell);
	}

	void SyncContentsList::SetContentsStatus(const long row, const std::wstring& wstrStatus)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;
		m_ctrl.GetCell(LIST_COL_CONTENTS_STATUS, row, &cell);
		cell.SetText(wstrStatus.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_STATUS, row, &cell);
	}

	void SyncContentsList::AddContents(const std::vector<rsc::ContentsSyncDataPtr>& dataList)
	{
		const long prevTotalNumRows = m_ctrl.GetNumberRows();
		const size_t addedSize = dataList.size();

		m_ctrl.SetNumberRows(prevTotalNumRows + addedSize, FALSE);

		long row = prevTotalNumRows;
		std::wstring wstr;
		for (auto &data : dataList)
		{
			SetContentsID(row, std::to_wstring(row));

			i3::mb_to_utf16(data->contents->strContentsName, wstr);
			SetContentsName(row, wstr);

			SetContentsStatus(row, rsc::ConvertContentsStatusToString(data->contents->status));

			row++;
		}

		m_ctrl.RedrawAll();
	}

	void SyncContentsList::DelContents(const long row)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		m_ctrl.DeleteRow(row);

		// 삭제된 줄 아래 줄 부터 ID 재입력.
		const long totalNumRows = m_ctrl.GetNumberRows();
		if (totalNumRows > 1)
		{
			for (long currRow = row; currRow < m_ctrl.GetNumberRows(); currRow++)
			{
				m_ctrl.QuickSetText(LIST_COL_CONTENTS_ID, currRow, std::to_wstring(currRow).c_str());
				m_ctrl.RedrawCell(LIST_COL_CONTENTS_ID, currRow);
			}
		}
	}

	void SyncContentsList::ClearContents()
	{
		m_ctrl.SetNumberRows(0, TRUE);
	}

	void SyncContentsList::SetSyncHighlight(const long row, const bool ok)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		for (int col = 0; col < LIST_COL_MAX_COUNT; col++)
		{
			if (ok)
				m_ctrl.QuickSetTextColor(col, row, COLOR_SKYBLUE);
			else
			{
				if (m_ctrl.GetCurrentRow() == row && m_ctrl.GetCurrentCol() != col)
					m_ctrl.QuickSetTextColor(col, row, COLOR_BLACK);
				else
					m_ctrl.QuickSetTextColor(col, row, COLOR_WHITE);
			}
		}

		m_ctrl.RedrawRow(row);
	}
}