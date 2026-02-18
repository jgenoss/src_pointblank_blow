#pragma once

#define			WM_TDK_UPDATEPRIMARYCOLOR		(WM_USER + 5218)

// i3TDKPrimaryColorPickCtrl

class I3_EXPORT_TDK i3TDKPrimaryColorPickCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKPrimaryColorPickCtrl)
protected:
	HDC				m_hBackDC = nullptr;
	HBITMAP			m_hOldBackBitmap = nullptr;
	HBITMAP			m_hPrimaryColorTableBitmap = nullptr;
	HBITMAP			m_hPickBitmap = nullptr;
	HBITMAP			m_hPickMaskBitmap = nullptr;

	HDC				m_hDrawDC = nullptr;
	HBITMAP			m_hDrawBitmap, m_hOldDrawBitmap;

	bool			m_bDrag = false;
	INT32			m_Width = 0;
	INT32			m_Height = 0;

	UINT8			m_Color[258][3] = { 0 };

	INT32			m_SelectIndex = -1;

protected:
	void		_CreatePrimaryColorTableBitmap(void);
	void		_ResizeDrawBitmap( INT32 cx, INT32 cy);
	void		_Draw(void);
	void		_Select( INT32 y);

	void		_getColorTableRect( CRect * pRect);

public:
	i3TDKPrimaryColorPickCtrl() {}
	virtual ~i3TDKPrimaryColorPickCtrl();

	BOOL	Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

	void	setColor( COLORREAL * pColor);
	void	getPrimaryColor( COLORREAL * pColor);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


