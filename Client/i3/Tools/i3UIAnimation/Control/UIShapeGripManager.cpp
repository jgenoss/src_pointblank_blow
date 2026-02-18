#include "StdAfx.h"
#include "UIShapeGripManager.h"



I3_CLASS_INSTANCE( CUIShapeGripManager); //, i3NamedElement);

CUIShapeGripManager::CUIShapeGripManager(void)
{
	m_pControllerMeta = NULL;

	m_pFocusedPoint = NULL;
	m_pFocusedShape = NULL;
	m_pCurFocusShape = NULL;
	m_bLBDown	= false;

	m_ScrollPos.x	= m_ScrollPos.y = 0;
	m_OffsetPos.x	= m_OffsetPos.y = 0;
	m_ZoomRate		= 1.0f;

	m_SnapSize		= DEFAULT_SNAP_SIZE;

	m_ptAnchor = CPoint(0,0);
	m_szAnchor = CPoint(0,0);
	m_nAnchorStyle = 0;

	m_bTransforming = false;

	m_mode = GCM_NONE;

	m_ptDelta.SetPoint(0,0);

	// Banding
	{
		m_BandRect.SetRect( 0, 0, 0, 0);
		m_bBanding = false;
		m_hbrBand = i3TDK::CreateHalftoneBrush();
	}
}

CUIShapeGripManager::~CUIShapeGripManager(void)
{
	if( m_hbrBand != NULL)
	{
		::DeleteObject( m_hbrBand);
		m_hbrBand = NULL;
	}

	Reset();
}

void CUIShapeGripManager::Create( i3ClassMeta * pMeta)
{
	m_pControllerMeta = pMeta;
}

void CUIShapeGripManager::Reset(void)
{	
	for( size_t i=m_SelectedList.size(); i > 0; i--)
	{
		CUIGripCtrlRect * pRect = static_cast<CUIGripCtrlRect*>(m_SelectedList[i - 1]);

		//pRect->OnUnSelect();		// TDK Update Notify 과정에서 m_SelectedList에 있는 Control에 접근하는 경우가 생겨 Crash로 이어진다.
		pRect->Release();
	}
	m_SelectedList.clear();

	RemoveAllSnapPoint();

	m_pFocusedPoint = NULL; 
	m_pFocusedShape = NULL;
	m_pCurFocusShape = NULL;
}

INT32 CUIShapeGripManager::RemoveSelect( void * pElement)
{
	for( size_t i = 0; i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

		if( pRect->getObject() == pElement)
		{
			//Tricky part
			i3::vu::erase_index(m_SelectedList, i);
			INT32 nResult = m_SelectedList.size();	//Delete
			
			pRect->OnUnSelect();	//Notify

			pRect->Release();		//Release

			if( m_SelectedList.size() == 1)
			{
				CUIGripCtrlRect* pSecondRect = (CUIGripCtrlRect*)m_SelectedList[0];
				pSecondRect->setPenColor( COLORSET_GRIP_NORMAL_BORDER);
				pSecondRect->setBrushColor( COLORSET_GRIP_NORMAL_BACK);
			}
			else if( i == m_SelectedList.size() && m_SelectedList.size() > 1)
			{
				// 마지막 원소가 삭제되었다면 
				// 새로 마지막이 된 Grip의 색을 바꿔준다. 
				CUIGripCtrlRect* pSecondRect = (CUIGripCtrlRect*)m_SelectedList[ m_SelectedList.size()-1];
				pSecondRect->setPenColor( COLORSET_GRIP_MULTIPLE_BORDER);
				pSecondRect->setBrushColor( COLORSET_GRIP_MULTIPLE_BACK);
			}

			return nResult;
		}
	}

	return -1;
}

void CUIShapeGripManager::RemoveAllSelect()
{
	while(m_SelectedList.size() > 0)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[0];

		//Tricky part
		i3::vu::erase_index(m_SelectedList, 0);
		pRect->OnUnSelect();	//Notify
		pRect->Release();		//Release
	}
}

void CUIShapeGripManager::getSelectedObjList( i3::vector<i3UIControl*>& Out)
{
	size_t count = m_SelectedList.size();

	for( size_t i=0; i < count; i++)
	{
		CUIGripCtrl * pCtrl		= m_SelectedList[i];
		I3ASSERT( pCtrl != NULL);

		CUIGripObjHandler * pHandler = pCtrl->getHandler();
		
		if( pHandler == NULL)
			continue;

		if( !i3::kind_of<CUIGripObjHandler_RECT*>(pHandler)) //->IsTypeOf( CUIGripObjHandler_RECT::static_meta()) == FALSE)
		{
			i3UIControl* pObj = (i3UIControl*)pCtrl->getObject();
		
			Out.push_back( pObj);
		}
	}
}

bool CUIShapeGripManager::isSelected(CPoint point)
{
	for( size_t i = 0;i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];
		if( pRect->ptInRect(point))
		{
			return true;
		}
	}

	return false;
}

CUIGripCtrlRect* CUIShapeGripManager::FindElement(void* pElement)
{
	for (size_t i = 0; i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];
		if (pRect->getObject() == pElement)
		{
			return pRect;
		}
	}

	return false;
}

INT32 CUIShapeGripManager::AddSelect( void * pElement, i3ClassMeta * pMeta, bool bReverse)
{
	RemoveAllSnapPoint();

	if( bReverse == false)
	{
		
		for(size_t i = 0;i < m_SelectedList.size(); ++i)
		{
			CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];
			if( pRect->getObject() == pElement)
			{
				return i;
			}
		}

		//Reset();			이거 없어야 한다.
	}
	else
	{
		
		for(size_t i = 0;i < m_SelectedList.size(); ++i)
		{
			CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

			if( pRect->getObject() == pElement)
			{
				return RemoveSelect( pElement);
			}
		}
	}

	CUIGripCtrlRect * pNewRect = CUIGripCtrlRect::new_object();

	CUIGripObjHandler * pHandler = NULL;
	if( pMeta == NULL)
		pHandler = (CUIGripObjHandler*)m_pControllerMeta->create_instance(); // CREATEINSTANCE( m_pControllerMeta);
	else
		pHandler = (CUIGripObjHandler*)pMeta->create_instance(); // CREATEINSTANCE( pMeta);

	pHandler->setObject( pElement);
	pNewRect->Create( pHandler);

	pNewRect->setOffsetValues( m_ScrollPos, m_OffsetPos, m_ZoomRate);

	AllPenColorNormal();

	INT32 nResult = (INT32)m_SelectedList.size();
	m_SelectedList.push_back( pNewRect);	//마지막에 선택된게 맨 뒤

	if (m_SelectedList.size() > 0)
	{
		pNewRect->setPenColor(COLORSET_GRIP_MULTIPLE_BORDER);
		pNewRect->setBrushColor(COLORSET_GRIP_MULTIPLE_BACK);
	}

	pNewRect->OnSelect();

	return nResult;
}

bool CUIShapeGripManager::isOnGrip( CPoint point)
{
	if( _hasFocusedCtrl( point))
		return true;

	return false;
}

// 주어지는 list내에 있는 shape가 포함하는 point중 마우스가 위에 올라가 있는 포인트가 있는지 찾아본다.
CUIGripCtrlPoint * CUIShapeGripManager::_hasFocusedCtrl( CPoint point)
{
	for (size_t i = m_SelectedList.size() - 1; i >= 0; --i)
	{
		if (m_SelectedList.size() < i) return NULL;

		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];
		CUIGripCtrlPoint * pPoint = pRect->isPoint( point);
		if( pPoint != NULL)
			return pPoint;
	}

	return NULL;
}

// 주어지는 list내에 있는 shape중 마우스가 위에 올라가 있는 shape가 있는지 찾아본다.
CUIGripCtrlRect * CUIShapeGripManager::_hasFocusedShape( CPoint point)
{
	for(size_t i = m_SelectedList.size()-1; i >= 0; --i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

		if( pRect->ptInRect( point))
			return pRect;
	}

	return NULL;
}

void	CUIShapeGripManager::SetZoomRate( REAL32 zoom)
{
	
	for(size_t i = 0;i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

		pRect->setZoomRate( zoom);
	}
	
	m_ZoomRate		= zoom;
}

void	CUIShapeGripManager::SetScrollPos( POINT pt)
{

	for( size_t i = 0;i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

		pRect->setScrollPos( pt);
	}

	m_ScrollPos.x = pt.x;
	m_ScrollPos.y = pt.y;
}

void	CUIShapeGripManager::SetOffsetPos( POINT pt)
{
	
	for(size_t i = 0;i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

		pRect->setOffsetPos( pt);
	}

	m_OffsetPos.x = pt.x;
	m_OffsetPos.y = pt.y;
}

bool CUIShapeGripManager::OnLButtonDown( CPoint point)
{
	//if( m_SelectedList.GetCount() <= 0)
	//	return false;

	m_pFocusedPoint = NULL;
	m_pFocusedShape	= NULL;
	m_bLBDown		= true;
	m_ptOldPos		= m_ptDragStart	= point;

	CUIGripCtrlPoint * pFocused = _hasFocusedCtrl( point);
	if( pFocused != NULL)
	{
		m_pFocusedPoint = pFocused;
		m_mode			= GCM_SIZING;

		//_FixedRatioTransform

		CUIGripCtrl* pCtrl = m_pFocusedPoint->getOwner();
		UINT32 style = m_pFocusedPoint->getStyle();

		INT32 l = pCtrl->getPosX();
		INT32 t = pCtrl->getPosY();
		INT32 r = l + pCtrl->getWidth();
		INT32 b = t + pCtrl->getHeight();

		switch( style)
		{
		case UIGRIP_CORNER_LEFT:
		case UIGRIP_CORNER_TOP:
		case UIGRIP_CORNER_LEFT | UIGRIP_CORNER_TOP:
			{
				m_nAnchorStyle =  UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_BOTTOM;
				m_ptAnchor.x = r;
				m_ptAnchor.y = b;
			}
			break;
		case UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_TOP:
			{
				m_ptAnchor.x = l;	
				m_ptAnchor.y = b;	
				m_nAnchorStyle = UIGRIP_CORNER_LEFT | UIGRIP_CORNER_BOTTOM;
			}
			break;
		case UIGRIP_CORNER_LEFT | UIGRIP_CORNER_BOTTOM:		
			{
				m_ptAnchor.x = r;	
				m_ptAnchor.y = t;	
				m_nAnchorStyle = UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_TOP;
			}
			break;
		case UIGRIP_CORNER_RIGHT:
		case UIGRIP_CORNER_BOTTOM:
		case UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_BOTTOM:	
			{
				m_ptAnchor.x = l;	
				m_ptAnchor.y = t;	
				m_nAnchorStyle = UIGRIP_CORNER_LEFT | UIGRIP_CORNER_TOP;
			}
			break;
		default:	
			break;
		}

		m_szAnchor = CPoint( pCtrl->getWidth(), pCtrl->getHeight());
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)pCtrl;
	
		AllPenColorNormal();

		pRect->setPenColor(COLORSET_GRIP_MULTIPLE_BORDER);
		pRect->setBrushColor(COLORSET_GRIP_MULTIPLE_BACK);
		
		m_pCurFocusShape = pCtrl;
		//End _FixedRatioTransform

		return true;
	}
	else
	{
		CUIGripCtrlRect * pRect = _hasFocusedShape( point);
		if( pRect != NULL)
		{
			AllPenColorNormal();

			pRect->setPenColor(COLORSET_GRIP_MULTIPLE_BORDER);
			pRect->setBrushColor(COLORSET_GRIP_MULTIPLE_BACK);

			m_pFocusedShape = pRect;
			m_pCurFocusShape = pRect;
			m_mode			= GCM_MOVING;

			return true;
		}
	}

	return false;
}

bool CUIShapeGripManager::OnLButtonUp( CPoint point)
{
	_EndTransform();

	m_pFocusedPoint = NULL;
	m_pFocusedShape = NULL;
	m_mode			= GCM_NONE;
	m_bLBDown		= false;

	return true;
}

bool CUIShapeGripManager::OnMouseMove( CPoint point, UINT32 nFlags, CPoint* ptDelta)
{
	bool bRet = false;

	if( m_bLBDown == true)
	{
		CPoint delta = CPoint( 0, 0);

		if( (nFlags & MK_SHIFT) == 0)	//Shift 가 안눌려져 있으면
		{
			CPoint snappedOldPos = GetSnappedPoint( m_ptOldPos);
			CPoint snappedPoint = GetSnappedPoint( point);
			
			delta = snappedPoint - snappedOldPos;

			if( delta.x != 0)
				m_ptOldPos.x = point.x;
			if( delta.y != 0)
				m_ptOldPos.y = point.y;

			point = snappedPoint;
		}
		else	//Shift를 누른 상태
		{
			delta = point - m_ptOldPos;
			m_ptOldPos = point;
		}

		if (ptDelta)
			ptDelta->SetPoint(delta.x,delta.y);
		if( m_bTransforming == false)
		{
			if( delta.x != 0 || delta.y != 0)
			{
				I3TRACE("_BeginTransform!!!!!\n");
				_BeginTransform();
			}
		}

		if( m_pFocusedPoint != NULL)
		{	
			if( nFlags & MK_CONTROL)	
			{
				// Fixed Ratio Transform

				I3TRACE("_FixedRatioTransform: Before %d, %d, %d, %d\n", m_pFocusedPoint->getPosition().x, m_pFocusedPoint->getPosition().y, point.x, point.y);

				_FixedRatioTransform( m_pFocusedPoint, point);

				I3TRACE("_FixedRatioTransform: After %d, %d, %d, %d\n", m_pFocusedPoint->getPosition().x, m_pFocusedPoint->getPosition().y, point.x, point.y);
			}
			else
			{
				if( _isResizablePoint( m_pFocusedPoint))
					m_pFocusedPoint->OnMoving( delta);			
			}
		}
		else if( m_pFocusedShape != NULL)
		{
			//for(size_t i = 0;i < m_SelectedList.size(); ++i)
			{
				CUIGripCtrlRect * pRect = m_pFocusedShape;
				
				if( _isMovableRect( pRect))
					pRect->OnMoving( delta);
			
			}
		}

		bRet = true;	// select된것이 없는경우엔 return 이 false되게하려고 일부러 여기 넣었음.
	}
	else
	{
		m_ptOldPos = point;
	}
	return bRet;
}

void CUIShapeGripManager::OnKeyDown( UINT32 nKey, UINT32 nFlags)
{
	if( nFlags & GRIP_CTRL_PRESSED)	// Sizing
	{
		CPoint delta = CPoint( 0, 0);
		UINT32 style = 0;
		INT32 nStep = (nFlags & GRIP_SHIFT_PRESSED) ? 1 : DEFAULT_SNAP_SIZE;
		
		switch( nKey)
		{
		case VK_LEFT:	delta = CPoint( -nStep, 0);		style = UIGRIP_CORNER_RIGHT;	break;
		case VK_RIGHT:	delta = CPoint( nStep, 0);		style = UIGRIP_CORNER_RIGHT;	break;
		case VK_UP:		delta = CPoint( 0, -nStep);		style = UIGRIP_CORNER_BOTTOM;	break;
		case VK_DOWN:	delta = CPoint( 0, nStep);		style = UIGRIP_CORNER_BOTTOM;	break;
		default: break;
		}

		for( size_t i = 0;i < m_SelectedList.size(); ++i)
		{
			CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];
			
			const i3::vector<CUIGripCtrlPoint*>& pointList = pRect->getPointList();
			for( size_t j=0; j< pointList.size(); j++)
			{
				CUIGripCtrlPoint* pPoint = pointList[j];
				
				if( (pPoint->getStyle() == style) && _isResizablePoint( pPoint))
				{
					pPoint->OnMoving( delta);
					break;
				}
			}
		}
	}
	else	// Moving
	{
		CPoint delta = CPoint( 0, 0);
		INT32 nStep = (nFlags & GRIP_SHIFT_PRESSED) ? 1 : DEFAULT_SNAP_SIZE;

		switch( nKey)
		{
		case VK_LEFT:	delta = CPoint( -nStep, 0);		break;
		case VK_RIGHT:	delta = CPoint( nStep, 0);		break;
		case VK_UP:		delta = CPoint( 0, -nStep);		break;
		case VK_DOWN:	delta = CPoint( 0, nStep);		break;
		default: break;
		}

		for( size_t i = 0;i < m_SelectedList.size(); ++i)
		{
			CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];
			
			if( _isMovableRect( pRect))
				pRect->OnMoving( delta);
		}
	}

	RecalcLayout();
}


HCURSOR CUIShapeGripManager::getCursor( CPoint point)
{
	HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW);

	CUIGripCtrlPoint* pGripPoint = _hasFocusedCtrl(point);
	if( pGripPoint != NULL)
	{
		UINT32 style = pGripPoint->getStyle();

		switch( style)
		{
		case UIGRIP_CORNER_LEFT:
		case UIGRIP_CORNER_RIGHT:
			hCursor = ::LoadCursor( NULL, IDC_SIZEWE);
			break;
		case UIGRIP_CORNER_TOP:
		case UIGRIP_CORNER_BOTTOM:
			hCursor = ::LoadCursor( NULL, IDC_SIZENS);
			break;
		case UIGRIP_CORNER_TOP|UIGRIP_CORNER_LEFT:
		case UIGRIP_CORNER_BOTTOM|UIGRIP_CORNER_RIGHT:
			hCursor = ::LoadCursor( NULL, IDC_SIZENWSE);
			break;
		case UIGRIP_CORNER_TOP|UIGRIP_CORNER_RIGHT:
		case UIGRIP_CORNER_BOTTOM|UIGRIP_CORNER_LEFT:
			hCursor = ::LoadCursor( NULL, IDC_SIZENESW);
			break;
		default:
			break;
		}
	}
	
	return hCursor;
	//SetCursor로 하면 마우스클릭시 다시 원래대로 돌아감..
	//::SetClassLong( g_hActiveView, GCL_HCURSOR, (LONG) hCursor);
	
	//::UpdateWindow( g_hActiveView);
	//::InvalidateRect( g_hActiveView, NULL, TRUE);
}

void CUIShapeGripManager::Draw( HDC hdc)
{
	
	for(size_t i = 0;i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

		REAL32 oldx = pRect->getPosX();
		REAL32 oldy	= pRect->getPosY();

		REAL32 x = pRect->getPosX() - (m_ptDelta.x / m_ZoomRate) ;
		REAL32 y = pRect->getPosY() - (m_ptDelta.y / m_ZoomRate) ;

		pRect->setPos(x, y);

		pRect->OnDraw( hdc);

		pRect->setPos(oldx, oldy);
	}

	if( m_bBanding)
	{
		//::SelectObject( hdc, GetStockObject( BLACK_PEN));
		CRect rt;

		rt.left		= MIN( m_BandRect.left, m_BandRect.right);
		rt.top		= MIN( m_BandRect.top, m_BandRect.bottom);
		rt.right	= MAX( m_BandRect.left, m_BandRect.right);
		rt.bottom	= MAX( m_BandRect.top, m_BandRect.bottom);

		::FrameRect( hdc, &rt, m_hbrBand);
	}
}

void CUIShapeGripManager::RecalcLayout(void)
{
	
	for(size_t i = 0;i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)m_SelectedList[i];

		pRect->RecalcLayout();
	}
}

bool CUIShapeGripManager::_isResizablePoint( CUIGripCtrlPoint* pPoint)
{
	I3ASSERT( pPoint != NULL);

	CUIGripCtrl* pOwner = pPoint->getOwner();
	if( i3::same_of<CUIGripCtrlRect*>(pOwner)) //->IsExactTypeOf( CUIGripCtrlRect::static_meta()))
	{
		CUIGripCtrlRect * pRect = (CUIGripCtrlRect*)pOwner;
		CUIGripObjHandler * pHandler = pRect->getHandler();
		if( i3::same_of<CUIGripObjHandler_UIControl*>(pHandler)) //->IsExactTypeOf( CUIGripObjHandler_UIControl::static_meta()))
		{
			i3UIControl* pControl = (i3UIControl*)pHandler->getObject();
			if( pControl != NULL) 
			{
				if( i3::same_of<i3UIListView_Header*>(pControl)) //->IsExactTypeOf( i3UIListView_Header::static_meta()))	// Header
				{
					i3UIListView_Header* pHeader = (i3UIListView_Header*)pControl;
					if( pHeader->getBoundBox() != NULL)
					{
						if(pPoint->getStyle() != UIGRIP_CORNER_BOTTOM) 
							return false;	
					}
				}
				else if( i3::same_of<i3UIListView_Item*>(pControl)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))	// BindedItem
				{
					i3UIListView_Item* pItem = (i3UIListView_Item*)pControl;
					if( pItem->getBoundBox() != NULL)
					{
						i3UIListView_Box* pBox = pItem->getBoundBox();
						i3::vector<i3UIListView_Item*>& List = pBox->getItemList();
					
						if( List[0] != pItem)
							return false;
						
						if(pPoint->getStyle() != UIGRIP_CORNER_BOTTOM) 
							return false;	
					}
				}
			}
		}
	}

	return true;
}

bool CUIShapeGripManager::_isMovableRect( CUIGripCtrlRect* pRect)
{
	I3ASSERT( pRect != NULL);

	CUIGripObjHandler * pHandler = pRect->getHandler();
	if( i3::same_of<CUIGripObjHandler_UIControl*>(pHandler)) //->IsExactTypeOf( CUIGripObjHandler_UIControl::static_meta()))
	{
		i3UIControl* pControl = (i3UIControl*)pHandler->getObject();
		if( pControl != NULL)
		{
			if( i3::same_of<i3UIListView_Item*>(pControl)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))
			{
				i3UIListView_Item* pItem = (i3UIListView_Item*)pControl;
				if( pItem->getBoundBox() != NULL)
					return false;		// binded item can't be moved
			}
			else if( i3::same_of<i3UIListView_Header*>(pControl)) //->IsExactTypeOf( i3UIListView_Header::static_meta()))
			{
				i3UIListView_Header* pHeader = (i3UIListView_Header*)pControl;
				if( pHeader->getBoundBox() != NULL)
					return false;		// binded header can't be moved
			}
		}
	}

	return true;	// otherwise, always return true
}

POINT CUIShapeGripManager::GetSnappedPoint( POINT pt)
{
	INT32 diff;
	bool bNoOpX = true, bNoOpY = true;

	for(size_t i = 0; i < m_SnapPoints.size(); i++)
	{
		SNAP_INFO * pInfo = m_SnapPoints[i];

		// X
		diff = abs( pt.x - pInfo->m_Pos.x);
		if( diff < pInfo->m_SnapSize)
		{
			pt.x = pInfo->m_Pos.x;
			bNoOpX = false;
		}

		diff = abs( pt.y - pInfo->m_Pos.y);
		if( diff < pInfo->m_SnapSize)
		{
			pt.y = pInfo->m_Pos.y;
			bNoOpY = false;
		}
	}

	//I3TRACE("Before: pt.x: %d, pt.y: %d\n", pt.x, pt.y);

	if( bNoOpX)
		pt.x = ((pt.x / m_SnapSize) * m_SnapSize);

	if( bNoOpY)
		pt.y = ((pt.y / m_SnapSize) * m_SnapSize);

	//I3TRACE("After: pt.x: %d, pt.y: %d\n", pt.x, pt.y);

	return pt;
}

void CUIShapeGripManager::AddSnapPoint( POINT pt, INT32 sz)
{
	/*
	SNAP_INFO * pInfo;

	pInfo = new SNAP_INFO;

	pInfo->m_Pos = pt;
	pInfo->m_SnapSize = sz;

	I3TRACE( "ADD SNAP : (%d, %d) - %d\n", pt.x, pt.y, sz);

	m_SnapPoints.push_back( pInfo);
	*/
}

void CUIShapeGripManager::RemoveAllSnapPoint(void)
{
	for(size_t i = 0; i < m_SnapPoints.size(); i++)
	{
		SNAP_INFO * pInfo = m_SnapPoints[i];

		delete pInfo;
	}

	m_SnapPoints.clear();
}

void CUIShapeGripManager::SetSnapSize( INT32 size)
{
	m_SnapSize = size;
}

//점 a,b,c,d를 꼭지점으로 하는 직사각형이 있을 때
//서로 대각선의 위치에 있는 두 점(a,c 또는 b,d)을 알면
//임의의 한 점 p가 직사각형의 내부에 위치하는 지 알 수 있다. 
bool _isPInsideRectP1P2( CPoint p1, CPoint p2, CPoint p)
{
	INT32 l = min( p1.x, p2.x);
	INT32 r = max( p1.x, p2.x);
	INT32 t = min( p1.y, p2.y);
	INT32 b = max( p1.y, p2.y);

	if( l <= p.x && p.x <= r)
	{
		if( t <= p.y && p.y <= b)
		{
			return true;
		}
	}

	return false;
}


//가로:세로 비율을 유지한 상태에서의 스케일
void CUIShapeGripManager::_FixedRatioTransform( CUIGripCtrlPoint* pPoint, CPoint pt)
{
	//m_ptAnchor는 상대좌표, pt는 절대좌표
	//pt를 상대좌표로 만들어준다.
	CUIGripCtrl* pOwner = pPoint->getOwner();
	CUIGripObjHandler* pHandler = pOwner->getHandler();

	if( i3::same_of<CUIGripObjHandler_UIControl*>(pHandler)) //->IsExactTypeOf( CUIGripObjHandler_UIControl::static_meta()))
	{
		INT32 diffX = pOwner->getAccPosX() - pOwner->getPosX();
		INT32 diffY = pOwner->getAccPosY() - pOwner->getPosY();
		pt.x -= diffX;
		pt.y -= diffY;
	}

	bool bInside = false;

	REAL32 wOverh = (REAL32)m_szAnchor.x / (REAL32)m_szAnchor.y;	// w/h 

	UINT32 style = pPoint->getStyle();
	
	CPoint theOtherPoint;
	
	if( style == UIGRIP_CORNER_LEFT || style == UIGRIP_CORNER_RIGHT)
	{
		INT32 calcHeight = (INT32)(abs( pt.x - m_ptAnchor.x) / wOverh);
		
		theOtherPoint.x = pt.x;
		theOtherPoint.y = (pt.y > m_ptAnchor.y) ? m_ptAnchor.y + calcHeight : m_ptAnchor.y - calcHeight;
	}
	else if( style == UIGRIP_CORNER_TOP || style == UIGRIP_CORNER_BOTTOM)
	{
		INT32 calcWidth = (INT32)(abs( pt.y - m_ptAnchor.y) * wOverh);

		theOtherPoint.y = pt.y;
		theOtherPoint.x = (pt.x > m_ptAnchor.x) ? m_ptAnchor.x + calcWidth : m_ptAnchor.x - calcWidth;
	}
	else
	{
		//Width Base
		{
			INT32 calcHeight = (INT32)(abs( pt.x - m_ptAnchor.x) / wOverh);

			theOtherPoint.x = pt.x;
			theOtherPoint.y = (pt.y > m_ptAnchor.y) ? m_ptAnchor.y + calcHeight : m_ptAnchor.y - calcHeight;

			if( _isPInsideRectP1P2( m_ptAnchor, theOtherPoint, pt))
				bInside = true;
		}

		//Height Base
		if( !bInside)
		{
			INT32 calcWidth = (INT32)(abs( pt.y - m_ptAnchor.y) * wOverh);
			
			theOtherPoint.y = pt.y;
			theOtherPoint.x = (pt.x > m_ptAnchor.x) ? m_ptAnchor.x + calcWidth : m_ptAnchor.x - calcWidth;

			if( _isPInsideRectP1P2( m_ptAnchor, theOtherPoint, pt))
				bInside = true;
		}

		I3ASSERT( bInside);
	}

	//m_nAnchorStyle에 따라 tempLeft, tempRight, tempTop, tempBottom을 l,r,t,b에 배치한다. 
	INT32 tempLeft = min( m_ptAnchor.x, theOtherPoint.x);
	INT32 tempRight = max( m_ptAnchor.x, theOtherPoint.x);
	INT32 tempTop = min( m_ptAnchor.y, theOtherPoint.y);
	INT32 tempBottom = max( m_ptAnchor.y, theOtherPoint.y);

	INT32 l = 0, t = 0, r = 0, b = 0;

	switch( m_nAnchorStyle)
	{
	case UIGRIP_CORNER_LEFT | UIGRIP_CORNER_TOP:
		{
			l = m_ptAnchor.x;
			t = m_ptAnchor.y;
			r = (l == tempLeft) ? tempRight : tempLeft;
			b = (t == tempTop) ? tempBottom : tempTop;
		}
		break;
	case UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_TOP:
		{
			r = m_ptAnchor.x;
			t = m_ptAnchor.y;
			l = (r == tempLeft) ? tempRight : tempLeft;
			b = (t == tempTop) ? tempBottom : tempTop;
		}
		break;
	case UIGRIP_CORNER_LEFT | UIGRIP_CORNER_BOTTOM:
		{
			l = m_ptAnchor.x;
			b = m_ptAnchor.y;
			r = (l == tempLeft) ? tempRight : tempLeft;
			t = (b == tempTop) ? tempBottom : tempTop;;
		}
		break;
	case UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_BOTTOM:
		{
			r = m_ptAnchor.x;
			b = m_ptAnchor.y;
			l = (r == tempLeft) ? tempRight : tempLeft;
			t = (b == tempTop) ? tempBottom : tempTop;;
		}
		break;
	default:
		I3ASSERT( 0);
		break;
	}
	
	CUIGripCtrl* pCtrl = pPoint->getOwner();
	pCtrl->setPosX( l);
	pCtrl->setPosY( t);
	pCtrl->setWidth( r - l);
	pCtrl->setHeight( b - t);

}

void CUIShapeGripManager::_BeginTransform( void)
{
	m_bTransforming = true;

	//List of controls 
	if( m_SelectedList.size() == 1)
	{
		CUIGripCtrlRect* pItem = (CUIGripCtrlRect*)m_SelectedList[0];
		CUIGripObjHandler* pHandler = pItem->getHandler();

		if( i3::same_of<CUIGripObjHandler_UIControl*>(pHandler)) //->IsExactTypeOf( CUIGripObjHandler_UIControl::static_meta()))
		{
			i3UIControl* pControl = (i3UIControl*)pHandler->getObject();
			I3ASSERT( pControl != NULL);
			
			//UIUndo::ADD_Transform( pControl);
		}
	}
	else if( m_SelectedList.size() > 1)
	{
		i3::vector<i3UIControl*> list;

		for( size_t i=0; i< m_SelectedList.size(); i++)
		{
			CUIGripCtrlRect* pItem = (CUIGripCtrlRect*)m_SelectedList[i];
			CUIGripObjHandler* pHandler = pItem->getHandler();

			if( i3::same_of<CUIGripObjHandler_UIControl*>(pHandler)) //->IsExactTypeOf( CUIGripObjHandler_UIControl::static_meta()))
			{
				i3UIControl* pControl = (i3UIControl*)pHandler->getObject();
				I3ASSERT( pControl != NULL);

				list.push_back( pControl);
			}
		}
					
		//UIUndo::ADD_Transform( list);
	}
}

void CUIShapeGripManager::_EndTransform( void)
{
	m_bTransforming = false;
}

void CUIShapeGripManager::StartBanding( const CPoint & pt)
{
	m_bBanding = true;

	m_BandRect.SetRect( pt.x, pt.y, pt.x, pt.y);
}

void CUIShapeGripManager::Banding( const CPoint & pt)
{
	m_BandRect.right = pt.x;
	m_BandRect.bottom = pt.y;
}

void CUIShapeGripManager::EndBanding(void)
{
	m_bBanding = false;
}

void CUIShapeGripManager::AllPenColorNormal()
{
	for (size_t i = 0; i < m_SelectedList.size(); ++i)
	{
		CUIGripCtrlRect* pCtrlRect = (CUIGripCtrlRect*)m_SelectedList[i];
		pCtrlRect->setPenColor(COLORSET_GRIP_NORMAL_BORDER);
		pCtrlRect->setBrushColor(COLORSET_GRIP_NORMAL_BACK);
	}

	m_pCurFocusShape = NULL;

}

void CUIShapeGripManager::SetFocusdItem(void* pElement)
{
	CUIGripCtrlRect* pCtrlRect = FindElement(pElement);
	AllPenColorNormal();
	if (pCtrlRect)
	{
		pCtrlRect->setPenColor(COLORSET_GRIP_MULTIPLE_BORDER);
		pCtrlRect->setBrushColor(COLORSET_GRIP_MULTIPLE_BACK);
	}

}