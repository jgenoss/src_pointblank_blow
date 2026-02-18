#pragma once

// i3TDKColorCtrl

#define		WM_TDK_SETCOLOR			(WM_USER + 1002)

class I3_EXPORT_TDK i3TDKColorCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKColorCtrl)
protected:
	COLORREAL		m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	HBRUSH			m_hBrush = nullptr;

public:
	i3TDKColorCtrl() {}
	virtual ~i3TDKColorCtrl();

	BOOL	Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

	void		setColor( COLORREAL * pColor);
	void		setColor( COLOR * pCol);
	COLORREAL *	getColor(void)						{ return &m_Color; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


