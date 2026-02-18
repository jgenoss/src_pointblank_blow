#pragma once

#define			WM_TDK_UPDATEALPHA		(WM_USER + 5220)

// i3TDKAlphaPickCtrl

class I3_EXPORT_TDK i3TDKAlphaPickCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKAlphaPickCtrl)
protected:
	HDC				m_hBackDC = nullptr;
	HBITMAP			m_hOldBackBitmap = nullptr;
	HBITMAP			m_hAlphaTableBitmap = nullptr;
	HBITMAP			m_hPickBitmap = nullptr;
	HBITMAP			m_hPickMaskBitmap = nullptr;

	UINT32 *		m_pBits = nullptr;

	HDC				m_hDrawDC = nullptr;
	HBITMAP			m_hDrawBitmap = nullptr, m_hOldDrawBitmap = nullptr;

	bool			m_bDrag = false;
	INT32			m_Width = 0;
	INT32			m_Height = 0;

	INT32			m_SelectIndex = -1;

protected:
	void		_CreateAlphaTableBitmap(void);
	void		_ResizeDrawBitmap( INT32 cx, INT32 cy);
	void		_Draw(void);
	void		_Select( INT32 y);

	void		_getColorTableRect( CRect * pRect);

public:
	i3TDKAlphaPickCtrl() {}
	virtual ~i3TDKAlphaPickCtrl();

	BOOL	Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

	void	setColor( COLORREAL * pColor);
	
	REAL32	getAlpha(void)							{ return MAX(0.0f, (255 - m_SelectIndex) * (1.0f / 255.0f)); }

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


