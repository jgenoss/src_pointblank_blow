#pragma once

#define	I3TDK_TAB_STATE_SELECTED		0x00000001
#define I3TDK_TAB_STATE_FOCUSED			0x00000002

// i3TDKTabCtrl
struct I3TDK_TAB_INFO
{
	UINT32		m_State = 0;
	CWnd *		m_pWnd = nullptr;
	CRect		m_TabRect = { 0, 0, 0, 0 };
} ;

class I3_EXPORT_TDK i3TDKTabCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKTabCtrl)

protected:
	i3::vector<I3TDK_TAB_INFO*>		m_WndList;
	INT32		m_Width = 0, m_Height = 0;
	INT32		m_OldSelectTab = -1;
	INT32		m_OldFocusTab = -1;
	HFONT		m_hTabFont = nullptr;
	bool		m_bSetTrackMouse = false;
	bool		m_bDrag = false;
	bool		m_bDockingMode = false;
	POINT		m_DragStartPt = { 0, 0 };

protected:
	void		GetTabArea( RECT * pRect);
	void		CalcTabSize( INT32 idx, RECT * pRect);
	void		CalcAllTabPos(void);
	void		GetTabRect( INT32 idx, RECT * pRect);
	void		GetPaneRect( RECT * pRect);
	void		DrawTab( HDC dc, I3TDK_TAB_INFO * pTab);
	void		FocusTab( INT32 idx);

public:
	void		AddPane( CWnd * pWnd);
	void		RemovePane( CWnd * pWnd);
	void		RemoveAllPane(void);
	void		SelectTab( INT32 idx);
	INT32		FindTabByPos( POINT pt);
	INT32		FindTab( CWnd * pWnd);
	INT32		GetPaneCount(void)				{ return (INT32)m_WndList.size(); }
	CWnd *		GetPane( INT32 idx)				
	{ 
		I3TDK_TAB_INFO * pInfo = m_WndList[idx];
		return pInfo->m_pWnd;
	}
	CWnd *		GetSelectedPane(void);
	void		MovePaneTo( i3TDKTabCtrl * pDest);

	void		SetDockMode( bool bFlag)		{ m_bDockingMode = bFlag; }

public:
	i3TDKTabCtrl() {}
	virtual ~i3TDKTabCtrl();

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave(void);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};


