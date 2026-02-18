#pragma once

#include "CustomButton.h"

namespace ui
{
	class SyncPlayerDlg;

	class SyncForceCopyModeButton
	{
	public:
		SyncForceCopyModeButton(SyncPlayerDlg& dlg);
		virtual ~SyncForceCopyModeButton();
		
		void DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
		void ToggleMode();
		bool IsForceCopyMode() const { return m_isForceCopyMode; }
		void MoveWindow(const RECT& rt);

	private:
		CustomButton m_button;
		CFont m_font;

		bool m_isForceCopyMode = false;
	};
}