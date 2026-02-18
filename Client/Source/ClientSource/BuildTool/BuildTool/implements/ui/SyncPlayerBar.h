#pragma once

#include "SyncPlayerDlg.h"

namespace { struct ISyncPlayCallback; }

namespace ui
{
	class SyncPlayerBar : public CDockablePane
	{
		DECLARE_DYNCREATE(SyncPlayerBar)

	public:
		SyncPlayerBar();
		virtual ~SyncPlayerBar();

		SyncPlayerDlg& GetDlg() { return *(m_dlg.get()); }

	private:
		std::unique_ptr<SyncPlayerDlg> m_dlg;

	public:
#ifdef _DEBUG
		virtual void AssertValid() const;
#ifndef _WIN32_WCE
		virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
	};
}