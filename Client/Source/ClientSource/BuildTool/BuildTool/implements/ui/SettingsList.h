#pragma once

#include "CustomUGCtrl.h"

namespace ui
{
	class TasksBar;

	class SettingsList
	{
	public:
		SettingsList(TasksBar* owner);
		virtual ~SettingsList();

	public:
		void AdjustLayout(const RECT& rt);
		void SetSelection(const int row);
		void ClearSelection();

	private:
		CustomUGCtrl m_ctrl;
		int m_currSelectionRow = -1;

		friend struct SettingsListCallback;
	};
}