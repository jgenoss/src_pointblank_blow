// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3UIEditorView.h : interface of the Ci3UIEditorView class
//


#pragma once

enum LISTVIEW_MOUSEMSG
{
	LISTVIEW_MOUSEMSG_MOVE = 0,
	LISTVIEW_MOUSEMSG_LDOWN,
	LISTVIEW_MOUSEMSG_LUP
};



struct CONTROL_COPY_INFO
{
//	이 구조체 해제에 대부분 i3MemFree를 쓰고 있는 상태라서 이런 소멸자는 혼동이 될수 있음.... 2012.11.29.수빈..
	~CONTROL_COPY_INFO()
	{
		I3TRACE("_uiControl_Copy_Info");
		delete m_pInfo;
	}

	i3UIControl*		m_pControl = nullptr;
	UI_CONTROL_INFO*	m_pInfo = nullptr;
};

class Ci3UIEditorView : public CView
{
protected: // create from serialization only
	Ci3UIEditorView();
	DECLARE_DYNCREATE(Ci3UIEditorView)

// Attributes
public:
	Ci3UIEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC) override;  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:
// Implementation
public:
	virtual ~Ci3UIEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool					m_bLBDragging;
	CPoint					m_ptDragStart;
	CPoint					m_ptOldPos;
	bool					m_bBanding;

	CUIShapeGripManager		m_pGripManager;

	bool					m_bSendToFramework;
	bool					m_bAddMode;
	bool					m_bNotifyUpdate;

	enum MOUSEMODE
	{
		MOUSEMODE_NONE = 0,
		MOUSEMODE_SCROLL,
	};
	MOUSEMODE				m_MouseMode;
	CPoint					m_ScrollStart;

protected:
	bool					IsPtInWindow( i3GameObj * pRoot, CPoint pt);
	bool					_IsptInImage( CPoint point, i3UIControl * pControl);
	bool					_IsincludeRect( CRect *prtDrag, i3UIControl * pControl);
	void					_SelectItem( CPoint point, bool bReverse = false);
	void					_SelectItemInRect( CRect *prtDrag, bool bReverse = false);

	void					_AdjustRect( CPoint point1, CPoint point2, CRect * prtDrag);

	i3UIControl		*		_rec_IsPtinControl( i3GameObj * pRoot, CPoint pt, bool bReverse, INT32 *pcnt);
	void					_rec_IsControlinRect( i3GameNode * pRoot, CRect *prtDrag, bool bReverse, INT32 *pcnt);

	void					_ClearCopiedControlList();

	i3UIControl *			GetPickedControl( const CPoint & point);
	i3UIControl *			Rec_GetPickedControl( i3GameNode * pParent, const CPoint & point);
	bool					InsideCtrl( i3UIControl * pCtrl, const CPoint & point);

	i3UIFrameWnd *			GetPickedFrameWnd( const CPoint & point);
	i3UIFrameWnd *			Rec_GetPickedFrameWnd( i3GameNode * pParent, const CPoint & point);

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
public:
	i3UIScene	*getScene(void);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);


public:
	bool	DropObject( i3UITemplate * pTemplate, POINT scrPT);
	bool	OnDragMoveObjectTemplate( POINT scrPT, bool bInsideFrame = false, i3ClassMeta * pInsideFrameMeta = NULL);

	INT32	Select( i3UIControl * pControl, bool bState = false);
	void	SelectAll(void);
	void	UnSelect( i3UIControl * pControl);
	void	UnSelectAll(void);

	void	DisableUI( bool bDisable);

	void	SetPrevMode( bool bSet);
	bool	isPrevMode(void)	{	return m_bSendToFramework;}
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void OnInitialUpdate() override;
protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	void	SpreadControl( const i3::vector<CUIGripCtrl*>& ItemList );
	void	AlignLeft( const i3::vector<CUIGripCtrl*>& ItemList);
	void	AlignRight(const i3::vector<CUIGripCtrl*>& ItemList);
	void	AlignTop(const i3::vector<CUIGripCtrl*>& ItemList);
	void	AlignBottom( const i3::vector<CUIGripCtrl*>& ItemList);
	void	AlignCenter( const i3::vector<CUIGripCtrl*>& ItemList);

	void	CascadeHorz( const i3::vector<CUIGripCtrl*> & ItemList);
	void	CascadeVert( const i3::vector<CUIGripCtrl*> & ItemList);
	
protected:
	bool	_SendMouseMsgToLVItem( LISTVIEW_MOUSEMSG eMsg, i3GameNode* pRoot, UINT nFlags, CPoint point);

public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
protected:
	bool					_isInsideEmptyListViewBox( i3GameObj* pGameObj, CPoint pt, bool bItem);
	i3UIListView_Box*		_recFindEmptyListViewBox( i3GameObj* pRoot, CPoint pt, bool bItem);
	i3UIListView_Box*		_getOwnerListViewBox( POINT scrPT, bool bItem);

public:
	INT32		getSelectedObjCount( void);
	void		getSelectedObjList( i3::vector<i3UIControl*>& Out);

	const i3::vector<CUIGripCtrl*>&	getSelectedItemList( void)			{ return m_pGripManager.getSelectedItemList(); }

	void		SetSnapSize( INT32 size);
	INT32		GetSnapSize( void);

	//Scroll & Zoom
	//referenced from CUITemplate_Editor
protected:
	INT32		m_ShowX, m_ShowY;
	INT32		m_ScrollX, m_ScrollY;
	
	CPoint		_AbsToRel( CPoint ptAbs);
public:
	void		SetupScroll( void);
	void		UpdateScroll( bool bVert, UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void		AdjustShowXY( void);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown( UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	REAL32		getZoomRatio( void);
	//--Scroll & Zoom

	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	i3::vector<CONTROL_COPY_INFO*>		m_CopiedControls;

public:
	INT32		GetCopiedControlCount( void)	{ return (INT32)m_CopiedControls.size(); }

public:
	void		OnCopy( void);
	void		OnCut( void);
	void		OnPaste( void);
	void		OnDelete( void);

#ifdef _DEBUG
	void		OnControlInfo( i3UIControl* pSelected);
#endif

	void		ViewTemplate( void);
	void		ShowListBox();
	void		ShowDummyLine();

};

#ifndef _DEBUG  // debug version in i3UIEditorView.cpp
inline Ci3UIEditorDoc* Ci3UIEditorView::GetDocument() const
   { return reinterpret_cast<Ci3UIEditorDoc*>(m_pDocument); }
#endif

