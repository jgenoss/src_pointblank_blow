#pragma once


// CInputDirectionCtrl

class CInputDirectionCtrl : public CWnd
{
	DECLARE_DYNAMIC(CInputDirectionCtrl)

public:
	POINT	m_StartPoint;
	POINT	m_EndPoint;

	VEC2D	m_Direction;
	bool	m_bDrag;

protected:

public:
	CInputDirectionCtrl();
	virtual ~CInputDirectionCtrl();

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


