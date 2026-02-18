#include "stdafx.h"
#include "VersionSettingList.h"
#include "VersionSettingDlg.h"
#include <array>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	enum COL_TYPE
	{
		COL_TOPHEAD = -1,
		COL_VERSION_NAME,
		COL_POINTBLANK_PATH,
		COL_I3ENGINE_PATH,
		COL_MAX_COUNT,
	};

	struct VersionSettingListCallback : CustomUGCtrlCallback
	{
		VersionSettingList& owner;

		VersionSettingListCallback(VersionSettingList& o) : owner(o)
		{}

		virtual ~VersionSettingListCallback()
		{}

		virtual void		OnSetup(CUGCtrl* ctrl)
		{
			ctrl->AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
			ctrl->AddFont(L"Arial", -12, 500);		// 1, cell

			ctrl->AddCellType(&owner.m_ellipType);

			ctrl->SetNumberCols(COL_MAX_COUNT);
			ctrl->SetSH_Width(0);

			ctrl->SetDoubleBufferMode(TRUE);
			ctrl->SetUserSizingMode(2);
			ctrl->SetVScrollMode(UG_SCROLLTRACKING);
			ctrl->SetMultiSelectMode(2);

			ctrl->SetDefFont(ctrl->GetFont(1));

			owner.m_penIvoryBlack.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);
			owner.m_penIvoryBlackLight.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);
			owner.m_penOrange.CreatePen(PS_SOLID, 1, COLOR_ORANGE);

			CUGCell cell;
			ctrl->GetHeadingDefault(&cell);
			cell.SetFont(ctrl->GetFont(0));
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_BLACK);
			cell.SetTextColor(COLOR_TAN3_PERU);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&owner.m_penIvoryBlack);
			ctrl->SetHeadingDefault(&cell);

			ctrl->QuickSetText(COL_VERSION_NAME, -1, L"Version");
			ctrl->QuickSetText(COL_POINTBLANK_PATH, -1, L"PointBlank Path");
			ctrl->QuickSetText(COL_I3ENGINE_PATH, -1, L"i3Engine Path");
		}

		virtual void		OnDClicked(CUGCtrl* ctrl, int col, long row, RECT* rt, POINT* pt, BOOL processed)
		{
			UNREFERENCED_PARAMETER(rt);
			UNREFERENCED_PARAMETER(pt);
			UNREFERENCED_PARAMETER(processed);

			if (col == COL_VERSION_NAME)
				return;

			ctrl->StartEdit();
		}

		virtual void		OnCharDown(CUGCtrl* ctrl, UINT *vcKey, BOOL processed)
		{}

		virtual void		OnCellChange(CUGCtrl* ctrl, int oldcol, int newcol, long oldrow, long newrow)
		{}

		virtual int			OnCellTypeNotify(CUGCtrl* ctrl, long ID, int col, long row, long msg, long param)
		{
			if (ID == owner.m_ellipType.GetID() && msg == UGCT_ELLIPSISBUTTONCLICK)
			{
				CFolderPickerDialog dlg(L"", 0, NULL, 0);
				if (dlg.DoModal() == IDOK)
				{
					std::wstring wstrInputPath(dlg.GetPathName().GetString());

					ctrl->QuickSetText(col, row, wstrInputPath.c_str());
					ctrl->RedrawCell(col, row);
				}
			}

			return TRUE;
		}

		virtual void		OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum) override
		{
			if (blockNum == 1)
			{
				// selection.
				CUGCell cell;
				if (owner.m_currSelectionRow != -1)
				{
					for (int i = COL_VERSION_NAME; i < COL_MAX_COUNT; i++)
					{
						ctrl->GetCell(i, owner.m_currSelectionRow, &cell);
						cell.SetBackColor(COLOR_IVORY_BLACK);
						cell.SetTextColor(COLOR_SILVER);
						cell.SetBorderColor(&owner.m_penIvoryBlackLight);
						ctrl->SetCell(i, owner.m_currSelectionRow, &cell);
					}
					ctrl->RedrawRow(owner.m_currSelectionRow);
				}

				for (int i = COL_VERSION_NAME; i < COL_MAX_COUNT; i++)
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
	};

	std::array<int, 2> g_arrPathEllipColType =
	{
		COL_POINTBLANK_PATH,
		COL_I3ENGINE_PATH
	};

	VersionSettingList::VersionSettingList(VersionSettingDlg& owner)
	{
		m_ctrl.SetCallback(std::make_unique<VersionSettingListCallback>(*this));
		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), &owner, ID_VERSION_SETTING_LIST_CTRL);
	}

	VersionSettingList::~VersionSettingList()
	{}

	void VersionSettingList::AdjustLayout(RECT& rt)
	{
		const int cx = (rt.right - rt.left) - 17;
		const int cy = rt.bottom - rt.top;

		rt.bottom = static_cast<LONG>(cy * 0.85f);
		m_ctrl.MoveWindow(&rt);

		const int cx_path = static_cast<int>((cx * 0.9f) / g_arrPathEllipColType.size());

		for (const auto &colType : g_arrPathEllipColType)
			m_ctrl.SetColWidth(colType, cx_path);

		int cx_rest = cx - (cx_path * g_arrPathEllipColType.size());
		m_ctrl.SetColWidth(COL_VERSION_NAME, cx_rest);
	}

	void VersionSettingList::GetClientRect(RECT* rt)
	{}

	long VersionSettingList::AddNewRow()
	{
		const int totalNumRow = m_ctrl.GetNumberRows() + 1;
		m_ctrl.SetNumberRows(totalNumRow);

		int row = totalNumRow - 1;

		CUGCell cell;

		m_ctrl.GetCell(COL_VERSION_NAME, row, &cell);
		cell.SetCellTypeEx(UGCT_NORMALMULTILINE);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetAlignment(UG_ALIGNVCENTER);
		cell.SetBorderColor(&m_penIvoryBlackLight);
		m_ctrl.SetCell(COL_VERSION_NAME, row, &cell);

		for (const auto &colType : g_arrPathEllipColType)
		{
			m_ctrl.GetCell(colType, row, &cell);
			cell.SetCellType(m_ellipType.GetID());
			cell.SetCellTypeEx(UGCT_NORMALMULTILINE);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetAlignment(UG_ALIGNVCENTER);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&m_penIvoryBlackLight);
			m_ctrl.SetCell(colType, row, &cell);
		}

		m_ctrl.RedrawRow(row);

		return row;
	}

	void VersionSettingList::SetVersionData(const long row, const rsc::VerDataPtr& data)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return;

		if (!data->GetVersion().empty())
		{
			std::wstring wstrVersionName;
			i3::mb_to_utf16(data->GetVersion(), wstrVersionName);
			m_ctrl.QuickSetText(COL_VERSION_NAME, row, wstrVersionName.c_str());
		}

		if (!data->GetPointBlankAbsPath().empty())
		{
			std::wstring wstrPointBlankPath;
			i3::mb_to_utf16(data->GetPointBlankAbsPath().string(), wstrPointBlankPath);
			m_ctrl.QuickSetText(COL_POINTBLANK_PATH, row, wstrPointBlankPath.c_str());
		}

		if (!data->Geti3EngineAbsPath().empty())
		{
			std::wstring wstri3EnginePath;
			i3::mb_to_utf16(data->Geti3EngineAbsPath().string(), wstri3EnginePath);
			m_ctrl.QuickSetText(COL_I3ENGINE_PATH, row, wstri3EnginePath.c_str());
		}

		m_ctrl.RedrawRow(row);
	}

	void VersionSettingList::GetAllDataList(std::vector<rsc::VersionData>& out)
	{
		CString str;
		rsc::VersionData data;

		for (long row = 0; row < m_ctrl.GetNumberRows(); row++)
		{
			m_ctrl.QuickGetText(COL_VERSION_NAME, row, &str);
			std::string strVersion;
			i3::utf16_to_mb(str.GetString(), strVersion);
			data.SetVersion(strVersion);

			m_ctrl.QuickGetText(COL_POINTBLANK_PATH, row, &str);
			std::string strPointBlankPath;
			i3::utf16_to_mb(str.GetString(), strPointBlankPath);
			data.SetPointBlankAbsPath(strPointBlankPath);

			m_ctrl.QuickGetText(COL_I3ENGINE_PATH, row, &str);
			std::string stri3EnginePath;
			i3::utf16_to_mb(str.GetString(), stri3EnginePath);
			data.Seti3EngineAbsPath(stri3EnginePath);

			out.push_back(data);
		}
	}
}