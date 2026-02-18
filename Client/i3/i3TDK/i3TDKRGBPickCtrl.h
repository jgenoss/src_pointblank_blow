#pragma once

#define		WM_TDK_SELECTCOLOR			(WM_USER + 5219)
// i3TDKRGBPickCtrl

class I3_EXPORT_TDK i3TDKRGBPickCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKRGBPickCtrl)
protected:
	COLORREAL		m_Color = { 1.0f, 0.5f, 0.5f, 0.5f };
	COLORREAL		m_BaseColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	INT32			m_PickX = 0, m_PickY = 0;

	HDC				m_hBackDC = nullptr;
	HBITMAP			m_hDrawBitmap = nullptr;
	UINT8 *			m_pBits = nullptr;
	BITMAPINFO		m_BMPInfo;

	bool			m_bDrag = false;

protected:
	void			_CreateBitmap(void);
	void			_BuildBitmap(void);

	void			_Pick( INT32 x, INT32 y);

public:
	i3TDKRGBPickCtrl();
	virtual ~i3TDKRGBPickCtrl();

	BOOL		Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT32 id);

	void		setColor( COLORREAL * pColor);
	COLORREAL *	getColor(void)						{ return &m_Color; }

	void		setBaseColor( COLORREAL * pColor);
	COLORREAL *	getBaseColor(void)					{ return &m_BaseColor; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


