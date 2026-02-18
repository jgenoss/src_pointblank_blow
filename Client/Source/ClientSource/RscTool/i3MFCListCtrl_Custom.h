#pragma once

#include "i3MFCListCtrl.h"

class i3MFCListCtrl_Custom;

struct i3MFCListCtrl_CustomDrawCallback
{

	virtual void OnPaint(i3MFCListCtrl_Custom* pCtrl, int first, int last) {}
	virtual ~i3MFCListCtrl_CustomDrawCallback() {}

};


class i3MFCListCtrl_Custom : public i3MFCListCtrl
{
	DECLARE_DYNAMIC(i3MFCListCtrl_Custom)
public:

	i3MFCListCtrl_Custom();
	virtual ~i3MFCListCtrl_Custom();
	
	void SetCallback_CustomDraw(i3MFCListCtrl_CustomDrawCallback* cb) { m_callback_customdraw = cb; }
	i3MFCListCtrl_CustomDrawCallback*	GetCallback_CustomDraw() const { return m_callback_customdraw; }
	
private:
	DECLARE_MESSAGE_MAP()

private:
	i3MFCListCtrl_CustomDrawCallback* m_callback_customdraw;

public:
	afx_msg void OnPaint();
};
