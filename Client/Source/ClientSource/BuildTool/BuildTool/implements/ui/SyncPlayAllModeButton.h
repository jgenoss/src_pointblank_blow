#pragma once

#include "CustomButton.h"

namespace ui
{
	class SyncPlayerDlg;

	class SyncPlayAllModeButton
	{
	public:
		SyncPlayAllModeButton(SyncPlayerDlg& dlg);
		virtual ~SyncPlayAllModeButton();

		void DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
		void ToggleMode();
		bool IsPlayAllMode() const { return m_isPlayAllMode; }
		void MoveWindow(const RECT& rt);

	private:
		CustomButton m_button;
		CFont m_font;

		bool m_isPlayAllMode = false;
	};
}