#pragma once


// CModifiedListCtrl

class CModifiedListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CModifiedListCtrl)

protected:
	CFont			m_Font;

public:
	CModifiedListCtrl();
	virtual ~CModifiedListCtrl();

public:
	void	SetLogFont( const LOGFONT * pLogFont);
	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


