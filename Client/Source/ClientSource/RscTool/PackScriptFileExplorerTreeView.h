#pragma once


// PackScriptFileExplorerTreeView view

#include "i3shelltreectrl.h"


class PackScriptFileExplorerTreeView : public CView
{
	DECLARE_DYNCREATE(PackScriptFileExplorerTreeView)

public:

	void SetRelation(class PackScriptFileExplorerListView* pListView);


protected:
	PackScriptFileExplorerTreeView();           // protected constructor used by dynamic creation
	virtual ~PackScriptFileExplorerTreeView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	
	i3ShellTreeCtrl	m_wndShellTree;
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


