#pragma once

#include "GICDiagramView.h"

class CPaneDesignView : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneDesignView)

public:
	CGICDiagramView *	m_pDiagramView;

public:
	INT32		getSelectedShape( i3List * pList)
	{
		m_pDiagramView->getDiagram()->GetSelectedShapes( pList);
		return pList->GetCount();
	}

	i3GICDiagramContext *	getDiagramContext( void)	{ return m_pDiagramView->getDiagramContext(); }

	HDC			getDrawDC( void)						{ return m_pDiagramView->getDrawDC(); }

	void		addShape( i3GICShape * pShape)
	{
		I3ASSERT( pShape != NULL);
		POINT pt;
		pt.x = (INT32) pShape->getPosX();
		pt.y = (INT32) pShape->getPosY();
		m_pDiagramView->_LP( &pt);
		pShape->setPos( (REAL32) pt.x, (REAL32) pt.y);
		m_pDiagramView->getDiagram()->AddShape( NULL, pShape);
		m_pDiagramView->RedrawWindow();
	}

	void		removeShape( i3GICShape * pShape)
	{
		I3ASSERT( pShape != NULL);
		m_pDiagramView->getDiagram()->RemoveShape( pShape);
	}

	void		removeAllShape( void)
	{
		m_pDiagramView->getDiagram()->RemoveAllShape();
	}

	bool		isGrid( void)					{ return m_pDiagramView->isGridEnable(); }
	void		setGridEnable( bool bVal)
	{
		if( bVal)	m_pDiagramView->enableGrid();
		else		m_pDiagramView->disableGrid();
	}

	INT32		getGridSize( void)					{ return m_pDiagramView->getGridX(); }
	void		setGridSize( INT32 size)			{ m_pDiagramView->setGridX( size); m_pDiagramView->setGridY( size); }

	bool		isLinkSnap( void)					{ return m_pDiagramView->isLinkSnapEnable(); }
	void		setLinkSnapEnable( bool bVal)
	{
		if( bVal)
		{
			m_pDiagramView->enableLinkSnap();
			m_pDiagramView->disableLinkUpdate();
		}
		else
		{
			m_pDiagramView->disableLinkSnap();
			m_pDiagramView->enableLinkUpdate();
		}
	}

	bool		isGridSnap( void)				{ return m_pDiagramView->isGridSnapEnable(); }
	void		setGridSnapEnable( bool bVal)
	{
		if( bVal)	m_pDiagramView->enableGridSnap();
		else		m_pDiagramView->disableGridSnap();
	}

	void		redrawView( void)				{ m_pDiagramView->RedrawWindow(); }

public:
	CPaneDesignView();
	virtual ~CPaneDesignView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize( UINT nType, int cx, int cy);
	afx_msg void OnMouseMove( UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp( UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags);
};

