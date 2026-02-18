#pragma once

#include "DockBaseWnd.h"
#include "ViewTree.h"

class cDockSceneGraphWnd : public cDockBaseWnd
{
	DECLARE_DYNAMIC(cDockSceneGraphWnd)

	

public:
	CStatic m_static;
	cViewTree m_SceneViewTree;
public:
	cDockSceneGraphWnd();
	virtual ~cDockSceneGraphWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
};


