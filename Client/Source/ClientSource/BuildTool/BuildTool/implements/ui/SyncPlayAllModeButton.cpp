#include "stdafx.h"
#include "SyncPlayAllModeButton.h"
#include "BuildTool.h"
#include "SyncPlayerDlg.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	struct SyncPlayAllModeButtonCallback : CustomButtonCallback
	{
		SyncPlayAllModeButtonCallback(const std::function<void()>& f) : m_f(f) {}
		virtual void OnClick(CustomButton* ctrl)
		{
			m_f();
		}
		std::function<void()> m_f;
	};

	SyncPlayAllModeButton::SyncPlayAllModeButton(SyncPlayerDlg& dlg)
	{
		m_button.SubclassDlgItem(IDC_BUTTON_PLAY_ALL_MODE, &dlg);

		LOGFONT lf;	 dlg.GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_MEDIUM;
		lf.lfHeight = 14;
		m_font.CreateFontIndirect(&lf);
		m_button.SetFont(&m_font, TRUE);

		m_isPlayAllMode = true;

		m_button.SetCallback(
			std::make_unique<SyncPlayAllModeButtonCallback>(std::bind(&SyncPlayerDlg::OnBnClickedPlayAllMode, &dlg)));
	}

	SyncPlayAllModeButton::~SyncPlayAllModeButton()
	{
	}

	void SyncPlayAllModeButton::DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		CRect rt(lpDrawItemStruct->rcItem);

		pDC->SetBkMode(TRANSPARENT);

		pDC->FillSolidRect(rt, COLOR_BLACK);
		pDC->Draw3dRect(rt, COLOR_BLACK, COLOR_BLACK);

		if (m_isPlayAllMode)
			pDC->SetTextColor(COLOR_SKYBLUE);
		else
			pDC->SetTextColor(COLOR_SILVER);

		CString caption;
		m_button.GetWindowText(caption);

		pDC->DrawText(caption, &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	void SyncPlayAllModeButton::ToggleMode()
	{
		m_isPlayAllMode = !m_isPlayAllMode;
	}

	void SyncPlayAllModeButton::MoveWindow(const RECT& rt)
	{
		m_button.MoveWindow(&rt);
	}
}