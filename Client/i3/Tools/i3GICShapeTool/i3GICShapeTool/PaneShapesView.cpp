//

#include "stdafx.h"
#include "PaneShapesView.h"
#include "resource.h"
#include "GlobalVariables.h"
#include "CommonDef.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CPaneShapesView, CDockablePane)

CPaneShapesView::CPaneShapesView()
{
	m_bDragging = false;
	m_bDropEnable = false;

	m_pDragImage = NULL;
	m_OldClassCursor = NULL;

	m_iSrcItem = -1;
	m_iTargetItem = -1;
}

CPaneShapesView::~CPaneShapesView()
{
	if( m_pDragImage)
	{
		delete m_pDragImage;
	}
}

void CPaneShapesView::_AddIcon( INT32 id)
{
	HICON hIcon;

	hIcon = ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));
	m_ImageList.Add( hIcon);
	::DestroyIcon( hIcon);
}

INT32 CPaneShapesView::_AddShape( i3GICShape * pShape)
{
	return m_ShapeList.AddShape( pShape);
}

void CPaneShapesView::ProcessMouseMove( CPoint point)
{
	if( (m_bDragging) && ( m_pDragImage != NULL) )
	{
		//ClientToScreen( &point);
		GetCursorPos( &point);
		m_pDragImage->DragMove( point);
	}
}


void CPaneShapesView::ProcessLButtonUp( UINT nFlags, CPoint point, bool bIsDesign)
{
	if( m_bDragging)
	{	
		m_bDragging = false;

		ReleaseCapture();

		if( m_pDragImage != NULL)
		{
			m_pDragImage->DragLeave( &m_ShapeList);
			m_pDragImage->EndDrag();
			delete m_pDragImage;
			m_pDragImage = NULL;
		}

		if( bIsDesign )
		{
			i3GICShape * pShape = (i3GICShape *) m_ShapeList.GetItemData( m_iSrcItem);

			if( pShape != NULL)
			{
				//ClientToScreen( &point);

				CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

				pMainWnd->DropShape( pShape, point);
			}
		}

		if( m_OldClassCursor != NULL)
		{
			::SetClassLong( m_ShapeList.m_hWnd, GCL_HCURSOR, (LONG) m_OldClassCursor);
		}
	}
}


BEGIN_MESSAGE_MAP(CPaneShapesView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY( LVN_BEGINDRAG, IDC_LIST_SHAPES, OnLVNBeginDrag)
END_MESSAGE_MAP()


// CUIPaneTimeLine message handlers
int CPaneShapesView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	RECT rect;
	int rv = CDockablePane::OnCreate( lpCreateStruct);

	// Shape's List Control
	GetClientRect( &rect);

	if( m_ShapeList.Create( WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE, rect, this, IDC_LIST_SHAPES) == FALSE)
	{
		I3FATAL( "can't create Shape Tree Ctrl.");
		return -1;
	}
	
	m_ImageList.Create( 16, 16, ILC_COLOR32 | ILC_MASK, 32, 0);

	_AddIcon( IDR_MAINFRAME);
	HICON hIcon	=  AfxGetApp()->LoadStandardIcon( IDI_QUESTION);
	INT32 nIndex = m_ImageList.Add( hIcon);

	m_ShapeList.SetImageList( &m_ImageList, 0);

	{
		i3GICShapeRect * pShape = i3GICShapeRect::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeRectBar * pShape = i3GICShapeRectBar::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeLine * pShape = i3GICShapeLine::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeBezierLine * pShape = i3GICShapeBezierLine::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeTextRect * pShape = i3GICShapeTextRect::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeRenderTexture * pShape = i3GICShapeRenderTexture::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeCircle * pShape = i3GICShapeCircle::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeCircleBar * pShape = i3GICShapeCircleBar::NewObjectRef();
		_AddShape( pShape);
	}

	{
		i3GICShapeTriangle * pShape = i3GICShapeTriangle::NewObjectRef();
		_AddShape( pShape);
	}

	return rv;
}

void CPaneShapesView::OnPaint()
{	
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages
	RECT rt;
	::GetClientRect( m_hWnd, &rt);
	
	::FillRect( dc.m_ps.hdc,&rt, (HBRUSH)COLOR_WINDOW);
}

void CPaneShapesView::OnSize( UINT nType, int cx, int cy)
{
	CDockablePane::OnSize( nType, cx, cy);
	
	RECT rect;
	GetClientRect( &rect);

	m_ShapeList.SetWindowPos( NULL, rect.left, rect.top, cx, cy, 0);
}

void CPaneShapesView::OnMouseMove( UINT nFlags, CPoint point)
{
	ProcessMouseMove( point);

	CDockablePane::OnMouseMove( nFlags, point);
}

void CPaneShapesView::OnLButtonUp( UINT nFlags, CPoint point)
{
	ProcessLButtonUp( nFlags, point, false);

	CDockablePane::OnLButtonUp( nFlags, point);
}

void CPaneShapesView::OnLVNBeginDrag( NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	CPoint Pt;

	*pResult = 0;

	//Get Drag item
	if( pNMListView->iItem == -1)
		return;

	Pt = pNMListView->ptAction;

	::GetCursorPos( &Pt);
	
	m_pDragImage = m_ShapeList.CreateDragImage( pNMListView->iItem, &Pt );

	m_pDragImage->DragShowNolock( FALSE);
	m_pDragImage->BeginDrag( 0, CPoint( 10, 10));
	m_pDragImage->DragEnter( NULL, Pt);

	m_iTargetItem = -1;
	m_iSrcItem = pNMListView->iItem;

	m_bDragging		= true;
	m_bDropEnable	= true;

	SetCapture();
	SetFocus();

	{
		// Set class cursor to NULL, so that change mouse cursor image.
		m_OldClassCursor = (HCURSOR) ::GetClassLong( m_ShapeList.m_hWnd, GCL_HCURSOR);
		::SetClassLong( m_ShapeList.m_hWnd, GCL_HCURSOR, NULL);
	}
}