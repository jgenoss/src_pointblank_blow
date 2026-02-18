#include "stdafx.h"
#include "SyncCartListCtrl.h"
#include <boost/lexical_cast.hpp>
#include "resource/DragdropDataContext.h"
#include "resource/ContentsDefines.h"
#include <algorithm>
#include "resource/ResourceController.h"
#include "ui/UIDefines.h"


namespace ui
{
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	BEGIN_MESSAGE_MAP(SyncCartListCtrl, CUGCtrl)
		ON_MESSAGE(WMU_INSERT_CONTENTS_SYNCLIST,		&SyncCartListCtrl::OnInsertContents)
		ON_MESSAGE(WMU_CLEAR_CONTENTS_SYNCLIST,			&SyncCartListCtrl::OnClearContents)
		ON_MESSAGE(WMU_START_CONTENTS_SYNC_PROGRESS,	&SyncCartListCtrl::OnStartSyncProgress)
		ON_MESSAGE(WMU_UPDATE_CONTENTS_SYNC_PROGRESS,	&SyncCartListCtrl::OnUpdateSyncProgress)
		ON_MESSAGE(WMU_DELETE_CONTENTS_SYNCLIST,		&SyncCartListCtrl::OnDeleteContents)
	END_MESSAGE_MAP()

	enum MenuID
	{
		ID_MENU_DELETE = 4001,
	};

	struct fn_findData
	{
		fn_findData(rsc::ContentsDataPtr data) : srcDataPtr(data) {}

		bool operator()(const rsc::ContentsDataPtr dstDataPtr) const
		{
			return *srcDataPtr.get() == *dstDataPtr.get();
		}

		rsc::ContentsDataPtr srcDataPtr;
	};

	SyncCartListCtrl::SyncCartListCtrl() : m_thpool(std::make_unique<ThreadPool>(1))
	{}

	SyncCartListCtrl::~SyncCartListCtrl()
	{}

	void SyncCartListCtrl::OnSetup()
	{
		m_syncProgressBarIdx = AddCellType(&m_syncProgressBar);

		AddFont(L"Arial", -12, 500);
		SetDefFont(GetFont(0));

		m_cellBorderColorPen.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);
		m_cellSelectionBorderColorPen.CreatePen(PS_SOLID, 1, COLOR_ORANGE);

		SetNumberCols(SYNCCARTLIST_COL_MAX_COUNT);
		EnableMenu(TRUE);
		SetDoubleBufferMode(TRUE);
		SetUserSizingMode(FALSE);
		SetVScrollMode(UG_SCROLLTRACKING);

		SetSH_Width(0);
		SetTH_Height(0);

		CUGCell cell;

		GetGridDefault(&cell);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		SetGridDefault(&cell);

		GetCell(SYNCCARTLIST_COL_ID, -1, &cell);
		cell.SetText(L"ID");
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_WHITE);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_cellBorderColorPen);
		SetCell(SYNCCARTLIST_COL_ID, -1, &cell);

		GetCell(SYNCCARTLIST_COL_NAME, -1, &cell);
		cell.SetText(L"Contents Name");
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_WHITE);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_cellBorderColorPen);
		SetCell(SYNCCARTLIST_COL_NAME, -1, &cell);

		GetCell(SYNCCARTLIST_COL_STATUS, -1, &cell);
		cell.SetText(L"Status");
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_WHITE);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&m_cellBorderColorPen);
		SetCell(SYNCCARTLIST_COL_STATUS, -1, &cell);
	}

	void SyncCartListCtrl::OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed)
	{
		if (row != -1)
			_SetSelection(col, row);
	}

	void SyncCartListCtrl::OnRClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed)
	{
		if (row != -1)
			_SetSelection(col, row);
	}

	void SyncCartListCtrl::_SetSelection(const int col, const int row)
	{
		CUGCell cell;

		if (m_currRow != -1)
		{
			for (int i = SYNCCARTLIST_COL_ID; i < SYNCCARTLIST_COL_MAX_COUNT; i++)
			{
				GetCell(i, m_currRow, &cell);
				cell.SetBackColor(COLOR_IVORY_BLACK);
				cell.SetTextColor(COLOR_WHITE);
				cell.SetBorderColor(&m_cellBorderColorPen);
				SetCell(i, m_currRow, &cell);
				RedrawCell(i, m_currRow);
			}
		}

		for (int i = SYNCCARTLIST_COL_ID; i < SYNCCARTLIST_COL_MAX_COUNT; i++)
		{
			GetCell(i, row, &cell);
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&m_cellSelectionBorderColorPen);
			SetCell(i, row, &cell);
			RedrawCell(i, row);
		}

		m_currRow = row;
	}

	void SyncCartListCtrl::InsertContents(rsc::ContentsDataPtr contentsDataPtr)
	{
		m_contentsDataPtrQueue.push(contentsDataPtr);

		PostMessage(WMU_INSERT_CONTENTS_SYNCLIST);
	}

	LRESULT SyncCartListCtrl::OnInsertContents(WPARAM wparam, LPARAM lparam)
	{
		auto contentsDataPtr = m_contentsDataPtrQueue.front();
		m_contentsDataPtrQueue.pop();
		
		auto it = 
			std::find_if(m_contentsDataPtrList.begin(), m_contentsDataPtrList.end(), fn_findData(contentsDataPtr));
		if (it == m_contentsDataPtrList.end())
		{
			m_contentsDataPtrList.push_back(contentsDataPtr);	// 데이터 목록에 저장/유지.

			const int row = GetNumberRows();
			InsertRow(row);

			CUGCell cell;

			GetCell(SYNCCARTLIST_COL_ID, row, &cell);
			cell.SetText(std::to_wstring(row).c_str());
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&m_cellBorderColorPen);
			SetCell(SYNCCARTLIST_COL_ID, row, &cell);
			RedrawCell(SYNCCARTLIST_COL_ID, row);

			GetCell(SYNCCARTLIST_COL_NAME, row, &cell);
			cell.SetCellType(m_syncProgressBarIdx);
			cell.SetHBackColor(COLOR_VIOLET_RED);
			cell.SetText(L"0");
			const std::wstring wstrContentsName(contentsDataPtr->contentsName.begin(), contentsDataPtr->contentsName.end());
			cell.SetLabelText(wstrContentsName.c_str());
			cell.SetParam(row);
			cell.SetCellTypeEx(UGCT_PROGRESSUSELABEL);
			cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&m_cellBorderColorPen);
			SetCell(SYNCCARTLIST_COL_NAME, row, &cell);
			RedrawCell(SYNCCARTLIST_COL_NAME, row);

			GetCell(SYNCCARTLIST_COL_STATUS, row, &cell);
			cell.SetText(L"!");
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_WHITE);
			cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
			cell.SetBorderColor(&m_cellBorderColorPen);
			SetCell(SYNCCARTLIST_COL_STATUS, row, &cell);
			RedrawCell(SYNCCARTLIST_COL_STATUS, row);
		}

		return 0;
	}

	void SyncCartListCtrl::DeleteContents(const int row)
	{
		PostMessage(WMU_DELETE_CONTENTS_SYNCLIST, row);
	}

	LRESULT SyncCartListCtrl::OnDeleteContents(WPARAM wparam, LPARAM lparam)
	{
		const int row = wparam;
		if (row == -1)
			return 0;

		DeleteRow(row);

		m_contentsDataPtrList.erase(m_contentsDataPtrList.begin() + row);

		CUGCell cell;
		for (int currRow = row; currRow < GetNumberRows(); currRow++)
		{
			for (int col = SYNCCARTLIST_COL_ID; col < SYNCCARTLIST_COL_MAX_COUNT; col++)
			{
				GetCell(SYNCCARTLIST_COL_ID, currRow, &cell);
				cell.SetText(std::to_wstring(currRow).c_str());
				cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
				cell.SetBackColor(COLOR_IVORY_BLACK);
				cell.SetTextColor(COLOR_WHITE);
				cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
				cell.SetBorderColor(&m_cellBorderColorPen);
				SetCell(SYNCCARTLIST_COL_ID, currRow, &cell);
				RedrawCell(col, currRow);
			}
		}

		if (GetNumberRows() > 0 && (m_currRow == row))
		{
			for (int col = 0; col < SYNCCARTLIST_COL_MAX_COUNT; col++)
			_SetSelection(col, row);
		}

		return 0;
	}

	void SyncCartListCtrl::ClearAllContents()
	{
		PostMessage(WMU_CLEAR_CONTENTS_SYNCLIST);
	}

	LRESULT SyncCartListCtrl::OnClearContents(WPARAM wparam, LPARAM lparam)
	{
		for (long i = 0; i < GetNumberRows(); i++)
			int rt = m_GI->m_defDataSource->DeleteRow(i);

		SetNumberRows(0);

		m_contentsDataPtrList.clear();

		std::queue<rsc::ContentsDataPtr> emptyQueue;
		std::swap(m_contentsDataPtrQueue, emptyQueue);

		m_isSynchronizing = false;

		m_currRow = -1;

		return 0;
	}

	LRESULT SyncCartListCtrl::OnStartSyncProgress(WPARAM wparam, LPARAM lparam)
	{
		const unsigned int row = wparam;

		CUGCell cell;
		GetCell(SYNCCARTLIST_COL_NAME, row, &cell);
		cell.SetText(L"0");
		SetCell(SYNCCARTLIST_COL_NAME, row, &cell);
		RedrawCell(SYNCCARTLIST_COL_NAME, row);

		return 0;
	}

	LRESULT SyncCartListCtrl::OnUpdateSyncProgress(WPARAM wparam, LPARAM lparam)
	{
		const unsigned int row = wparam;
		const unsigned int slideVal = lparam;

		CUGCell cell;
		GetCell(SYNCCARTLIST_COL_NAME, row, &cell);
		cell.SetText(std::to_wstring(slideVal).c_str());
		SetCell(SYNCCARTLIST_COL_NAME, row, &cell);
		RedrawCell(SYNCCARTLIST_COL_NAME, row);

		return 0;
	}

	void SyncCartListCtrl::AdjustLayout(const RECT& rt)
	{
		MoveWindow(&rt);

		const int width = rt.right - rt.left;
		SetColWidth(SYNCCARTLIST_COL_NAME, width);
	}

	void SyncCartListCtrl::GetContentsList(std::vector<rsc::ContentsDataPtr>& outList)
	{
		for (auto &contentsDataPtr : m_contentsDataPtrList)
			outList.push_back(contentsDataPtr);
	}

	DROPEFFECT SyncCartListCtrl::OnDragDrop(COleDataObject* data, int col, long row)
	{
		std::vector<rsc::ContentsDataPtr> contentsDataPtrList;
		rsc::DragdropDataContext::i()->GetDatas(contentsDataPtrList);

		if (contentsDataPtrList.empty())
			return DROPEFFECT_NONE;

		for (auto &contentsDataPtr : contentsDataPtrList)
		{
			m_contentsDataPtrQueue.push(contentsDataPtr);
			PostMessage(WMU_INSERT_CONTENTS_SYNCLIST);
		}

		return DROPEFFECT_COPY;
	}

	DROPEFFECT SyncCartListCtrl::OnDragEnter(COleDataObject* data)
	{
		return DROPEFFECT_COPY;
	}

	DROPEFFECT SyncCartListCtrl::OnDragOver(COleDataObject* data, int col, long row)
	{
		return DROPEFFECT_COPY;
	}

	void SyncCartListCtrl::OnMenuCommand(int col, long row, int section, int item)
	{
		switch (item)
		{
		case ID_MENU_DELETE:
		{
			DeleteContents(row);
		}
		break;
		}
	}

	int SyncCartListCtrl::OnMenuStart(int col, long row, int section)
	{
		EmptyMenu();

		if (section == UG_GRID && !m_isSynchronizing)
		{
			AddMenuItem(ID_MENU_DELETE, L"Delete");
		}

		return TRUE;
	}

	COLORREF SyncCartListCtrl::OnGetDefBackColor(int section)
	{
		return COLOR_IVORY_BLACK;
	}

	void SyncCartListCtrl::UpdateProgressGauge(const rsc::ContentsDataPtr data, const float targetRate)
	{
		auto it = std::find_if(m_contentsDataPtrList.begin(), m_contentsDataPtrList.end(), fn_findData(data));
		if (it == m_contentsDataPtrList.end())
			return;

		const int row = (it - m_contentsDataPtrList.begin());

		struct UpdateSyncProgress
		{
			UpdateSyncProgress(SyncCartListCtrl* ctrl_, const long row_, const float start, const float target_)
				: ctrl(ctrl_), row(row_), slider(start), target(target_) {}

			void operator()()
			{
				unsigned int sliderPercentage = 0;

				if (ctrl)
					ctrl->PostMessage(WMU_START_CONTENTS_SYNC_PROGRESS, (unsigned int)row);

				while (slider != target)
				{
					if (slider >= target)
						break;

					slider += speed;

					::Sleep(1);

					if (ctrl)
						ctrl->PostMessage(WMU_UPDATE_CONTENTS_SYNC_PROGRESS, (unsigned int)row, (unsigned int)(slider * 100.0f));
				}
			}

			SyncCartListCtrl* ctrl = nullptr;
			long row;
			float slider, target;
			float speed = 0.0005f;
		};

		struct Task_UpdateSyncProgress
		{
			void operator()(std::shared_ptr<UpdateSyncProgress> prog)
			{
				prog->operator()();
			}
		};
		m_thpool->push_back(Task_UpdateSyncProgress(), std::make_shared<UpdateSyncProgress>(this, row, m_currTargetProgressRate, targetRate));
		m_currTargetProgressRate = targetRate;
	}

	void SyncCartListCtrl::OnClose()
	{
		m_thpool->clear_tasks();
		m_thpool->join();
	}

	void SyncCartListCtrl::SetCurrSynchronizing(const bool syncOK)
	{
		m_isSynchronizing = syncOK;
	}

	void SyncCartListCtrl::OnDrawFocusRect(CDC *dc, RECT *rect)
	{}
}