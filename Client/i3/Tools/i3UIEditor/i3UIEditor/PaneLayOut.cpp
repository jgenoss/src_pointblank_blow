// PaneLayOut.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalVariable.h"
#include "i3UIEditor.h"
#include "PaneLayOut.h"

enum EDGE_INDEX
{
	EDGE_INDEX_LEFT = 0,
	EDGE_INDEX_TOP,
	EDGE_INDEX_RIGHT,
	EDGE_INDEX_BOTTOM,
	EDGE_INDEX_BOTTOMRIGHT,
	EDGE_INDEX_MAX,
};

// PaneLayOut dialog
IMPLEMENT_DYNAMIC(CPaneLayOut, i3TDKDialogBase)
CPaneLayOut::CPaneLayOut(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneLayOut::IDD, pParent)
{
	m_pCurLayerRoot	= NULL;

	m_pCurControl	= NULL;
	m_bLBDragging	= FALSE;

	m_nEdge			= EDGE_INDEX_MAX;
	m_bLBSizing		= false;
	m_bDragScreen	= false;

	m_zWheelDelta	= 0;
	m_nScreenScale	= 1;

	m_hMemDC		= NULL;
	m_hbrBack		= NULL;
	m_hbrLayerBack	= NULL;
	m_hpenSelect	= NULL;
	m_hbmpBack		= NULL;

	m_posScreen.SetPoint( 0,0);
}

CPaneLayOut::~CPaneLayOut()
{
	if( m_hMemDC != NULL)
	{
		::SelectObject( m_hMemDC, m_hpenOld);
		::SelectObject( m_hMemDC, m_hbrOld);
		::SelectObject( m_hMemDC, m_hbmpOld);
		::DeleteDC( m_hMemDC);
		m_hMemDC = NULL;
	}

	if( m_hbrBack != NULL)
	{
		::DeleteObject( m_hbrBack);
		m_hbrBack = NULL;
	}

	if( m_hbrLayerBack != NULL)
	{
		::DeleteObject( m_hbrLayerBack);
		m_hbrLayerBack = NULL;
	}

	if( m_hpenSelect != NULL)
	{
		::DeleteObject( m_hpenSelect);
		m_hpenSelect = NULL;
	}
}

void CPaneLayOut::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPaneLayOut, i3TDKDialogBase)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// PaneLayOut message handlers
BOOL CPaneLayOut::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText( "레이아웃");

	//GUITREE컨트롤에서 선택된 노드에대한 메시지
	i3TDK::RegisterUpdate( m_hWnd, i3GameNode::GetClassMeta(), I3_TDK_UPDATE_SELECT );

	//Control Shape에서 현재 선택된 컨트롤의 모양이 변경되었음을 알려주는 메시지
	i3TDK::RegisterUpdate( m_hWnd, i3UITemplate::GetClassMeta(), I3_TDK_UPDATE_EDIT);

	//property에서 현재 컨트롤 또는 윈도우의 속성이 변하였음을 알려줌
	i3TDK::RegisterUpdate( m_hWnd, i3GameObjBase::GetClassMeta(), I3_TDK_UPDATE_EDIT);

	//GUITREE에서 선택된 컨트롤을 삭제하고자 함을 알려줌.
	i3TDK::RegisterUpdate( m_hWnd, i3GameObjBase::GetClassMeta(), I3_TDK_UPDATE_REMOVE );

	//GUITREE에서 현재 에디트하는 컨트롤의 루트노드를 선택할때.
	i3TDK::RegisterUpdate( m_hWnd, i3GameObjBase::GetClassMeta(), I3_TDK_UPDATE_SET);

	{
		HDC dc = ::GetDC( NULL);

		m_hMemDC = CreateCompatibleDC( dc);

		::ReleaseDC( NULL, dc);

		m_hbrBack		= CreateSolidBrush( RGB( 0, 0, 0));
		m_hbrLayerBack	= CreateSolidBrush( RGB( 33, 66, 99));
		m_hpenSelect	= CreatePen( PS_SOLID, 1, RGB( 255, 0, 255));

		m_hbrOld = (HBRUSH) ::SelectObject( m_hMemDC, m_hbrBack);
		m_hpenOld = (HPEN)	::SelectObject( m_hMemDC, m_hpenSelect);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneLayOut::OnUpdate( I3_TDK_UPDATE_INFO * pInfo )
{
	if( pInfo->m_pObject == NULL )
	return ;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT )						//선택
	{
		if( pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH || pInfo->m_Event & I3_TDK_USRWND_CTRLSHAPE || pInfo->m_Event & I3_TDK_USRWND_MAINFRAME)				//출처 : gamegraph( guitree)
		{
			//방금 선택된 노드가 m_pCurLayerRoot의 자식이라면, 그냥 현재 노드로 
			// 그렇지 않다면 현재노드를 m_pCurLayerRoot/ 현재노드 두가지 모두 설정한다.
			if( pInfo->m_pObject == NULL || !pInfo->m_pObject->IsTypeOf( i3GameObjBase::GetClassMeta()))
				return;

			m_pCurControl = (i3UIControl*)pInfo->m_pObject;
			if( m_pCurLayerRoot == NULL)
			{
				m_pCurLayerRoot = m_pCurControl;

				m_posScreen.SetPoint( 0,0);
				m_nScreenScale = 1;
			}
			else
			{
				if( m_pCurControl != m_pCurLayerRoot)
				{
					i3GameNode * pNode = m_pCurControl;
					while( pNode != NULL)
					{
						if( pNode->getParent() == m_pCurLayerRoot)
							break;

						pNode = pNode->getParent();
					}

					if( pNode == NULL)
					{
						m_pCurLayerRoot = m_pCurControl;

						m_posScreen.SetPoint( 0,0);
						m_nScreenScale = 1;
					}
				}
			}
			Invalidate( FALSE);
		}

	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)					// 편집되었음
	{
		if( m_pCurControl == NULL )	return ;						//출처에 상관없이
		if( !m_pCurControl->IsTypeOf( i3UIControl::GetClassMeta())) return;
		
		if( pInfo->m_pMeta == i3UITemplate::GetClassMeta())
		{
			if( pInfo->m_Event & I3_TDK_USRWND_CTRLSHAPE)
			{
				i3UITemplate * pElement = (i3UITemplate*)pInfo->m_pObject;
				if( ((i3UIControl*)m_pCurControl)->GetControlData()->m_pShape == pElement )
				{
					Invalidate();
				}
			}
		}
		else if( pInfo->m_pMeta->IsTypeOf( i3GameObjBase::GetClassMeta()))
		{
			if( pInfo->m_Event & I3_TDK_USRWND_PROPERTY)
			{
				i3GameObjBase * pObj = (i3GameObjBase*)pInfo->m_pObject;
				if( m_pCurControl == pObj)
				{
					Invalidate();
				}
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)							// 삭제 메시지
	{
		if( pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH)						// guitree에서 삭제되었음
		{
			if( pInfo->m_pMeta->IsTypeOf(i3GameObjBase::GetClassMeta()))
			{
				i3GameObjBase * pObj = (i3GameObjBase*)pInfo->m_pObject;
				if(( pObj != NULL ) && ( pObj == m_pCurControl))
				{
					m_pCurControl = NULL;
				}
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SET)
	{
		if( pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH)
		{
			m_pCurLayerRoot = (i3UIControl*)pInfo->m_pObject;
			m_pCurControl	= (i3UIControl*)m_pCurLayerRoot->getFirstChild();

			m_posScreen.SetPoint( 0,0);
			m_nScreenScale = 1;

			Invalidate( FALSE);
		}
	}
}

LRESULT CPaneLayOut::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPaneLayOut::_Resize( INT32 cx, INT32 cy)
{
	if( (cx <= 0) || (cy <= 0))
		return;

	if( m_hbmpBack != NULL)
	{
		::SelectObject( m_hMemDC, m_hbmpOld);

		::DeleteObject( m_hbmpBack);
		m_hbmpBack = NULL;
	}

	m_hbmpBack = ::CreateBitmap( cx, cy, ::GetDeviceCaps( m_hMemDC, PLANES), ::GetDeviceCaps( m_hMemDC, BITSPIXEL), NULL);

	m_hbmpOld = (HBITMAP) ::SelectObject( m_hMemDC, m_hbmpBack);
}


void CPaneLayOut::_Draw(void)
{
	RECT rt;

	::SelectObject( m_hMemDC, GetStockObject( WHITE_PEN));
	::SelectObject( m_hMemDC, GetStockObject( NULL_BRUSH));

	GetClientRect(&rt );

	::FillRect( m_hMemDC, &rt, m_hbrBack); 

	if( m_pCurLayerRoot != NULL )
	{
		_CalcObjectRect( &rt, m_pCurLayerRoot);

		::FillRect( m_hMemDC, &rt, m_hbrLayerBack);
	}

	if( m_pCurLayerRoot != NULL)
	{
		if( m_pCurLayerRoot->IsTypeOf( i3UIManager::GetClassMeta()))
		{
			i3UIControl * pCtrl = (i3UIControl*)m_pCurLayerRoot->getFirstChild();
			while( pCtrl != NULL)
			{
				OnDrawObject( m_hMemDC, pCtrl);

				pCtrl = (i3UIControl*)pCtrl->getNext();
			}
		}
		else
		{
			OnDrawObject( m_hMemDC, m_pCurLayerRoot );
		}
	}
	else if( m_pCurControl != NULL )
	{
		OnDrawControl( m_hMemDC, (i3UIControl*)m_pCurControl, m_pCurControl->getGNodeState());
	}
	else return;

	if( m_pCurControl != NULL)
	{
		_CalcObjectRect( &rt, m_pCurControl);
		
		::SelectObject( m_hMemDC, m_hpenSelect);

		::Rectangle( m_hMemDC, rt.left, rt.top, rt.right, rt.bottom);
	}
}

void CPaneLayOut::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call i3TDKDialogBase::OnPaint() for painting messages
	//Draw Window

	_Draw();

	RECT * pRt = & dc.m_ps.rcPaint;
	INT32 w, h;

	w = pRt->right - pRt->left;
	h = pRt->bottom - pRt->top;

	::BitBlt( dc.m_hDC, pRt->left, pRt->top, w, h, m_hMemDC, pRt->left, pRt->top, SRCCOPY);
}

void	CPaneLayOut::OnDrawControl( HDC hdc,i3UIControl * pCtrl, INT32	nState, i3GameObjBase * pParent)
{
	if( pCtrl == NULL )	return ;
	INT32	i = 0;

	i3UIControlData * pData = pCtrl->GetControlData();

	i3UITemplate * pTemplate = pData->m_pShape;
	INT32	nShapeCount = 0;

	if( pTemplate == NULL )
	{
		RECT rt;
		_CalcObjectRect( &rt, pCtrl);

		::DrawFrameControl( hdc, &rt, DFC_BUTTON, DFCS_BUTTONPUSH);

		return;
	}
	else
	{
		nShapeCount = pTemplate->getShapeCount();
	}

	for(i = 0; i < nShapeCount; ++i)
	{
		i3UIImage * pImage = pTemplate->getShape(i);

		if( pImage != NULL )
		{
			RECT rt;
			_CalcObjectRect( &rt, pCtrl);

			rt.left		+= (LONG)(pImage->m_nX) * m_nScreenScale;
			rt.top		+= (LONG)(pImage->m_nY) * m_nScreenScale;
			rt.right	= (LONG)(pImage->m_nWidth) * m_nScreenScale;
			rt.bottom	= (LONG)(pImage->m_nHeight) * m_nScreenScale;

			i3Texture * pTex = pImage->m_pTexture;
			if( pTex != NULL )
			{
				i3GuiToolImageElement * pElm = g_pResList->FindElementByTexture( pImage->m_pTexture);

				i3TDK::AlphaBlend(hdc,
							rt.left,
							rt.top,
							rt.right,
							rt.bottom,
							pElm->getBitmap(),
							pImage->m_u,
							pImage->m_v,
							pImage->m_w,
							pImage->m_h
							);

				if( pCtrl->IsTypeOf( i3UIEditBox::GetClassMeta()) == TRUE)
				{
					rt.right	+= rt.left;	
					rt.bottom	+= rt.top;
					::Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom);
				}
			}
			else
			{
				//blt는 width,height개념이구 drawframecontrol, drawrectangle은 position개념입니다.
				rt.right	+= rt.left;	
				rt.bottom	+= rt.top;

				::DrawFrameControl( hdc, &rt, DFC_BUTTON, DFCS_BUTTONPUSH);
			}
		}
	}

	i3UIControl *pChild = (i3UIControl*)pCtrl->getFirstChild();
	while( pChild != NULL)
	{
		OnDrawControl( hdc, pChild, pChild->getGNodeState(), pCtrl);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

/*
LayOut Window 내에서만 통용되는 컨트롤과 윈도우의 표시 법입니다.
표시되는 root컨트롤 또는 윈도우의 위치는 
*/
void	CPaneLayOut::OnDrawObject( HDC hdc, i3UIControl * pObj)
{
	if( pObj == NULL )
		return;

	OnDrawControl( hdc, (i3UIControl*)pObj, ((i3UIControl*)pObj)->getGNodeState());

	//Draw Controls
	i3GameObjBase * pChild	= (i3GameObjBase*)pObj->getFirstChild();
	while( pChild != NULL )
	{
		if( pChild->IsTypeOf( i3UIControl::GetClassMeta()))
		{
			i3UIControl * pCtrl = (i3UIControl*)pChild;

			OnDrawControl( hdc, pCtrl, pCtrl->getGNodeState(), pObj );
		}

		pChild = (i3GameObjBase*)pChild->getNext();
	}
}

void CPaneLayOut::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	_Resize( cx, cy);
	// TODO: Add your message handler code here
}

void CPaneLayOut::_CalcObjectRect( RECT * rt, i3UIControl * pCtrl)
{
	i3UIControl * pLast = pCtrl;

	REAL32 X = 0.0f;
	REAL32 Y = 0.0f;

	if( pCtrl->IsTypeOf( i3UIManager::GetClassMeta()))
	{
		rt->left	= (LONG)((X * m_nScreenScale) + m_posScreen.x);
		rt->top		= (LONG)((Y * m_nScreenScale) + m_posScreen.y);

		rt->right	= rt->left + ( 1024 * m_nScreenScale);
		rt->bottom	= rt->top  + ( 768 * m_nScreenScale);
	}
	else
	{
		while( pLast != NULL && (pLast->IsTypeOf( i3UIManager::GetClassMeta())) == FALSE)
		{
			X		+=	pLast->getPositionX();
			Y		+=	pLast->getPositionY();

			pLast	=	(i3UIControl*)pLast->getParent();
		}

		rt->left	= __PTI(X * m_nScreenScale) + m_posScreen.x;
		rt->top		= __PTI(Y * m_nScreenScale) + m_posScreen.y;

		rt->right	= rt->left + __PTI((pCtrl->getWidth()) * m_nScreenScale);
		rt->bottom	= rt->top  + __PTI((pCtrl->getHeight())* m_nScreenScale);
	}
}

INT32 CPaneLayOut::_IsMouseOverEdge(CPoint point)
{
	if( m_pCurControl == NULL)
		return EDGE_INDEX_MAX;

	RECT rt;
	_CalcObjectRect( &rt, m_pCurControl);

	INT32	left	= rt.left;
	INT32	top		= rt.top;
	INT32	right	= rt.right;
	INT32	bottom	= rt.bottom;

	INT32 nEdge = EDGE_INDEX_MAX;
	INT32 nEdgeCnt = 0;
	if( (left <= point.x) && (right >= point.x))
	{
		if(( point.y >= bottom - 2) && (point.y <= bottom +2))
		{
			nEdge = EDGE_INDEX_BOTTOM;
			nEdgeCnt++;
		}
	}

	if( (top <= point.y) && (bottom >= point.y))
	{
		if( (point.x >= right - 2) && (point.x <= right +2))
		{
			nEdge = EDGE_INDEX_RIGHT;
			nEdgeCnt++;
		}
	}

	if( nEdgeCnt == 2)
	{
		nEdge = EDGE_INDEX_BOTTOMRIGHT;
		SetCursor( g_pTDKGlobalRes->m_hCursorSizeNS);
	}
	else if(( nEdge == EDGE_INDEX_LEFT) || ( nEdge == EDGE_INDEX_RIGHT))
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorSizeWE  );
	}
	else if((nEdge == EDGE_INDEX_TOP) || (nEdge == EDGE_INDEX_BOTTOM))
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorSizeNS);
	}
	else
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorArrow );
	}

	return nEdge;
}

void CPaneLayOut::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pCurLayerRoot == NULL )
		return;

	SetFocus();

	if( m_pCurControl != NULL)
	{
		RECT	rt;
		_CalcObjectRect( &rt, m_pCurControl);

		m_bRedraw = true;
		InvalidateRect( &rt);
	}

	//Select Controls
	m_nEdge = _IsMouseOverEdge( point);
	if( m_nEdge != EDGE_INDEX_MAX)
	{
		m_bLBSizing = true;
	}
	else
	{
		i3GameNode * pNode = m_pCurLayerRoot->getFirstChild();
		m_pCurControl = NULL;

		i3List List;

		while( pNode != NULL )
		{
			if( pNode->IsTypeOf( i3UIControl::GetClassMeta()))
			{
				i3UIControl * pCtrl = (i3UIControl*)pNode;

				RECT rt;
				_CalcObjectRect( &rt, pCtrl);

				if(::PtInRect( &rt, point))
				{
					m_pCurControl = pCtrl;
				}
			}

			if( pNode->getFirstChild() != NULL)
			{
				if(pNode->getNext() != NULL)
				{
					List.Add( pNode->getNext());
				}
				pNode = pNode->getFirstChild();
			}
			else
			{
				pNode = pNode->getNext();
				if( pNode == NULL)
				{
					if( List.GetCount() != 0)
					{
						pNode = (i3GameNode*)List.GetLast();
						List.Delete(List.GetCount()-1);
					}
				}
			}
		}
	}

	if( m_pCurControl != NULL )
	{
		m_posOldDrag	= point;
		m_bLBDragging	= TRUE;
		
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_LAYOUT, m_pCurControl);//Control Shape, Property에 선택되었음을 알림

		SetCapture();

		RECT rt;
		_CalcObjectRect( &rt, m_pCurControl);

		InvalidateRect( &rt);
	}

	i3TDKDialogBase::OnLButtonDown(nFlags, point);
}

void CPaneLayOut::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pCurLayerRoot == NULL )
		return;

	SetFocus();
	//Select Controls

	m_posOldDragScreen	= point;
	m_bDragScreen		= true;

	SetCapture();
	Invalidate();

	i3TDKDialogBase::OnRButtonDown(nFlags, point);
}

void CPaneLayOut::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pCurLayerRoot == NULL )
		return;
	RECT rt;
	rt.left = 1000;

	CPoint delta = point - m_posOldDrag;
	//delta.SetPoint( delta.x * m_nScreenScale, delta.y * m_nScreenScale);
	if( m_pCurControl != NULL && m_bLBDragging)
	{
		if( !m_bLBSizing )
		{
			//for InvalidateRect
			_CalcObjectRect( &rt, m_pCurControl);

			//move control
			INT32	X = __PTI(m_pCurControl->getPositionX()) + delta.x;
			INT32	Y = __PTI(m_pCurControl->getPositionY()) + delta.y;

			m_pCurControl->setPosition( (REAL32) X, (REAL32) Y);
		}
		else
		{
			//for InvalidateRect
			_CalcObjectRect( &rt, m_pCurControl);

			if( m_nEdge == EDGE_INDEX_LEFT || m_nEdge == EDGE_INDEX_RIGHT)
			{
				m_pCurControl->setSize( m_pCurControl->getWidth() + delta.x, m_pCurControl->getHeight());
			}
			else if( m_nEdge == EDGE_INDEX_TOP || m_nEdge == EDGE_INDEX_BOTTOM)
			{
				m_pCurControl->setSize( m_pCurControl->getWidth(), m_pCurControl->getHeight() + delta.y);
			}
			else if( m_nEdge == EDGE_INDEX_BOTTOMRIGHT)
			{
				m_pCurControl->setSize( m_pCurControl->getWidth() + delta.x, m_pCurControl->getHeight() + delta.y);
			}
		}

		((i3UIControl*)m_pCurControl)->UpdateRenderObjects();

		// For InvalidateRect()
		RECT rect;
		_CalcObjectRect( &rect, m_pCurControl);	

		rt.left		= min(rt.left	, rect.left);
		rt.top		= min(rt.top	, rect.top);
		rt.right	= max(rt.right	, rect.right);
		rt.bottom	= max(rt.bottom , rect.bottom);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_LAYOUT, m_pCurControl);//Property Wnd를 위한 Update

		m_posOldDrag = point;
	}
	else if( m_bDragScreen)
	{
		CPoint delta = point - m_posOldDragScreen;
		//delta.SetPoint( delta.x * m_nScreenScale, delta.y * m_nScreenScale);

		m_posScreen += delta;
		m_posOldDragScreen = point;

		::GetClientRect( m_hWnd, &rt);
	}

	if( rt.left != 1000)
	{
		InvalidateRect( &rt);
	}

	i3TDKDialogBase::OnMouseMove(nFlags, point);
}

BOOL CPaneLayOut::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	INT32 delta = 115;
	INT32 nCurrentScale = m_nScreenScale;

	if( abs( zDelta ) > delta)
	{
		if( zDelta > 0)
		{
			m_nScreenScale++;

			m_nScreenScale = MIN( m_nScreenScale, 3);
		}
		else
		{
			m_nScreenScale--;

			m_nScreenScale = MAX( m_nScreenScale, 1);
		}

		if( nCurrentScale != m_nScreenScale)
		{
			m_posScreen.SetPoint( (m_nScreenScale / nCurrentScale) * m_posScreen.x  , (m_nScreenScale / nCurrentScale) * m_posScreen.y);

			Invalidate();
		}
	}

	return TRUE;
}

void CPaneLayOut::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pCurControl != NULL )
	{
		m_bLBSizing = false;

		if( m_bLBDragging )
		{
			m_bLBDragging = false;
			::ReleaseCapture();
		}
	}

	i3TDKDialogBase::OnLButtonUp(nFlags, point);
}

void CPaneLayOut::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pCurLayerRoot == NULL )
		return;

	if( m_bDragScreen )
	{
		m_bDragScreen = false;
		::ReleaseCapture();

		RECT rt;
		::GetClientRect( m_hWnd, &rt);

		InvalidateRect(&rt);
	}

	i3TDKDialogBase::OnRButtonUp(nFlags, point);
}

BOOL CPaneLayOut::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return 1;
	//return i3TDKDialogBase::OnEraseBkgnd(pDC);
}

BOOL CPaneLayOut::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN)
	{
		TranslateMessage( pMsg);

		DispatchMessage( pMsg);

		return TRUE;
	}

	return i3TDKDialogBase::PreTranslateMessage(pMsg);
}

void CPaneLayOut::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pCurControl == NULL )	return ;

	VEC2D vPos;
	RECT rect1, rect2;

	vPos.x = m_pCurControl->getPositionX();
	vPos.y = m_pCurControl->getPositionY();

	_CalcObjectRect( &rect1, m_pCurControl);	

	REAL32	rDeltaX = 0.0f;
	REAL32	rDeltaY = 0.0f;

	switch( nChar )
	{
		case VK_LEFT:
			{
				rDeltaX = -1.0f;
			}
			break;
			
		case VK_RIGHT:
			{
				rDeltaX = 1.0f;
			}
			break;


		case VK_DOWN:
			{
				rDeltaY = 1.0f;
			}
			break;
			
		case VK_UP:		
			{
				rDeltaY = -1.0f;
			}
			break;
		default:
			return;
	}

	if(GetKeyState( VK_LSHIFT ) & 0x80 && m_pCurControl->IsTypeOf( i3UIControl::GetClassMeta()))
	{
		((i3UIControl*)m_pCurControl)->setSize( m_pCurControl->getWidth() + (INT32)rDeltaX, m_pCurControl->getHeight() + (INT32)rDeltaY);
	}
	else
	{
		m_pCurControl->setPosition(  getX( &vPos) + rDeltaX, getY( &vPos) + rDeltaY);
	}

	_CalcObjectRect( &rect2, m_pCurControl);	

	rect1.left		= min( rect1.left, rect2.left);
	rect1.top		= min( rect1.top, rect2.top);
	rect1.right		= max( rect1.right, rect2.right);
	rect1.bottom	= max( rect1.bottom, rect2.bottom);

	InvalidateRect( &rect1);

	i3TDKDialogBase::OnKeyDown(nChar, nRepCnt, nFlags);
}
