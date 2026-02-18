#pragma once


// PaneLayOut dialog
class CPaneLayOut : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneLayOut)

public:
	CPaneLayOut(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneLayOut();

public:
	//현재 작업중인 컨트롤 및 윈도우에대한 핸들입니다.
	i3GuiObjBase	*	m_pCurLayerRoot;	//root를 설정하고 그 하위부분들만 레이아웃에 표시합니다.
	i3GuiObjBase	*	m_pCurControl;

	bool	m_bRedraw;

	bool	m_bLBDragging;
	
	CPoint	m_posOldDrag;

	bool	m_bLBSizing;
	INT32	m_nEdge;

	CPoint	m_posScreen;			//레이아웃 화면의 좌상단 위치
	CPoint	m_posOldDragScreen;		//레이아웃 화면의 좌상단 위치
	bool	m_bDragScreen;			//레이아웃 화면을 드래그중인지 확인
	INT32	m_nScreenScale;			//레이아웃 줌 배율( 정배율)

	INT32	m_zWheelDelta;			//Wheel Delta;

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );
	void	OnDrawControl( HDC hdc, i3GuiControl * pCtrl, INT32	nState, i3GuiObjBase * pParent = NULL);
	void	OnDrawWindow( HDC hdc, i3GuiWindow * pWindow );
	void	OnDrawObject( HDC hdc, i3GuiObjBase * pObj);

	void	_ChangeControlSize( INT32 nEdge, CPoint nDelta);
	INT32	_IsMouseOverEdge(CPoint point);							//마우스가 현재 선택된 컨트롤의 외곽선에 있는지

	void	_CalcObjectRect( RECT * rt, i3GuiObjBase * pObj);
//	void	_CreateBitmap(HBITMAP *hBitmap, INT32	nStartX, INT32	nStartY, INT32	nWidth, INT32	nHeight, i3Texture * pTex);

public:
// Dialog Data
	enum { IDD = IDD_PANE_LAYOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
