#pragma once

#include "i3TDKTabCtrl.h"

#define		DOCK_SPACE		16

enum I3TDK_DOCK_PART
{
	I3TDK_DOCK_PART_NONE = -1,
	I3TDK_DOCK_PART_LEFT = 0,
	I3TDK_DOCK_PART_TOP,
	I3TDK_DOCK_PART_RIGHT,
	I3TDK_DOCK_PART_BOTTOM,
	I3TDK_DOCK_PART_IN,
};

// i3TDKDockWnd

#define	I3TDK_DOCK_STATE_DOCKED			0x00000001
#define	I3TDK_DOCK_STATE_FOCUSED		0x00000002

class I3_EXPORT_TDK i3TDKDockWnd : public CWnd
{
	DECLARE_DYNAMIC(i3TDKDockWnd)

protected:
	UINT32			m_State = 0;
	HFONT			m_hCaptionFont = nullptr;
	bool			m_bSetTrackMouse = false;
	i3TDKTabCtrl	m_TabCtrl;

	INT32			m_Width = 0, m_Height = 0;
	INT32			m_DefWidth = 0, m_DefHeight = 0;
	CRect			m_WindowRect;
	CRect			m_LeftBorderRect;
	CRect			m_RightBorderRect;
	CRect			m_BottomBorderRect;
	CRect			m_SizeGripRect;

protected:
	void		GetCaptionArea( RECT * pRect, bool bScreenCoord = false);
	void		GetPaneRect( RECT * pRect);
	void		DrawCaption( HDC dc);
	void		DrawBorder( HDC dc);

	void		UpdateSize(void);

public:

public:
	i3TDKDockWnd() {}
	virtual ~i3TDKDockWnd();

	BOOL	Create( DWORD dwStyle, INT32 defw, INT32 defh, const RECT &rect, CWnd * pParentWnd, UINT nID);

	void	AddPane( CWnd * pWnd);
	void	RemovePane( CWnd * pWnd);
	void	RemoveAllPane(void);
	INT32	GetPaneCount(void)				{ return m_TabCtrl.GetPaneCount(); }
	CWnd *	GetPane( INT32 idx)				{ return m_TabCtrl.GetPane( idx); }
	INT32	FindPane( CWnd * pWnd)			{ return m_TabCtrl.FindTab( pWnd); }
	void	MovePaneTo( i3TDKDockWnd * pDest);

	UINT32	GetState(void)					{ return m_State; }
	void	SetState( UINT32 state)			{ m_State = state; }
	void	AddState( UINT32 state)			{ m_State |= state; }
	void	RemoveState( UINT32 state)		{ m_State &= ~state; }
	bool	IsDocked(void)					{ return (m_State & I3TDK_DOCK_STATE_DOCKED) != 0; }

	void	SaveContext( char * pszStr);
	void	LoadContext( char * pszStr);

	I3TDK_DOCK_PART		HitTestRect( POINT pt);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave(void);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);

	#if _MSC_VER >= 1400
		afx_msg LRESULT OnNcHitTest(CPoint point);
	#else
		afx_msg UINT	OnNcHitTest( CPoint point);
	#endif
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};


