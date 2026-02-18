#pragma once

#include "ugctrl/UGCtrl.h"
#include "ugctrl/UGCTprog.h"
#include "ugctrl/UGCTbutn.h"
#include "resource/ContentsDefines.h"
#include <mutex>
#include <queue>
#include "UIDefines.h"
#include "util/ThreadPool.hpp"

namespace ui
{
	enum SYNCCARTLIST_COL_TYPE
	{
		SYNCCARTLIST_COL_ID,
		SYNCCARTLIST_COL_NAME,
		SYNCCARTLIST_COL_STATUS,
		SYNCCARTLIST_COL_MAX_COUNT
	};

	class SyncCartListCtrl : public CUGCtrl
	{
	public:
		SyncCartListCtrl();
		virtual ~SyncCartListCtrl();

	public:
		void InsertContents(rsc::ContentsDataPtr data);
		void DeleteContents(const int row);
		void ClearAllContents();

		void AdjustLayout(const RECT& rt);
		void GetContentsList(std::vector<rsc::ContentsDataPtr>& outList);

		void UpdateProgressGauge(const rsc::ContentsDataPtr data, const float targetRate);

		void OnClose();

		void SetCurrSynchronizing(const bool syncOK);

	protected:
		DECLARE_MESSAGE_MAP()
		virtual void OnSetup();
		virtual void OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed) override;
		virtual void OnRClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed) override;

		LRESULT OnInsertContents(WPARAM wparam, LPARAM lparam);
		LRESULT OnClearContents(WPARAM wparam, LPARAM lparam);
		LRESULT OnStartSyncProgress(WPARAM wparam, LPARAM lparam);
		LRESULT OnUpdateSyncProgress(WPARAM wparam, LPARAM lparam);
		LRESULT OnDeleteContents(WPARAM wparam, LPARAM lparam);

		virtual DROPEFFECT OnDragDrop(COleDataObject* data, int col, long row) override;
		virtual DROPEFFECT OnDragEnter(COleDataObject* data) override;
		virtual DROPEFFECT OnDragOver(COleDataObject* data, int col, long row) override;

		virtual void OnMenuCommand(int col, long row, int section, int item) override;
		virtual int	OnMenuStart(int col, long row, int section) override;

		virtual COLORREF OnGetDefBackColor(int section);
		virtual void OnDrawFocusRect(CDC *dc, RECT *rect);

	private:
		void _SetSelection(const int col, const int row);

		CUGProgressType		m_syncProgressBar;
		int					m_syncProgressBarIdx = 0;

		std::queue<rsc::ContentsDataPtr> m_contentsDataPtrQueue;	// MFC 메시지 큐잉 처리 용도.

		std::vector<rsc::ContentsDataPtr> m_contentsDataPtrList;

		std::unique_ptr<ThreadPool> m_thpool;

		float m_currTargetProgressRate = 0.0f;

		bool m_isSynchronizing = false;

		CPen m_cellBorderColorPen;
		CPen m_cellSelectionBorderColorPen;

		int m_currRow = -1;
	};
}