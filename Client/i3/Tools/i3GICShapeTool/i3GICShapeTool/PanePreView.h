#pragma once

#include "Framework.h"

class CPanePreView : public CDockablePane
{
	DECLARE_DYNAMIC(CPanePreView)

public:

public:
	CPanePreView();
	virtual ~CPanePreView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point);
};

