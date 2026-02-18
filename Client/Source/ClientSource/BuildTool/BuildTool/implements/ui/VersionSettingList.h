#pragma once

#include "CustomUGCtrl.h"
#include "resource/Settings.h"
#include "ugctrl/ugctelps.h"

namespace ui
{
	class VersionSettingDlg;

	class VersionSettingList
	{
	public:
		VersionSettingList(VersionSettingDlg& owner);
		virtual ~VersionSettingList();

		void AdjustLayout(RECT& rt);
		void GetClientRect(RECT* rt);

		long AddNewRow();
		void SetVersionData(const long row, const rsc::VerDataPtr& data);

		void GetAllDataList(std::vector<rsc::VersionData>& out);

	private:
		CustomUGCtrl m_ctrl;

		CUGEllipsisType m_ellipType;

		CPen m_penIvoryBlack, m_penIvoryBlackLight, m_penOrange;
		long m_currSelectionRow = 0;
		long m_currSelectionCol = 0;

		friend struct VersionSettingListCallback;
	};
}