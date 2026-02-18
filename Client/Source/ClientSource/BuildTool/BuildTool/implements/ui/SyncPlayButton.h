#pragma once

#include "CustomButton.h"

namespace ui
{
	class SyncPlayerDlg;

	class SyncPlayButton
	{
	public:
		SyncPlayButton(SyncPlayerDlg& dlg);
		virtual ~SyncPlayButton();

		void DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
		void SetPause(const bool ok);
		bool IsPaused() const			{ return m_isPaused; }
		void MoveWindow(const RECT& rt);

	private:
		CustomButton m_button;
		CFont m_font;
		bool m_isPaused = false;
	};
}