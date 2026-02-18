#if !defined( __I3_TDK_DOCK_FRAME_WND_H)
#define __I3_TDK_DOCK_FRAME_WND_H

#include "i3TDKDockSplitWnd.h"

enum I3TDK_FOLDER_SIDE
{
	I3TDK_FOLDER_SIDE_LEFT = 0,
	I3TDK_FOLDER_SIDE_RIGHT = 1,
	I3TDK_FOLDER_SIDE_UP = 2,
	I3TDK_FOLDER_SIDE_DOWN = 3
};

class I3_EXPORT_TDK i3TDKDockFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE( i3TDKDockFrameWnd)
protected:
	CRect					m_ClientRect;
	i3TDKDockSplitWnd *			m_pDock = nullptr;

	I3TDK_FIND_DOCK_INFO		m_FindDockInfo;

	i3::vector<i3TDKDockWnd*>	m_FloatWndList;

	CWnd *					m_pDragWnd = nullptr;
	CRect					m_DragWndRect;
	CRect					m_OldDragRect;
	POINT					m_DragStartPt = { 0, 0 };
	HDC						m_DragDC = nullptr;
	bool					m_bDockDrag = false;

	i3::vector<CWnd*>		m_PaneList;
	i3::vector<CWnd*>		m_MovePaneList;			// Drag & Drop 등에서 이동 대상이 되는 Pane들의 List

protected:
	i3TDKDockWnd *	NewFloatWnd( INT32 defw, INT32 defh, RECT * pRect);
	void			DrawDragRect( RECT * pRect);
	void			_ExtractPanes( I3TDK_FIND_DOCK_INFO * pInfo);
	i3TDKDockWnd *	_PrepareDock( I3TDK_FIND_DOCK_INFO * pInfo);
	BOOL			_FindPaneByPos(void);
	INT32			_FindPaneInDockList( i3::vector<i3TDKDockWnd*>& List, CWnd * pWnd);
	void			_GarbageCollect(void);

	void			SaveDockContext(void);
	void			LoadDockContext(void);

public:
	i3TDKDockFrameWnd(void);
	virtual ~i3TDKDockFrameWnd(void) {}

	void	Reset(void);
	void	DockWindow( CWnd * pWnd, const TCHAR* pszLoc, INT32 w, INT32 h);

	void	BeginDock( CWnd * pWnd, I3TDK_DOCK_DROP_TYPE type, INT32 idx, POINT pt, INT32 defw, INT32 defh);
	void	DragDock( POINT pt, bool bFloat);
	void	EndDock( POINT pt, bool bFloat);
	void	CancelDock(void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnDestroy();
protected:
	virtual void PostNcDestroy();
};

extern i3TDKDockFrameWnd *		g_pDockFrameWnd;

#endif
