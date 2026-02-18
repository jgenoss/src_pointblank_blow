#pragma once

#include "i3Base.h"
#include "i3TDKWindowInfo.h"
#include "i3TDKWndFoldCtrl.h"
#include "i3TDKDockWnd.h"

#define I3TDK_DOCK_STYLE_VERT				0x00000001

enum I3TDK_DOCK_DROP_TYPE
{
	I3TDK_DOCK_DROP_WHOLE = 0,			// 모든 Dock을 한꺼번에 이동시킨다.
	I3TDK_DOCK_DROP_PANE,				// 개별 Pane만 이동시킨다.
};

struct I3TDK_FIND_DOCK_INFO
{
	I3TDK_DOCK_DROP_TYPE	m_DropType = I3TDK_DOCK_DROP_WHOLE;
	CWnd *					m_pDragWnd = nullptr;
	INT32					m_PaneIndex = 0;			// I3TDK_DOCK_DROP_PANE의 경우 Index.
	POINT					m_Point = { 0, 0 };
	CWnd *					m_pWnd = nullptr;
	CWnd *					m_pParent = nullptr;
	I3TDK_DOCK_PART			m_Hit = I3TDK_DOCK_PART_NONE;
} ;

// i3TDKDockSplitWnd

class I3_EXPORT_TDK i3TDKDockSplitWnd : public CWnd
{
	DECLARE_DYNAMIC(i3TDKDockSplitWnd)

protected:
	CWnd *				m_pPane[2] = { nullptr, nullptr };

	CRect				m_OldRect[2] = { {0, 0, 0, 0}, {0, 0, 0, 0} };

	INT32				m_SplitPos = -1;
	UINT32				m_ExStyle = 0;
	INT32				m_MinSize = 10;

	BOOL				m_bDragging = FALSE;
	CRect				m_DragRect = { 0, 0, 0, 0 };
	HDC					m_ScreenDC = nullptr;
	INT32				m_Width = 0, m_Height = 0;

protected:
	virtual void		CalcLayout( int cx, int cy, CRect * pRect1, CRect * pRect2);
	virtual void		CalcSplitter( int cx, int cy, int newSz, CRect * pRect);

	void				getRect( INT32 side, INT32 w, INT32 h, CRect * pRect);

	void				SetChildAttr( CWnd * pWnd);

	void				ReplacePanes( int cx, int cy, bool bRecurse = false);
	void				DragStart( CPoint pt);
	void				Drag( CPoint pt);
	void				DragEnd( CPoint pt);

	void				_PreparePath( const TCHAR* pszLoc, INT32 w, INT32 h);
	i3TDKDockWnd *		_PrepareDockWnd( INT32 side, INT32 w, INT32 h);
	i3TDKDockSplitWnd *	_FindPath( const TCHAR* pszLoc);
	i3TDKDockSplitWnd *	_SplitDown( INT32 side, INT32 w, INT32 h, bool bHorz);
	i3TDKDockSplitWnd *	InsertChildDock( INT32 side, INT32 w, INT32 h, bool bHorz);
	i3TDKDockSplitWnd *	NewDockWnd( bool bHorz, INT32 w, INT32 h);
	void				_collapseChildSplit( INT32 idx);
	void				_collapseChildDock( INT32 idx);

	BOOL				IsEmpty(void)
	{
		return (m_pPane[0] == nullptr) || (m_pPane[1] == nullptr);
	}

	bool				IsHorz(void)
	{
		return (m_ExStyle & I3TDK_DOCK_STYLE_VERT) == 0;
	}

	void				SetWindowInfo( CWnd * pWnd, i3TDKWindowInfo * pInfo);

public:
	i3TDKDockSplitWnd() {}
	virtual ~i3TDKDockSplitWnd();

	BOOL Create(	UINT32 dwExStyle, UINT32 dwStyle, const RECT & rect, UINT32 MinSz, 
					CWnd * pParentWnd, UINT32 nID, CCreateContext * pContext = nullptr);

	INT32			GetPaneCount(void);
	CWnd *			GetPane( INT32 idx)					{ return m_pPane[idx]; }
	void			SetPane( INT32 idx, CWnd * pPane, INT32 w, INT32 h);
	void			RemovePane( INT32 idx);
	void			RemovePane( CWnd * pPane);
	void			RemoveAllPane(void);
	void			GetPanePath( CWnd * pPane, TCHAR* pszStr);		// 1차 Child에 대한 Path만을 구한다.
	BOOL			FindPanePath( CWnd * pPane, TCHAR* pszStr);		// Descendents들을 모두 검색해 Path를 구한다.

	void			GetAllDockWnd( i3::vector<i3TDKDockWnd*>& List);


	I3TDK_DOCK_PART		HitTestRect( POINT pt);
	BOOL				FindPaneByPos( i3TDKDockSplitWnd * pParent, I3TDK_FIND_DOCK_INFO * pInfo);
	BOOL				AdjustPaneSize( INT32 level, CWnd * pChild, INT32 wishcx, INT32 wishcy);

	i3TDKDockWnd *	PreparePath( const TCHAR* pszLoc, INT32 w, INT32 h);
	void			UndockWindow( CWnd * pWnd);
	void			GarbageCollect(void);
	void			Rearrange(void);

	void			DumpPath( INT32 level);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
};


