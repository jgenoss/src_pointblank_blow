#pragma once

// i3TDKColorPreviewCtrl

class I3_EXPORT_TDK i3TDKColorPreviewCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKColorPreviewCtrl)
protected:
	UINT32 *		m_pBits = nullptr;

	HDC				m_hDrawDC = nullptr;
	HBITMAP			m_hDrawBitmap = nullptr;

	INT32			m_Width = 0;
	INT32			m_Height = 0;

protected:
	void		_ResizeDrawBitmap( INT32 cx, INT32 cy);

public:
	i3TDKColorPreviewCtrl() {}
	virtual ~i3TDKColorPreviewCtrl();

	BOOL	Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

	void	setColor( COLORREAL * pColor);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


