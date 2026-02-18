#include "stdafx.h"
#include "CountrySettingList.h"
#include "CountrySettingDlg.h"
#include "UIDefines.h"
#include <array>
#include <boost/filesystem.hpp>

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
		COL_COUNTRY_NAME,
		COL_VERSION,
		COL_MEDIA_PATH,
		COL_MEDIA_LOCALE_RELATIVE_PATH,
		COL_QA_PATH,
		COL_LIVE_PATH,
		COL_PACKSCRIPT_PATH,
		COL_PEF_IDX,
		COL_V3_UI_1_0_CHECKBOX,
		COL_V3_UI_1_1_CHECKBOX,
		COL_V3_UI_1_5_CHECKBOX,
		COL_MAX_COUNT,
	};

	enum ROW_TYPE
	{
		ROW_UPPER = 0,
		ROW_LOWER,
		ROW_MAX_COUNT,
	};

	ROW_TYPE CalcRowType(long row)
	{
		return static_cast<ROW_TYPE>(row % ROW_MAX_COUNT);
	}

	bool CalcRowIndex(const long row, long* outRowUpper, long* outRowLower)
	{
		if (row < 0)
			return false;

		if (outRowUpper == nullptr)
			return false;

		if (outRowLower == nullptr)
			return false;

		if (CalcRowType(row) == ROW_UPPER)
		{
			*outRowUpper = row;
			*outRowLower = row + 1;
		}
		else
		{
			*outRowUpper = row - 1;
			*outRowLower = row;
		}

		return true;
	}

	std::array<int, 5> g_arrPathEllipColType =
	{
		COL_MEDIA_PATH,
		COL_MEDIA_LOCALE_RELATIVE_PATH,
		COL_QA_PATH,
		COL_LIVE_PATH,
		COL_PACKSCRIPT_PATH
	};

	std::array<int, 2> g_arrComboColType =
	{
		COL_VERSION,
		COL_PEF_IDX,
	};

	struct CountrySettingListCallback : CustomUGCtrlCallback
	{
		CountrySettingList& owner;

		CountrySettingListCallback(CountrySettingList& o) : owner(o)
		{}

		virtual ~CountrySettingListCallback()
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

			ctrl->QuickSetText(COL_COUNTRY_NAME, -1, L"Country Name");
			ctrl->QuickSetText(COL_VERSION, -1, L"Version");
			ctrl->QuickSetText(COL_MEDIA_PATH, -1, L"Media Path");
			ctrl->QuickSetText(COL_MEDIA_LOCALE_RELATIVE_PATH, -1, L"Media Locale Path");
			ctrl->QuickSetText(COL_QA_PATH, -1, L"QA Path");
			ctrl->QuickSetText(COL_LIVE_PATH, -1, L"Live Path");
			ctrl->QuickSetText(COL_PACKSCRIPT_PATH, -1, L"PackScript Path");
			ctrl->QuickSetText(COL_PEF_IDX, -1, L"PEF Index");
			ctrl->QuickSetText(COL_V3_UI_1_0_CHECKBOX, -1, L"V3_1_0");
			ctrl->QuickSetText(COL_V3_UI_1_1_CHECKBOX, -1, L"V3_1_1");
			ctrl->QuickSetText(COL_V3_UI_1_5_CHECKBOX, -1, L"V3_1_5");
		}

		virtual void		OnDClicked(CUGCtrl* ctrl, int col, long row, RECT* rt, POINT* pt, BOOL processed) 
		{
			UNREFERENCED_PARAMETER(rt);
			UNREFERENCED_PARAMETER(pt);
			UNREFERENCED_PARAMETER(processed);

			if (col != COL_VERSION && col != COL_PEF_IDX)
			{
				if (CalcRowType(row) == ROW_UPPER)
					ctrl->StartEdit();
			}
		}

		virtual void		OnCharDown(CUGCtrl* ctrl, UINT *vcKey, BOOL processed)
		{
			UNREFERENCED_PARAMETER(processed);

			ctrl->StartEdit();
		}

		virtual void		OnCellChange(CUGCtrl* ctrl, int oldcol, int newcol, long oldrow, long newrow)
		{
			if (newrow != -1)	// TOPHEAD는 제외.
				owner._SetSelection(newrow, newcol);
		}

		virtual int			OnCellTypeNotify(CUGCtrl* ctrl, long ID, int col, long row, long msg, long param)
		{ 
			if (ID == owner.m_ellipType.GetID() && msg == UGCT_ELLIPSISBUTTONCLICK)
			{
				CFolderPickerDialog dlg(L"", 0, NULL, 0);
				if (IDOK == dlg.DoModal())
				{
					std::wstring wstrInputPath(dlg.GetPathName().GetString());

					if (col == COL_MEDIA_LOCALE_RELATIVE_PATH)
					{
						CString strBaseWorkdir;
						
						ctrl->QuickGetText(COL_MEDIA_PATH, row, &strBaseWorkdir);

						if (strBaseWorkdir.IsEmpty())
						{
							AfxMessageBox(L"왼칸의 기준 경로를 먼저 입력하시기 바랍니다.");
						}
						else
						{
							Path baseWorkdir(strBaseWorkdir.GetString());
							baseWorkdir.make_preferred();
							const std::wstring wstrBaseWorkdir = baseWorkdir.wstring();

							Path preferredInputPath(wstrInputPath);
							preferredInputPath.make_preferred();

							const std::wstring wstrPreferredInputPath = preferredInputPath.wstring();

							if (wstrBaseWorkdir == wstrPreferredInputPath)
							{
								AfxMessageBox(L"왼칸 경로를 기준으로 상대경로를 입력바랍니다");
								return TRUE;
							}

							auto pos = wstrPreferredInputPath.find(wstrBaseWorkdir);
							if (pos == std::wstring::npos)
							{
								AfxMessageBox(L"왼칸 경로를 기준으로 상대경로를 입력바랍니다.");
							}
							else
							{
								std::wstring wstrRelPathResult =
									wstrPreferredInputPath.substr(wstrBaseWorkdir.length() + 1, wstrPreferredInputPath.length());

								ctrl->QuickSetText(col, row, wstrRelPathResult.c_str());
								ctrl->RedrawCell(col, row);
							}
						}
					}
					else
					{
						ctrl->QuickSetText(col, row, wstrInputPath.c_str());
						ctrl->RedrawCell(col, row);
					}
				}
			}
			else if (msg == UGCT_DROPLISTSELECT && param == COL_VERSION)
			{
				CString *pStrVersion = (CString*)param;

				const bool isEnableV3 = i3::generic_is_iequal(pStrVersion->GetString(), "v3");

				CUGCell cell;
				int style = 0;

				if (isEnableV3)
				{
					ctrl->GetCell(COL_V3_UI_1_0_CHECKBOX, row, &cell);
					style = cell.GetCellTypeEx();
					style &= ~UGCT_CHECKBOXDISABLED;
					cell.SetCellTypeEx(style);
					ctrl->SetCell(COL_V3_UI_1_0_CHECKBOX, row, &cell);

					ctrl->GetCell(COL_V3_UI_1_1_CHECKBOX, row, &cell);
					style = cell.GetCellTypeEx();
					style &= ~UGCT_CHECKBOXDISABLED;
					cell.SetCellTypeEx(style);
					ctrl->SetCell(COL_V3_UI_1_1_CHECKBOX, row, &cell);

					ctrl->GetCell(COL_V3_UI_1_5_CHECKBOX, row, &cell);
					style = cell.GetCellTypeEx();
					style &= ~UGCT_CHECKBOXDISABLED;
					cell.SetCellTypeEx(style);
					ctrl->SetCell(COL_V3_UI_1_5_CHECKBOX, row, &cell);
				}
				else
				{
					ctrl->GetCell(COL_V3_UI_1_0_CHECKBOX, row, &cell);
					style = cell.GetCellTypeEx();
					style |= UGCT_CHECKBOXDISABLED;
					cell.SetNumber(0);
					cell.SetCellTypeEx(style);
					ctrl->SetCell(COL_V3_UI_1_0_CHECKBOX, row, &cell);

					ctrl->GetCell(COL_V3_UI_1_1_CHECKBOX, row, &cell);
					style = cell.GetCellTypeEx();
					style |= UGCT_CHECKBOXDISABLED;
					cell.SetNumber(0);
					cell.SetCellTypeEx(style);
					ctrl->SetCell(COL_V3_UI_1_1_CHECKBOX, row, &cell);

					ctrl->GetCell(COL_V3_UI_1_5_CHECKBOX, row, &cell);
					style = cell.GetCellTypeEx();
					style |= UGCT_CHECKBOXDISABLED;
					cell.SetNumber(0);
					cell.SetCellTypeEx(style);
					ctrl->SetCell(COL_V3_UI_1_5_CHECKBOX, row, &cell);
					ctrl->RedrawCell(COL_V3_UI_1_5_CHECKBOX, row);
				}

				ctrl->RedrawCell(COL_V3_UI_1_0_CHECKBOX, row);
				ctrl->RedrawCell(COL_V3_UI_1_1_CHECKBOX, row);
				ctrl->RedrawCell(COL_V3_UI_1_5_CHECKBOX, row);
			}

			return TRUE;
		}
	};
	
	CountrySettingList::CountrySettingList(CountrySettingDlg& owner)
	{
		m_ctrl.SetCallback(std::make_unique<CountrySettingListCallback>(*this));
		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), &owner, ID_COUNTRY_SETTING_LIST_CTRL);
	}

	CountrySettingList::~CountrySettingList()
	{}

	void CountrySettingList::AdjustLayout(RECT& rt)
	{
		const int cx = (rt.right - rt.left) - 17;
		const int cy = rt.bottom - rt.top;

		rt.bottom = static_cast<LONG>(cy * 0.9f);
		m_ctrl.MoveWindow(&rt);

		std::array<int, 5> arrPathColType =
		{
			COL_MEDIA_PATH,
			COL_MEDIA_LOCALE_RELATIVE_PATH,
			COL_QA_PATH,
			COL_LIVE_PATH,
			COL_PACKSCRIPT_PATH
		};

		const int cx_path = static_cast<int>((cx * 0.75f) / arrPathColType.size());

		for (auto &colType : arrPathColType)
			m_ctrl.SetColWidth(colType, cx_path);

		std::array<int, 3> arrV3UICheckboxType =
		{
			COL_V3_UI_1_0_CHECKBOX,
			COL_V3_UI_1_1_CHECKBOX,
			COL_V3_UI_1_5_CHECKBOX,
		};

		const int cx_V3UICheckbox = static_cast<int>((cx * 0.1f) / arrV3UICheckboxType.size());
		for (auto &colType : arrV3UICheckboxType)
			m_ctrl.SetColWidth(colType, cx_V3UICheckbox);

		int cx_rest = cx - ((cx_path * arrPathColType.size()) + (cx_V3UICheckbox * arrV3UICheckboxType.size()));

		const int cx_countryName = static_cast<int>(cx_rest * 0.5f);
		m_ctrl.SetColWidth(COL_COUNTRY_NAME, cx_countryName);

		cx_rest -= cx_countryName;
		const int cx_ver = static_cast<int>(cx_rest / 2);
		m_ctrl.SetColWidth(COL_VERSION, cx_ver);

		const int cx_pef = cx_rest - cx_ver;
		m_ctrl.SetColWidth(COL_PEF_IDX, cx_pef);
	}

	void CountrySettingList::GetClientRect(RECT* rt)
	{
		if (rt)
			m_ctrl.GetClientRect(rt);
	}

	void CountrySettingList::AddNewRow(long* outRowUpper, long* outRowLower)
	{
		const long rowUpper = _AddNewRowUpper();
		const long rowLower = _AddNewRowLower();
		_JoinNewRow(rowUpper, rowLower);

		m_ctrl.RedrawRow(rowUpper);
		m_ctrl.RedrawRow(rowLower);

		if (outRowUpper)
			*outRowUpper = rowUpper;

		if (outRowLower)
			*outRowLower = rowLower;
	}

	bool CountrySettingList::DeleteCurrRow()
	{
		if (m_currSelectionRow == -1)
			return false;

		_DeleteRow(m_currSelectionRow);

		return true;
	}

	long CountrySettingList::_AddNewRowUpper()
	{
		const int totalNumRow = m_ctrl.GetNumberRows() + 1;
		m_ctrl.SetNumberRows(totalNumRow);

		int row = totalNumRow - 1;

		CUGCell cell;

		m_ctrl.GetCell(COL_COUNTRY_NAME, row, &cell);
		cell.SetCellTypeEx(UGCT_NORMALMULTILINE);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetAlignment(UG_ALIGNVCENTER);
		cell.SetBorderColor(&m_penIvoryBlackLight);
		m_ctrl.SetCell(COL_COUNTRY_NAME, row, &cell);

		for (auto &colType : g_arrPathEllipColType)
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

		for (auto &colType : g_arrComboColType)
		{
			m_ctrl.GetCell(colType, row, &cell);
			cell.SetCellType(UGCT_DROPLIST);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetAlignment(UG_ALIGNVCENTER);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&m_penIvoryBlackLight);
			m_ctrl.SetCell(colType, row, &cell);
		};

		m_ctrl.GetCell(COL_VERSION, row, &cell);
		cell.SetCellType(UGCT_DROPLIST);
		cell.SetParam(COL_VERSION);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetAlignment(UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlackLight);

		std::string strLabelText;
		for (auto &ver : rsc::GetVersionStringList())
		{
			strLabelText += ver;	strLabelText += "\n";
		}
		std::wstring wstrLabelText;
		i3::mb_to_utf16(strLabelText, wstrLabelText);
		cell.SetLabelText(wstrLabelText.c_str());

		m_ctrl.SetCell(COL_VERSION, row, &cell);

		m_ctrl.GetCell(COL_PEF_IDX, row, &cell);
		cell.SetCellType(UGCT_DROPLIST);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetAlignment(UG_ALIGNVCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlackLight);
		cell.SetLabelText(L"1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n");
		m_ctrl.SetCell(COL_PEF_IDX, row, &cell);

		bool isDisabled = false;

		m_ctrl.GetCell(COL_V3_UI_1_0_CHECKBOX, row, &cell);
		cell.SetCellType(UGCT_CHECKBOX);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlackLight);
		m_ctrl.SetCell(COL_V3_UI_1_0_CHECKBOX, row, &cell);

		m_ctrl.GetCell(COL_V3_UI_1_1_CHECKBOX, row, &cell);
		cell.SetCellType(UGCT_CHECKBOX);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlackLight);
		m_ctrl.SetCell(COL_V3_UI_1_1_CHECKBOX, row, &cell);

		m_ctrl.GetCell(COL_V3_UI_1_5_CHECKBOX, row, &cell);
		cell.SetCellType(UGCT_CHECKBOX);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNCENTER);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_penIvoryBlackLight);
		m_ctrl.SetCell(COL_V3_UI_1_5_CHECKBOX, row, &cell);

		return row;
	}

	long CountrySettingList::_AddNewRowLower()
	{
		const int totalNumRow = m_ctrl.GetNumberRows() + 1;
		m_ctrl.SetNumberRows(totalNumRow);

		int row = totalNumRow - 1;

		CUGCell cell;

		for (auto &colType : g_arrPathEllipColType)
		{
			if (colType == COL_MEDIA_LOCALE_RELATIVE_PATH)
				continue;

			m_ctrl.GetCell(colType, row, &cell);
			cell.SetCellType(UGCT_DROPLIST);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetAlignment(UG_ALIGNVCENTER);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&m_penIvoryBlackLight);
			cell.SetLabelText(L"SVN\nGIT\n");
			m_ctrl.SetCell(colType, row, &cell);
		}

		return row;
	}

	void CountrySettingList::_JoinNewRow(const long rowUpper, const long rowLower)
	{
		m_ctrl.JoinCells(COL_COUNTRY_NAME, rowUpper, COL_COUNTRY_NAME, rowLower);
		m_ctrl.JoinCells(COL_VERSION, rowUpper, COL_VERSION, rowLower);
		m_ctrl.JoinCells(COL_MEDIA_LOCALE_RELATIVE_PATH, rowUpper, COL_MEDIA_LOCALE_RELATIVE_PATH, rowLower);
		m_ctrl.JoinCells(COL_PEF_IDX, rowUpper, COL_PEF_IDX, rowLower);
		m_ctrl.JoinCells(COL_V3_UI_1_0_CHECKBOX, rowUpper, COL_V3_UI_1_0_CHECKBOX, rowLower);
		m_ctrl.JoinCells(COL_V3_UI_1_1_CHECKBOX, rowUpper, COL_V3_UI_1_1_CHECKBOX, rowLower);
		m_ctrl.JoinCells(COL_V3_UI_1_5_CHECKBOX, rowUpper, COL_V3_UI_1_5_CHECKBOX, rowLower);
	}

	bool CountrySettingList::_DeleteRow(const long row)
	{
		if (row < 0 || m_ctrl.GetNumberRows() <= row)
			return false;

		long rowUpper, rowLower;
		CalcRowIndex(row, &rowUpper, &rowLower);

		m_ctrl.DeleteRow(rowLower);
		m_ctrl.DeleteRow(rowUpper);

		if (m_ctrl.GetNumberRows() == row)
			_SetSelection(rowUpper - 1, m_currSelectionCol);
		else
			_SetSelection(rowUpper, m_currSelectionCol);

		m_ctrl.RedrawAll();

		m_srcCountryDataMap.erase(rowUpper);

		return true;
	}

	void CountrySettingList::SetCountryData(const long rowUpper, const long rowLower, const rsc::CountryDataPtr& countryData)
	{
		if (rowUpper < 0 || m_ctrl.GetNumberRows() <= rowUpper)
			return;

		if (rowLower < 0 || m_ctrl.GetNumberRows() <= rowLower)
			return;

		std::wstring wstrCountryName;
		i3::mb_to_utf16(countryData->GetCountryName(), wstrCountryName);
		m_ctrl.QuickSetText(COL_COUNTRY_NAME, rowUpper, wstrCountryName.c_str());

		std::wstring wstrVersion;
		i3::mb_to_utf16(countryData->GetVersion(), wstrVersion);
		m_ctrl.QuickSetText(COL_VERSION, rowUpper, wstrVersion.c_str());

		m_ctrl.QuickSetText(COL_MEDIA_PATH, rowUpper, countryData->GetMediaWorkdir().wstring().c_str());
		m_ctrl.QuickSetText(COL_MEDIA_LOCALE_RELATIVE_PATH, rowUpper, countryData->GetMediaLocaleFolderRelPath().wstring().c_str());
		m_ctrl.QuickSetText(COL_QA_PATH, rowUpper, countryData->GetQAWorkdir().wstring().c_str());
		m_ctrl.QuickSetText(COL_LIVE_PATH, rowUpper, countryData->GetLiveWorkdir().wstring().c_str());
		m_ctrl.QuickSetText(COL_PACKSCRIPT_PATH, rowUpper, countryData->GetPackScriptWorkdir().wstring().c_str());
		m_ctrl.QuickSetText(COL_PEF_IDX, rowUpper, std::to_wstring(countryData->GetPEFCountryIdx()).c_str());

		std::wstring wstrVCS;

		if (!countryData->GetMediaVCS().empty())
			i3::mb_to_utf16(countryData->GetMediaVCS(), wstrVCS);
		m_ctrl.QuickSetText(COL_MEDIA_PATH, rowLower, wstrVCS.c_str());

		if (!countryData->GetQAVCS().empty())
			i3::mb_to_utf16(countryData->GetQAVCS(), wstrVCS);
		m_ctrl.QuickSetText(COL_QA_PATH, rowLower, wstrVCS.c_str());

		if (!countryData->GetLiveVCS().empty())
			i3::mb_to_utf16(countryData->GetLiveVCS(), wstrVCS);
		m_ctrl.QuickSetText(COL_LIVE_PATH, rowLower, wstrVCS.c_str());

		if (!countryData->GetPackScriptVCS().empty())
			i3::mb_to_utf16(countryData->GetPackScriptVCS(), wstrVCS);
		m_ctrl.QuickSetText(COL_PACKSCRIPT_PATH, rowLower, wstrVCS.c_str());

		CUGCell cell;
		int style = 0;

		if (i3::generic_is_iequal(wstrVersion, "v3"))
		{
			m_ctrl.GetCell(COL_V3_UI_1_0_CHECKBOX, rowUpper, &cell);
			style = cell.GetCellTypeEx();
			style &= ~UGCT_CHECKBOXDISABLED;
			if (countryData->IsEnableV3UI_1_0())
				cell.SetNumber(1);
			else
				cell.SetNumber(0);
			cell.SetCellTypeEx(style);
			m_ctrl.SetCell(COL_V3_UI_1_0_CHECKBOX, rowUpper, &cell);

			m_ctrl.GetCell(COL_V3_UI_1_1_CHECKBOX, rowUpper, &cell);
			style = cell.GetCellTypeEx();
			style &= ~UGCT_CHECKBOXDISABLED;
			if (countryData->IsEnableV3UI_1_1())
				cell.SetNumber(1);
			else
				cell.SetNumber(0);
			cell.SetCellTypeEx(style);
			m_ctrl.SetCell(COL_V3_UI_1_1_CHECKBOX, rowUpper, &cell);

			m_ctrl.GetCell(COL_V3_UI_1_5_CHECKBOX, rowUpper, &cell);
			style &= ~UGCT_CHECKBOXDISABLED;
			style = cell.GetCellTypeEx();
			if (countryData->IsEnableV3UI_1_5())
				cell.SetNumber(1);
			else
				cell.SetNumber(0);
			cell.SetCellTypeEx(style);
			m_ctrl.SetCell(COL_V3_UI_1_5_CHECKBOX, rowUpper, &cell);
		}
		else
		{
			m_ctrl.GetCell(COL_V3_UI_1_0_CHECKBOX, rowUpper, &cell);
			style = cell.GetCellTypeEx();
			style |= UGCT_CHECKBOXDISABLED;
			cell.SetCellTypeEx(style);
			cell.SetNumber(0);
			m_ctrl.SetCell(COL_V3_UI_1_0_CHECKBOX, rowUpper, &cell);

			m_ctrl.GetCell(COL_V3_UI_1_1_CHECKBOX, rowUpper, &cell);
			style = cell.GetCellTypeEx();
			style |= UGCT_CHECKBOXDISABLED;
			cell.SetCellTypeEx(style);
			cell.SetNumber(0);
			m_ctrl.SetCell(COL_V3_UI_1_1_CHECKBOX, rowUpper, &cell);

			m_ctrl.GetCell(COL_V3_UI_1_5_CHECKBOX, rowUpper, &cell);
			style = cell.GetCellTypeEx();
			style |= UGCT_CHECKBOXDISABLED;
			cell.SetCellTypeEx(style);
			cell.SetNumber(0);
			m_ctrl.SetCell(COL_V3_UI_1_5_CHECKBOX, rowUpper, &cell);
		}
	
		m_ctrl.RedrawRow(rowUpper);
		m_ctrl.RedrawRow(rowLower);
	}

	void CountrySettingList::AddNewCountry(const rsc::CountryDataPtr& countryData)
	{
		long rowUpper, rowLower;
		AddNewRow(&rowUpper, &rowLower);

		SetCountryData(rowUpper, rowLower, countryData);

		m_srcCountryDataMap.insert(CountryDataMap::value_type(rowUpper, countryData));
	}

	void CountrySettingList::_SetSelection(const long row, const int col)
	{
		if (col <= COL_TOPHEAD && COL_MAX_COUNT <= col)
			return;

		if (row <= -1)
			return;

		long rowUpper, rowLower;

		CUGCell cell;

		if (m_currSelectionRow != -1)
		{
			CalcRowIndex(m_currSelectionRow, &rowUpper, &rowLower);

			for (long row = rowUpper; row <= rowLower; row++)
			{
				for (int i = COL_COUNTRY_NAME; i < COL_MAX_COUNT; i++)
				{
					m_ctrl.GetCell(i, row, &cell);
					cell.SetBackColor(COLOR_IVORY_BLACK);
					cell.SetBorderColor(&m_penIvoryBlackLight);
					cell.SetTextColor(COLOR_SILVER);
					m_ctrl.SetCell(i, row, &cell);
				}

				m_ctrl.RedrawRow(row);
			}
		}

		CalcRowIndex(row, &rowUpper, &rowLower);
		const ROW_TYPE rowType = CalcRowType(row);

		for (long row = rowUpper; row <= rowLower; row++)
		{
			for (int i = COL_COUNTRY_NAME; i < COL_MAX_COUNT; i++)
			{
				m_ctrl.GetCell(i, row, &cell);

				if (rowType != CalcRowType(row))
				{
					cell.SetBackColor(COLOR_ORANGE);
					cell.SetTextColor(COLOR_BLACK);
					cell.SetBorderColor(&m_penOrange);
				}
				else
				{
					if (i == col)
					{
						cell.SetBorderColor(&m_penOrange);
					}
					else
					{
						cell.SetBackColor(COLOR_ORANGE);
						cell.SetTextColor(COLOR_BLACK);
						cell.SetBorderColor(&m_penOrange);
					}
				}

				m_ctrl.SetCell(i, row, &cell);
			}

			m_ctrl.RedrawRow(row);
		}

		m_currSelectionRow = row;
		m_currSelectionCol = col;
	}

	void CountrySettingList::GetCountryDataList(std::vector<rsc::CountryData>& out)
	{
		CString text;
		long rowUpper, rowLower;

		rsc::CountryData data;
		std::string strVCS;
		for (long row = 0; row < m_ctrl.GetNumberRows(); row++)
		{
			CalcRowIndex(row, &rowUpper, &rowLower);

			if (CalcRowType(row) == ROW_UPPER)
			{
				m_ctrl.QuickGetText(COL_COUNTRY_NAME, rowUpper, &text);
				std::string strCountryName;
				i3::utf16_to_mb(text.GetString(), strCountryName);
				data.SetCountryName(strCountryName);

				m_ctrl.QuickGetText(COL_VERSION, rowUpper, &text);
				std::string strVersion;
				i3::utf16_to_mb(text.GetString(), strVersion);
				data.SetVersion(strVersion);

				m_ctrl.QuickGetText(COL_MEDIA_PATH, rowUpper, &text);
				data.SetMediaWorkdir(text.GetString());

				m_ctrl.QuickGetText(COL_MEDIA_LOCALE_RELATIVE_PATH, rowUpper, &text);
				data.SetMediaLocaleFolderRelPath(text.GetString());

				m_ctrl.QuickGetText(COL_QA_PATH, rowUpper, &text);
				data.SetQAWorkdir(text.GetString());

				m_ctrl.QuickGetText(COL_LIVE_PATH, rowUpper, &text);
				data.SetLiveWorkdir(text.GetString());

				m_ctrl.QuickGetText(COL_PACKSCRIPT_PATH, rowUpper, &text);
				data.SetPackScriptWorkdir(text.GetString());

				m_ctrl.QuickGetText(COL_PEF_IDX, rowUpper, &text);
				std::string strPEFIdx;
				i3::utf16_to_mb(text.GetString(), strPEFIdx);
				data.SetPEFCountryIdx(std::atoi(strPEFIdx.c_str()));

				CUGCell cell;
				m_ctrl.GetCell(COL_V3_UI_1_0_CHECKBOX, rowUpper, &cell);
				if (cell.GetNumber() == 0)
					data.SetEnableV3UI_1_0(false);
				else
					data.SetEnableV3UI_1_0(true);

				m_ctrl.GetCell(COL_V3_UI_1_1_CHECKBOX, rowUpper, &cell);
				if (cell.GetNumber() == 0)
					data.SetEnableV3UI_1_1(false);
				else
					data.SetEnableV3UI_1_1(true);

				m_ctrl.GetCell(COL_V3_UI_1_5_CHECKBOX, rowUpper, &cell);
				if (cell.GetNumber() == 0)
					data.SetEnableV3UI_1_5(false);
				else
					data.SetEnableV3UI_1_5(true);
				
				continue;
			}
			
			m_ctrl.QuickGetText(COL_MEDIA_PATH, rowLower, &text);
			i3::utf16_to_mb(text.GetString(), strVCS);
			data.SetMediaVCS(strVCS);

			m_ctrl.QuickGetText(COL_QA_PATH, rowLower, &text);
			i3::utf16_to_mb(text.GetString(), strVCS);
			data.SetQAVCS(strVCS);

			m_ctrl.QuickGetText(COL_LIVE_PATH, rowLower, &text);
			i3::utf16_to_mb(text.GetString(), strVCS);
			data.SetLiveVCS(strVCS);

			m_ctrl.QuickGetText(COL_PACKSCRIPT_PATH, rowLower, &text);
			i3::utf16_to_mb(text.GetString(), strVCS);
			data.SetPackScriptVCS(strVCS);

			out.push_back(data);
		}
	}
}