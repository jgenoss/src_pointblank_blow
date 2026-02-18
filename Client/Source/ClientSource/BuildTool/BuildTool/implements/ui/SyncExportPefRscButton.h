#pragma once

#include "CustomButton.h"

namespace ui
{
	class SyncPlayerDlg;

	class SyncExportPefRscButton
	{
	public:
		SyncExportPefRscButton(SyncPlayerDlg& dlg);
		virtual ~SyncExportPefRscButton();

		void DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
		void ToggleMode();
		void DrawMode(const bool isExportMode);
		bool IsExportMode() const { return m_isExportMode; }
		void MoveWindow(const RECT& rt);

	private:
		CustomButton m_button;
		CFont m_font;

		bool m_isExportMode = false;
	};
}
