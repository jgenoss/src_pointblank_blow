#include "stdafx.h"
#include "LoadingProgressMsgCallback.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	LoadingProgressMsgCallback::LoadingProgressMsgCallback()
	{
		m_dlg.Create(IDD_DIALOG_LOADING_PROGRESS, GetMainFrame());
		m_dlg.ShowWindow(SW_SHOW);
		/*m_splash.Create(GetMainFrame(), L"Saving", 2000, CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
		m_splash.SetBitmap(IDB_LOADING, 255, 0, 255);
		m_splash.SetTextFont(L"Impact", 280, CSS_TEXT_BOLD);
		m_splash.SetTextColor(RGB(0,0,0));
		m_splash.SetTextFormat(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		m_splash.Show();*/
	}

	LoadingProgressMsgCallback::~LoadingProgressMsgCallback()
	{
		//m_splash.Hide();
		m_dlg.DestroyWindow();
	}

	void LoadingProgressMsgCallback::OnTextMessage(const std::wstring& wstrText)
	{
		m_dlg.SetText(wstrText);
		//m_splash.SetText(wstrText.c_str());
	}

	void LoadingProgressMsgCallback::OnGaugeProgress(const int progress)
	{

	}

	void LoadingProgressMsgCallback::MoveWindow(const RECT& rt)
	{
		m_dlg.MoveWindow(&rt);
	}
}