#pragma once

#include <queue>

namespace ui
{
	class OutputBar : public CDockablePane
	{
	public:
		OutputBar();
		virtual ~OutputBar();

		// Attributes
	protected:
		CMFCTabCtrl	m_tabs;

		CListCtrl	m_outputList;
		// Operations
	public:

		// Overrides

		// Implementation
		void Log(const std::string& message);
		void ClearLog();

	protected:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		DECLARE_MESSAGE_MAP()

	private:
		CBrush m_bkGndBrush;
	};
}

