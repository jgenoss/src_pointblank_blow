#include "stdafx.h"
#include "SyncButton.h"
#include "Resource.h"
#include "resource/ResourceController.h"
#include <boost/filesystem.hpp>

namespace ui
{
#ifdef _DEBUG
	void SyncButton::AssertValid() const
	{
		CButton::AssertValid();
	}

#ifndef _WIN32_WCE
	void SyncButton::Dump(CDumpContext& dc) const
	{
		CButton::Dump(dc);
	}
#endif
#endif //_DEBUG

	IMPLEMENT_DYNCREATE(SyncButton, CButton)

	BEGIN_MESSAGE_MAP(SyncButton, CButton)
		ON_WM_CREATE()
	END_MESSAGE_MAP()

	SyncButton::SyncButton()
	{
	}
	
	SyncButton::~SyncButton()
	{
	}

	int SyncButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CButton::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_font.CreateFont(16, 0, 0, 0, FW_BOLD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, L"Arial");
		SetFont(&m_font, TRUE);
		EnableWindow(TRUE);

		return 0;
	}

	void SyncButton::OnClicked()
	{
		rsc::ResourceController::i()->OnSyncButtonClicked();
	}

	void SyncButton::SetButtonEnable(const bool enable)
	{
		m_enable = enable;
	}

	bool SyncButton::GetButtonEnable() const
	{
		return m_enable;
	}

	void SyncButton::AdjustLayout(const RECT& rt)
	{
		MoveWindow(&rt);
	}
}