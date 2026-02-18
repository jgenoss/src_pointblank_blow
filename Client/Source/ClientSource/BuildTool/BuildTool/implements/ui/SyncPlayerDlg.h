#pragma once

#include "SyncContentsList.h"
#include "SyncForceCopyModeButton.h"
#include "SyncPlayAllModeButton.h"
#include "SyncExportPefRscButton.h"
#include "SyncPlayButton.h"
#include "SyncProgressBar.h"
#include <mutex>
#include "CustomStatic.h"
#include "resource/ContentsSynchronizer.h"

namespace ui
{
	struct IRequestSyncCallback;
	struct ISyncPlayState;

	class SyncPlayerDlg : public CDialogEx
	{
		DECLARE_DYNAMIC(SyncPlayerDlg)

	public:
		SyncPlayerDlg(CWnd* pParent = NULL);
		virtual ~SyncPlayerDlg();

		void AddContents(const std::vector<rsc::ContentsSyncDataPtr>& contentsDataList);
		void DelContents(const std::vector<long>& rows);
		void ClearContents();
		void Reset();

		void OnPreSync(const std::string& strCountryName);
		void OnSync(const float progress);
		void OnPostSync(const bool success);

	private:
		void SetSyncPlayState(ISyncPlayState& callback);
		void SetReqSyncCallback(IRequestSyncCallback& cb);
		IRequestSyncCallback& GetReqSyncCallback() const;
		size_t GetTotalNumOfContents() const;
		const rsc::ContentsSyncDataPtr GetContentsData(const long row) const;
		void SetCurrSyncRow(const long row);
		long GetCurrSyncRow() const { return m_currSyncRow; }
		void ShowSyncContentsInfo(const long row, const std::string& wstrCountryName);

		CBrush m_bkGndBrush;
		CBrush m_bkGndBottomBrush;

		CustomStatic m_textContentsName;
		CFont m_fontContentsName;

		CustomStatic m_textCountryName;
		CFont m_fontCountryName;

		CustomStatic m_bkGndBottom;

		std::unique_ptr<SyncForceCopyModeButton> m_forceCopyModeButtonCtrl;
		std::unique_ptr<SyncPlayAllModeButton> m_playAllModeButtonCtrl;
		std::unique_ptr<SyncExportPefRscButton> m_exportPefRscButtonCtrl;
		std::unique_ptr<SyncPlayButton> m_playButtonCtrl;
		std::unique_ptr<SyncContentsList> m_contentsListCtrl;
		std::unique_ptr<SyncProgressBar> m_progressBarCtrl;

		ISyncPlayState* m_currStateCallback = nullptr;

		std::mutex m_mtx;
		std::vector<rsc::ContentsSyncDataPtr> m_contentsDataAddQ;
		std::vector<rsc::ContentsSyncDataPtr> m_contentsDataDelQ;
		std::vector<rsc::ContentsSyncDataPtr> m_contentsDataList;

		long m_currSyncRow = -1;

		IRequestSyncCallback* m_reqSyncCallback = nullptr;

#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_DIALOG_SYNCPLAYER };
#endif

	public:
		virtual void DoDataExchange(CDataExchange* pDX);
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
		afx_msg void OnBnClickedOK() {}
		afx_msg void OnBnClickedCancel() {}
		void OnBnClickedForceCopyMode();
		void OnBnClickedPlayAllMode();
		void OnBnClickedExportPefRsc();
		void OnBnClickedPlay();
		afx_msg LRESULT OnAddContents(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnDelContents(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClearContents(WPARAM wparam, LPARAM lparam);

		friend class SyncPlayButton;
		friend class SyncPlayAllModeButton;
		friend class SyncForceCopyModeButton;
		friend class SyncExportPefRscButton;

		friend struct SyncState_OnReadyPlay;
		friend struct SyncState_OnPlay;
		friend struct SyncState_OnReadyPlayAll;
		friend struct SyncState_OnPlayAll;

		friend struct RequestSync;
		friend struct RequestSyncSequentially;
		friend struct RequestSyncRandomly;
	};
}
