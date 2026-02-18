#pragma once


// CImageButton

class CImageButton : public CButton
{
	DECLARE_DYNAMIC(CImageButton)
private:
	CBitmap Image;
	CSize  ImageSize;
	BOOL	m_bTimeEvent;

public:
	CImageButton();
	virtual ~CImageButton();
	BOOL LoadBitmap( UINT BitmapID, int CX, int CY);	//Load Bitmap 
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDraw);
	void DrawButtonAsUp( HDC dc, LPRECT Rect);
	void DrawButtonAsDown( HDC dc, LPRECT Rect);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
};


