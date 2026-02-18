#if !defined( __I3_TDK_POPUP_LISTBOX_CTRL_H)
#define __I3_TDK_POPUP_LISTBOX_CTRL_H

#pragma once

/////////////////////////////////////////////////////////////////////////////
// i3TDKPopupListBoxCtrl window

class I3_EXPORT_TDK i3TDKPopupListBoxCtrl : public CWnd
{
// Construction
public:
	i3TDKPopupListBoxCtrl();

// Attributes
public:
	CListBox List;
	CWnd * ParentWnd = nullptr;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(i3TDKPopupListBoxCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~i3TDKPopupListBoxCtrl() {}
	void SetFont( CFont *pFont, BOOL bRedraw = TRUE);

	BOOL Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID);
	int GetCount(void);
	int GetHorizontalExtent(void);
	int GetText( int nIndex, LPTSTR strBuf);
	int GetCurSel(void);
	int SetCurSel( int nSelect);
	int AddString( LPCTSTR lpszText);
	void ResetContent(void);
	int SelectString( int nStartAfter, LPCTSTR lpszItem);
	// Generated message map functions
protected:
	//{{AFX_MSG(i3TDKPopupListBoxCtrl)
	 	// NOTE - the ClassWizard will add and remove member functions here.
		afx_msg void OnSize( UINT nType, int cx ,int cy);
//		afx_msg BOOL OnCommand( WPARAM wParam, LPARAM lParam);
		//afx_msg void OnSetFocus( CWnd * pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPUPLISTBOX_H__6117A109_3F6D_11D3_B5AD_0080C70D2C8F__INCLUDED_)
