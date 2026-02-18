#pragma once

// CPaneControlShape dialog
class CPaneControlShape : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneControlShape)

public:
	CPaneControlShape(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneControlShape();

	i3GuiTemplateElement	* m_pTemplate;
	INT32					m_nCurShape;
	i3::vector<class i3GuiToolRectElement*>	m_ShapeRectList;

	INT32					m_LastMouseOverEdge;
	BOOL					m_bLBDragging;
	CPoint					m_posOldDrag;
	RECT					m_CurCtrlRect;	//현재 컨트롤의 0기준 영역

	CPoint					m_posScreen;			//화면의 좌상단 위치
	CPoint					m_posOldDragScreen;		//화면의 좌상단 위치
	bool					m_bDragScreen;			//화면을 드래그중인지 확인

	CPoint					m_posStartDrag;
	i3::vector<i3GuiImage*>	m_listSelected;

	i3GuiControl	*		m_pParentCtrl;
	BOOL					m_bFromControl;

	BOOL					m_bNeedCalculateShapeRect;

	INT32					m_nScale;

// Dialog Data
	enum { IDD = IDD_PANE_CONTROLSHAPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	void	InitTemplate(i3GuiTemplateElement* pElement);			//템플릿 초기화
	bool	InitShapeRect(i3GuiTemplateElement* pElement);			//Shape 리스트를 초기화
	void	_SetShapeRect(i3GuiImage * pImage, INT32	nIndex);	//초기화 된 리스트중 변경사항만 다시 셋
	void	_CreateBitmapList();									//비트맵 리스트를 초기화
	void	_UpdateBitmap( INT32	nIndex);						//초기화 된 리스트중 변경사항만 다시 셋
	
	//비트맵 생성
	//void	_CreateBitmap(HBITMAP * hBitmap,INT32	nStartX, INT32	nStartY, INT32	nWidth, INT32	nHeight, i3Texture * pTex);
	void	_OnDrawTemplate(HDC hdc);								//템플릿을 그림

	void	_AddShape();											//쉐이프 추가
	void	_DeleteShape(INT32	nShapIndex);						//쉐이프 제거
	void	_SetImage( i3Texture * pTex);							//쉐이프에 텍스쳐 셋
	void	_SetAlign( UINT32 ncmd, INT32 OriginShape);

	void	_SelectAllShape();										//모든 쉐이프를 선택

	INT32	_GetMouseOverShape(CPoint point);						//마우스가 쉐이프위에있는지
	INT32	_IsMouseOverEdge(CPoint point);							//마우스가 쉐이프의 외곽선에 있는지
	void	_MoveShape(CPoint delta, INT32 nShapeIndex, bool bUpdate = true);	//쉐이프 이동
	void	_MoveShapeByDrag( CPoint curPos, INT32 nShapeIndex, bool bUpdate = true);
	void	_ChangeShapeSize( INT32	nEdge, CPoint nDelta);				//쉐이프 크기 변경

	void	_AdjustSizebyUV( INT32	nShapeIndex);					//uv를 이용하여 쉐이프의 크기를 맞춤
	void	_SetTexture( INT32	nShapeIndex );
	void	_InitUV(INT32	nShapeIndex, i3GuiImage * pImage);
	void	_CopyTextureUVtoAllOthers( INT32 OriginShape);

	void	_CopySizetoOthers( INT32 nSrcIdx, INT32 nShapeIndex);

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

	void	_SpreadShapes( INT32 nCol, REAL32 rSeperateX, REAL32 rSeperateY, INT32 nOrigin = 0, bool bAll = true, bool byValue = false);

public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
