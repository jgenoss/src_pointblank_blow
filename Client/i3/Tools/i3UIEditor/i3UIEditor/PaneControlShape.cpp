// PaneControlShape.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "PaneControlShape.h"
#include "GlobalVariable.h"
#include "i3UIEditorDef.h"
#include "SpreadPropertyDlg.h"

enum EDGE_INDEX
{
	EDGE_INDEX_LEFT = 0,
	EDGE_INDEX_TOP,
	EDGE_INDEX_RIGHT,
	EDGE_INDEX_BOTTOM,
	EDGE_INDEX_MAX,
};

// CPaneControlShape dialog

IMPLEMENT_DYNAMIC(CPaneControlShape, i3TDKDialogBase)
CPaneControlShape::CPaneControlShape(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneControlShape::IDD, pParent)
{
	m_pTemplate = NULL;
	m_nCurShape = 1000;

	m_nScale	= 1;
	m_bFromControl	= FALSE;
	m_bLBDragging	= FALSE;
	m_posOldDrag	= CPoint( 0, 0);

	m_bNeedCalculateShapeRect = TRUE;

	m_posStartDrag.SetPoint( 0,0);;
	m_listSelected.Clear();

	m_posScreen.SetPoint( 0,0);			//화면의 좌상단 위치
	m_posOldDragScreen.SetPoint( 0,0);	//화면의 좌상단 위치
	m_bDragScreen	= false;			//화면을 드래그중인지 확인
}

CPaneControlShape::~CPaneControlShape()
{
	m_pTemplate = NULL;
	m_ShapeRectList.SafeReleaseAll();
}

void CPaneControlShape::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneControlShape, i3TDKDialogBase)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CPaneControlShape message handlers

BOOL CPaneControlShape::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText("Control Shape");

	i3TDK::RegisterUpdate( m_hWnd, i3GameNode::GetClassMeta(), I3_TDK_UPDATE_SET);
	i3TDK::RegisterUpdate( m_hWnd, i3UITemplate::GetClassMeta(), I3_TDK_UPDATE_ALL );
	i3TDK::RegisterUpdate( m_hWnd, i3Texture::GetClassMeta(), I3_TDK_UPDATE_SET );
	i3TDK::RegisterUpdate( m_hWnd, i3UIImage::GetClassMeta(), I3_TDK_UPDATE_EDIT);
	i3TDK::RegisterUpdate( m_hWnd, i3UIControl::GetClassMeta(), I3_TDK_UPDATE_SELECT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneControlShape::_SetShapeRect( i3UIImage * pImage, INT32	nIndex)
{
	if( pImage == NULL )	return ;

	if( nIndex > m_ShapeRectList.GetCount())	return ;

	i3GuiToolRectElement * pElm = i3GuiToolRectElement::NewObject();
	RECT * prt = pElm->getRect();

	prt->left	= (LONG)pImage->m_nX;
	prt->top	= (LONG)pImage->m_nY;
	prt->right	= (LONG)pImage->getWidth();
	prt->bottom	= (LONG)pImage->getHeight();

	//width, height가 초기화 되지 않은 상태인경우
	if( prt->right == 0)
	{
		prt->right = 100;
		pImage->setWidth( 100.0f);
	}

	if( prt->bottom == 0)
	{
		prt->bottom = 100;
		pImage->setHeight( 100.0f);
	}

	i3GuiToolRectElement * pOldElm = (i3GuiToolRectElement*)m_ShapeRectList.GetItem(nIndex);
	I3_SAFE_RELEASE( pOldElm);

	m_ShapeRectList.SetItem( nIndex, pElm );
}

bool CPaneControlShape::InitShapeRect(i3UITemplate* pElement)
{
	if ( !m_bNeedCalculateShapeRect)
		return false;

	m_ShapeRectList.SafeReleaseAll();

	if( pElement == NULL )	return false;

	INT32	i;
	INT32	nCount = pElement->getShapeCount();

	for( i = 0; i< nCount; ++i)
	{
		i3UIImage * pImage = (i3UIImage*)pElement->getShape(i);
		I3ASSERT( pImage );

		i3GuiToolRectElement * pElm = i3GuiToolRectElement::NewObject();
		RECT * rt = pElm->getRect();

		if( pImage->m_pTexture != NULL)
		{
			i3GuiToolImageElement * pElmImage = g_pResList->FindElementByTexture( pImage->m_pTexture);

			if(pElmImage != NULL)
			{
				pElm->setElement( pElmImage);
			}
		}

		rt->left	= (LONG)pImage->m_nX;
		rt->top		= (LONG)pImage->m_nY;
		rt->right	= (LONG)pImage->getWidth();
		rt->bottom	= (LONG)pImage->getHeight();

		//width, height가 초기화 되지 않은 상태인경우
		if( rt->right == 0)
		{
			rt->right = 100;
			pImage->setWidth( 100.0f);
		}

		if( rt->bottom == 0)
		{
			rt->bottom = 100;
			pImage->setHeight( 100.0f);
		}

		m_ShapeRectList.Add( pElm );

		if( i == 0 )
		{
			m_CurCtrlRect.left	= 0;
			m_CurCtrlRect.top	= 0;
			m_CurCtrlRect.right = rt->left + rt->right;
			m_CurCtrlRect.bottom= rt->top + rt->bottom;
		}
		else
		{
			m_CurCtrlRect.right	= max(m_CurCtrlRect.right, rt->left + rt->right);
			m_CurCtrlRect.bottom= max(m_CurCtrlRect.bottom, rt->top + rt->bottom);

			m_CurCtrlRect.left	= min(m_CurCtrlRect.left, rt->left);
			m_CurCtrlRect.top	= min(m_CurCtrlRect.top, rt->top);
		}
	}

	if( m_bFromControl && (m_pParentCtrl != NULL))
	{
		m_pParentCtrl->setSize( (REAL32) m_CurCtrlRect.right, (REAL32)m_CurCtrlRect.bottom); 
	}

	m_bNeedCalculateShapeRect = FALSE;

	return true;
}

void CPaneControlShape::InitTemplate(i3UITemplate * pElement)
{
	m_ShapeRectList.SafeReleaseAll();
	if( pElement == NULL )
	{
		m_pTemplate = NULL;
	}
	else
	{
		I3ASSERT( pElement->IsTypeOf( i3UITemplate::GetClassMeta()));

		m_pTemplate = pElement;

		m_bNeedCalculateShapeRect = TRUE;
		InitShapeRect( m_pTemplate );
	}

	m_listSelected.Clear();
	m_posScreen.SetPoint( 0,0);
	m_nScale = 1;
	Invalidate();
}

void CPaneControlShape::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_hwndFrom == this->m_hWnd )	return ;

	//Control Shape는 pInfo->m_pObject가 NULL이면 아무것도 안하는 윈도우임
	if( pInfo->m_pObject == NULL )
	{
		InitTemplate( NULL );
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT )
	{
		if(( pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH) || (pInfo->m_Event & I3_TDK_USRWND_LAYOUT))
		{
			if( pInfo->m_pMeta->IsTypeOf(i3UIControl::GetClassMeta()))
			{
				m_pParentCtrl = (i3UIControl*)pInfo->m_pObject;
				i3UIControlData * pData = m_pParentCtrl->GetControlData();

				m_bFromControl = TRUE;

				InitTemplate( pData->m_pShape );
			}
		}
		else if( pInfo->m_Event & I3_TDK_USRWND_RESLIST)
		{
			if( pInfo->m_pMeta->IsTypeOf( i3UITemplate::GetClassMeta()))
			{
				m_bFromControl	= FALSE;
				m_pParentCtrl	= NULL;
				InitTemplate((i3UITemplate*)pInfo->m_pObject);
			}
		}
		else
		{
			m_bFromControl  = FALSE;
			m_pParentCtrl	= NULL;
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SET)
	{
		if( pInfo->m_Event & I3_TDK_USRWND_MAINFRAME)
		{
			if( pInfo->m_pMeta->IsExactTypeOf( i3UIManager::GetClassMeta()))
			{
				m_pTemplate = NULL;
				Invalidate();
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT )
	{
		if( pInfo->m_Event & I3_TDK_USRWND_UVEDIT || pInfo->m_Event & I3_TDK_USRWND_PROPERTY)
		{
			//Image UV의 변경사항이 있는경우의 업데이트 메시지 입니다.
			//필요없는 작업을 줄이기위해 한가지씩 업데이트를 해야합니다.
			if( pInfo->m_pMeta == i3UIImage::GetClassMeta())
			{
				if( m_pTemplate != NULL )
				{
					m_bNeedCalculateShapeRect = TRUE;
					InitShapeRect(m_pTemplate);
					Invalidate();
				}
			}
		}
		else if( pInfo->m_Event & I3_TDK_USRWND_RESLIST)
		{
			if( pInfo->m_pMeta->IsTypeOf(i3UIImage::GetClassMeta()))
			{
				_InitUV( m_nCurShape, (i3UIImage*)pInfo->m_pObject);
			}
		}
	}
	/*
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		if( pInfo->m_pMeta->IsExactTypeOf( i3UITemplate::GetClassMeta()))
		{
			if( (m_pTemplate != NULL) && ( m_pTemplate == pInfo->m_pObject))
			{
				InitTemplate( NULL );
			}
		}
	}
	*/
}

BOOL CPaneControlShape::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	INT32 delta = 115;
	INT32 nCurrentScale = m_nScale;

	if( abs( zDelta ) > delta)
	{
		if( zDelta > 0)
		{
			m_nScale++;

			m_nScale = MIN( m_nScale, 4);
		}
		else
		{
			m_nScale--;

			m_nScale = MAX( m_nScale, 1);
		}

		if( nCurrentScale != m_nScale)
		{
			Invalidate();
		}
	}

	return TRUE;
}

LRESULT CPaneControlShape::WindowProc( UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate((I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc( message, wParam, lParam);
}

void CPaneControlShape::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call i3TDKDialogBase::OnPaint() for painting messages

	RECT WndRt;
	GetClientRect(&WndRt);

	RECT ctrlrect;
	ctrlrect = m_CurCtrlRect;

	ctrlrect.left = ctrlrect.left * m_nScale - m_posScreen.x;
	ctrlrect.top = ctrlrect.top * m_nScale - m_posScreen.y;
	ctrlrect.right = ctrlrect.right * m_nScale - m_posScreen.x;
	ctrlrect.bottom = ctrlrect.bottom * m_nScale - m_posScreen.y;

	dc.FillSolidRect(&ctrlrect,RGB(125,125,125));

	if( m_pTemplate == NULL )	return ;

	_OnDrawTemplate(dc.m_hDC);

	if( m_bLBDragging && m_posOldDrag != m_posStartDrag && (GetKeyState( VK_LCONTROL) & 0x80))
	{
		dc.SelectStockObject(WHITE_PEN);
		dc.SelectStockObject(NULL_BRUSH);

		CRect rt;
		rt.left		= MIN( m_posOldDrag.x, m_posStartDrag.x);
		rt.top		= MIN( m_posOldDrag.y, m_posStartDrag.y);
		rt.right	= MAX( m_posOldDrag.x, m_posStartDrag.x);
		rt.bottom	= MAX( m_posOldDrag.y, m_posStartDrag.y);

		::Rectangle( dc.m_hDC, rt.left, rt.top, rt.right, rt.bottom);

		dc.SelectStockObject(WHITE_PEN);
		dc.SelectStockObject(NULL_BRUSH);
		return;
	}

	char conv[MAX_PATH];

	HPEN hPen, hOldPen;
	hPen = CreatePen( PS_SOLID, 1, RGB( 255,0,255));
	
	for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
	{
		i3GuiToolRectElement * pElm = ( i3GuiToolRectElement*)m_ShapeRectList.GetItem( i);
		
		RECT *pRt = pElm->getRect();

		RECT rt;
		rt.left = pRt->left * m_nScale - m_posScreen.x;
		rt.top	= pRt->top * m_nScale - m_posScreen.y;
		rt.right = pRt->right * m_nScale + rt.left;
		rt.bottom = pRt->bottom * m_nScale + rt.top;

		if( m_listSelected.GetCount() > 0)
		{
			i3UIImage * pImage = m_pTemplate->getShape( i);
			INT32 idx = m_listSelected.IndexOf( pImage);
			if( idx != -1)
			{
				hOldPen = (HPEN)::SelectObject( dc.m_hDC, hPen);
				dc.SelectStockObject(NULL_BRUSH);

				::Rectangle( dc.m_hDC, rt.left, rt.top, rt.right, rt.bottom);

				::SelectObject( dc.m_hDC, hOldPen);
			}
		}

		sprintf( conv, "%d", i);
		if( (pElm->getElement() != NULL) && ( pElm->getElement()->getTexture() != NULL) && (i != m_nCurShape))
		{
			dc.SelectStockObject(WHITE_PEN);
			dc.SelectStockObject(NULL_BRUSH);
			dc.SetBkMode( TRANSPARENT);

			dc.TextOut( rt.right - 17, rt.bottom - 17, conv);
			continue;
		}

		if( i != m_nCurShape)
		{
			dc.SelectStockObject( NULL_PEN);
		}
		else
		{
			dc.SelectStockObject(WHITE_PEN);
		}

		if( (pElm->getElement() != NULL) && ( pElm->getElement()->getTexture() != NULL))
		{
			dc.SelectStockObject(NULL_BRUSH);
		}
		else
		{
			dc.SelectStockObject(DKGRAY_BRUSH);
		}

		::Rectangle( dc.m_hDC, rt.left, rt.top, rt.right, rt.bottom);

		dc.SelectStockObject(WHITE_PEN);
		dc.SelectStockObject(NULL_BRUSH);
		dc.SetBkMode( TRANSPARENT);
		dc.TextOut( rt.right - 17, rt.bottom - 17, conv);
	}
}

void CPaneControlShape::_UpdateBitmap( INT32	nIndex)
{
	if( m_pTemplate == NULL )	return;
	if( nIndex >= m_pTemplate->getShapeCount())	return ;

	i3UIImage			 * pImage	= m_pTemplate->getShape(nIndex);
	i3Texture			 * pTex		= pImage->m_pTexture;

	i3GuiToolRectElement * pElm		= (i3GuiToolRectElement*)m_ShapeRectList.GetItem( nIndex);
	i3GuiToolImageElement * pElmImage = (i3GuiToolImageElement*)g_pResList->FindElementByTexture( pTex);
	I3ASSERT( pElmImage != NULL);

	pElm->setElement( pElmImage);
}

void CPaneControlShape::_OnDrawTemplate(HDC hdc)
{
	INT32	nShapeCount = m_pTemplate->getShapeCount();
	INT32	i = 0;

	for(i = 0; i < nShapeCount; ++i)
	{
		i3UIImage * pImage = m_pTemplate->getShape(i);
		I3ASSERT( pImage != NULL );

		i3Texture * pTex = pImage->m_pTexture;
		if( pTex == NULL)
		{
			continue;
		}

		i3GuiToolRectElement	* pElm		= ( i3GuiToolRectElement *)m_ShapeRectList.GetItem( i);
		i3GuiToolImageElement	* pElmImage = pElm->getElement();

		RECT	*	pRt		= pElm->getRect();
		HBITMAP		hBitmap = pElmImage->getBitmap();
		i3Texture * pResTexture = pElmImage->getTexture();

		I3ASSERT( pResTexture != NULL);
		I3ASSERT( hBitmap	!= NULL);

		I3ASSERT( i3String::Compare( pResTexture->GetName(), pTex->GetName()) == 0);

		i3TDK::AlphaBlend(hdc,
					pRt->left * m_nScale - m_posScreen.x,
					pRt->top * m_nScale - m_posScreen.y,
					pRt->right * m_nScale,
					pRt->bottom * m_nScale,
					hBitmap,
					pImage->m_u,
					pImage->m_v,
					pImage->m_w,
					pImage->m_h
					);
		
	}	
}

void CPaneControlShape::_AddShape()
{
	/*
	if( m_pTemplate != NULL )
	{
		m_pTemplate->AddShape();

		m_bNeedCalculateShapeRect = TRUE;

		InitShapeRect(m_pTemplate);

		Invalidate();
	}*/
}

void CPaneControlShape::_DeleteShape(INT32	nShapIndex)
{
	/*
	if( m_pTemplate != NULL )
	{
		m_pTemplate->RemoveShape(nShapIndex);

		m_bNeedCalculateShapeRect = TRUE;
		InitShapeRect(m_pTemplate );

		Invalidate();
	}*/
}
void CPaneControlShape::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pTemplate == NULL )	return ;

	m_posOldDragScreen = point;
	m_bDragScreen = true;

	SetCapture();

	i3TDKDialogBase::OnMButtonDown(nFlags, point);
}

void CPaneControlShape::OnMButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragScreen )
	{
		m_bDragScreen = false;
		::ReleaseCapture();

		Invalidate();
	}

	i3TDKDialogBase::OnMButtonUp(nFlags, point);
}

//Mouse Action
void CPaneControlShape::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pTemplate == NULL )	return ;

	SetFocus();

	INT32	nShapeCount = m_pTemplate->getShapeCount();

	/*
	//Set CurShape
	//이전에 유효한 Shape가 설정되어있고, 선택된 리스트들이 없는경우라면..선택 리스트에 추가
	if(( nShapeCount > m_nCurShape) && (m_nCurShape >= 0) && (GetKeyState( VK_LCONTROL) & 0x80))
	{
		if( m_listSelected.GetCount() <= 0)
		{
			i3UIImage * pImage = m_pTemplate->getImage( m_nCurShape );
			if( pImage != NULL )
			{
				m_listSelected.Add( pImage);
			}
		}
	}*/

	//선택된 SHAPE가 유효하면 사각형의 각변에 걸쳐 있는지 파악
	if( m_nCurShape < nShapeCount )
	{
		_IsMouseOverEdge(point);
	}
	else
	{
		m_LastMouseOverEdge = EDGE_INDEX_MAX;
	}

	//걸쳐 있지 않으면 사각형의 내부에 있는지 파악하여 선택
	if( m_LastMouseOverEdge >= EDGE_INDEX_MAX)
	{
		_GetMouseOverShape(point);
	}

	//어떤 사각형의 내부에 있어 선택된 상태라면
	if( m_nCurShape < nShapeCount )
	{
		i3UIImage * pImage = m_pTemplate->getShape( m_nCurShape );
		if( pImage != NULL )
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_CTRLSHAPE, pImage );
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT | I3_TDK_USRWND_CTRLSHAPE, pImage);
		}

		//사각형의 변에 걸치지 않고 그냥 선택된 경우라면 MOVE모드 커서모양
		if( m_LastMouseOverEdge >= EDGE_INDEX_MAX)
		{
			SetCursor( g_pTDKGlobalRes->m_hCursorHand );
		}

		//컨트롤 키가 눌린상태로 마우스 왼쪽버튼이 클릭되면 선택 리스트에 추가
		if( GetKeyState( VK_LCONTROL) & 0x80)
		{
			INT32 idx = m_listSelected.IndexOf( pImage);
			if( idx == -1)
			{
				m_listSelected.Add( (void*)pImage);
			}
			else
			{
				m_listSelected.Remove( (void*)pImage);
			}
		}
		else
		{
			//선택리스트에 1개만 있고 컨트롤 키가 눌러져 있지 않다면, 리스트를 클리어하고 새선택된 shape를 추가합니다.
			if( m_listSelected.GetCount() <= 1)
			{
				m_listSelected.Clear();
				m_listSelected.Add( (void*)pImage);
			}
		}
	}

	Invalidate();

	if( GetKeyState( VK_LMENU) & 0x80)
	{
		m_bLBDragging = TRUE;
		m_posOldDrag  = point;
		m_posStartDrag= point;
		SetCapture();
	}

	i3TDKDialogBase::OnLButtonDown(nFlags, point);
}

INT32 CPaneControlShape::_GetMouseOverShape(CPoint point)
{
	m_nCurShape = -1;
	if( m_pTemplate != NULL )
	{
		INT32	i;
		INT32	nShapeCount = m_pTemplate->getShapeCount();
		m_nCurShape = nShapeCount;
		for( i = 0; i < nShapeCount; ++i)
		{
			i3GuiToolRectElement * pElm = ( i3GuiToolRectElement*)m_ShapeRectList.GetItem( i);
			RECT* pRT = pElm->getRect();
			RECT rt;

			rt.left = pRT->left * m_nScale - m_posScreen.x;
			rt.top = pRT->top * m_nScale - m_posScreen.y;
			rt.right = pRT->right * m_nScale;
			rt.bottom = pRT->bottom * m_nScale;

			rt.right += rt.left;
			rt.bottom += rt.top;

			if( ::PtInRect( &rt, point))
			{
				m_nCurShape = i;
			}
		}
	}

	return m_nCurShape;
}

INT32 CPaneControlShape::_IsMouseOverEdge(CPoint point)
{
	i3GuiToolRectElement * pElm = ( i3GuiToolRectElement*)m_ShapeRectList.GetItem( (INT32)m_nCurShape);
	RECT* pRt = pElm->getRect();

	INT32	left	= pRt->left * m_nScale - m_posScreen.x;
	INT32	top		= pRt->top * m_nScale - m_posScreen.y;
	INT32	right	= left + pRt->right * m_nScale;
	INT32	bottom	= top + pRt->bottom * m_nScale;

	m_LastMouseOverEdge = EDGE_INDEX_MAX;
	if( (left <= point.x) && (right >= point.x))
	{
		/*if( (point.y >= top - 2) && (point.y <= top + 2))		m_LastMouseOverEdge = EDGE_INDEX_TOP;
		else */if(( point.y >= bottom - 2) && (point.y <= bottom +2))	m_LastMouseOverEdge = EDGE_INDEX_BOTTOM;
	}

	if( (top <= point.y) && (bottom >= point.y))
	{
		/*if( (point.x >= left - 2) && (point.x <= left + 2) )		m_LastMouseOverEdge = EDGE_INDEX_LEFT;
		else */if( (point.x >= right - 2) && (point.x <= right +2))	m_LastMouseOverEdge = EDGE_INDEX_RIGHT;
	}

	if(( m_LastMouseOverEdge == EDGE_INDEX_LEFT) || ( m_LastMouseOverEdge == EDGE_INDEX_RIGHT))
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorSizeWE  );
	}
	else if((m_LastMouseOverEdge == EDGE_INDEX_TOP) || (m_LastMouseOverEdge == EDGE_INDEX_BOTTOM))
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorSizeNS);
	}
	else
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorArrow );
	}

	return m_LastMouseOverEdge;
}

void CPaneControlShape::_MoveShapeByDrag( CPoint curPos, INT32 nShapeIndex, bool bUpdate)
{
	if( nShapeIndex >= m_ShapeRectList.GetCount())
		return;

	LONG delX = (LONG)((m_posOldDrag.x - m_posStartDrag.x));
	LONG delY = (LONG)((m_posOldDrag.y - m_posStartDrag.y));

	LONG nextX = (LONG)((curPos.x - m_posStartDrag.x));
	LONG nextY = (LONG)((curPos.y - m_posStartDrag.y));

	POINT lastPt;
	lastPt.x = (nextX - delX);
	lastPt.y = (nextY - delY);

	_MoveShape(lastPt, nShapeIndex, bUpdate);
}

void CPaneControlShape::_MoveShape(CPoint delta, INT32 nShapeIndex, bool bUpdate)
{
	if( nShapeIndex >= m_ShapeRectList.GetCount())
		return;

	i3GuiToolRectElement * pElm = ( i3GuiToolRectElement*)m_ShapeRectList.GetItem( nShapeIndex);
	RECT* pRt = pElm->getRect();

	REAL32 moveX = ((REAL32)delta.x / m_nScale);
	REAL32 moveY = ((REAL32)delta.y / m_nScale);

	i3UIImage * pImage = m_pTemplate->getShape( nShapeIndex );
	if( pImage != NULL )
	{
		pImage->m_nX += moveX;
		pImage->m_nY += moveY;
	}

	pRt->left = (LONG)(pImage->m_nX);
	pRt->top = (LONG)(pImage->m_nY);

	m_bNeedCalculateShapeRect = TRUE;
/*
	if( bUpdate)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_CTRLSHAPE, m_pTemplate);
	}
	*/
}

void CPaneControlShape::_ChangeShapeSize( INT32	nEdge, CPoint nDelta)
{
	i3GuiToolRectElement * pElm = ( i3GuiToolRectElement*)m_ShapeRectList.GetItem( (INT32)m_nCurShape);
	RECT* pRt = pElm->getRect();

	i3UIImage * pImage = m_pTemplate->getShape( m_nCurShape );

	REAL32 deltaX = ((REAL32)nDelta.x / m_nScale);
	REAL32 deltaY = ((REAL32)nDelta.y / m_nScale);

	if( nEdge == EDGE_INDEX_LEFT )
	{
		if( pImage != NULL )
		{
			pImage->m_nX += deltaX;
			pImage->m_nWidth -= deltaX;
		}

		pRt->left = (LONG)pImage->m_nX;
		pRt->right = (LONG)pImage->m_nWidth;
	}
	else if( nEdge == EDGE_INDEX_RIGHT )
	{
		if( pImage != NULL )
		{
			pImage->m_nWidth += deltaX;
		}

		pRt->right = (LONG)pImage->m_nWidth;
	}
    else if( nEdge == EDGE_INDEX_TOP )
	{
		if( pImage != NULL )
		{
			pImage->m_nY += deltaY;
			pImage->m_nHeight -= deltaY;
		}

		pRt->top = (LONG)pImage->m_nY;
		pRt->bottom = (LONG)pImage->m_nHeight;
		
	}
	else if( nEdge == EDGE_INDEX_BOTTOM )
	{
		if( pImage != NULL )
		{
			pImage->m_nHeight += deltaY;
		}

		pRt->bottom = (LONG)pImage->m_nHeight;
		
	}
	else
	{
	 	return ;
	}

	m_bNeedCalculateShapeRect = TRUE;

	//i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_CTRLSHAPE, m_pTemplate);
}

void CPaneControlShape::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_pTemplate == NULL )	return ;
	// TODO: Add your message handler code here and/or call default
	if( m_bLBDragging )
	{
		if(!( GetKeyState( VK_LCONTROL) & 0x80))
		{
			RECT rt;
			CPoint delta = point - m_posOldDrag;

			//1개만 선택인경우
			if(( m_nCurShape >= 0) && (m_nCurShape < m_ShapeRectList.GetCount()) && (m_listSelected.GetCount() <= 1))
			{
				i3GuiToolRectElement * pElm = ( i3GuiToolRectElement*)m_ShapeRectList.GetItem( (INT32)m_nCurShape);
				RECT* pRt = pElm->getRect();
				rt = *pRt;

				if( m_LastMouseOverEdge < EDGE_INDEX_MAX )
				{
					_ChangeShapeSize( m_LastMouseOverEdge, delta );
				}
				else
				{
					_MoveShape( delta, m_nCurShape);
				}

				rt.right = max(rt.right + rt.left , pRt->left + pRt->right);
				rt.bottom = max(rt.bottom + rt.top, pRt->top + pRt->bottom);

				rt.left = min(rt.left, pRt->left);
				rt.top = min(rt.top, pRt->top);
				
				rt.left = rt.left * m_nScale - m_posScreen.x - 2;
				rt.top = rt.top * m_nScale - m_posScreen.y - 2;
				rt.right = rt.right * m_nScale - m_posScreen.x + 4;
				rt.bottom = rt.bottom * m_nScale - m_posScreen.y + 4;

				InvalidateRect(&rt);
			}
			else//선택되지 않았거나 여러개가 선택된 경우
			{
				if( m_listSelected.GetCount() > 0)
				{
					bool bUpdate = false;
					bool bFirst = true;
					rt.left = rt.right = rt.top = rt.bottom = 0;

					for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
					{
						i3UIImage * pImage = m_pTemplate->getShape( i);
						if( pImage != NULL)
						{
							INT32 idx = m_listSelected.IndexOf( pImage);
							if( idx != -1)
							{
								if( bFirst)
								{
									rt.left		= (LONG)(pImage->m_nX);
									rt.top		= (LONG)(pImage->m_nY);
									rt.right	= (LONG)(pImage->m_nWidth + pImage->m_nX);
									rt.bottom	= (LONG)(pImage->m_nHeight + pImage->m_nY);

									bFirst = false;
								}
								else
								{
									rt.right = (LONG)max(pImage->m_nWidth + pImage->m_nX, (REAL32)rt.right);
									rt.bottom = (LONG)max(pImage->m_nHeight + pImage->m_nY, (REAL32)rt.bottom);
									
									rt.left = (LONG)min((REAL32)rt.left, pImage->m_nX);
									rt.top = (LONG)min((REAL32)rt.top, pImage->m_nY);
								}

								_MoveShape( delta, i, false);
								bUpdate = true;

								{
									rt.right = (LONG)max(pImage->m_nWidth + pImage->m_nX, (REAL32)rt.right);
									rt.bottom = (LONG)max(pImage->m_nHeight + pImage->m_nY, (REAL32)rt.bottom);
									
									rt.left = (LONG)min((REAL32)rt.left, pImage->m_nX);
									rt.top = (LONG)min((REAL32)rt.top, pImage->m_nY);
								}
							}
						}
					}

					if( bUpdate)
					{
						rt.left = rt.left * m_nScale - 2 - m_posScreen.x;
						rt.top = rt.top * m_nScale -2 - m_posScreen.y;
						rt.right = rt.right * m_nScale + 4 - m_posScreen.x;
						rt.bottom = rt.bottom * m_nScale + 4 - m_posScreen.y;

						InvalidateRect( &rt);
					}
/*
					if( bUpdate)
					{
						i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_CTRLSHAPE, m_pTemplate);
						Invalidate();
					}
					*/
				}
			}
		}
		else
		{
			Invalidate();
		}

		m_posOldDrag = point;
	}
	else if( m_bDragScreen)
	{
		CPoint delta = point - m_posOldDragScreen;
		
		m_posScreen -= delta;
		m_posOldDragScreen = point;

		Invalidate();
	}
	else
	{
		if( m_nCurShape < m_pTemplate->getShapeCount())		
		{
			_IsMouseOverEdge( point);
		}
	}

	i3TDKDialogBase::OnMouseMove(nFlags, point);
}

void CPaneControlShape::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bLBDragging )
	{
		m_bLBDragging = FALSE;
		::ReleaseCapture();

		if( GetKeyState( VK_LCONTROL) & 0x80)
		{
			CRect rt;
			rt.left		= MIN( point.x, m_posStartDrag.x) + m_posScreen.x;
			rt.top		= MIN( point.y, m_posStartDrag.y) + m_posScreen.y;
			rt.right	= MAX( point.x, m_posStartDrag.x) + m_posScreen.x;
			rt.bottom	= MAX( point.y, m_posStartDrag.y) + m_posScreen.y;

			for( INT32 i = 0; i< m_pTemplate->getShapeCount(); ++i)
			{
				CRect rect, rect1;
				i3UIImage * pImage = m_pTemplate->getShape(i);
				if( pImage != NULL)
				{
					rect.left	= (LONG)pImage->getPosX() * m_nScale;
					rect.top	= (LONG)pImage->getPosY() * m_nScale;
					rect.right	= (LONG)pImage->getWidth() * m_nScale + rect.left;
					rect.bottom = (LONG)pImage->getHeight() * m_nScale + rect.top;

					if( rect1.IntersectRect( &rt, &rect))
					{
						INT32 idx = m_listSelected.IndexOf( pImage);
						if( idx == -1)
						{
							m_listSelected.Add( (void*) pImage);
						}
					}
				}
			}

			Invalidate();
		}
		else
		{
			if( InitShapeRect( m_pTemplate))
			{
				Invalidate();
			}
		}

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_CTRLSHAPE, m_pTemplate);
	}

	i3TDKDialogBase::OnLButtonUp(nFlags, point);
}

void CPaneControlShape::_SelectAllShape()
{
	for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
	{
		i3UIImage * pImage = m_pTemplate->getShape( i);
		if( pImage == NULL)
			continue;

		INT32 idx = m_listSelected.IndexOf( pImage);
		if( idx == -1)
		{
			m_listSelected.Add( pImage);
		}
	}

	if( m_listSelected.GetCount() > 0)
	{
		Invalidate();
	}
}

void CPaneControlShape::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pTemplate == NULL )	return ;

	_GetMouseOverShape(point);

	UINT32	cmd;
	CMenu Menu, * pPopup;
	Menu.LoadMenu( IDR_POPUP_CTRLSHAPE );

	POINT pt;
	::GetCursorPos(&pt);

	pPopup = Menu.GetSubMenu(0);

	INT32	nCount = m_pTemplate->getShapeCount();

	if( m_nCurShape >= nCount)
	{
		pPopup->EnableMenuItem( ID_SHAPE_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ADJUST_SIZEBY_UV, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_CTRLSHAPE_SETTEXTURE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_COPY_UV_TOALLOTHERS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}

	if( m_bFromControl )
	{
		pPopup->EnableMenuItem( ID_SHAPE_ADD, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_SHAPE_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
	}

	if( m_listSelected.GetCount() <= 1 && ((m_nCurShape < nCount) && (m_nCurShape >= 0)))
	{
		pPopup->EnableMenuItem( ID_ALIGN_LEFT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ALIGN_RIGHT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ALIGN_TOP, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ALIGN_BOTTOM, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ALIGN_CENTER_X, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ALIGN_CENTER_Y, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ALIGN_LEFTTOP, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		pPopup->EnableMenuItem( ID_ALIGN_RIGHTBOTTOM, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
	}

	if( m_nCurShape < 0 || m_nCurShape >= nCount)
	{
		m_listSelected.Clear();

		Invalidate();

		i3TDKDialogBase::OnRButtonDown(nFlags, point);
		return ;
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *)this, NULL);

	switch( cmd )
	{
	case ID_SHAPE_ADD:		_AddShape();	break;
	case ID_SHAPE_DELETE:
		{
			_DeleteShape( m_nCurShape );
			m_nCurShape = 100;
			break;
		}
	case ID_ADJUST_SIZEBY_UV:
		{
			if( m_listSelected.GetCount() >= 2)
			{
				_AdjustSizebyUV( -1);
			}
			else
			{
				_AdjustSizebyUV( m_nCurShape );
			}
		}
		break;
	case ID_CTRLSHAPE_SETTEXTURE:
		{
			_SetTexture( m_nCurShape );
		}
		break;
	case ID_COPY_UV_TOALLOTHERS:
		{
			_CopyTextureUVtoAllOthers( m_nCurShape);
		}
		break;

	case ID_COPY_SIZE_TOOTHERS:
		{
			_CopySizetoOthers( m_nCurShape, -1);
		}
		break;

	case ID_ALIGN_LEFT:
	case ID_ALIGN_RIGHT:
	case ID_ALIGN_TOP:
	case ID_ALIGN_BOTTOM:
	case ID_ALIGN_CENTER_X:
	case ID_ALIGN_CENTER_Y:
	case ID_ALIGN_LEFTTOP:
	case ID_ALIGN_RIGHTBOTTOM:
		_SetAlign( cmd, m_nCurShape);
		break;
	case ID_CTRLSHAPE_SELECTALL:
		_SelectAllShape();
		break;
	case ID_SPREADSHAPES_DEFAULT:
		_SpreadShapes( 10, 1.0f, 1.0f, 1, true);
		break;
	case ID_SPREADSHAPES_ALL:
		_SpreadShapes( 10, 1.0f, 1.0f, 0, true);
		break;
	case ID_SPREADSHAPES_SELECTED:
		_SpreadShapes( 10, 1.0f, 1.0f, 0, false);
		break;
	case ID_SPREADSHAPES_DEFAULT_BYVALUE:
		_SpreadShapes( 10, 1.0f, 1.0f, 1, true, true);
		break;
	case ID_SPREADSHAPES_ALL_BYVALUE:
		_SpreadShapes( 10, 1.0f, 1.0f, 0, true, true);
		break;
	case ID_SPREADSHAPES_SELECTED_BYVALUE:
		_SpreadShapes( 10, 1.0f, 1.0f, 0, false, true);
		break;
	default:
		break;
	}

	i3TDKDialogBase::OnRButtonDown(nFlags, point);
}

void CPaneControlShape::_CopyTextureUVtoAllOthers( INT32 OriginShape)
{
	if( m_nCurShape < 0 || m_nCurShape >= m_pTemplate->getShapeCount())
		return;

	if( m_pTemplate->getShapeCount() <= 1)
		return;

	i3UIImage * pOrigin = (i3UIImage*)m_pTemplate->getShape( m_nCurShape);
	if( pOrigin == NULL)
		return;

	if( pOrigin->m_pTexture == NULL)
		return;

	for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
	{
		if( i == m_nCurShape)
			continue;

		i3UIImage * pImage = m_pTemplate->getShape( i);
		if( pImage == NULL)
			continue;

		bool bChange = true;
		if( m_listSelected.GetCount() >= 2)
		{
			INT32 idx = m_listSelected.IndexOf( pImage);
			if( idx == -1)
			{
				bChange = false;
			}
		}

		if( bChange)
		{
			pImage->m_u = pOrigin->m_u;
			pImage->m_v = pOrigin->m_v;
			pImage->m_w = pOrigin->m_w;
			pImage->m_h = pOrigin->m_h;
		}
	}

	Invalidate();
}

void CPaneControlShape::_SetAlign( UINT32 ncmd, INT32 OriginShape)
{
	if( m_listSelected.GetCount() <= 1)
		return;

	if( m_nCurShape < 0 || m_nCurShape >= m_pTemplate->getShapeCount())
		return;

	i3UIImage * pOrigin = (i3UIImage*)m_pTemplate->getShape( m_nCurShape);
	if( pOrigin == NULL)
		return;

	REAL32 X		= pOrigin->getPosX();
	REAL32 Y		= pOrigin->getPosY();
	REAL32 Width	= pOrigin->getWidth();
	REAL32 Height	= pOrigin->getHeight();

	bool bUpdate = false;
	for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
	{
		i3UIImage * pImage = m_pTemplate->getShape( i);
		if( pImage == NULL)
			continue;

		INT32 idx = m_listSelected.IndexOf( pImage);
		if( idx == -1)
			continue;

		if( i == m_nCurShape)
			continue;

		CPoint delta;
		delta.SetPoint( 0,0);
		switch( ncmd)
		{
			case ID_ALIGN_LEFT:
				delta.x = (LONG)(X - pImage->m_nX);
				break;

			case ID_ALIGN_RIGHT:
				delta.x = (LONG)(X + Width - pImage->getWidth() - pImage->m_nX);
				break;

			case ID_ALIGN_TOP:
				delta.y = (LONG)(Y - pImage->m_nY);
				break;

			case ID_ALIGN_BOTTOM:
				delta.y = (LONG)(Y + Height - pImage->getHeight() - pImage->m_nY);
				break;

			case ID_ALIGN_CENTER_X:
				delta.x = (LONG)(X + Width/2 - pImage->getWidth()/2 - pImage->m_nX);
				break;

			case ID_ALIGN_CENTER_Y:
				delta.y = (LONG)(Y + Height/2 - pImage->getHeight()/2 - pImage->m_nY);
				break;
			case ID_ALIGN_LEFTTOP:
				delta.x = (LONG)(X - pImage->m_nX);
				delta.y = (LONG)(Y - pImage->m_nY);
				break;
			case ID_ALIGN_RIGHTBOTTOM:
				delta.x = (LONG)(X + Width - pImage->getWidth() - pImage->m_nX);
				delta.y = (LONG)(Y + Height - pImage->getHeight() - pImage->m_nY);
				break;
			default:
				break;
		}

		if( delta.x != 0 || delta.y != 0)
		{
			delta.x = delta.x * m_nScale;
			delta.y = delta.y * m_nScale;
			_MoveShape( delta, i, false);
			bUpdate = true;
		}
	}

	if( bUpdate)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_CTRLSHAPE, m_pTemplate);
		Invalidate();
	}
}

void CPaneControlShape::_SetTexture( INT32	nShapeIndex )
{
	if( m_pTemplate == NULL )	return ;

	i3UIImage * pImage = m_pTemplate->getShape( nShapeIndex );
	if( pImage != NULL )
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REQUEST | I3_TDK_USRWND_CTRLSHAPE, pImage);
	}
}

void CPaneControlShape::_InitUV(INT32	nShapeIndex, i3UIImage * pImage)
{
	if( pImage == NULL )
		return ;

	if( nShapeIndex >= m_pTemplate->getShapeCount())
	{
		return ;
	}

	i3Texture * pTex = pImage->m_pTexture;
	if( pTex != NULL )
	{
		for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
		{
			i3UIImage * pOtherImage = m_pTemplate->getShape( i);
			if( pOtherImage->m_pTexture != pTex)
			{
				pOtherImage->SetTexture( pTex);
			}

			if( i == nShapeIndex)
			{
				i3GuiToolRectElement * pElm = ( i3GuiToolRectElement*)m_ShapeRectList.GetItem( (INT32)m_nCurShape);
				RECT* pRt = pElm->getRect();

				pOtherImage->m_nX		= (REAL32)pRt->left;
				pOtherImage->m_nY		= (REAL32)pRt->top;
				pOtherImage->m_nWidth	= (REAL32)pRt->right;
				pOtherImage->m_nHeight	= (REAL32)pRt->bottom;

				{
					pOtherImage->m_u = 0;
					pOtherImage->m_v = 0;
					pOtherImage->m_w = pTex->GetWidth();
					pOtherImage->m_h = pTex->GetHeight();
				}
			}

			_UpdateBitmap(i);
		}

		Invalidate();
	}
}

void CPaneControlShape::_AdjustSizebyUV( INT32	nShapeIndex)
{
	if( m_pTemplate == NULL )	return ;

	if( nShapeIndex == -1)
	{
		if( m_listSelected.GetCount() >= 2)
		{
			for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
			{
				i3UIImage * pImage = m_pTemplate->getShape( i);
				if( m_listSelected.IndexOf( pImage) != -1)
				{
					if( pImage->m_pTexture != NULL )
					{
						pImage->m_nWidth  = (REAL32)pImage->m_w;
						pImage->m_nHeight = (REAL32)pImage->m_h;
					}
				}
			}
		}
	}
	else
	{
		i3UIImage * pImage = m_pTemplate->getShape( nShapeIndex );
		if( pImage->m_pTexture != NULL )
		{
			pImage->m_nWidth  = (REAL32)pImage->m_w;
			pImage->m_nHeight = (REAL32)pImage->m_h;
		}
	}

	m_bNeedCalculateShapeRect = TRUE;
	InitShapeRect(m_pTemplate);
	Invalidate();
}

void CPaneControlShape::_CopySizetoOthers( INT32 nSrcIndex, INT32 nShapeIndex)
{
	if( m_pTemplate == NULL )	return ;

	i3UIImage * pSrc = m_pTemplate->getShape( nSrcIndex);
	if( pSrc == NULL)
		return;

	if( nShapeIndex == -1)
	{
		if( m_listSelected.GetCount() >= 2)
		{
			for( INT32 i = 0; i < m_pTemplate->getShapeCount(); ++i)
			{
				i3UIImage * pImage = m_pTemplate->getShape( i);
				if( m_listSelected.IndexOf( pImage) != -1)
				{
					if( pImage->m_pTexture != NULL )
					{
						pImage->m_nWidth  = (REAL32)pSrc->m_nWidth;
						pImage->m_nHeight = (REAL32)pSrc->m_nHeight;
					}
				}
			}
		}
	}
	else
	{
		i3UIImage * pImage = m_pTemplate->getShape( nShapeIndex );
		if( pImage->m_pTexture != NULL )
		{
			pImage->m_nWidth  = (REAL32)pSrc->m_nWidth;
			pImage->m_nHeight = (REAL32)pSrc->m_nHeight;
		}
	}

	m_bNeedCalculateShapeRect = TRUE;
	InitShapeRect(m_pTemplate);
	Invalidate();
}

BOOL CPaneControlShape::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return i3TDKDialogBase::OnEraseBkgnd(pDC);
}

BOOL CPaneControlShape::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN)
	{
		TranslateMessage( pMsg );

		DispatchMessage( pMsg );

		return TRUE;
	}

	return i3TDKDialogBase::PreTranslateMessage(pMsg);
}

void CPaneControlShape::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( m_pTemplate == NULL )	return ;

	CPoint pt( 0,0);

	INT32	nEdge = EDGE_INDEX_LEFT;

	bool bMoveCommand = true;
	switch( nChar )
	{
		case VK_LEFT:
			{
				pt.x = - m_nScale;
				nEdge = EDGE_INDEX_RIGHT;
			}
			break;
			
		case VK_RIGHT:
			{
				pt.x = m_nScale;
				nEdge = EDGE_INDEX_RIGHT;
			}
			break;


		case VK_DOWN:
			{
				pt.y = m_nScale;
				nEdge = EDGE_INDEX_BOTTOM;
			}
			break;
			
		case VK_UP:		
			{
				pt.y = - m_nScale;
				nEdge = EDGE_INDEX_BOTTOM;
			}
			break;
		case VK_TAB:
			{
				bMoveCommand = false;
				INT32 curshape = m_nCurShape + 1;

				if( m_listSelected.GetCount() >=2)
					break;

				if( m_pTemplate->getShapeCount() <= curshape)
					curshape = 0;

				i3UIImage * pImage = m_pTemplate->getShape( curshape);
				if( pImage == NULL)
					break;

				m_nCurShape = curshape;

				m_listSelected.Clear();
				m_listSelected.Add( pImage);
			}
			break;
		default:
			return;
	}

	if( bMoveCommand)
	{
		if( GetKeyState( VK_LSHIFT ) & 0x80)
		{
			_ChangeShapeSize( nEdge, pt); 
		}
		else
		{
			_MoveShape( pt, m_nCurShape);
		}

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_CTRLSHAPE, m_pTemplate);
	}

	Invalidate();

	i3TDKDialogBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

void	CPaneControlShape::_SpreadShapes( INT32 nCol, REAL32 rSeperateX, REAL32 rSeperateY, INT32 nOrigin, bool bAll, bool byValue)
{
	if( m_pTemplate == NULL || m_pTemplate->getShapeCount() <= 2)
		return;

	if( m_nCurShape >= m_pTemplate->getShapeCount())
		return;

	if( byValue)
	{
		CSpreadPropertyDlg dlg;
		if( dlg.DoModal() == IDOK)
		{
			nCol = dlg.m_nColCount;
			rSeperateX = dlg.m_nSpreadWidth;
			rSeperateY = dlg.m_nSpreadHeight;
		}
		else
		{
			return;
		}
	}

	i3UIImage * pImage = m_pTemplate->getShape( m_nCurShape);
	I3ASSERT( pImage != NULL);

	INT32 nCurCol = 0;

	REAL32 cOriginX = pImage->m_nX;
	REAL32 cOriginY = pImage->m_nY;

	if( nOrigin == 1)
	{
		cOriginX = 0.0f;
		cOriginY = 0.0f;
	}
	
	REAL32 nextPosY = cOriginY;
	REAL32 nextPosX = cOriginX;

	REAL32 maxHeight = 0.0f;

	for( INT32 i = 0;i  < m_pTemplate->getShapeCount(); ++i)
	{
		i3UIImage * pDstImage = m_pTemplate->getShape( i);

		if( !bAll)
		{
			INT32 idx = m_listSelected.IndexOf( pDstImage);
			if( idx == -1)
				continue;
		}

		pDstImage->m_nX = nextPosX;
		pDstImage->m_nY = nextPosY;

		nextPosX += pDstImage->m_nWidth + rSeperateX;
		maxHeight = max( pDstImage->m_nHeight, maxHeight);

		nCurCol++;
		if(( nCurCol % nCol) == 0)
		{
			nCurCol = 0;
			nextPosX = cOriginX;
			nextPosY += maxHeight + rSeperateY;
			maxHeight = 0.0f;
		}
	}

	m_bNeedCalculateShapeRect = TRUE;

	InitShapeRect( m_pTemplate );

	Invalidate();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_CTRLSHAPE, m_pTemplate);
}