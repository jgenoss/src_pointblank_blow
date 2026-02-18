#pragma once

#include "i3PackToolDlg.h"

namespace ui
{
	class i3PackToolDlg;

	class i3PackToolBar : public CDockablePane
	{
	public:
		i3PackToolBar();
		virtual ~i3PackToolBar();

		i3PackToolDlg* Geti3PackToolDlg() const { return m_dlg.get(); }

	private:
		CBrush m_bkGndBrush;
		std::unique_ptr<i3PackToolDlg> m_dlg;

	protected:
		DECLARE_MESSAGE_MAP();
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	};
}