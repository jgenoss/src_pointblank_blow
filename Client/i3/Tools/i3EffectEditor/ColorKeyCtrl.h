#pragma once
#include "i3Math.h"

// CColorKeyCtrl

class CColorKeyCtrl : public CWnd
{
	DECLARE_DYNAMIC(CColorKeyCtrl)

protected:
	i3KeyframeTable *	m_pRTable;
	i3KeyframeTable *	m_pGTable;
	i3KeyframeTable *	m_pBTable;
	i3KeyframeTable *	m_pATable;

	HBITMAP				m_hBitmap, m_hOldBitmap;
	HDC					m_MemDC;
	CRect				m_ClientRect;

	UINT32 *			m_pPixel;

	INT32				m_DragIndex;
	BOOL				m_bAlphaDrag;
	bool				m_bDragging;
	POINT				m_DragStartPt;

protected:
	INT32				FindKeyframeByPos( POINT pt, BOOL * pUpSide);

public:
	BOOL				Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);
	void				SetColorKeyframes( i3KeyframeTable * pRTable, i3KeyframeTable * pGTable, i3KeyframeTable * pBTable, i3KeyframeTable * pATable);
	i3KeyframeTable *	GetRKeyframe(void)			{ return m_pRTable; }
	i3KeyframeTable *	GetGKeyframe(void)			{ return m_pGTable; }
	i3KeyframeTable *	GetBKeyframe(void)			{ return m_pBTable; }
	i3KeyframeTable *	GetAKeyframe(void)			{ return m_pATable; }

	void				Update( void);
	void				Popup( POINT pt);

public:
	CColorKeyCtrl();
	virtual ~CColorKeyCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


