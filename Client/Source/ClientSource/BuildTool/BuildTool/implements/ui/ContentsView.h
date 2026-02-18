#pragma once

#include "ContentsViewList.h"
#include "resource/ContentsDefines.h"
#include "ContentsViewToolbar.h"
#include <mutex>

namespace ui
{
	class SyncPlayerDlg;

	struct IContentsViewCallback
	{
		virtual void OnContentsClick(const long row) {}
		virtual void OnAddContents(const long row) {}
		virtual ~IContentsViewCallback() {}
	};

	using ContentsViewListCallbackPtr = std::unique_ptr<IContentsViewCallback>;

	class ContentsView : public CView
	{
		DECLARE_DYNCREATE(ContentsView)

	protected:
		ContentsView();           // protected constructor used by dynamic creation
		virtual ~ContentsView();

	public:
		void AddContents(const std::vector<rsc::ContentsDataPtr>& contentsDataList);
		const rsc::ContentsDataPtr& GetContents(const long row) const;
		void ClearContents();
		void UpdateContents(const std::vector<rsc::ContentsDataPtr>& contentsDataList);

		void SetToolbarCountryName(const std::wstring& wstrCountryName);
		const std::wstring& GetCountryName() const { return m_wstrCountryName; }

		void					SetCallback(ContentsViewListCallbackPtr cb) { m_callback = std::move(cb); }
		IContentsViewCallback*	GetCallback() const { return m_callback.get(); }

	private:
		std::unique_ptr<ContentsViewList> m_contentsListCtrl;
		ContentsViewToolbar m_toolbar;

		std::mutex m_mtx;
		std::vector<rsc::ContentsDataPtr> m_contentsDataAddQ;
		std::vector<rsc::ContentsDataPtr> m_contentsDataUpdateQ;
		std::vector<rsc::ContentsDataPtr> m_contentsDataList;
		CBrush m_bkGndBrush;

		std::wstring m_wstrCountryName;

		ContentsViewListCallbackPtr m_callback;

	public:
		virtual void OnDraw(CDC* pbDC);      // overridden to draw this view
#ifdef _DEBUG
		virtual void AssertValid() const;
#ifndef _WIN32_WCE
		virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		LRESULT OnAddContents(WPARAM wparam, LPARAM lparam);
		LRESULT OnClearContents(WPARAM wparam, LPARAM lparam);
		LRESULT OnUpdateContents(WPARAM wparam, LPARAM lparam);

		friend struct ContentsViewListCallback;
		friend struct CustomUGCtrlCallback;
	};
}