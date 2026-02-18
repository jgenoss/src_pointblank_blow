#pragma once


// Ci3UIEditorView_Template view
#include "i3UIEditorDoc.h"
#include "UIShapeGripManager.h"
#include "UITemplate_Editor.h"
#include "i3UIShapeListCtrl.h"
#include "UIShapeListCtrl.h"

class Ci3UIEditorView_Template : public CView
{
	DECLARE_DYNCREATE(Ci3UIEditorView_Template)

public:
#ifdef _DEBUG
	Ci3UIEditorDoc* GetDocument() const;
#endif

	Ci3UIEditorView_Template();           // protected constructor used by dynamic creation
	virtual ~Ci3UIEditorView_Template();	

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void	SetCurrentItem( i3UITemplate * pTemplate);

	void	PlaceControls( INT32 cx = -1, INT32 cy = -1);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	UIShapeListCtrl		m_ShapeListCtrl;
	CUITemplate_Editor	m_Editor;

	INT32				m_ShapeList_Height;

	bool				m_bResizing;
	POINT				m_StartPt;
	INT32				m_StartPos;
	

/////////////////////////////////////////
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
public:
	virtual void	OnDraw( CDC * pDC) override;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDebugDumpvirtualtexture();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

public:
	void	setPrevMode( bool bEnable) { m_Editor.SendInputToFramework( bEnable);}
	bool	isPrevMode(void)			{ return m_Editor.isPrevMode();}

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	INT32		getSelectedObjCount( void);
	void		getSelectedObjList( i3::vector<i3UIControl*>& Out);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	REAL32		getZoomRatio( void)		{	return m_Editor.getZoomRatio();	}
	void		SetupScroll( void)		{	m_Editor.SetupScroll();	}

	//리스트 갱신..
	void		UpdateShapeListCtrl();
};
