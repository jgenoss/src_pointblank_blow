#pragma once

class CUVEditCtrl :	public i3TDKImageViewCtrl
{
	DECLARE_DYNCREATE( CUVEditCtrl );
public:
	CUVEditCtrl(void);
	virtual ~CUVEditCtrl(void);

protected:
	DECLARE_MESSAGE_MAP();

	i3UIImage * m_pEditUVImage;
	BOOL		 m_bEditUVEnable;

	CRect		 m_FocusRect;
	CPoint		 m_posOldLBDrag;
	CPoint		 m_posDragStart;
	CRect		 m_rtDragStart;
	BOOL		 m_bLBDragging;

	INT32		 m_LastMouseOverEdge;
	INT32		 m_nPenColor;									// 20070704 ±èÇö¿ì Ãß°¡

	void		_SetValidRect(i3UIImage * pImage);
	void		_MoveUV(CPoint delta);
	INT32		_IsMouseEdgeOver(CPoint point);
	void		_ChangeSize( INT32	nEdge, CPoint	nDelta );
	RECT		_FindMinRange( RECT * pRect1, RECT * pRect2);

	
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void		 SetImageForEditUV( i3UIImage * pImage );
	virtual void OnDraw(HDC dc);
public:
	void	SetEditUVEnable( BOOL bEdit = TRUE){	m_bEditUVEnable = bEdit;}
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void	SetPenColor( INT32 nPenColor )		{ m_nPenColor = nPenColor; }; // 20070704 ±èÇö¿ì Ãß°¡
};
