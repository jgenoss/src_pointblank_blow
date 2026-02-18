#if !defined( __I3_TDK_PROPERTY_TOOLTIP_CTRL_H)
#define __I3_TDK_PROPERTY_TOOLTIP_CTRL_H

#pragma once

/////////////////////////////////////////////////////////////////////////////
// i3TDKPropertyToolTipCtrl window

class I3_EXPORT_TDK i3TDKPropertyToolTipCtrl : public CToolTipCtrl
{
// Construction
public:
	i3TDKPropertyToolTipCtrl() {}
	virtual ~i3TDKPropertyToolTipCtrl() {}

	CFont Font;
	int Width = 0, Height = 0;
	COLORREF TextColor = RGB(0, 0, 0), BackColor = RGB(255, 255, 220);
	BOOL Showed = FALSE;
	CWnd *pOwner = nullptr;
	TCHAR m_Buffer[256] = { 0 };

// Attributes
public:

// Operations
public:
	BOOL Create( CWnd *pParentWnd, UINT Style = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(i3TDKPropertyToolTipCtrl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(i3TDKPropertyToolTipCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetTextFont( const LOGFONT *logFont);
	void SetText( CWnd *pParent, const TCHAR* Buf);
	void Show( int x = -1, int y = -1);
	void Hide(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYTOOLTIP_H__F26ACC8B_7FDE_4805_90B8_3A89DD9057E4__INCLUDED_)
