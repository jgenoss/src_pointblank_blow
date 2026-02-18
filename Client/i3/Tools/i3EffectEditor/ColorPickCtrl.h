#pragma once
#include "i3Math.h"

// CColorPickCtrl

class CColorPickCtrl : public CWnd
{
	DECLARE_DYNAMIC(CColorPickCtrl)
protected:
	COLOR		m_PickedColor;

public:
	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);
	COLOR *	GetPickedColor(void)				{ return & m_PickedColor; }

public:
	CColorPickCtrl();
	virtual ~CColorPickCtrl();

protected:
	DECLARE_MESSAGE_MAP()
};


