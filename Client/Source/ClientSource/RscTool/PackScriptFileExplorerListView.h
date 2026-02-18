#pragma once


// PackScriptFileExplorerListView view
#include "i3shelllistctrl.h"

class PackScriptFileExplorerListView : public CView
{
	DECLARE_DYNCREATE(PackScriptFileExplorerListView)
public:

	i3ShellListCtrl*	GetListCtrl()  { return &m_wndList; }

protected:
	PackScriptFileExplorerListView();           // protected constructor used by dynamic creation
	virtual ~PackScriptFileExplorerListView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	i3ShellListCtrl	m_wndList;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


