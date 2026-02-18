#include "stdafx.h"
#include "SyncForceCopyModeButton.h"
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
	struct SyncForceCopyModeButtonCallback : CustomButtonCallback
	{
		SyncForceCopyModeButtonCallback(const std::function<void()>& f) : m_f(f) {}
		virtual void OnClick(CustomButton* ctrl)
		{
			m_f();
		}
		std::function<void()> m_f;
	};

	SyncForceCopyModeButton::SyncForceCopyModeButton(SyncPlayerDlg& dlg)
	{
		m_button.SubclassDlgItem(IDC_BUTTON_FORCE_COPY_MODE, &dlg);

		LOGFONT lf;	 dlg.GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_MEDIUM;
		lf.lfHeight = 14;
		m_font.CreateFontIndirect(&lf);
		m_button.SetFont(&m_font, TRUE);

		m_isForceCopyMode = true;

		m_button.SetCallback(
			std::make_unique<SyncForceCopyModeButtonCallback>(std::bind(&SyncPlayerDlg::OnBnClickedForceCopyMode, &dlg)));
	}

	SyncForceCopyModeButton::~SyncForceCopyModeButton()
	{
	}

	void SyncForceCopyModeButton::DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		CRect rt(lpDrawItemStruct->rcItem);

		pDC->SetBkMode(TRANSPARENT);

		pDC->FillSolidRect(rt, COLOR_BLACK);
		pDC->Draw3dRect(rt, COLOR_BLACK, COLOR_BLACK);

		if (m_isForceCopyMode)
			pDC->SetTextColor(COLOR_SKYBLUE);
		else
			pDC->SetTextColor(COLOR_SILVER);

		CString caption;
		m_button.GetWindowText(caption);

		pDC->DrawText(caption, &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	void SyncForceCopyModeButton::ToggleMode()
	{
		m_isForceCopyMode = !m_isForceCopyMode;
	}

	void SyncForceCopyModeButton::MoveWindow(const RECT& rt)
	{
		m_button.MoveWindow(&rt);
	}
}