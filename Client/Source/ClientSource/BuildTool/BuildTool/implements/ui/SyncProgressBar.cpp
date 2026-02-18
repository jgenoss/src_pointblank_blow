#include "stdafx.h"
#include "SyncProgressBar.h"
#include "BuildTool.h"
#include "SyncPlayerDlg.h"

namespace ui
{
	SyncProgressBar::SyncProgressBar(SyncPlayerDlg& dlg)
	{
		m_gauge.SubclassDlgItem(IDC_PROGRESS_SYNC, &dlg);
		SetWindowTheme(m_gauge.GetSafeHwnd(), L"", L"");
		m_gauge.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)COLOR_SKYBLUE);
		m_gauge.SendMessage(PBM_SETBKCOLOR, 0, (LPARAM)COLOR_BLACK);
		m_gauge.SetRange(0, 100);

		m_text.SubclassDlgItem(IDC_PROGRESS_SYNC_TEXT, &dlg);
		LOGFONT lf;	dlg.GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = 14;
		m_font.CreateFontIndirect(&lf);
		m_text.SetFont(&m_font, TRUE);
		m_text.SetText(L"");
	}

	SyncProgressBar::~SyncProgressBar()
	{}

	void SyncProgressBar::MoveWindow(const RECT& rt)
	{
		const LONG cy_half = LONG((rt.bottom - rt.top) * 0.5f);
		
		RECT rtGauge = rt;
		rtGauge.bottom = rtGauge.top + cy_half;
		m_gauge.MoveWindow(&rtGauge);

		RECT rtText;
		rtText.top = rtGauge.bottom;
		rtText.bottom = rt.bottom;
		rtText.left = rt.left;
		rtText.right = rt.right;
		m_text.MoveWindow(&rtText);
	}

	void SyncProgressBar::SetGaugePos(const int pos)
	{
		m_gauge.SetGaugePos(pos);
	}

	void SyncProgressBar::SetText(const wchar_t* text)
	{
		m_text.SetText(text);
	}
}