#include "stdafx.h"
#include "LiveCountryList.h"
#include "CustomUGCtrl.h"
#include "UIDefines.h"
#include "TasksBar.h"
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	struct LiveCountryListCallback : CustomUGCtrlCallback
	{
		LiveCountryList& owner;

		using ClickCountryFuncType = std::function<void(const std::wstring&)>;
		ClickCountryFuncType f_ClickCountry;

		LiveCountryListCallback(LiveCountryList& o, const ClickCountryFuncType& f) : owner(o), f_ClickCountry(f)
		{}

		virtual ~LiveCountryListCallback()
		{}

		virtual void		OnSetup(CUGCtrl* ctrl)
		{
			ctrl->AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
			ctrl->AddFont(L"Arial", -12, 500);		// 1, cell

			ctrl->SetNumberCols(1);
			ctrl->SetDoubleBufferMode(TRUE);
			ctrl->SetUserSizingMode(2);
			ctrl->SetVScrollMode(UG_SCROLLTRACKING);
			ctrl->SetMultiSelectMode(FALSE);

			ctrl->SetDefFont(ctrl->GetFont(1));

			owner.m_penIvoryBlack.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);

			CUGCell cell;
			ctrl->GetHeadingDefault(&cell);
			cell.SetFont(ctrl->GetFont(0));
			ctrl->SetHeadingDefault(&cell);

			ctrl->GetCell(0, -1, &cell);
			cell.SetText(L"Live Services");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penIvoryBlack);
			ctrl->SetCell(0, -1, &cell);

			ctrl->SetSH_Width(0);
		}

		virtual void		OnLClickedUp(CUGCtrl* ctrl, int col, long row, RECT* rect, POINT* pt, int processed)
		{
			const long totalNumOfRow = ctrl->GetNumberRows();

			CUGCell cell;
			for (long currRow = 0; currRow < totalNumOfRow; currRow++)
			{
				ctrl->GetCell(0, currRow, &cell);

				if (currRow == row)
				{
					cell.SetTextColor(COLOR_BLACK);
					cell.SetBackColor(COLOR_DEEP_DARK_ORANGE);
				}
				else
				{
					cell.SetTextColor(COLOR_WHITE);
					cell.SetBackColor(COLOR_IVORY_BLACK);
				}

				ctrl->SetCell(0, currRow, &cell);
				ctrl->RedrawCell(0, currRow);
			}

			if (ctrl->GetCell(0, row, &cell) == 0)
			{
				std::wstring wstrCountryName(cell.GetText());
				size_t pos = wstrCountryName.find(L"[PB_SVC]");
				if (pos != std::wstring::npos)
				{
					wstrCountryName.erase(0, pos + 9);
				}

				f_ClickCountry(wstrCountryName);
			}
		}
	};

	LiveCountryList::LiveCountryList(TasksBar* owner)
	{
		m_ctrl.SetCallback(std::make_unique<LiveCountryListCallback>(
			*this, std::bind(&TasksBar::OnClickCountry, owner, std::placeholders::_1)));

		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), owner, ID_COUNTRY_LIST);
	}

	LiveCountryList::~LiveCountryList()
	{}

	void LiveCountryList::AdjustLayout(const RECT& rt)
	{
		m_ctrl.MoveWindow(&rt);

		const int cx = rt.right - rt.left;
		m_ctrl.SetColWidth(0, cx);
	}

	void LiveCountryList::AddCountry(const std::string& strCountryName)
	{
		const long row = m_ctrl.GetNumberRows();
		m_ctrl.InsertRow(row);

		CUGCell cell;
		m_ctrl.GetCell(0, row, &cell);
		std::wstring wstrCountryName(L"  [PB_SVC] ");
		wstrCountryName.append(strCountryName.begin(), strCountryName.end());
		cell.SetText(wstrCountryName.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_WHITE);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlack);
		
		m_ctrl.SetCell(0, row, &cell);
		m_ctrl.RedrawRow(row);
	}

	void LiveCountryList::DelCountry(const std::string& strCountryName)
	{
		CUGCell cell;
		std::string strCurrCountryName;
		std::wstring wstrCurrCountryName;
		for (long row = 0; row < m_ctrl.GetNumberRows(); row++)
		{
			m_ctrl.GetCell(0, row, &cell);
			wstrCurrCountryName = cell.GetText();
			strCurrCountryName.assign(wstrCurrCountryName.begin(), wstrCurrCountryName.end());
			if (strCurrCountryName == strCountryName)
			{
				m_ctrl.DeleteRow(row);
				m_ctrl.RedrawRow(row);
			}
		}
	}

	void LiveCountryList::Clear()
	{
		m_ctrl.SetNumberRows(0, TRUE);
	}
}