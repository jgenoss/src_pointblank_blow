#pragma once

#include "ControlShapeList.h"

class CPaneShapesView : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneShapesView)

protected:
	bool			m_bDragging;
	bool			m_bDropEnable;
	

	//////////////////////////////////////////////////////////////////////////////////////
	//										List Ctrl									//
protected:
	CImageList 		m_ImageList;
	CControlShapeList	m_ShapeList;
	INT32			m_iSrcItem;
	INT32			m_iTargetItem;

	CImageList *	m_pDragImage;

	HCURSOR			m_OldClassCursor;

	void			_AddIcon( INT32 id);

	INT32			_AddShape( i3GICShape * pShape);

public:
	CPaneShapesView();
	virtual ~CPaneShapesView();

	void			ProcessMouseMove( CPoint point);
	void			ProcessLButtonUp( UINT nFlags, CPoint point, bool bIsDesign);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int		OnCreate( LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnPaint();
	afx_msg void	OnSize( UINT nType, int cx, int cy);
	afx_msg void	OnMouseMove( UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp( UINT nFlags, CPoint point);
	afx_msg void	OnLVNBeginDrag( NMHDR *pNMHDR, LRESULT *pResult);
};

