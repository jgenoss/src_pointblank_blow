#if !defined( __I3_TDK_IMAGEBTN_CTRL_H)
#define __I3_TDK_IMAGEBTN_CTRL_H

#pragma once

/////////////////////////////////////////////////////////////////////////////
// i3TDKImageBtnCtrl window

class I3_EXPORT_TDK i3TDKImageBtnCtrl : public CButton
{
// Construction
public:
	i3TDKImageBtnCtrl();

// Attributes
public:
	CBitmap Image;
	CSize  ImageSize;
	BOOL	m_bTimeEvent = FALSE;

// Operations
public:
	void DrawButtonAsUp( HDC dc, LPRECT Rect);
	void DrawButtonAsDown( HDC dc, LPRECT Rect);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(i3TDKImageBtnCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~i3TDKImageBtnCtrl() {}
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDraw);
	BOOL LoadBitmap( UINT BitmapID, int CX, int CY);

	// Generated message map functions
protected:
	//{{AFX_MSG(i3TDKImageBtnCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEBUTTON_H__6117A104_3F6D_11D3_B5AD_0080C70D2C8F__INCLUDED_)
