#include "stdafx.h"
#include "ContentsViewList.h"
#include "resource/DragdropDataContext.h"
#include "UIDefines.h"
#include "ContentsView.h"

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
		LIST_COL_CONTENTS_CATEGORY,
		LIST_COL_CONTENTS_SUBCATEGORY,
		LIST_COL_CONTENTS_STATUS,
		LIST_COL_MAX_COUNT
	};

	struct ContentsViewListCtrlCallback : CustomUGCtrlCallback
	{
		ContentsViewList& owner;
		ContentsView& view;

		ContentsViewListCtrlCallback(ContentsViewList& own, ContentsView& v) : owner(own), view(v)
		{
		}

		virtual ~ContentsViewListCtrlCallback()
		{
		}

		virtual void		OnSetup(CUGCtrl* ctrl) override
		{
			ctrl->AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
			ctrl->AddFont(L"Arial", -12, 500);		// 1, cell

			ctrl->SetNumberCols(LIST_COL_MAX_COUNT);
			ctrl->SetNumberRows(0);
			ctrl->EnableMenu(TRUE);
			ctrl->SetDoubleBufferMode(TRUE);
			ctrl->SetUserSizingMode(2);
			ctrl->SetVScrollMode(UG_SCROLLTRACKING);
			ctrl->SetMultiSelectMode(2);
			//ctrl->DragDropTarget(TRUE);

			ctrl->SetDefFont(ctrl->GetFont(1));

			CUGCell cell;
			ctrl->GetHeadingDefault(&cell);
			cell.SetFont(ctrl->GetFont(0));
			ctrl->SetHeadingDefault(&cell);

			ctrl->SetSH_Width(0);	// side heading 안보이도록. 0번 cell 이 side heading 역할을 한다.

			owner.m_penIvoryBlackLight.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);
			owner.m_penGray.CreatePen(PS_SOLID, 1, COLOR_GRAY);
			owner.m_penOrange.CreatePen(PS_SOLID, 1, COLOR_ORANGE);
			owner.m_brushIvoryBlack.CreateSolidBrush(GetSysColor(COLOR_IVORY_BLACK));

			ctrl->GetCell(LIST_COL_CONTENTS_ID, -1, &cell);
			cell.SetText(L"ID");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penGray);
			ctrl->SetCell(LIST_COL_CONTENTS_ID, -1, &cell);

			ctrl->GetCell(LIST_COL_CONTENTS_NAME, -1, &cell);
			cell.SetText(L"Name");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penGray);
			ctrl->SetCell(LIST_COL_CONTENTS_NAME, -1, &cell);

			ctrl->GetCell(LIST_COL_CONTENTS_CATEGORY, -1, &cell);
			cell.SetText(L"Category");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penGray);
			ctrl->SetCell(LIST_COL_CONTENTS_CATEGORY, -1, &cell);

			ctrl->GetCell(LIST_COL_CONTENTS_SUBCATEGORY, -1, &cell);
			cell.SetText(L"SubCategory");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penGray);
			ctrl->SetCell(LIST_COL_CONTENTS_SUBCATEGORY, -1, &cell);

			ctrl->GetCell(LIST_COL_CONTENTS_STATUS, -1, &cell);
			cell.SetText(L"Status");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penGray);
			ctrl->SetCell(LIST_COL_CONTENTS_STATUS, -1, &cell);
		}

		virtual void		OnDClicked(CUGCtrl* ctrl, int col, long row, RECT* rt, POINT* pt, BOOL processed)
		{
			view.GetCallback()->OnAddContents(row);
		}

		virtual void		OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum) override
		{
			if (blockNum == 1)
			{
				view.GetCallback()->OnContentsClick(endRow);

				// selection.
				CUGCell cell;
				if (owner.m_currSelectionRow != -1)
				{
					for (int i = LIST_COL_CONTENTS_ID; i < LIST_COL_MAX_COUNT; i++)
					{
						ctrl->GetCell(i, owner.m_currSelectionRow, &cell);
						cell.SetBackColor(COLOR_IVORY_BLACK);
						cell.SetTextColor(COLOR_SILVER);
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
						cell.SetTextColor(COLOR_SILVER);
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

				owner.m_currSelectionRow = endRow;
			}
		}

		virtual void		OnMenuCommand(CUGCtrl* ctrl, int col, long row, int section, int item) override
		{
			switch (item)
			{
			case ID_MENU_ADD_CONTENTS_TO_SYNCLIST:
				view.GetCallback()->OnAddContents(row);
				break;
			}
		}

		virtual int			OnMenuStart(CUGCtrl* ctrl, int col, long row, int section) override
		{
			ctrl->EmptyMenu();

			if (section == UG_GRID)
			{
				ctrl->AddMenuItem(ID_MENU_ADD_CONTENTS_TO_SYNCLIST, L"-> SyncPlayer");
			}

			return TRUE;
		}
	};

	ContentsViewList::ContentsViewList(ContentsView& owner)
	{
		m_ctrl.SetCallback(std::make_unique<ContentsViewListCtrlCallback>(*this, owner));
		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), &owner, ID_CONTENTSVIEW_LIST);
	}

	ContentsViewList::~ContentsViewList()
	{
	}

	void ContentsViewList::AdjustLayout(const RECT& rt)
	{
		m_ctrl.MoveWindow(&rt);

		int cx = rt.right - rt.left;
		cx -= 17;	// 종 스크롤바 사이즈 만큼 빼줘야 횡 스크롤바가 나타나지 않는다.                  

		int accumSize = 0;

		int size = static_cast<int>(cx * 0.08f);// ID
		accumSize += size;
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_ID, size);

		size = static_cast<int>(cx * 0.50f);	// Name
		accumSize += size;
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_NAME, size);

		size = static_cast<int>(cx * 0.18f);	// Category
		accumSize += size;
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_CATEGORY, size);

		size = static_cast<int>(cx * 0.18f);	// Sub Category
		accumSize += size;
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_SUBCATEGORY, size);

		size = cx - accumSize;					// Status
		m_ctrl.SetColWidth(LIST_COL_CONTENTS_STATUS, size);
	}

	void ContentsViewList::SetContentsID(const long row, const std::wstring& wstrID)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;

		m_ctrl.GetCell(LIST_COL_CONTENTS_ID, row, &cell);
		cell.SetText(wstrID.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)	// 셀렉션 상태.
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_ID, row, &cell);
	}

	std::wstring g_null_wstr;

	const std::wstring ContentsViewList::GetContentsID(const long row)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return g_null_wstr;

		CString str;
		m_ctrl.QuickGetText(LIST_COL_CONTENTS_ID, row, &str);

		return str.GetString();
	}

	void ContentsViewList::SetContentsName(const long row, const std::wstring& wstrName)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;

		m_ctrl.GetCell(LIST_COL_CONTENTS_NAME, row, &cell);
		cell.SetText(wstrName.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)	// 셀렉션 상태.
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_NAME, row, &cell);
	}

	const std::wstring ContentsViewList::GetContentsName(const long row)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return g_null_wstr;

		CString str;
		m_ctrl.QuickGetText(LIST_COL_CONTENTS_NAME, row, &str);

		return str.GetString();
	}

	void ContentsViewList::SetContentsCategory(const long row, const std::wstring& wstrCategory)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;

		m_ctrl.GetCell(LIST_COL_CONTENTS_CATEGORY, row, &cell);
		cell.SetText(wstrCategory.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)	// 셀렉션 상태.
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_CATEGORY, row, &cell);
	}

	const std::wstring ContentsViewList::GetContentsCategory(const long row)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return g_null_wstr;

		CString str;
		m_ctrl.QuickGetText(LIST_COL_CONTENTS_CATEGORY, row, &str);

		return str.GetString();
	}

	void ContentsViewList::SetContentsSubCategory(const long row, const std::wstring& wstrSubCategory)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;

		m_ctrl.GetCell(LIST_COL_CONTENTS_SUBCATEGORY, row, &cell);
		cell.SetText(wstrSubCategory.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)	// 셀렉션 상태.
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_SUBCATEGORY, row, &cell);
	}

	const std::wstring ContentsViewList::GetContentsSubCategory(const long row)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return g_null_wstr;

		CString str;
		m_ctrl.QuickGetText(LIST_COL_CONTENTS_SUBCATEGORY, row, &str);

		return str.GetString();
	}

	void ContentsViewList::SetContentsStatus(const long row, const std::wstring& wstrStatus)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		CUGCell cell;

		m_ctrl.GetCell(LIST_COL_CONTENTS_STATUS, row, &cell);
		cell.SetText(wstrStatus.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		if (m_currSelectionRow == row)	// 셀렉션 상태.
		{
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_penOrange);
		}
		else
		{
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetBorderColor(&m_penIvoryBlackLight);
		}
		m_ctrl.SetCell(LIST_COL_CONTENTS_STATUS, row, &cell);
	}

	const std::wstring ContentsViewList::GetContentsStatus(const long row)
	{
		CString str;
		m_ctrl.QuickGetText(LIST_COL_CONTENTS_STATUS, row, &str);

		return str.GetString();
	}

	void ContentsViewList::AddContents(const std::vector<rsc::ContentsDataPtr>& contentsDataList)
	{
		const long prevTotalNumRows = m_ctrl.GetNumberRows();
		const size_t addedSize = contentsDataList.size();

		m_ctrl.SetNumberRows(prevTotalNumRows + addedSize, FALSE);

		long row = prevTotalNumRows;
		std::wstring wstr;
		for (auto &currContentsData : contentsDataList)
		{
			SetContentsID(row, std::to_wstring(row));

			i3::mb_to_utf16(currContentsData->strContentsName, wstr);
			SetContentsName(row, wstr);

			i3::mb_to_utf16(currContentsData->strCategoryName, wstr);
			SetContentsCategory(row, wstr);

			i3::mb_to_utf16(currContentsData->strSubcategoryName, wstr);
			SetContentsSubCategory(row, wstr);

			SetContentsStatus(row, rsc::ConvertContentsStatusToString(currContentsData->status));

			row++;
		}

		m_ctrl.RedrawAll();	// 속도 때문에 매번 그리는 것보다 한 번에 그리는 편이 훨씬 낫다.
	}

	void ContentsViewList::ClearContents()
	{
		m_ctrl.SetNumberRows(0, TRUE);
	}

	void ContentsViewList::UpdateContents(const long row, const rsc::ContentsDataPtr contentsData)
	{
		CUGCell cell;
		CString str;
		std::wstring wstr;

		if ((long)row >= m_ctrl.GetNumberRows())
			return;

		if (std::to_wstring(row) != GetContentsID(row))
			SetContentsID(row, wstr);

		i3::mb_to_utf16(contentsData->strContentsName, wstr);
		if (wstr != GetContentsName(row))
			SetContentsName(row, wstr);

		i3::mb_to_utf16(contentsData->strCategoryName, wstr);
		if (wstr != GetContentsCategory(row))
			SetContentsCategory(row, wstr);

		i3::mb_to_utf16(contentsData->strSubcategoryName, wstr);
		if (wstr != GetContentsSubCategory(row))
			SetContentsSubCategory(row, wstr);

		const std::wstring wstrStatus(rsc::ConvertContentsStatusToString(contentsData->status));
		if (wstrStatus != GetContentsStatus(row))
			SetContentsStatus(row, wstrStatus);

		m_ctrl.RedrawRow(row);
	}
}