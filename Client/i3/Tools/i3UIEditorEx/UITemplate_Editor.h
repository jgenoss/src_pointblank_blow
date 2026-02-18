#pragma once


// CUITemplate_Editor

class CUITemplate_Editor : public CWnd
{
	DECLARE_DYNAMIC(CUITemplate_Editor)
	typedef void (*pfCallback)();

	pfCallback				m_pfClickEvent;
	i3UITemplate	*		m_pCurrentItem;
	i3UIControl		*		m_pCurrentCtrl;
	
	//
	i3UIScene		*		m_pTestScene;
	i3UIFrameWnd	*		m_pDummyFrameWnd;

public:
	i3::pack::RECT					m_Rect;
protected:
	CUIShapeGripManager		m_pGripManager;	

	bool					m_bLBDragging;
	CPoint					m_ptDragStart;
	CPoint					m_ptOldPos;

	CUIGripCtrlPoint	*	m_pFocusedPoint;
	CUIGripCtrlRect		*	m_pFocusedShape;
	GRIP_CONTROL_MODE		m_mode;

	bool					m_bSelLock;	// true이면 select관련 동작을 하지 않는다.

	//ShowX, ShowY는 Src dc에 대한 값(zoom과 무관)
	INT32					m_ShowX;
	INT32					m_ShowY;

	//ScrollX, ScrollY는 Dest dc에 대한 값(zoom과 관련)
	// => ScrollBar의 Position
	INT32					m_ScrollX;
	INT32					m_ScrollY;

	//Control을 Src dc의 가운데에 위치시키기 위한 값(zoom과 무관)
	INT32					m_OffsetX;
	INT32					m_OffsetY;

	SIZE					m_WndSz;
	VEC2D					m_vSizeCurCtrl;

	bool					m_bSpreaded;
	bool					m_bSelectTemplate;
	bool					m_bNotify;
	bool					m_bBanding;

	enum MOUSEMODE
	{
		MOUSEMODE_NONE = 0,
		MOUSEMODE_SCROLL,
	};
	MOUSEMODE				m_MouseMode;
	CPoint					m_ScrollStart;
	
	int						m_nLRCnt;
	int						m_nUDCnt;

public:
	bool	IsSpreadedLayout() const
	{
		return m_bSpreaded;
	}

protected:
	bool					_ValidateTemplate( i3UITemplate * pTemplate);
	void					_CleanItem(void);			// clear temporary control
	bool					_BuildItem();				// create temporary control

	bool					_UVSelect( i3UIShape * pShape, I3VT_TEXINFO * pTexInfo,bool bHasUV = false);
	INT32					_UVSelectOneTouch( const i3::vector<i3UIShape*>& ShapeList, I3VT_TEXINFO* pTexInfo, INT32 curIdx);

	i3VirtualTexture	*	_checkVirtualTexture();

	/////
	bool					_IsptInImage( CPoint point, i3UIShape * pShape);
	bool					_IsincludeRect( CRect *prtDrag, i3UIShape * pShape);
	void					_SelectItem( CPoint point, bool bReverse = false);
	void					_SelectItemInRect( CRect *prtDrag, bool bReverse);

	void					_AdjustRect( CPoint point1, CPoint point2, CRect * prtDrag);
	bool					_RecalculateTexcoordByVTex( i3UIShape * pShape, I3VT_TEXINFO * pTexInfo);

public:
	INT32					Select( i3UIShape * pShape);
	void					SelectAll(void);
	void					UnSelect( i3UIShape * pShape);
	void					UnSelectAll(void);

	CUIGripCtrlPoint	*	getFocusedPoint(void)	{	return m_pFocusedPoint;}

protected:
	INT32					SelectTemplate(bool bSelect);
	
	void					AlignLeftTop();
	void					Spread();

	void					OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	bool					m_bSendToFramework;
	bool					isPrevMode(void)	{	return m_bSendToFramework;}
	void					SendInputToFramework( bool bSend);

	void					AdjustLayout();
	void					SpreadLayout();

	void					AttachScene();				// attach scene to rendering root
	void					DetachScene();				// detach scene from rendering root

	void					SetupScroll(void);
	void					UpdateScroll( bool bVert, UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void					AdjustShowXY( void);

	void					DumpVirtualTexture();
	
	void					MoveShapeCell();	//키보드로 템플릿 셀만큼 이동
	void					ResetMoveCellValue();

	void					SelectTexture(void);
	void					SelectUV(void);
public:
	CUITemplate_Editor();
	virtual ~CUITemplate_Editor();

	void		setCurrentTemplate( i3UITemplate * pTemplate);
	BOOL		Create( DWORD dwStyle, const RECT &rt, CWnd * pWnd, UINT nID);
	
	void		SetMouseClickCallBackFunc(void (*pfCallback)());

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnMButtonDown( UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

public:
	INT32		getSelectedObjCount( void);
	void		getSelectedObjList( i3::vector<i3UIControl*>& Out);


	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	REAL32		getZoomRatio( void);

protected:
	CPoint		_AbsToRel( CPoint ptAbs, bool bOffset = true);

};


