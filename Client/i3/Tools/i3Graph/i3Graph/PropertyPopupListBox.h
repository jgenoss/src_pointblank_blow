#pragma once


// CPropertyPopupListBox

class CPropertyPopupListBox : public CWnd
{
	DECLARE_DYNAMIC(CPropertyPopupListBox)
private :
	//For Member Value 
	CWnd*		ParentWnd;
	CListBox	List;

public:
	//For Member Value

public:
	CPropertyPopupListBox();
	virtual ~CPropertyPopupListBox();
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

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnCommand( WPARAM wParam, LPARAM lParam);
};


