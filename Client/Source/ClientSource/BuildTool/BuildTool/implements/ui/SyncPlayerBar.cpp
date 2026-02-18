#include "stdafx.h"
#include "SyncPlayerBar.h"
#include "BuildTool.h"
#include "ui/UIDefines.h"
#include "resource/ResourceController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
#ifdef _DEBUG
	void SyncPlayerBar::AssertValid() const
	{
		CDockablePane::AssertValid();
	}

#ifndef _WIN32_WCE
	void SyncPlayerBar::Dump(CDumpContext& dc) const
	{
		CDockablePane::Dump(dc);
	}
#endif
#endif //_DEBUG

	IMPLEMENT_DYNCREATE(SyncPlayerBar, CDockablePane)

	BEGIN_MESSAGE_MAP(SyncPlayerBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_SETFOCUS()
	END_MESSAGE_MAP()

	SyncPlayerBar::SyncPlayerBar()
	{
	}

	SyncPlayerBar::~SyncPlayerBar()
	{
	}

	int SyncPlayerBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_dlg = std::make_unique<SyncPlayerDlg>(this);
		m_dlg->Create(IDD_DIALOG_SYNCPLAYER, this);

		CenterWindow();

		return 0;
	}

	void SyncPlayerBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);
		
		if (m_dlg.get())
			m_dlg->MoveWindow(0, 0, cx, cy);
	}

	void SyncPlayerBar::OnSetFocus(CWnd* pOldWnd)
	{
		CDockablePane::OnSetFocus(pOldWnd);

		if (m_dlg.get())
			m_dlg->SetFocus();
	}
}