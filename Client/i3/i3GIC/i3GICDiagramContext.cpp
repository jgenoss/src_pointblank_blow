#include "stdafx.h"
#include "i3GICDiagramContext.h"
#include "i3GICDiagram.h"
#include "i3GICDiagramView.h"
#include "i3GICLinkPoint.h"
#include "i3GICShapeLine.h"
#include "resource.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICDiagramContext);

i3GICDiagramContext::i3GICDiagramContext(void)
{
	m_hPenCP = ::CreatePen( PS_SOLID, 1, RGB( 250, 50, 50));
}

i3GICDiagramContext::~i3GICDiagramContext(void)
{
	I3_SAFE_RELEASE( m_pDiagram);
	I3_SAFE_RELEASE( m_pLinkLine);

	if( m_hPenCP != nullptr)
		::DeleteObject( m_hPenCP);
}

void i3GICDiagramContext::Create( i3GICDiagramView * pView, RECT * pRect)
{
	i3GICDiagram * pDiagram = i3GICDiagram::new_object_ref();

	setDiagram( pDiagram);

	m_pView = pView;

	if( pRect != nullptr)
	{
		m_UpdateRect.left = (REAL32) pRect->left;
		m_UpdateRect.top = (REAL32) pRect->top;
		m_UpdateRect.right = (REAL32) pRect->right;
		m_UpdateRect.bottom = (REAL32) pRect->bottom;
	}

	m_pLinkLine = i3GICShapeLine::new_object();
	m_pLinkLine->OnBindDiagram( true);

	INT32 i;
	for( i = 0; i < m_pLinkLine->getChildCount(); i++)
	{
		i3GICShape * pChild = (i3GICShape*) m_pLinkLine->getChild( i);
		pChild->addState( I3GIC_STATE_DISABLED);
	}

	m_pLinkLine->setLineStyle( GIC_LINE_STYLE_DASH);
	m_pLinkLine->addState( I3GIC_STATE_DISABLED);
}

void i3GICDiagramContext::DrawShapes( RECT * pRect)
{
	INT32 i;
	i3GICShape * pShape;

	if( pRect != nullptr)
		setUpdateRect( pRect);

	if( m_pDiagram != nullptr)
	{
		for( i = 0; i < m_pDiagram->getShapeCount(); i++)
		{
			pShape = m_pDiagram->getShape( i);

			if( pShape->IsEnableDraw( this) )
				pShape->OnDraw( this);
		}
	}

	if( m_pLinkLine !=nullptr)
	{
		if( m_pLinkLine->IsEnableDraw( this) )
		{
			m_pLinkLine->OnDraw( this);
		}
	}
}


void i3GICDiagramContext::ObserveShapeState( void)
{
	INT32 i;
	i3GICShape * pShape;

	bool bRedraw = false;

	if( m_pDiagram != nullptr)
	{
		for( i = 0; i < m_pDiagram->getShapeCount(); i++)
		{
			pShape = m_pDiagram->getShape( i);

			if( pShape->isState( I3GIC_STATE_REDRAW))
			{
				bRedraw = true;
				pShape->removeState( I3GIC_STATE_REDRAW);
			}
		}
	}

	if( bRedraw)
	{
		i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
		//m_pView->RedrawWindow();
	}
}

void i3GICDiagramContext::_LBtnDown_ModeNone( UINT32 nFlags, POINT * pPoint)
{
	// 우선 클릭 지점 밑에 Shape가 있는지 확인한다.
	bool bHit = false;
	
	// Hit Test
	// Linker는 제외
	addFlags( I3GIC_CTX_FLAG_NOLINKER);
	bHit = FindShapeByPos( (REAL32)pPoint->x, (REAL32) pPoint->y);
	removeFlags( I3GIC_CTX_FLAG_NOLINKER);

	if( bHit == false)
	{	// 공백을 클릭했다...
		// Drag box를 그리기 시작한다.
		m_pView->_DrawSelBox( &m_DragOldRect);

		UnselectShape();

		m_MouseMode = GIC_MOUSEMODE_MULTISEL;
	}
	else if( getCurrentHitResult()->m_pHitShape != nullptr)
	{
		// 어떤 Shape 위에 클릭했다.

		// click point에 대한 Semantic을 구한다.
		if( ( getCurrentHitResult()->m_pHitShape->isStyle( I3GIC_STYLE_NORESCALE) == false) &&
			((getCurrentHitResult()->m_Semantic & I3GIC_CP_RESIZE_ALL) ||
			 (getCurrentHitResult()->m_Semantic & I3GIC_CP_MOVE_LINEALL) ||
			 (getCurrentHitResult()->m_Semantic & I3GIC_CP_VALUE_ALL) ||
			 (getCurrentHitResult()->m_Semantic & I3GIC_CP_MOVE_POINTALL)) )
		{
			// Control Point Drag mode
			m_MouseMode = GIC_MOUSEMODE_MOVE_CP;
		}
		else if( getCurrentHitResult()->m_Semantic & I3GIC_CP_MOVE_ALL)
		{
			// Move mode
			m_MouseMode = GIC_MOUSEMODE_MOVE_OBJ;
		}

		//  선택된 Shape를 선택한다.
		if( getCurrentHitResult()->m_pHitShape->isSelected() == false)
		{
			if( !(nFlags & MK_SHIFT))
			{
				UnselectShape();
			}

			PreSelectShape( getCurrentHitResult()->m_pHitShape);

			SelectShape( getCurrentHitResult()->m_pHitShape);
		}
	}
}

void i3GICDiagramContext::_EnableLinkLine( i3GICShape * pShape)
{
	VEC2D vPos;

	I3ASSERT( pShape != nullptr);

	pShape->GetTransformedPos( &vPos);

	i3Vector::AddX( &vPos, pShape->getWidth() * 0.5f);
	i3Vector::AddY( &vPos, pShape->getHeight() * 0.5f);

	m_pLinkLine->setStartPoint( &vPos);
	m_pLinkLine->setEndPoint( &vPos);

	m_pLinkLine->removeState( I3GIC_STATE_DISABLED);
}

void i3GICDiagramContext::ProcessLButtonDown( UINT32 nFlags, CPoint point)
{
	if( m_pDiagram == nullptr)
		return;

	m_DragStartPt = point;

	//I3TRACE( "StartPoint : %d, %d\n", m_DragStartPt.x, m_DragStartPt.y);

	POINT lp = point;

	m_pView->_LP( &lp);

	setCurrentHitNum( GIC_HIT_RESULT_MOUSEDOWN);
	getCurrentHitResult()->reset();

	if( getMode() == GIC_MODE_DESIGN)
	{
		switch( m_MouseMode)
		{
		case GIC_MOUSEMODE_NONE :
			{
				m_DragOldRect.left = m_DragOldRect.right = point.x;
				m_DragOldRect.top = m_DragOldRect.bottom = point.y;

				_LBtnDown_ModeNone( nFlags, &lp);

				m_pDiagram->BeginDrag();
			}
			break;

		case GIC_MOUSEMODE_CREATE :
			if( m_pMetaToCreate != nullptr)
			{
				i3GICShape * pNewShape = (i3GICShape *)	m_pMetaToCreate->create_instance();	//CREATEINSTANCE( m_pMetaToCreate);

				pNewShape->setPosX( (REAL32) (lp.x));
				pNewShape->setPosY( (REAL32) (lp.y));

				pNewShape->setDepth( (REAL32)m_pDiagram->getShapeCount() );

				// Link Point를 생성한다.
				pNewShape->OnCreate();

				m_pDiagram->AddShape( nullptr, pNewShape);

				// external virtual function 외부에서 정의된 Shape의 경우 이 함수에서 처리한다.
				m_pView->OnCreateShape( pNewShape);

				UnselectShape();
				SelectShape( pNewShape);

				m_pDiagram->BeginDrag();

				m_MouseMode = GIC_MOUSEMODE_MOVE_CP;
				getCurrentHitResult()->m_Semantic = I3GIC_CP_RESIZE_RIGHT | I3GIC_CP_RESIZE_DOWN;
			}
			break;

		case GIC_MOUSEMODE_MULTISELECTED :
			m_pDiagram->BeginDrag();

			m_MouseMode = GIC_MOUSEMODE_MOVE_MULTIOBJ;
			break;
		}
	}
	else if( getMode() == GIC_MODE_DESIGNTREENODE)
	{	// Tree Node를 편집하는 모드
		switch( m_MouseMode)
		{
		case GIC_MOUSEMODE_NONE :
			{
				// 우선 클릭 지점 밑에 Shape가 있는지 확인한다.
				m_DragOldRect.left = m_DragOldRect.right = point.x;
				m_DragOldRect.top = m_DragOldRect.bottom = point.y;

				bool bHit = false;
				
				// Linker는 제외
				addFlags( I3GIC_CTX_FLAG_NOLINKER);
				bHit = FindShapeByPos( (REAL32)lp.x, (REAL32) lp.y);
				removeFlags( I3GIC_CTX_FLAG_NOLINKER);

				if( bHit &&
					(getCurrentHitResult()->m_pHitShape != nullptr) )
				{
					if( !(nFlags & MK_SHIFT))
					{
						UnselectShape();
					}

					SelectShape( getCurrentHitResult()->m_pHitShape);

					m_MouseMode = GIC_MOUSEMODE_PICKEDSHAPE;

					// MouseMode가 다르므로 따로 호출한다.
					_EnableLinkLine( getCurrentHitResult()->m_pHitShape);
				}

				m_pDiagram->BeginDrag();
			}
			break;
		}
	}
}

void i3GICDiagramContext::ProcessLButtonUp( UINT nFlags, CPoint point)
{
	POINT lp;
	i3::vector<i3GICShape*> list;
	i3::pack::RECT rect;

	if( m_pDiagram == nullptr)
		return;

	lp = point;

	m_pView->_LP( &lp);

	setCurrentHitNum( GIC_HIT_RESULT_MOUSEUP);
	getCurrentHitResult()->reset();

	m_pLinkLine->addState( I3GIC_STATE_DISABLED);

	m_pDiagram->EndDrag();

	if( getMode() == GIC_MODE_DESIGN)
	{
		switch( m_MouseMode)
		{
		case GIC_MOUSEMODE_MULTISEL :
			{
				m_pView->_DrawSelBox( &m_DragOldRect);

				if( !(nFlags & MK_SHIFT))
					m_pDiagram->UnselectAllShapes();

				m_pView->_LP( &m_DragOldRect);

				// Linker는 제외
				addFlags( I3GIC_CTX_FLAG_NOLINKER);
				FindShapeByRect( &m_DragOldRect, list);
				removeFlags( I3GIC_CTX_FLAG_NOLINKER);

				if( list.size() > 0)
				{
					m_pDiagram->SelectShape( list);
			
					// Viewer에 이벤트를 넘긴다.
					for(size_t i = 0; i < list.size(); i++)
					{
						m_pView->OnSelectShape( list[ i], m_MouseMode);
					}
					m_MouseMode = GIC_MOUSEMODE_MULTISELECTED;
				}
				else
				{
					UnselectShape();
					m_MouseMode = GIC_MOUSEMODE_NONE;
				}
			}
			break;

		case GIC_MOUSEMODE_LINKCREATE :
			{
				m_DragOldRect.left = m_DragOldRect.right = point.x;
				m_DragOldRect.top = m_DragOldRect.bottom = point.y;

				bool bHit = false;
				
				{
					rect.left = (REAL32) lp.x - 5.f;
					rect.right = (REAL32) lp.x + 5.f;
					rect.top = (REAL32) lp.y + 5.f;
					rect.bottom = (REAL32) lp.y - 5.f;

					// Linker는 제외
					addFlags( I3GIC_CTX_FLAG_NOLINKER);
					FindShapeByRect( &rect, list);
					removeFlags( I3GIC_CTX_FLAG_NOLINKER);

					// Input 이나 Output Shape을 찾는다.
					
					for(size_t i = 0; i < list.size(); i++)
					{
						i3GICShape * pDestShape = list[ i];

						if( m_pView->OnMouseUpForLink( pDestShape))
						{	
							getCurrentHitResult()->m_pHitShape = pDestShape;
							bHit = true;
							break;
						}
					}
				}

				if( bHit)
				{
					// View에 Link Line Create를 알려준다.
					i3GICShape * pLinkLine = m_pView->OnCreateLinkShape( getCurrentHitResult()->m_pHitShape);
					if( pLinkLine != nullptr)
					{
						getDiagram()->AddShape( nullptr, pLinkLine);
						I3_SAFE_RELEASE( pLinkLine);
					}
				}

				m_MouseMode = GIC_MOUSEMODE_NONE;
			}
			break;

		case GIC_MOUSEMODE_LINKMOVE :
			{
				m_DragOldRect.left = m_DragOldRect.right = point.x;
				m_DragOldRect.top = m_DragOldRect.bottom = point.y;

				bool bHit = false;
				
				{
					rect.left = (REAL32) lp.x - 5.f;
					rect.right = (REAL32) lp.x + 5.f;
					rect.top = (REAL32) lp.y + 5.f;
					rect.bottom = (REAL32) lp.y - 5.f;

					// Linker는 제외
					addFlags( I3GIC_CTX_FLAG_NOLINKER);
					FindShapeByRect( &rect, list);
					removeFlags( I3GIC_CTX_FLAG_NOLINKER);

					// Input 이나 Output Shape을 찾는다.
					
					for( size_t i = 0; i < list.size(); i++)
					{
						i3GICShape * pDestShape = list[ i];

						if( m_pView->OnMouseUpForLink( pDestShape))
						{	
							getCurrentHitResult()->m_pHitShape = pDestShape;
							bHit = true;
							break;
						}
					}
				}

				m_pView->OnMoveLinkShape( getCurrentHitResult()->m_pHitShape);
				m_MouseMode = GIC_MOUSEMODE_NONE;
			}
			break;

		case GIC_MOUSEMODE_MOVE_MULTIOBJ :
			UnselectShape();
			m_MouseMode = GIC_MOUSEMODE_NONE;
			break;

		default :
			m_MouseMode = GIC_MOUSEMODE_NONE;
			break;
		}
	}
	else if( getMode() == GIC_MODE_DESIGNTREENODE)
	{
		switch( m_MouseMode)
		{
		case GIC_MOUSEMODE_PICKEDSHAPE :
			{
				i3GICShape * pChildShape = getHitResult( GIC_HIT_RESULT_MOUSEDOWN)->m_pHitShape;
				if( pChildShape != nullptr)
				{
					if( FindShapeByPos( (REAL32) lp.x, (REAL32) lp.y))
					{
						i3GICShape * pParentShape = getCurrentHitResult()->m_pHitShape;

						if( (pParentShape != nullptr) && 
							(pParentShape != pChildShape) &&
							(pChildShape->getParent() != pParentShape) &&
							(pParentShape->getParent() != pChildShape) )
						{
							// Tree Node 구조를 바꾼다.
							// Shape List에서 삭제
							I3_SAFE_ADDREF( pChildShape);

							getDiagram()->RemoveShape( pChildShape);
							getDiagram()->AddShape( pParentShape, pChildShape);

							I3_SAFE_RELEASE( pChildShape);
						}
					}
					else
					{
						I3_SAFE_ADDREF( pChildShape);
						
						// 최상위 Root로 옮긴다.
						i3GICShape * pRoot = pChildShape;

						while( pRoot)
						{
							if( pRoot->getParent() != nullptr)
								pRoot = (i3GICShape*) pRoot->getParent();
							else
								break;
						}

						if( pRoot != nullptr)
						{
							getDiagram()->RemoveShape( pChildShape);
							getDiagram()->AddShape( nullptr, pChildShape);
							// 수동으로 ShapeRoot에 붙인다.
							pRoot->AddChild( pChildShape);
							I3_SAFE_RELEASE( pChildShape);
						}
					}
				}
				m_MouseMode = GIC_MOUSEMODE_NONE;
			}
			break;
		}
	}
	
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

GIC_MOUSE_CURSOR i3GICDiagramContext::ProcessMouseMove( UINT nFlags, CPoint point)
{
	POINT pt;

	GIC_MOUSE_CURSOR newCursor = GIC_CURSOR_NA;

	if( m_pDiagram == nullptr)
		return newCursor;

	setCurrentHitNum( GIC_HIT_RESULT_MOUSEMOVE);
	getCurrentHitResult()->reset();

	pt.x = point.x - m_DragStartPt.x;
	pt.y = point.y - m_DragStartPt.y;

	m_pView->_LP_SIZE( &pt);

	if( getMode() == GIC_MODE_DESIGN)
	{
		switch( m_MouseMode)
		{
		case GIC_MOUSEMODE_NONE :
			{
				POINT lp = point;
				m_pView->_LP( &lp);

				// Linker는 제외
				addFlags( I3GIC_CTX_FLAG_NOLINKER);
				bool bRv = FindShapeByPos( (REAL32) lp.x, (REAL32) lp.y);
				removeFlags( I3GIC_CTX_FLAG_NOLINKER);

				if( bRv && (getCurrentHitResult()->m_pHitShape != nullptr) )
				{
					newCursor = _GetCursorTypeByHit();

					// 외부 Tool에서 Overlap event에 대한 함수를 호출한다.
					// CharaTool에서는 Shape Name에 따라 Cursor를 바꾼다. 
					newCursor = m_pView->OnOverlapShape( getCurrentHitResult()->m_pHitShape, newCursor);
				}
				else
				{
					newCursor = GIC_CURSOR_SELECT;
				}
			}
			break;

		case GIC_MOUSEMODE_MULTISEL :
			m_pView->_DrawSelBox( &m_DragOldRect);

			m_DragOldRect.left = m_DragStartPt.x;
			m_DragOldRect.top = m_DragStartPt.y;
			m_DragOldRect.right = point.x;
			m_DragOldRect.bottom = point.y;

			m_pView->_DrawSelBox( &m_DragOldRect);
			break;

		case GIC_MOUSEMODE_CREATE :
			break;

		case GIC_MOUSEMODE_MOVE_MULTIOBJ :
		case GIC_MOUSEMODE_MOVE_OBJ :
			{
				setDelta( (REAL32) pt.x, (REAL32) pt.y);

				m_pDiagram->DragMove( this);

				//m_pView->RedrawWindow();
				i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
			}
			break;

		case GIC_MOUSEMODE_MOVE_CP :
			{
				setDelta( (REAL32) pt.x, (REAL32) pt.y);

				m_pDiagram->DragCP( this);

				//m_pView->RedrawWindow();
				i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
			}
			break;

		case GIC_MOUSEMODE_LINKCREATE :
			{
				// Link Line을 그린다.
				VEC2D vPos;

				POINT lp = point;

				m_pView->_LP( &lp);
				

				// Link point만을 찾는다.

				FindShapeByPos( (REAL32)lp.x, (REAL32)lp.y);

				// View에 Link용 MouseMove를 호출한다.
				if( m_pView->OnMouseMoveForLink( getCurrentHitResult()->m_pHitShape) )
				{
					// Attach 가능한 Shape이면 해당 Shape의 중앙에 위치토록 한다.
					getCurrentHitResult()->m_pHitShape->GetTransformedPos( &vPos);
					
					lp.x = (INT32) (getX( &vPos) + getCurrentHitResult()->m_pHitShape->getWidth() * 0.5f);
					lp.y = (INT32) (getY( &vPos) + getCurrentHitResult()->m_pHitShape->getHeight() * 0.5f);
				}

				i3Vector::Set( &vPos, (REAL32)lp.x, (REAL32)lp.y);
				m_pLinkLine->setEndPoint( &vPos);

				//m_pView->RedrawWindow();
				i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
			}
			break;

		case GIC_MOUSEMODE_LINKMOVE:
			{
				// Link point만을 찾는다.
				FindShapeByPos( (REAL32)pt.x, (REAL32)pt.y);

				// View에 Link용 MouseMove를 호출한다.
				if( m_pView->OnMouseMoveForLink( getCurrentHitResult()->m_pHitShape) )
				{
					// Attach 가능한 Shape이면 해당 Shape의 중앙에 위치토록 한다.
					VEC2D vPos;

					getCurrentHitResult()->m_pHitShape->GetTransformedPos( &vPos);
					
					pt.x = (INT32) (getX( &vPos) + getCurrentHitResult()->m_pHitShape->getWidth() * 0.5f) - m_DragStartPt.x;
					pt.y = (INT32) (getY( &vPos) + getCurrentHitResult()->m_pHitShape->getHeight() * 0.5f) - m_DragStartPt.y;
					
					m_pView->_LP_SIZE( &pt);
				}

				setDelta( (REAL32) pt.x, (REAL32) pt.y);

				m_pDiagram->DragCP( this);

				//m_pView->RedrawWindow();
				i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
			}
			break;

		case GIC_MOUSEMODE_CONTROL_VALUE :
			{
				// Control Value bar를 움직인다.
				setDelta( (REAL32) pt.x, (REAL32) pt.y);

				setCurrentHitNum( GIC_HIT_RESULT_MOUSEDOWN);

				m_pView->OnMouseMoveForSeletedShape( getCurrentHitResult()->m_pHitShape);

				m_pDiagram->DragCV( this);
			}
			break;
		}
	}
	else if( getMode() == GIC_MODE_DESIGNTREENODE)
	{
		switch( m_MouseMode)
		{
		case GIC_MOUSEMODE_PICKEDSHAPE :
			{
				// Link Line을 그린다.
				VEC2D vPos;

				RECT rt;

				rt.left = m_DragStartPt.x;
				rt.top = m_DragStartPt.y;
				rt.right = point.x;
				rt.bottom = point.y;

				m_pView->_LP_SIZE( &rt);

				i3Vector::Set( &vPos, (REAL32)rt.right, (REAL32)rt.bottom);

				if( FindShapeByPos( (REAL32)rt.right, (REAL32)rt.bottom) )
				{
					getCurrentHitResult()->m_pHitShape->GetTransformedPos( &vPos);
					i3Vector::AddX( &vPos, getCurrentHitResult()->m_pHitShape->getWidth() * 0.5f);
					i3Vector::AddY( &vPos, getCurrentHitResult()->m_pHitShape->getHeight() * 0.5f);
				}

				m_pLinkLine->setStartPoint( &vPos);

				i3Vector::Set( &vPos, (REAL32)rt.left, (REAL32)rt.top);
				m_pLinkLine->setEndPoint( &vPos);

				//m_pView->RedrawWindow();
				i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
			}
			break;
		}
	}

	return newCursor;
}

void i3GICDiagramContext::ProcessPopupMenu( UINT32 cmd)
{
	switch( cmd)
	{
		case ID_DIAGRAM_SELECTALL :		// Select all
			m_pDiagram->SelectAllShapes();
			break;

		case ID_DIAGRAM_UNSELECTALL :		// Unselect all
			m_pDiagram->UnselectAllShapes();
			break;

		case ID_DIAGRAM_COPY :		// Copy
			CopyToClipboard();
			break;

		case ID_DIAGRAM_PASTE :		// Paste
			PasteFromClipboard();
			break;

		case ID_DIAGRAM_DELETE :		// Delete
			DeleteShape();
			break;

		case ID_DEPTHPOSITION_TOBACK :		ShapeToBack();			break;
		case ID_DEPTHPOSITION_TOTHETAIL :	ShapeToLastBack();		break;
		case ID_DEPTHPOSITION_TOFRONT :		ShapeToFront();			break;
		case ID_DEPTHPOSITION_TOTHEFOREMOST : ShapeToFirstFront();	break;

		case ID_ALIGN_TOLEFT :				AlignToLeft();			break;
		case ID_ALIGN_TOCENTER :			AlignToCenter();		break;
		case ID_ALIGN_TORIGHT :				AlignToRight();			break;
		case ID_ALIGN_TOP :				AlignToTop();			break;
		case ID_ALIGN_MIDDLE :			AlignToMiddle();		break;
		case ID_ALIGN_BOTTOM :			AlignToBottom();		break;
	}
}

void i3GICDiagramContext::ProcessKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Movement Shape
	REAL32 moveX = 0.f, moveY = 0.f;
	
	if( nChar & VK_LEFT)		moveX = -1.f * nRepCnt;
	else if( nChar & VK_RIGHT)	moveX = 1.f * nRepCnt;

	if( nChar & VK_UP)			moveY = -1.f * nRepCnt;
	else if( nChar & VK_DOWN)	moveY = 1.f * nRepCnt;

	if( (moveX != 0.f) ||
		(moveY != 0.f) )
	{
		getCurrentHitResult()->m_Semantic = I3GIC_CP_MOVE_ALL;
		setDeltaX( moveX);
		setDeltaY( moveY);

		m_pDiagram->DragMove( this);
	}

	
}

GIC_MOUSE_CURSOR i3GICDiagramContext::_GetCursorTypeByHit( void)
{
	GIC_MOUSE_CURSOR newCursor = GIC_CURSOR_NA;

	I3GIC_CP_STYLE semantic = getCurrentHitResult()->m_Semantic;

	bool bResize = true;
	if( (getCurrentHitResult()->m_pHitShape != nullptr) &&
		getCurrentHitResult()->m_pHitShape->isStyle( I3GIC_STYLE_NORESCALE) )
		bResize = false;

	if( bResize)
	{
		if( semantic & I3GIC_CP_RESIZE_LEFT)
		{
			if( semantic & I3GIC_CP_RESIZE_UP)
				newCursor = GIC_CURSOR_RESIZE_NWSE;
			else if( semantic & I3GIC_CP_RESIZE_DOWN)
				newCursor = GIC_CURSOR_RESIZE_NESW;
			else
				newCursor = GIC_CURSOR_RESIZE_WE;
		}
		else if( semantic & I3GIC_CP_RESIZE_RIGHT)
		{
			if( semantic & I3GIC_CP_RESIZE_UP)
				newCursor = GIC_CURSOR_RESIZE_NESW;
			else if( semantic & I3GIC_CP_RESIZE_DOWN)
				newCursor = GIC_CURSOR_RESIZE_NWSE;
			else
				newCursor = GIC_CURSOR_RESIZE_WE;
		}
		else
		{
			if( semantic & (I3GIC_CP_RESIZE_UP | I3GIC_CP_RESIZE_DOWN))
				newCursor = GIC_CURSOR_RESIZE_NS;
		}

		if( (semantic & I3GIC_CP_VALUE_ALL) )
		{
			newCursor = GIC_CURSOR_RESIZE_WE;
		}
	}

	if( (semantic & I3GIC_CP_MOVE_ALL) ||
		(semantic & I3GIC_CP_MOVE_LINEALL) ||
		(semantic & I3GIC_CP_MOVE_POINTALL) )
	{
		newCursor = GIC_CURSOR_MOVE_ALL;
	}

	return newCursor;
}

GIC_MOUSE_CURSOR i3GICDiagramContext::SetMouseMode( GIC_MOUSEMODE mode)
{
	GIC_MOUSE_CURSOR newCursor = GIC_CURSOR_NA;
	m_MouseMode = mode;

	switch( mode)
	{
		case GIC_MOUSEMODE_CREATE :		newCursor = GIC_CURSOR_CREATE; break;
		case GIC_MOUSEMODE_MOVE_CP :	newCursor = _GetCursorTypeByHit();	break;
		case GIC_MOUSEMODE_MOVE_OBJ :	newCursor = GIC_CURSOR_MOVE_ALL;	break;
	}

	return newCursor;
}

void i3GICDiagramContext::SetModeToDevice(void)
{
	::GetWindowExtEx( m_pView->getDrawDC(), &m_SavedWndExt);
	::GetWindowOrgEx( m_pView->getDrawDC(), &m_SavedWndOrg);

	::GetViewportExtEx( m_pView->getDrawDC(), &m_SavedViewExt);
	::GetViewportOrgEx( m_pView->getDrawDC(), &m_SavedViewOrg);

	m_SavedMapMode = ::GetMapMode( m_pView->getDrawDC());

	::SetMapMode( m_pView->getDrawDC(), MM_TEXT);
}

void i3GICDiagramContext::SetModeToDraw(void)
{
	::SetMapMode( m_pView->getDrawDC(), m_SavedMapMode);
	::SetWindowExtEx( m_pView->getDrawDC(), m_SavedWndExt.cx, m_SavedWndExt.cy, nullptr);
	::SetWindowOrgEx( m_pView->getDrawDC(), m_SavedWndOrg.x, m_SavedWndOrg.y, nullptr);

	::SetViewportExtEx( m_pView->getDrawDC(), m_SavedViewExt.cx, m_SavedViewExt.cy, nullptr);
	::SetViewportOrgEx( m_pView->getDrawDC(), m_SavedViewOrg.x, m_SavedViewOrg.y, nullptr);

	::SetMapMode( m_pView->getDrawDC(), MM_ANISOTROPIC);
}

void i3GICDiagramContext::CopyToClipboard( void)
{
	UINT	idFormat;
	UINT32	Size;
	INT32 i;
	i3GICShape * pShape;
	HANDLE hMem;
	HENHMETAFILE hMeta;
	i3::vector<i3PersistantElement*>	selList;

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	m_pView->OnBeforeCopyToClipboard( selList);

	// 저장할 Memory의 크기를 구한다.
	{
		i3NullStream strm;
		i3ResourceFile file;
		
		// Null Stream을 이용해, 파일에 저장하는 흉내를 내며
		// 크기를 구한다.
		file.SetObjects( selList);
		file.Save( &strm);	// 파일명은

		Size = strm.GetSize();
	}

	I3ASSERT( Size > 0);

	///////////////////////////////////////////////////////////////
	// Metafile operation
	{
		HDC MetaDC, ScrDC;
		RECT Rect = {0,0,0,0};
		int iWidthMM, iHeightMM, iWidthPels, iHeightPels;

		ScrDC = m_pView->getDrawDC();

		// 객체들의 영역을 모두 포함하는 Rect 좌표를 구한다.
		for( i = 0; i < INT32(selList.size()); i++) 
		{
			pShape = (i3GICShape *) selList[i];

			if( i == 0)
			{
				Rect.left	= (INT32) pShape->getPosX();
				Rect.top	= (INT32) pShape->getPosY();
				Rect.right	= (INT32) (pShape->getPosX() + pShape->getWidth());
				Rect.bottom	= (INT32) (pShape->getPosY() + pShape->getHeight());
			}
			else
			{
				Rect.left	= MIN( Rect.left,	(INT32) pShape->getPosX());
				Rect.top	= MIN( Rect.top,	(INT32) pShape->getPosY());
				Rect.right	= MAX( Rect.right,	(INT32) (pShape->getPosX() + pShape->getWidth()));
				Rect.bottom	= MAX( Rect.bottom, (INT32) (pShape->getPosY() + pShape->getHeight()));
			}
		}

		iWidthMM = ::GetDeviceCaps( ScrDC, HORZSIZE); 
		iHeightMM = ::GetDeviceCaps( ScrDC, VERTSIZE); 
		iWidthPels = ::GetDeviceCaps( ScrDC, HORZRES); 
		iHeightPels = ::GetDeviceCaps( ScrDC, VERTRES); 

		Rect.left = (Rect.left * iWidthMM * 100) / iWidthPels; 
		Rect.top = (Rect.top * iHeightMM * 100) / iHeightPels; 
		Rect.right = (Rect.right * iWidthMM * 100) / iWidthPels; 
		Rect.bottom = (Rect.bottom * iHeightMM * 100) / iHeightPels; 

		MetaDC = ::CreateEnhMetaFile( ScrDC, nullptr, &Rect, _T("GIC_Integraed"));
		I3ASSERT( MetaDC != nullptr);

		//SetViewportOrgEx( MetaDC, Rect.left, Rect.top, nullptr);

		HDC hOldDC = m_pView->getDrawDC();

		m_pView->setDrawDC( MetaDC);
		addFlags( I3GIC_CTX_FLAG_NOCONTROL);

		for( i = 0; i < INT32(selList.size()); i++) 
		{
			pShape = (i3GICShape *) selList[i];

			pShape->OnDraw( this);
		}

		// 복귀
		m_pView->setDrawDC( hOldDC);
		removeFlags( I3GIC_CTX_FLAG_NOCONTROL);

		// MetaFile Handle을 구하기 위해 MetaFile DC를 닫는다.
		hMeta = ::CloseEnhMetaFile( MetaDC);
	}

	// 실제 메모리를 할당하고 저장한다.
	{
		i3MemStream strm;
		i3ResourceFile file;
		void * pBuf;

		hMem = ::GlobalAlloc( GMEM_DDESHARE, Size);
		I3ASSERT( hMem != INVALID_HANDLE_VALUE);

		pBuf = (void *) ::GlobalLock( hMem);

		strm.Open( (const char *) pBuf, STREAM_READ | STREAM_WRITE, Size);
		
		file.SetObjects( selList);
		file.Save( &strm);

		::GlobalUnlock( hMem);
	}

	m_pView->OnAfterCopyToClipboard( selList);

	if( ::OpenClipboard( nullptr) == FALSE)
	{
		::GlobalFree( hMem);
		return;
	}

	if( ::EmptyClipboard() == FALSE)
	{
		::GlobalFree( hMem);
		return;
	}

	idFormat = ::RegisterClipboardFormat( I3GIC_CLIPBOARDFORMAT);
	if( idFormat == 0)
	{
		::GlobalFree( hMem);
		return;
	}

	::SetClipboardData( idFormat, hMem);
	::SetClipboardData( CF_ENHMETAFILE, hMeta);

	::CloseClipboard();
}

void i3GICDiagramContext::PasteFromClipboard( void)
{
	i3ResourceFile file;
	i3::vector<i3PersistantElement*> ShapeList;
//	INT32 i;

	// Clipboard에서 데이터를 읽어 온다.
	{
		HANDLE hMem;
		UINT idFormat;
		UINT32 rc;

		if( ::OpenClipboard( nullptr) == FALSE)
		{
			return;
		}

		idFormat = ::RegisterClipboardFormat( I3GIC_CLIPBOARDFORMAT);
		if( idFormat == 0)
		{
			return;
		}

		hMem = ::GetClipboardData( idFormat);
		if( hMem == nullptr)
			return;

		{
			char * pBuf = (char *) ::GlobalLock( hMem);
			i3MemStream strm;

			strm.Open( (const char *)pBuf, STREAM_READ, 0x7FFFFFFF);

			rc = file.Load( &strm);
			if( rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  _T("Clipboard에서 데이터를 읽어오지 못했습니다."));
			}

			::GlobalUnlock( hMem);
		}

		::CloseClipboard();

		if( rc == STREAM_ERR)
			return;
	}

	// 읽어 들여온 파일을 정리한다.
	{
		i3PersistantElement * pObj;

		for(INT32 i = 0; i < file.GetObjectCount(); i++)
		{
			pObj = file.GetObject( i);

			if( i3::kind_of<i3GICShape*>(pObj))
			{
				ShapeList.push_back( pObj);
			}
		}
	}

	m_pView->OnBeforePasteFromClipboard( ShapeList);

	for(size_t i = 0; i < ShapeList.size(); i++)
	{
		m_pDiagram->AddShape( nullptr, (i3GICShape *) ShapeList[i]);
	}

	m_pView->OnAfterPasteFromClipboard( ShapeList);
}

bool i3GICDiagramContext::IsClipboardData(void)
{
	UINT idFormat;

	idFormat = ::RegisterClipboardFormat( I3GIC_CLIPBOARDFORMAT);

	return (bool)(::IsClipboardFormatAvailable( idFormat) != 0);
}

void i3GICDiagramContext::ShapeToBack( void)
{
	INT32 i, j;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	for( i = 0; i < INT32(selList.size()); i++)
	{
		pShape = (i3GICShape*) selList[ i];

		// 다음으로 가까운 뒤의 SHape을 찾는다.
		REAL32 rMax = -1000000.f;
		REAL32 rMax2 = rMax;

		for( j = 0; j < m_pDiagram->getShapeCount(); j++)
		{
			i3GICShape * pDestShape = m_pDiagram->getShape( j);

			if( (pShape->getDepth() > pDestShape->getDepth()) &&
				(rMax < pDestShape->getDepth()) )
			{	
				rMax = pDestShape->getDepth();
			}
		}

		// 다음 다음으로 가까운 Shape
		for( j = 0; j < m_pDiagram->getShapeCount(); j++)
		{
			i3GICShape * pDestShape = m_pDiagram->getShape( j);

			if( (rMax > pDestShape->getDepth()) &&
				(rMax2 < pDestShape->getDepth()) )
			{
				rMax2 = pDestShape->getDepth();
			}
		}

		if( rMax > -1000000.f)
		{
			if( rMax2 > -1000000.f)
				pShape->setDepth( (rMax + rMax2) * 0.5f);
			else
				pShape->setDepth( rMax - 1.f);
		}
		else
			return;
	}

	m_pDiagram->SortByDepth();
	//m_pView->RedrawWindow();

	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::ShapeToLastBack( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	REAL32 rMin = 0.f;

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	for( i = 0; i < m_pDiagram->getShapeCount(); i++)
	{
		pShape = m_pDiagram->getShape( i);

		if( pShape->getDepth() < rMin)
			rMin = pShape->getDepth();
	}

	
	for( i = 0; i < INT32(selList.size()); i++)
	{
		pShape = (i3GICShape*) selList[ i];
		pShape->setDepth( rMin - 1.f);
	}

	m_pDiagram->SortByDepth();
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::ShapeToFront( void)
{
	INT32 i, j;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;
	
	for( i = 0; i < INT32(selList.size()); i++)
	{
		pShape = (i3GICShape*) selList[ i];

		// 다음으로 가까운 앞의 SHape을 찾는다.
		REAL32 rMin = 1000000.f;
		REAL32 rMin2 = rMin;

		for( j = 0; j < m_pDiagram->getShapeCount(); j++)
		{
			i3GICShape * pDestShape = m_pDiagram->getShape( j);

			if( (pShape->getDepth() < pDestShape->getDepth()) &&
				(rMin > pDestShape->getDepth()) )
			{	
				rMin = pDestShape->getDepth();
			}
		}

		// 다음 다음으로 가까운 Shape
		for( j = 0; j < m_pDiagram->getShapeCount(); j++)
		{
			i3GICShape * pDestShape = m_pDiagram->getShape( j);

			if( (rMin < pDestShape->getDepth()) &&
				(rMin2 > pDestShape->getDepth()) )
			{
				rMin2 = pDestShape->getDepth();
			}
		}

		if( rMin < 1000000.f)
		{
			if( rMin2 < 1000000.f)
				pShape->setDepth( (rMin + rMin2) * 0.5f);
			else
				pShape->setDepth( rMin + 1.f);
		}
		else
			return;
	}

	m_pDiagram->SortByDepth();
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::ShapeToFirstFront( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	REAL32 rMax = 0.f;

	for( i = 0; i < m_pDiagram->getShapeCount(); i++)
	{
		pShape = m_pDiagram->getShape( i);
		if( pShape->getDepth() > rMax)
			rMax = pShape->getDepth();
	}
	
	for( i = 0; i < INT32(selList.size()); i++)
	{
		pShape = (i3GICShape*) selList[ i];
		pShape->setDepth( rMax + 1.f);
	}

	m_pDiagram->SortByDepth();
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::AlignToCenter( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	I3ASSERT( m_pDiagram != nullptr);

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	pShape = (i3GICShape*) selList[ 0];
	REAL32 rCenter = pShape->getPosX() + pShape->getWidth() * 0.5f;

	if( selList.size() > 1)
	{
		for( i = 1; i < INT32(selList.size()); i++)
		{
			pShape = (i3GICShape*) selList[ i];
			REAL32 rSrcCenter = pShape->getPosX() + pShape->getWidth() * 0.5f;

			pShape->setPosX( pShape->getPosX() + (rCenter - rSrcCenter));
		}
	}
	else
	{	
		//pShape->setPosX( pShape->getPosX() + (m_pView->getPageWidth() * 0.5f - rCenter) );
		pShape->setPosX( pShape->getPosX() + (m_pView->getViewWidth() * 0.5f - rCenter) );
	}
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::AlignToLeft( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	I3ASSERT( m_pDiagram != nullptr);

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	pShape = (i3GICShape*) selList[ 0];
	REAL32 rLeft = pShape->getPosX();

	if( selList.size() > 1)
	{
		for( i = 1; i < INT32(selList.size()); i++)
		{
			pShape = (i3GICShape*) selList[ i];

			pShape->setPosX( rLeft);
		}
	}
	else
	{	
		pShape->setPosX( 0.f );
	}
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::AlignToRight( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	I3ASSERT( m_pDiagram != nullptr);

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	pShape = (i3GICShape*) selList[ 0];
	REAL32 rRight = pShape->getPosX() + pShape->getWidth();

	if( selList.size() > 1)
	{
		for( i = 1; i < INT32(selList.size()) ; i++)
		{
			pShape = (i3GICShape*) selList[ i];

			pShape->setPosX( (rRight - pShape->getWidth()));
		}
	}
	else
	{	
		//pShape->setPosX( (m_pView->getPageWidth() - pShape->getWidth()) );
		pShape->setPosX( (m_pView->getViewWidth() - pShape->getWidth()) );
	}
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::AlignToTop( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	I3ASSERT( m_pDiagram != nullptr);

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	pShape = (i3GICShape*) selList[ 0];
	REAL32 rTop = pShape->getPosY();

	if( selList.size() > 1)
	{
		for( i = 1; i < INT32(selList.size()); i++)
		{
			pShape = (i3GICShape*) selList[ i];
			pShape->setPosY( rTop);
		}
	}
	else
	{	
		pShape->setPosY( 0.f );
	}
	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::AlignToMiddle( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	I3ASSERT( m_pDiagram != nullptr);

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	pShape = (i3GICShape*) selList[ 0];
	REAL32 rMiddle = pShape->getPosY() + pShape->getHeight() * 0.5f;

	if( selList.size() > 1)
	{
		for( i = 1; i < INT32(selList.size()); i++)
		{
			pShape = (i3GICShape*) selList[ i];
			REAL32 rSrcMiddle = pShape->getPosY() + pShape->getHeight() * 0.5f;

			pShape->setPosY( pShape->getPosY() + (rMiddle - rSrcMiddle));
		}
	}
	else
	{	
//		pShape->setPosY( pShape->getPosY() + (m_pView->getPageHeight() * 0.5f - rMiddle) );
		pShape->setPosY( pShape->getPosY() + (m_pView->getViewHeight() * 0.5f - rMiddle) );
	}

	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::AlignToBottom( void)
{
	INT32 i;
	i3::vector<i3PersistantElement*> selList;
	i3GICShape * pShape;

	I3ASSERT( m_pDiagram != nullptr);

	m_pDiagram->GetSelectedShapes( selList);
	if( selList.size() <= 0)
		return;

	pShape = (i3GICShape*) selList[ 0];
	REAL32 rBottom = pShape->getPosY() + pShape->getHeight();

	if( selList.size() > 1)
	{
		for( i = 1; i < INT32(selList.size()); i++)
		{
			pShape = (i3GICShape*) selList[ i];
			pShape->setPosY( rBottom - pShape->getHeight());
		}
	}
	else
	{	
		//pShape->setPosY( m_pView->getPageHeight() - pShape->getHeight() );
		pShape->setPosY( m_pView->getViewHeight() - pShape->getHeight() );
	}

	//m_pView->RedrawWindow();
	i3TDK::Update( m_pView->m_hWnd, I3_TDK_UPDATE_EDIT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagramContext::PreSelectShape( i3GICShape * pShape)
{
	// Viewer에 이벤트를 넘긴다.
	m_MouseMode = m_pView->OnPreSelectShape( pShape, m_MouseMode);
}

void i3GICDiagramContext::SelectShape( i3GICShape * pShape)
{
	I3ASSERT( m_pDiagram != nullptr);

	m_pDiagram->SelectShape( pShape);

	m_pView->OnSelectShape( pShape, m_MouseMode);

	// Link Line을 생성하는 경우
	if( m_MouseMode == GIC_MOUSEMODE_LINKCREATE)
	{
		_EnableLinkLine( getCurrentHitResult()->m_pHitShape);
	}
}

void i3GICDiagramContext::UnselectShape( void)
{	
	m_pView->OnUnselectShape();

	m_pDiagram->UnselectAllShapes();
}

void i3GICDiagramContext::DeleteShape( void)
{
	INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getDiagram()->getShapeCount(); i++)
	{
		pShape = getDiagram()->getShape( i);

		if( pShape->isSelected())
		{
			if( m_pView->OnDeleteShape( pShape) )
			{
				getDiagram()->RemoveShape( pShape);
				i--;
			}
		}
	}
}


bool i3GICDiagramContext::FindShapeByPos( REAL32 x, REAL32 y)
{
	INT32 i;
	i3GICShape * pShape;
	bool rv = false;

	if( m_pDiagram != nullptr)
	{
		resetOffstack( 0.f, 0.f);

		for( i = 0; i < m_pDiagram->getShapeCount(); i++)
		{
			pShape = m_pDiagram->getShape( i);

			if( pShape->IsEnableHitTest( this))
			{	
				if( pShape->OnHitTest( this, x, y))
					rv = true;
			}
		}
	}

	return rv;
}

void i3GICDiagramContext::FindShapeByRect( i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List)
{
	INT32 i;
	i3GICShape * pShape;

	if( m_pDiagram != nullptr)
	{
		i3Math::NormalizeRect( pRect);

		resetOffstack( 0.0f, 0.0f);

		for( i = 0; i < m_pDiagram->getShapeCount(); i++)
		{
			pShape = m_pDiagram->getShape( i);

			if( pShape->IsEnableHitTest( this))
				pShape->OnHitTest( this, pRect, List);
		}
	}
}


bool i3GICDiagramContext::IsGridSnapPos( INT32 x, INT32 y)
{
	if( m_bGridSnap)
	{
		INT32 X = x % m_iGridX;
		INT32 Y = y % m_iGridY;

		if( (X > m_iSnapEps) && (X < m_iGridX - m_iSnapEps) )
			return false;

		if( (Y > m_iSnapEps) && (Y < m_iGridY - m_iSnapEps) )
			return false;
	}
	else
		return false;

	return true;
}

bool i3GICDiagramContext::_GridSnap( REAL32 * pX, REAL32 * pY, INT32 iSnapX, INT32 iSnapY)
{
	INT32 iRePosX, iRePosY;
	bool bSnap = false;

	INT32 iPosX = (INT32) *pX;
	INT32 iPosY = (INT32) *pY;

	INT32 iReX = iPosX % iSnapX;
	INT32 iReY = iPosY % iSnapY;

	//	left top snapping
	if( iReX < m_iSnapEps )
	{
		/// Snap to Grid
		iRePosX = iPosX / iSnapX;

		*pX = (REAL32) (iRePosX * iSnapX);
		bSnap = true;
	}

	if( iSnapX-iReX < m_iSnapEps)
	{
		/// Snap to Grid
		iRePosX = iPosX / iSnapX;

		*pX = (REAL32) ((iRePosX + 1) * iSnapX);
		bSnap = true;
	}

	if( iReY < m_iSnapEps )
	{	
		// Snap to Grid
		iRePosY = iPosY / iSnapY;

		*pY = (REAL32) (iRePosY * iSnapY);
		bSnap = true;
	}

	if( iSnapY-iReY < m_iSnapEps)
	{
		// Snap to Grid
		iRePosY = iPosY / iSnapY;

		*pY = (REAL32) ((iRePosY + 1) * iSnapY);
		bSnap = true;
	}

	return bSnap;
}

bool i3GICDiagramContext::GridSnapPos( REAL32 * pX, REAL32 * pY)
{
	if( !isGridSnapEnable() || (getFlags() & I3GIC_CTX_FLAG_NOSNAP) )
		return false;

	return _GridSnap( pX, pY, m_iGridX, m_iGridY);
}


bool i3GICDiagramContext::LinkSnapPos( i3GICShape * pShape, REAL32 * pX, REAL32 * pY, i3GICLinkPoint * pPointer /*= nullptr*/)
{
	INT32 i;
	i3GICShape * pDest;
	i3GICLinkPoint * pPoint;
	i3::vector<i3TreeNode*> list;

	if( !isLinkSnapEnable() || (getFlags() & I3GIC_CTX_FLAG_NOSNAP))
		return false;

	for( i = 0; i < m_pDiagram->getShapeCount(); i++)
	{
		pDest = m_pDiagram->getShape( i);

		if( pDest == pShape)
			continue;
		
		list.clear();

		i3TreeNode::FindNodeByExactType( pDest, i3GICLinkPoint::static_meta(), list);

		for(size_t j = 0; j < list.size(); j++)
		{
			bool bChecker = true;

			pPoint = (i3GICLinkPoint*) list[ j];

			if( pPoint->getPosX() + pDest->getPosX() - m_iSnapEps > *pX )
				bChecker = false;
			else if( pPoint->getPosX() + pDest->getPosX() + m_iSnapEps < *pX )
				bChecker = false;

			if( pPoint->getPosY() + pDest->getPosY() - m_iSnapEps > *pY)
				bChecker = false;
			else if( pPoint->getPosY() + pDest->getPosY() + m_iSnapEps < *pY)
				bChecker = false;

			if( bChecker)
			{
				*pX = pPoint->getPosX() + pDest->getPosX();
				*pY = pPoint->getPosY() + pDest->getPosY();

				if( pPointer != nullptr)
				{	// Linker를 붙인다.
					pShape->OnLinkShape( pPointer, pPoint);
				}

				return true;
			}
			else
			{
				// Linker를 제거한다.
				if( pPointer != nullptr)
				{
					pShape->OnUnlinkShape( pPointer, pPoint);
				}
			}
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										GDI Function
void i3GICDiagramContext::toLP( POINT * pPt)
{
	::DPtoLP( m_pView->getDrawDC(), pPt, 1);
}

void i3GICDiagramContext::toLP( RECT * pRect)
{
	::DPtoLP( m_pView->getDrawDC(), (LPPOINT) pRect, 2);
}

void i3GICDiagramContext::toDP( POINT * pPt)
{
	::LPtoDP( m_pView->getDrawDC(), pPt, 1);
}

void i3GICDiagramContext::toDP( RECT * pRect)
{
	::LPtoDP( m_pView->getDrawDC(), (LPPOINT) pRect, 2);
}


HGDIOBJ	 i3GICDiagramContext::selectObject( HGDIOBJ hObj)
{
	return ::SelectObject( m_pView->getDrawDC(), hObj);
}

COLORREF i3GICDiagramContext::setTextColor( COLORREF ref)
{
	return ::SetTextColor( m_pView->getDrawDC(), ref); 
}

COLORREF i3GICDiagramContext::setBkColor( COLORREF ref)
{
	return ::SetBkColor( m_pView->getDrawDC(), ref);
}

INT32 i3GICDiagramContext::setBkMode( INT32 mode)
{
	return ::SetBkMode( m_pView->getDrawDC(), mode);
}

INT32 i3GICDiagramContext::drawText( const char * pszText, LPRECT pRect, UINT nFormat /*= DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE*/)
{
	RECT rt;
	VEC2D * pOff = m_OffStack.getTop();

	rt.left = (LONG) (pRect->left + pOff->x);
	rt.top	= (LONG) (pRect->top + pOff->y);
	rt.right = (LONG) (pRect->right + pOff->x);
	rt.bottom = (LONG) (pRect->bottom + pOff->y);

	return ::DrawText( m_pView->getDrawDC(), pszText, -1, &rt, nFormat);
}

void i3GICDiagramContext::fillRect( INT32 left, INT32 top, INT32 right, INT32 bottom, HBRUSH hBrush)
{
	RECT rt;
	VEC2D * pOff = m_OffStack.getTop();

	rt.left = (INT32)(left + pOff->x);
	rt.top = (INT32)(top + pOff->y);
	rt.right = (INT32)(right + pOff->x);
	rt.bottom = (INT32)(bottom + pOff->y);

	::FillRect( m_pView->getDrawDC(), &rt, hBrush);
}

void i3GICDiagramContext::fillRect( RECT * pRect, HBRUSH hBrush)
{
	RECT rt;
	VEC2D * pOff = m_OffStack.getTop();

	rt = * pRect;
	::OffsetRect( &rt, (INT32) pOff->x, (INT32) pOff->y);

	::FillRect( m_pView->getDrawDC(), &rt, hBrush);
}

void i3GICDiagramContext::moveTo( INT32 x, INT32 y)
{
	VEC2D * pOff = m_OffStack.getTop();
	::MoveToEx( m_pView->getDrawDC(), (INT32)(x + pOff->x), (INT32)(y + pOff->y), nullptr);
}

void i3GICDiagramContext::lineTo( INT32 x, INT32 y)
{
	VEC2D * pOff = m_OffStack.getTop();
	::LineTo( m_pView->getDrawDC(), (INT32)(x + pOff->x), (INT32)(y + pOff->y));
}

void i3GICDiagramContext::rectangle( INT32 left, INT32 top, INT32 right, INT32 bottom)
{
	VEC2D * pOff = m_OffStack.getTop();
	::Rectangle( m_pView->getDrawDC(), (INT32)(left + pOff->x), (INT32)(top + pOff->y), 
						(INT32)(right + pOff->x), (INT32)(bottom + pOff->y));
}

void i3GICDiagramContext::rectangle( RECT * pRect)
{
	VEC2D * pOff = m_OffStack.getTop();
	::Rectangle( m_pView->getDrawDC(), (INT32)(pRect->left + pOff->x), (INT32)(pRect->top + pOff->y), 
						(INT32)(pRect->right + pOff->x), (INT32)(pRect->bottom + pOff->y));
}

void i3GICDiagramContext::roundRect( INT32 l, INT32 t, INT32 r, INT32 b, INT32 rw, INT32 rh)
{
	VEC2D * pOff = m_OffStack.getTop();

	::RoundRect( m_pView->getDrawDC(), (INT32)(l + pOff->x), (INT32)(t + pOff->y),
						(INT32)(r + pOff->x), (INT32)(b + pOff->y),
						rw, rh);
}

void i3GICDiagramContext::DP_RoundRect( INT32 l, INT32 t, INT32 r, INT32 b, INT32 rw, INT32 rh)
{
	::RoundRect( m_pView->getDrawDC(), l, t, r, b, rw, rh);
}

void i3GICDiagramContext::ellipse( INT32 l, INT32 t, INT32 r, INT32 b)
{
	VEC2D * pOff = m_OffStack.getTop();
	::Ellipse( m_pView->getDrawDC(), (INT32)(l + pOff->x), (INT32)(t + pOff->y),
						(INT32)(r + pOff->x), (INT32)(b + pOff->y));
}

void i3GICDiagramContext::DP_Ellipse( INT32 l, INT32 t, INT32 r, INT32 b)
{
	::Ellipse( m_pView->getDrawDC(), l, t, r, b);
}

void i3GICDiagramContext::drawBitmap( HDC memDC, RECT * pDest, RECT * pSrc)
{
	::SetStretchBltMode( m_pView->getDrawDC(), HALFTONE);

	::StretchBlt( m_pView->getDrawDC(),
		pDest->left, pDest->top, pDest->right, pDest->bottom,
		memDC,
		pSrc->left, pSrc->top, pSrc->right, pSrc->bottom, SRCCOPY);
}

void i3GICDiagramContext::penDashStyle( Pen * pPen, GIC_LINE_STYLE dashStyle)
{
	I3ASSERT( pPen != nullptr);

	switch( dashStyle)
	{
	case GIC_LINE_STYLE_SOLID :			pPen->SetDashStyle( DashStyleSolid);	break;
	case GIC_LINE_STYLE_DASH :			pPen->SetDashStyle( DashStyleDash);	break;
	case GIC_LINE_STYLE_DOT :			pPen->SetDashStyle( DashStyleDot);	break;
	case GIC_LINE_STYLE_DASHDOT :		pPen->SetDashStyle( DashStyleDashDot);	break;
	case GIC_LINE_STYLE_DASHDOTDOT :	pPen->SetDashStyle( DashStyleDashDotDot);	break;
	}
}

void i3GICDiagramContext::drawBezierLine( GIC_LINE_STYLE dashStyle, REAL32 rWidth, COLORREF color, POINT &p1, POINT &p2, POINT &p3, POINT &p4)
{
	VEC2D * pOff = m_OffStack.getTop();

	UINT8 r = (UINT8)(color & 0xFF);
	UINT8 g = (UINT8)((color>>8) & 0xFF);
	UINT8 b = (UINT8)((color>>16) & 0xFF);

	Pen pen( Color( r, g, b));

	penDashStyle( &pen, dashStyle);

	pen.SetWidth( rWidth);

	Graphics myGraphic( m_pView->getDrawDC());
	
	myGraphic.DrawBezier( &pen, (INT)(p1.x + pOff->x), (INT)(p1.y + pOff->y),
		(INT)(p2.x + pOff->x), (INT)(p2.y + pOff->y), (INT)(p3.x + pOff->x),
		(INT)(p3.y + pOff->y), (INT)(p4.x + pOff->x), (INT)(p4.y + pOff->y));
}

void i3GICDiagramContext::drawPolygon( GIC_LINE_STYLE dashStyle, REAL32 rWidth, COLORREF color,
	COLORREF fillColor, POINT * pts, INT32 count, bool bFill /*= true*/)
{
	Pen pen( Color( 255, GetRValue( color), GetGValue( color), GetBValue( color)));

	penDashStyle( &pen, dashStyle);
	pen.SetWidth( rWidth);

	Graphics myGraphics( m_pView->getDrawDC());

	if( bFill)
	{
		SolidBrush brush( Color( 255, GetRValue( fillColor), GetGValue( fillColor), GetBValue( fillColor)));
		myGraphics.FillPolygon( &brush, (const Gdiplus::Point*) pts, count);
	}

	myGraphics.DrawPolygon( &pen, (const Gdiplus::Point *) pts, count);
}


#endif // I3_DEBUG
