#include "stdafx.h"
#include "ToolList.h"
#include "CustomUGCtrl.h"
#include "UIDefines.h"
#include "TasksBar.h"
#include <functional>
#include "ToolFactory.h"
#include <boost/algorithm/string.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	struct ToolListCallback : CustomUGCtrlCallback
	{
		ToolList& owner;

		ToolListCallback(ToolList& o) : owner(o)
		{}

		virtual ~ToolListCallback()
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
			cell.SetText(L"Tools");
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
				std::wstring wstrToolName(cell.GetText());
				boost::algorithm::trim_left(wstrToolName);

				// tool ½ÇÇà.
				std::string strToolName;
				i3::utf16_to_mb(wstrToolName, strToolName);

				const ToolCreateFuncType& f = ToolFactory::i()->Request(strToolName);
				f();
			}
		}
	};

	ToolList::ToolList(TasksBar* owner)
	{
		m_ctrl.SetCallback(std::make_unique<ToolListCallback>(*this));
		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), owner, ID_TOOL_LIST);
	}

	ToolList::~ToolList()
	{}

	void ToolList::AdjustLayout(const RECT& rt)
	{
		m_ctrl.MoveWindow(&rt);

		const int cx = rt.right - rt.left;
		m_ctrl.SetColWidth(0, cx);
	}

	void ToolList::AddTool(const std::string& strToolName)
	{
		if (strToolName.empty())
			return;

		std::wstring wstr;
		i3::mb_to_utf16(strToolName, wstr);

		std::wstring wstrToolName = L"  " + wstr;

		const long row = m_ctrl.GetNumberRows();
		m_ctrl.InsertRow(row);

		CUGCell cell;
		m_ctrl.GetCell(0, row, &cell);
		cell.SetText(wstrToolName.c_str());
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_WHITE);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlack);
		
		m_ctrl.SetCell(0, row, &cell);
		m_ctrl.RedrawRow(row);
	}

	long ToolList::GetNumOfTools()
	{
		return m_ctrl.GetNumberRows();
	}
}