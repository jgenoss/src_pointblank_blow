#include "stdafx.h"
#include "SyncExportPefRscButton.h"
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
	struct SyncExportPefRscModeButtonCallback : CustomButtonCallback
	{
		SyncExportPefRscModeButtonCallback(const std::function<void()>& f) : m_f(f) {}
		virtual void OnClick(CustomButton* ctrl)
		{
			m_f();
		}
		std::function<void()> m_f;
	};

	SyncExportPefRscButton::SyncExportPefRscButton(SyncPlayerDlg& dlg)
	{
		m_button.SubclassDlgItem(IDC_BUTTON_EXPORT_PEF_RSC, &dlg);

		LOGFONT lf;	 dlg.GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_MEDIUM;
		lf.lfHeight = 13;
		m_font.CreateFontIndirect(&lf);
		m_button.SetFont(&m_font, TRUE);

		m_isExportMode = true;

		m_button.SetCallback(
			std::make_unique<SyncExportPefRscModeButtonCallback>(std::bind(&SyncPlayerDlg::OnBnClickedExportPefRsc, &dlg)));
	}

	SyncExportPefRscButton::~SyncExportPefRscButton()
	{
	}

	void SyncExportPefRscButton::DrawButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		CRect rt(lpDrawItemStruct->rcItem);

		pDC->SetBkMode(TRANSPARENT);

		pDC->FillSolidRect(rt, COLOR_BLACK);
		pDC->Draw3dRect(rt, COLOR_BLACK, COLOR_BLACK);

		if (m_isExportMode)
			pDC->SetTextColor(COLOR_SKYBLUE);
		else
			pDC->SetTextColor(COLOR_SILVER);

		CString caption;
		m_button.GetWindowText(caption);

		pDC->DrawText(caption, &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	void SyncExportPefRscButton::ToggleMode()
	{
		m_isExportMode = !m_isExportMode;
	}

	void SyncExportPefRscButton::DrawMode(const bool isExportMode)
	{
		m_isExportMode = isExportMode;
		m_button.Invalidate();
	}

	void SyncExportPefRscButton::MoveWindow(const RECT& rt)
	{
		m_button.MoveWindow(&rt);
	}
}