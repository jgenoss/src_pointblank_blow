#pragma once

#include "i3Base.h"
#include "i3TDKWindowInfo.h"

#define I3_SPLS_EX_VERT				0x00000001

// i3TDKSplitterWnd

class I3_EXPORT_TDK i3TDKSplitterWnd : public CWnd
{
	DECLARE_DYNAMIC(i3TDKSplitterWnd)

protected:
	CWnd *				m_pPane[2] = { nullptr, nullptr };

	CRect				m_OldRect[2] = { { 0, 0, 0, 0 }, {0, 0, 0, 0} };

	UINT32				m_Size = 0;
	UINT32				m_ExStyle = 0;
	UINT32				m_MinSize = 10;

	BOOL				m_bDragging = FALSE;
	CRect				m_DragRect;
	HDC					m_ScreenDC = nullptr;
	INT32				m_Width = 0, m_Height = 0;

	HCURSOR				m_hCursorSize = nullptr;
	HCURSOR				m_hCursorArrow = nullptr;

protected:
	virtual void		CalcLayout( int cx, int cy, CRect * pRect1, CRect * pRect2);
	virtual void		CalcSplitter( int cx, int cy, int newSz, CRect * pRect);

	void				ReplacePanes( int cx, int cy);
	void				DragStart( CPoint pt);
	void				Drag( CPoint pt);
	void				DragEnd( CPoint pt);

	void				GarbageCollect(void);

	void				SetWindowInfo( CWnd * pWnd, i3TDKWindowInfo * pInfo);
	virtual BOOL		OnSaveWindowInfo( i3TDKWindowInfo * pInfo);
	virtual BOOL		OnLoadWindowInfo( i3TDKWindowInfo * pInfo);

public:
	i3TDKSplitterWnd() {}
	virtual ~i3TDKSplitterWnd();

	BOOL Create(	UINT32 dwExStyle, UINT32 dwStyle, const RECT & rect, UINT32 MinSz, 
					CWnd * pParentWnd, UINT32 nID, CCreateContext * pContext = nullptr);

	INT32			GetPaneCount(void);
	CWnd *			GetPane( INT32 idx)					{ return m_pPane[idx]; }
	void			SetPane( INT32 idx, CWnd * pPane);
	BOOL			AddPane( CWnd * pPane);
	void			RemovePane( INT32 idx);
	void			RemovePane( CWnd * pPane);

	void			Rearrange(void);

	BOOL			SaveWindowInfo( const char * pszPath);
	BOOL			LoadWindowInfo( const char * pszPath);
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
};


