#include "stdafx.h"
#include "SyncPlayButton.h"
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
	struct SyncButtonCallback : CustomButtonCallback
	{
		SyncButtonCallback(const std::function<void ()>& f) : m_f(f) {}
		virtual void OnClick(CustomButton* ctrl)
		{
			m_f();
		}
		std::function<void ()> m_f;
	};

	SyncPlayButton::SyncPlayButton(SyncPlayerDlg& dlg)
	{
		m_button.SubclassDlgItem(IDC_BUTTON_PLAY, &dlg);

		LOGFONT lf;	 dlg.GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = 32;
		m_font.CreateFontIndirect(&lf);
		m_button.SetFont(&m_font, TRUE);

		m_button.SetCallback(
			std::make_unique<SyncButtonCallback>(std::bind(&SyncPlayerDlg::OnBnClickedPlay, &dlg)));
	}

	SyncPlayButton::~SyncPlayButton()
	{
	}

	void SyncPlayButton::DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		CRect rt(lpDrawItemStruct->rcItem);

		pDC->SetBkMode(TRANSPARENT);

		pDC->FillSolidRect(rt, COLOR_BLACK);
		pDC->Draw3dRect(rt, COLOR_SKYBLUE, COLOR_SKYBLUE);

		if (m_isPaused)
		{
			pDC->SetTextColor(COLOR_SILVER);
			pDC->DrawText(L"||", &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		else
		{
			pDC->SetTextColor(COLOR_SKYBLUE);
			pDC->DrawText(L">", &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	void SyncPlayButton::SetPause(const bool ok)
	{
		m_isPaused = ok;
		m_button.Invalidate(TRUE);
	}

	void SyncPlayButton::MoveWindow(const RECT& rt)
	{
		m_button.MoveWindow(&rt);
	}
}