#include "stdafx.h"
#include "ContentsDescriptionList.h"
#include "ContentsDescriptionBar.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	namespace
	{
		enum ROW_TYPE
		{
			ROW_CONTENTS_NAME,
			ROW_COUNTRY_NAME,
			ROW_CONTENTS_CATEGORY,
			ROW_CONTENTS_SUBCATEGORY,
			ROW_CONTENTS_STATUS,
			ROW_MAX_COUNT,
		};

		const int g_sideHeadingWidth = 120;
		const int g_cy_col = 25;
	}

	struct ContentsDescListCallback : CustomUGCtrlCallback
	{
		ContentsDescList& owner;

		ContentsDescListCallback(ContentsDescList& o) : owner(o) 
		{}

		virtual ~ContentsDescListCallback()
		{}

		virtual void		OnSetup(CUGCtrl* ctrl) override
		{
			ctrl->AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
			ctrl->AddFont(L"Arial", -12, 500);		// 1, cell

			ctrl->SetNumberCols(1);
			ctrl->SetNumberRows(ROW_MAX_COUNT);

			ctrl->SetTH_Height(0);
			ctrl->SetSH_Width(g_sideHeadingWidth);

			ctrl->SetDoubleBufferMode(TRUE);
			ctrl->SetUserSizingMode(2);
			ctrl->SetVScrollMode(UG_SCROLLTRACKING);
			ctrl->SetMultiSelectMode(FALSE);

			ctrl->SetDefFont(ctrl->GetFont(1));

			owner.m_penIvoryBlack.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);
			owner.m_penIvoryBlackLight.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);

			CUGCell cell;
			ctrl->GetHeadingDefault(&cell);
			cell.SetFont(ctrl->GetFont(0));
			ctrl->SetHeadingDefault(&cell);

			ctrl->GetCell(-1, ROW_CONTENTS_NAME, &cell);
			cell.SetText(L"Contents Name");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penIvoryBlack);
			ctrl->SetCell(-1, ROW_CONTENTS_NAME, &cell);

			ctrl->GetCell(-1, ROW_COUNTRY_NAME, &cell);
			cell.SetText(L"Country");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penIvoryBlack);
			ctrl->SetCell(-1, ROW_COUNTRY_NAME, &cell);

			ctrl->GetCell(-1, ROW_CONTENTS_CATEGORY, &cell);
			cell.SetText(L"Category");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penIvoryBlack);
			ctrl->SetCell(-1, ROW_CONTENTS_CATEGORY, &cell);

			ctrl->GetCell(-1, ROW_CONTENTS_SUBCATEGORY, &cell);
			cell.SetText(L"Subcategory");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penIvoryBlack);
			ctrl->SetCell(-1, ROW_CONTENTS_SUBCATEGORY, &cell);

			ctrl->GetCell(-1, ROW_CONTENTS_STATUS, &cell);
			cell.SetText(L"Status");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penIvoryBlack);
			ctrl->SetCell(-1, ROW_CONTENTS_STATUS, &cell);

			for (int row = 0; row < ROW_MAX_COUNT; row++)
			{
				ctrl->GetCell(0, row, &cell);
				cell.SetText(L"");
				cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
				cell.SetBackColor(COLOR_IVORY_BLACK);
				cell.SetTextColor(COLOR_WHITE);
				cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
				cell.SetBorderColor(&owner.m_penIvoryBlackLight);
				ctrl->SetCell(0, row, &cell);
			}
		}
	};

	ContentsDescList::ContentsDescList(ContentsDescriptionBar& owner)
	{
		m_ctrl.SetCallback(std::make_unique<ContentsDescListCallback>(*this));
		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), &owner, ID_CONTENTS_DESC_LIST_CTRL);
	}

	ContentsDescList::~ContentsDescList()
	{}

	void ContentsDescList::AdjustLayout(const int x, const int y, const int cx, const int cy)
	{
		const int cx_col = static_cast<int>(cx - g_sideHeadingWidth);

		for (int row = 0; row < ROW_MAX_COUNT; row++)
		{
			m_ctrl.SetRowHeight(row, g_cy_col);
			m_ctrl.SetColWidth(0, cx_col);
		}

		m_ctrl.MoveWindow(x, y, cx, cy);
	}

	void ContentsDescList::SetContentsName(const std::wstring& wstrContentsName, const bool redraw)
	{
		m_ctrl.QuickSetText(0, ROW_CONTENTS_NAME, wstrContentsName.c_str());
		
		if (redraw)
			m_ctrl.RedrawCell(0, ROW_CONTENTS_NAME);
	}

	void ContentsDescList::SetContentsName(const std::wstring& wstrContentsName)
	{
		SetContentsName(wstrContentsName, true);
	}

	void ContentsDescList::SetCountryName(const std::wstring& wstrCountryName, const bool redraw)
	{
		m_ctrl.QuickSetText(0, ROW_COUNTRY_NAME, wstrCountryName.c_str());

		if (redraw)
			m_ctrl.RedrawCell(0, ROW_COUNTRY_NAME);
	}

	void ContentsDescList::SetCountryName(const std::wstring& wstrCountryName)
	{
		SetCountryName(wstrCountryName, true);
	}

	void ContentsDescList::SetCategoryName(const std::wstring& wstrCategory, const bool redraw)
	{
		m_ctrl.QuickSetText(0, ROW_CONTENTS_CATEGORY, wstrCategory.c_str());

		if (redraw)
			m_ctrl.RedrawCell(0, ROW_CONTENTS_CATEGORY);
	}

	void ContentsDescList::SetCategoryName(const std::wstring& wstrCategory)
	{
		SetCategoryName(wstrCategory, true);
	}

	void ContentsDescList::SetSubCategoryName(const std::wstring& wstrSubCategory, const bool redraw)
	{
		m_ctrl.QuickSetText(0, ROW_CONTENTS_SUBCATEGORY, wstrSubCategory.c_str());

		if (redraw)
			m_ctrl.RedrawCell(0, ROW_CONTENTS_SUBCATEGORY);
	}

	void ContentsDescList::SetSubCategoryName(const std::wstring& wstrSubCategory)
	{
		SetSubCategoryName(wstrSubCategory, true);
	}

	void ContentsDescList::SetContentsStatus(const std::wstring& wstrStatus, const bool redraw)
	{
		m_ctrl.QuickSetText(0, ROW_CONTENTS_STATUS, wstrStatus.c_str());

		if (redraw)
			m_ctrl.RedrawCell(0, ROW_CONTENTS_STATUS);
	}

	void ContentsDescList::SetContentsStatus(const std::wstring& wstrStatus)
	{
		SetContentsStatus(wstrStatus, true);
	}
}