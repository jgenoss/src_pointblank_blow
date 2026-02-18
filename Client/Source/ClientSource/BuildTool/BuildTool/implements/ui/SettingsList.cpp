#include "stdafx.h"
#include "SettingsList.h"
#include "UIDefines.h"
#include "TasksBar.h"
#include "CountrySettingDlg.h"
#include "MainFrm.h"
#include "VersionSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	enum ROW_TYPE
	{
		ROW_TOPHEAD = -1,
		ROW_VERSION_SETTING,
		ROW_COUNTRY_SETTING,
		ROW_EXIT,
		ROW_MAX_COUNT
	};

	struct SettingsListCallback : CustomUGCtrlCallback
	{
		SettingsList& owner;
		CPen penIvoryBlack;

		SettingsListCallback(SettingsList& o) : owner(o) 
		{
		}

		virtual ~SettingsListCallback()
		{}

		virtual void		OnSetup(CUGCtrl* ctrl)
		{
			ctrl->AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
			ctrl->AddFont(L"Arial", -12, 500);		// 1, cell

			ctrl->SetNumberCols(1);
			ctrl->SetNumberRows(ROW_MAX_COUNT);

			ctrl->SetSH_Width(0);

			ctrl->SetDoubleBufferMode(TRUE);
			ctrl->SetUserSizingMode(2);
			ctrl->SetVScrollMode(UG_SCROLLTRACKING);
			ctrl->SetMultiSelectMode(FALSE);

			ctrl->SetDefFont(ctrl->GetFont(1));

			penIvoryBlack.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);

			CUGCell cell;
			ctrl->GetHeadingDefault(&cell);
			cell.SetFont(ctrl->GetFont(0));
			ctrl->SetHeadingDefault(&cell);

			ctrl->GetCell(0, ROW_TOPHEAD, &cell);
			cell.SetText(L"Settings");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&penIvoryBlack);
			ctrl->SetCell(0, ROW_TOPHEAD, &cell);

			ctrl->GetCell(0, ROW_VERSION_SETTING, &cell);
			cell.SetText(L"  Version");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&penIvoryBlack);
			ctrl->SetCell(0, ROW_VERSION_SETTING, &cell);

			ctrl->GetCell(0, ROW_COUNTRY_SETTING, &cell);
			cell.SetText(L"  Country");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&penIvoryBlack);
			ctrl->SetCell(0, ROW_COUNTRY_SETTING, &cell);

			ctrl->GetCell(0, ROW_EXIT, &cell);
			cell.SetText(L"  Exit");
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_MAROON);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&penIvoryBlack);
			ctrl->SetCell(0, ROW_EXIT, &cell);
		}

		virtual void		OnLClickedUp(CUGCtrl* ctrl, int col, long row, RECT* rect, POINT* pt, int processed)
		{
			if (owner.m_currSelectionRow == row)
				return;

			owner.SetSelection(row);

			switch (row)
			{
			case ROW_EXIT:
				GetMainFrame()->PostMessage(WM_CLOSE);
				break;

			case ROW_COUNTRY_SETTING:
			{
				CountrySettingDlg dlg;
				dlg.DoModal();
				owner.ClearSelection();
			}break;

			case ROW_VERSION_SETTING:
			{
				VersionSettingDlg dlg;
				dlg.DoModal();
				owner.ClearSelection();
			}break;
			}
		}
	};

	SettingsList::SettingsList(TasksBar* owner)
	{
		m_ctrl.SetCallback(std::make_unique<SettingsListCallback>(*this));
		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), owner, ID_SETTING_LIST);
	}

	SettingsList::~SettingsList()
	{}

	void SettingsList::AdjustLayout(const RECT& rt)
	{
		m_ctrl.MoveWindow(&rt);

		const int cx = rt.right - rt.left;
		m_ctrl.SetColWidth(0, cx);
	}

	void SettingsList::SetSelection(const int row)
	{
		const long totalNumOfRow = m_ctrl.GetNumberRows();
		CUGCell cell;

		for (long currRow = 0; currRow < totalNumOfRow; currRow++)
		{
			m_ctrl.GetCell(0, currRow, &cell);

			if (currRow == row)
			{
				cell.SetTextColor(COLOR_BLACK);
				cell.SetBackColor(COLOR_DEEP_DARK_ORANGE);
			}
			else
			{
				if (currRow != ROW_EXIT)
					cell.SetTextColor(COLOR_WHITE);

				cell.SetBackColor(COLOR_IVORY_BLACK);
			}

			m_ctrl.SetCell(0, currRow, &cell);

			m_ctrl.RedrawCell(0, currRow);
		}
	}

	void SettingsList::ClearSelection()
	{
		CUGCell cell;

		for (long currRow = 0; currRow < m_ctrl.GetNumberRows(); currRow++)
		{
			m_ctrl.GetCell(0, currRow, &cell);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			m_ctrl.SetCell(0, currRow, &cell);

			m_ctrl.RedrawCell(0, currRow);
		}

		m_currSelectionRow = -1;
	}
}