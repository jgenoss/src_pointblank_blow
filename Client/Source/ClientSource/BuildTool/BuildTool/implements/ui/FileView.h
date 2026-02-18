#pragma once

#include "RscTool/i3ShellTreeCtrl.h"
#include "RscTool/i3ShellListCtrl.h"

namespace ui
{
	class FileView : public CView
	{
		DECLARE_DYNCREATE(FileView)

	protected:
		FileView();
	public:
		virtual ~FileView();

	private:
		CBrush m_bkGndBrush;

		CSplitterWnd m_splitter;
		CFrameWndEx* m_frame;

		i3ShellTreeCtrl m_treeCtrl;
		i3ShellListCtrl m_listCtrl;

		bool m_isExplorerCreated = false;

	public:
		bool IsEsplorerCreated() { return m_isExplorerCreated; }
		void CreateExplorer();
		i3ShellListCtrl* GetListCtrl() { return &m_listCtrl; }
		
	public:
		virtual void OnDraw(CDC* pbDC);      // overridden to draw this view
#ifdef _DEBUG
		virtual void AssertValid() const;
#ifndef _WIN32_WCE
		virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	};
}