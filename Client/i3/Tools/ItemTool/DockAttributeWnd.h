#pragma once

#include "DockBaseWnd.h"

class cDockAttributeWnd : public cDockBaseWnd
{
	DECLARE_DYNAMIC(cDockAttributeWnd)

public:
	cDockAttributeWnd();
	virtual ~cDockAttributeWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
};



