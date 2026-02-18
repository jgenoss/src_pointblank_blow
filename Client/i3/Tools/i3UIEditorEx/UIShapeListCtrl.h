#pragma once

#include "i3TDK_DrawBoard.h"

class CUITemplate_Editor;

// UIShapeListCtrl

struct UISHAPE
{
	i3UIShape *		m_pShape = nullptr;
	INT32			m_idx = 0;
	bool			m_bSelected = false;
	HBITMAP			m_hBitmap = nullptr;
	INT32			m_bmpW = 0;
	INT32			m_bmpH = 0;

	void			SetUV( INT32 u, INT32 v, INT32 w, INT32 h);
};

class UIShapeListCtrl : public CWnd
{
	DECLARE_DYNAMIC(UIShapeListCtrl)

protected:
	CUITemplate_Editor	*	m_pEditor;

	i3::vector<UISHAPE *>	m_ShapeList;

	i3UITemplate	*	m_pTemplate;

	i3TDK_DrawBoard			m_DrawBoard;
	HDC						m_BmpDC;

	INT32			m_ViewSize;
	INT32			m_RowCount;
	INT32			m_ColCount;
	INT32			m_WndCX;
	INT32			m_WndCY;
	INT32			m_ScrollPos;

	HFONT			m_hFont;

	bool			m_bModified;
	bool			m_bNotifyUpdate;

protected:
	void		RemoveAll(void);
	bool		ExtractBitmap( UISHAPE * pInfo);
	void		ExtractBitmapSelected(void);

	void		AdjustView(void);

	void		Draw( void);
	void		DrawShape( INT32 x, INT32 y, const UISHAPE & shape);

	INT32		getItemCX(void) const;
	INT32		getItemCY(void) const;
	void		getViewRange( INT32 * pStart, INT32 * pEnd);

	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	INT32		getPickedShape( CPoint pt);
	void		InvalidateShape( INT32 idx);

	void		UpdateScrollBar( INT32 newPos);

	void		PopupMenu(void);

	void		resortTemplateCount(i3UITemplate_User * pUser);

	INT32		GetShapeIndex( i3UIShape * pShape);

public:
	UIShapeListCtrl();
	virtual ~UIShapeListCtrl();

	BOOL		Create( LPCTSTR pszCaption, DWORD dwStyle, const CRect & rc, CWnd * pParent, UINT nID);

	void		SetEditor( CUITemplate_Editor * pEditor);
	void		setTemplate( i3UITemplate * pTemplate);

	void		UpdateAll(void);
	void		Redraw(void);
	void		SelectShape( INT32 idx, bool bSelect = true);
	void		ToggleSelectShape( INT32 idx);
	void		UnselectAll(void);

	void		EnsureVisible( INT32 idx);

	INT32		AddShape( void);
	INT32		RemoveShape( INT32 idx);
	INT32		EditShape( INT32 idx);
	INT32		MakeClone( INT32 idx);
	INT32		MoveShape( INT32 nSel, INT32 nMove );
	void		SelectTexture(void);
	void		SelectUV(void);

	i3UIShape *	getShape( INT32 idx);
	INT32		getSelected(void);
	INT32		getShapeIndex( i3UIShape * pShape);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


