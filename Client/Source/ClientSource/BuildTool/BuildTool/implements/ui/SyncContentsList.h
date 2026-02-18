#pragma once

#include "resource/ContentsDefines.h"
#include "CustomUGCtrl.h"
#include "resource/ContentsSynchronizer.h"

namespace ui
{
	class SyncPlayerDlg;
	class SyncContentsList
	{
	public:
		SyncContentsList(SyncPlayerDlg& owner);
		virtual ~SyncContentsList();

	public:
		void AdjustLayout(const RECT& rt);

		void SetContentsID(const long row, const std::wstring& wstrID);
		void SetContentsName(const long row, const std::wstring& wstrName);
		void SetContentsStatus(const long row, const std::wstring& wstrStatus);

		void AddContents(const std::vector<rsc::ContentsSyncDataPtr>& dataList);
		void DelContents(const long row);
		void ClearContents();

		long GetCurrSelectionRow() const { return m_currSelectionRow; }

		void SetSyncHighlight(const long row, const bool ok);

	private:
		CustomUGCtrl m_ctrl;

		CPen m_penIvoryBlackLight;
		CPen m_penOrange;

		long m_currSelectionRow = -1;

		friend struct SyncContentsListCallback;
	};
}