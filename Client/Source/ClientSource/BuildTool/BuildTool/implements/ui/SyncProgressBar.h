#pragma once

#include "CustomProgressCtrl.h"
#include "CustomStatic.h"

namespace ui
{
	class SyncPlayerDlg;

	class SyncProgressBar
	{
	public:
		SyncProgressBar(SyncPlayerDlg& dlg);
		virtual ~SyncProgressBar();

		void MoveWindow(const RECT& rt);
		void SetGaugePos(const int pos);
		void SetText(const wchar_t* text);

	private:
		CustomProgressCtrl m_gauge;
		CustomStatic m_text;
		CFont m_font;
	};
}