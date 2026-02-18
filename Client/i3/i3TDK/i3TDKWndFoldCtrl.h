#pragma once


// i3TDKWndFoldCtrl

#define			I3TDK_FOLD_STATE_COLLAPSED		0x00000001
#define			I3TDK_FOLD_STATE_FOCUSED		0x00000002

struct I3WNDFOLDINFO
{
	UINT32		m_State = 0;
	CWnd *		m_pWnd = nullptr;
	INT32		m_Width = 0;
	INT32		m_Height = 0;
	CRect		m_CaptionRect = { 0, 0, 0, 0 };
	CRect		m_Rect = { 0, 0, 0, 0 };

#ifdef _UNICODE
	i3::rc_wstring	m_strCaption;
#else
	i3::rc_string	m_strCaption;
#endif	
};

class I3_EXPORT_TDK i3TDKWndFoldCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKWndFoldCtrl)

protected:
	i3::vector<I3WNDFOLDINFO*>	m_WndList;

#ifdef _UNICODE
	i3::rc_wstring	m_strCaption;
#else
	i3::rc_string	m_strCaption;
#endif

	CScrollBar		m_HorzScr;
	CScrollBar		m_VertScr;
	INT32			m_ScrollPos = 0;

	CPoint			m_DragStartPt;
	INT32			m_DragStartScrollPos = 0;
	INT32			m_DragMode;					// [initialize at constructor]
	INT32			m_DragFoldIndex = 0;
	SIZE			m_DragStartSize = { 0, 0 };

	INT32			m_Width = 0, m_Height = 0;
	INT32			m_OldFocusFold = -1;
	bool			m_bSetTrackMouse = false;

	HFONT		m_hFont = nullptr;
	HBRUSH		m_hBrush = nullptr;

protected:
	void		FocusFold( INT32 idx);
	void		GetAllRect( RECT * pRect);
	void		GetCaptionRect( INT32 idx, RECT * pRect);
	void		RecalcScrollInfo(void);
	void		UpdateScrollInfo(void);
	void		AdjustPanePos(void);
	void		CalcPos(void);
	INT32		FindWndByPos( CPoint pt);
	void		DrawCaption( HDC dc, I3WNDFOLDINFO * pInfo);

	void		SetMouseCursor( HCURSOR hCursor);

public:
	i3TDKWndFoldCtrl() {}
	virtual ~i3TDKWndFoldCtrl();

	INT32		GetPaneCount(void)				{ return (INT32)m_WndList.size(); }
	CWnd *		GetPane( INT32 idx)
	{
		I3WNDFOLDINFO * pInfo = m_WndList[idx];

		return pInfo->m_pWnd;
	}

	INT32		FindPane( CWnd * pWnd);
	void		AddPane( CWnd * pWnd, const TCHAR * pszCaption);
	void		RemovePane( CWnd * pWnd);
	void		RemoveAllPane(void);

	BOOL	Create( const TCHAR * pszCaption, DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseLeave(void);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


