// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3UIEditorView.cpp : implementation of the Ci3UIEditorView class
//

#include "stdafx.h"
#include "i3UIEditor.h"

#include "i3UIEditorDoc.h"
#include "i3UIEditorView.h"
#include "GlobalVariables.h"

#include "MainFrm.h"
#include "resource.h"
#include "Dlg_SplitValue.h"

#include "ScriptManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "UndoSystem2/UIUndoSystem.h"
#include "UndoSystem2/UIUndoInfo.h"

#include "GameStringTable.h"

// 정체를 알수 없는 _GAME_STRING 과 관련된 모든 소스코드 주석처리하고 향후 삭제 예정!
/*
extern "C"
{
	int		_GAME_STRING( LuaState* L )
	{
		char sz[ 1000 ];
		i3Lua::GetStringArg( L, 1, sz, 1000 );

		std::string szText = theGameString.GetString( sz );

		i3Lua::PushString( L, szText.empty() ? "" : szText.c_str() );

		return 1;
	}
};
*/

// Ci3UIEditorView

IMPLEMENT_DYNCREATE(Ci3UIEditorView, CView)

BEGIN_MESSAGE_MAP(Ci3UIEditorView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Ci3UIEditorView construction/destruction

Ci3UIEditorView::Ci3UIEditorView()
{
	// TODO: add construction code here
	//m_pUIRoot = NULL;

	m_pGripManager.Create( CUIGripObjHandler_UIControl::static_meta());

	m_bSendToFramework = false;

	m_ptDragStart.x = m_ptDragStart.y = 0;

	m_ShowX = 0;
	m_ShowY = 0;
	m_ScrollX = 0;
	m_ScrollY = 0;

	m_MouseMode = MOUSEMODE_NONE;

	m_bNotifyUpdate = true;

	m_bBanding = false;
}

Ci3UIEditorView::~Ci3UIEditorView()
{
	_ClearCopiedControlList();
}

BOOL Ci3UIEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Ci3UIEditorView drawing

void Ci3UIEditorView::OnDraw(CDC* /*pDC*/)
{
	Ci3UIEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDC * pDC = GetDC();

	// TODO: add draw code for native data here
	if( g_pViewer != NULL && pDoc->isLoaded())
	{
		i3UIRenderer * pUIRenderer = i3UIRenderer::getInstance();

		if( g_pInterface != NULL && m_bSendToFramework == true && theApp.getRunModuleEnable())
		{
			g_pInterface->OnDrive( g_pViewer);
		}
		else
		{
			i3UIManager * pUI = i3UI::getManager();
			if((pUI != NULL) && (m_bSendToFramework == false))
			{
				i3::vector<I3UI_SCENE_INFO*>& List = pUI->getSceneInfoList();

				for( size_t i = 0;i < List.size(); ++i)
				{
					I3UI_SCENE_INFO * pInfo = List[i];
					if( pInfo->m_pScene != NULL && pInfo->m_pScene->isEnable())
					{
						pInfo->m_pScene->OnUpdate(g_pViewer->getDeltaTime());
						pInfo->m_pScene->RenderCtrl();
					}
				}

				pUIRenderer->Draw_Finish();
			}

			g_pViewer->Render();
		}

		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

		HDC dc = pFrame->GetUIDC();
		if( dc != NULL)
		{
			if( m_bSendToFramework == false)
			{
				m_pGripManager.Draw( dc);
			}

			CRect rt;
			GetClientRect( &rt);

			//source's left and top depend on the scroll position
			INT32 xSrc = m_ShowX;
			INT32 ySrc = m_ShowY;

			//source's width and height depend on the zoom rate
			INT32 wSrc = (INT32)(rt.Width() / g_fZoom);
			INT32 hSrc = (INT32)(rt.Height() / g_fZoom);
			
			//Always same
			INT32 xDest = 0;	
			INT32 yDest = 0;
			INT32 wDest = rt.Width();
			INT32 hDest = rt.Height();
			
			::StretchBlt( pDC->m_hDC, xDest, yDest, wDest, hDest, dc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);

			pFrame->ReleaseUIDC( dc);
		}
	}

	ReleaseDC( pDC);
}

void Ci3UIEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	INT32 nSubMenuIdx = -1;
	i3UIControl * pUICtrl = NULL;

	if( m_pGripManager.getSelectedItemCount() > 1)
	{
		nSubMenuIdx = 1;
	}
	else if( m_pGripManager.getSelectedItemCount() == 1)
	{
		//OnContextMenu
		const i3::vector<CUIGripCtrl*>& ItemList = m_pGripManager.getSelectedItemList();
		CUIGripCtrl * pCtrl = ItemList[0];
		
		pUICtrl = (i3UIControl*)pCtrl->getObject();

		//무조건 'Insert Event Handler' Menu를 보여주는 것이 아니다. 
		if( pUICtrl->hasName() == false
			|| (pUICtrl->getState() & I3UI_STYLE_SLAVE)
			|| (pUICtrl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE))
		{
			return;
		}

		nSubMenuIdx = 2;
	}
	else
	{
		nSubMenuIdx = 3;
	}

	POINT	pt;
	GetCursorPos( &pt);
	
	CMenu	Menu;
	CMenu*	pPopup;
	Menu.LoadMenu( IDR_MENU_SCREEN);
	pPopup = Menu.GetSubMenu( nSubMenuIdx);

	if( m_CopiedControls.size() < 1)
	{
		pPopup->EnableMenuItem( ID_VIEW_PASTE, MF_DISABLED | MF_BYCOMMAND);
		pPopup->EnableMenuItem( ID_NONESELECT_PASTE, MF_DISABLED | MF_BYCOMMAND);

		const i3::vector<CUIGripCtrl*>& ItemList = m_pGripManager.getSelectedItemList();
		
		if (ItemList.size() == 0) return;

		CUIGripCtrl * pCtrl = ItemList[0];

		pUICtrl = (i3UIControl*)pCtrl->getObject();

		if (i3::kind_of<i3UIComboBase*>(pUICtrl))
		{
			pPopup->EnableMenuItem(ID_LISTBOXONOFF, MF_BYCOMMAND);
		}
		else
		{
			pPopup->DeleteMenu(ID_LISTBOXONOFF, MF_BYCOMMAND);
		}

		if (i3::kind_of<i3UIListView_Box*>(pUICtrl))
		{
			pPopup->EnableMenuItem(ID_LISTBOXDUMMYLINE, MF_BYCOMMAND);
		}
		else
		{
			pPopup->DeleteMenu(ID_LISTBOXDUMMYLINE, MF_BYCOMMAND);
		}
	}

	UINT32	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
	case ID_VIEW_SPREADCONTROL:		SpreadControl( m_pGripManager.getSelectedItemList());	break;
	case ID_VIEW_LEFTALIGN:			AlignLeft( m_pGripManager.getSelectedItemList());		break;
	case ID_VIEW_RIGHTALIGN:		AlignRight(m_pGripManager.getSelectedItemList());		break;
	case ID_VIEW_TOPALIGN:			AlignTop(m_pGripManager.getSelectedItemList());			break;
	case ID_VIEW_ALIGNBOTTOM:		AlignBottom(m_pGripManager.getSelectedItemList());		break;
	case ID_VIEW_CENTERALIGN:		AlignCenter(m_pGripManager.getSelectedItemList());		break;
	case ID_VIEW_INSERTEVENTHANDLER:	ScriptManager::InsertEventHandler( pUICtrl->GetName(), true);	break;
	case ID_VIEW_SELECTALL:				SelectAll();	break;
	case ID_VIEW_COPY:					OnCopy();		break;
	case ID_VIEW_CUT:					OnCut();		break;
	case ID_NONESELECT_PASTE :
	case ID_VIEW_PASTE:					OnPaste();		break;
	case ID_VIEW_DELETE:				OnDelete();		break;
	case ID_SINGLESELECT_VIEWTEMPLATE :	ViewTemplate();	break;
	case ID_LISTBOXONOFF:				ShowListBox();	break;
	case ID_LISTBOXDUMMYLINE:			ShowDummyLine();	break;
	default:				
		break;
	}
}

// Ci3UIEditorView diagnostics

#ifdef _DEBUG
void Ci3UIEditorView::AssertValid() const
{
	CView::AssertValid();
}

void Ci3UIEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3UIEditorDoc* Ci3UIEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3UIEditorDoc)));
	return (Ci3UIEditorDoc*)m_pDocument;
}
#endif //_DEBUG

// Ci3UIEditorView message handlers
void Ci3UIEditorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// 뷰 전환시, 자신이 activate되면 active window handle을 자신으로 설정한다.
	if( bActivate && (pActivateView == this))
	{
		g_hActiveView = m_hWnd;

		if( pActivateView != pDeactiveView)
		{
			DisableUI( false);

			CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
			CUIPaneScreen* pPane = pMain->getScreenPane();

			i3UIManager * pUI = i3UI::getManager();
			I3ASSERT( pUI != NULL);

			i3::vector<I3UI_SCENE_INFO*>& SceneList = pUI->getSceneInfoList();
			
			for(size_t i = 0; i < SceneList.size(); ++i)
			{
				I3UI_SCENE_INFO * pInfo = SceneList[i];
				I3ASSERT( pInfo->m_pScene != NULL);

				i3GameNode * pChild = pInfo->m_pScene->getFirstChild();
				while( pChild)
				{
					pPane->AdjustTreeItem( pChild);

					pChild = pChild->getNext();
				}
			}
		}

		//SetTimer( 1, 10, NULL);
	}
	else if( !bActivate && (pDeactiveView == this))
	{
		if( pActivateView != pDeactiveView)
		{
			UnSelectAll();

			if( isPrevMode())
			{
				SetPrevMode(false);
			}

			DisableUI( true);
		}

		//KillTimer( 1);
	}
	else
	{
	}

	InvalidateRect(NULL);

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void Ci3UIEditorView::DisableUI( bool bDisable)
{
	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != NULL);

	i3::vector<I3UI_SCENE_INFO*>& SceneList = pUI->getSceneInfoList();
	
	for(size_t i = 0; i < SceneList.size(); ++i)
	{
		I3UI_SCENE_INFO * pInfo =SceneList[i];
		I3ASSERT( pInfo->m_pScene != NULL);

		if( bDisable)
		{
			pInfo->m_pScene->EnableScene( FALSE);
		}
		else
		{
			if( pInfo->m_bEnabled)	//?
			{
				pInfo->m_pScene->EnableScene( TRUE);
			}
		}
	}
}

BOOL Ci3UIEditorView::OnEraseBkgnd(CDC* pDC)
{
	// 뷰 전환시 깜빡임을 없애기 위해 백그라운드를 다시 그리지 않는다. ( rendering하기 때문에)
	return CWnd::OnEraseBkgnd(pDC);
	//return TRUE;
}

void Ci3UIEditorView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	Invalidate(FALSE);

	CView::OnTimer(nIDEvent);
}

void Ci3UIEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
	
	CPoint ptRel = _AbsToRel( point);

	// ListView
	if( getScene() != NULL)
	{
		_SendMouseMsgToLVItem( LISTVIEW_MOUSEMSG_LDOWN, getScene(), nFlags, ptRel);
	}

	m_ptOldPos = m_ptDragStart = ptRel;

	SetCapture();

	i3UIControl * pPickedCtrl = GetPickedControl( ptRel );
	bool bSelectedPicking = false;
	
	if( m_pGripManager.isSelected( pPickedCtrl ) || m_pGripManager.isOnGrip( ptRel))
	{
		if( m_pGripManager.OnLButtonDown( ptRel ) )
		{
			m_bLBDragging = true;
			bSelectedPicking = true;
			::InvalidateRect( g_hActiveView, &g_rtTarget, false );
		}
	}

	if( bSelectedPicking == false)
	{
		m_bBanding = true;
		m_bLBDragging = false;
		m_pGripManager.StartBanding( ptRel);
	}


	//Ctrl을 누른 상태에서 Rect를 클릭한 경우에만
	m_bAddMode = ( (::GetKeyState( VK_LCONTROL) & 0x80) && m_pGripManager.getFocusedPoint() == NULL) ? true : false;
	
}

void Ci3UIEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if( m_bBanding)
	{
		m_bBanding = false;
		m_pGripManager.EndBanding();
	}

	CPoint ptRel = _AbsToRel( point);
	//CPoint ptRel = point;
	
	// ListView
	if( getScene() != NULL)
	{
		_SendMouseMsgToLVItem( LISTVIEW_MOUSEMSG_LUP, getScene(), nFlags, ptRel);
	}

	CRect rtDrag;

	_AdjustRect( ptRel, m_ptDragStart, &rtDrag);	// 주어지는 두 위치를 이용하여 뒤집혀 지지 않은 rect를 만들어 낸다.

	if( m_bLBDragging)
	{
		m_pGripManager.OnLButtonUp( ptRel);
	}
	else
	{
		if( m_ptDragStart == ptRel)
			_SelectItem( ptRel, m_bAddMode);	
		else
			_SelectItemInRect( &rtDrag, m_bAddMode);
	}

	m_bLBDragging = false;
	
	::InvalidateRect( m_hWnd, &g_rtTarget, FALSE);

	CView::OnLButtonUp(nFlags, point);
}

bool Ci3UIEditorView::_SendMouseMsgToLVItem( LISTVIEW_MOUSEMSG eMsg, i3GameNode* pRoot, UINT nFlags, CPoint point)
{
	if( i3::same_of<i3UIListView_Item*>(pRoot)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))
	{
		i3UIListView_Item* pLVItem = (i3UIListView_Item*)pRoot;
		if( _IsptInImage( point, pLVItem) && m_pGripManager.isSelected( pLVItem) && m_pGripManager.getSelectedItemCount() == 1)
		{
			switch( eMsg)
			{
			case LISTVIEW_MOUSEMSG_MOVE:
				{
					HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW);	//default 
					HCURSOR hSizeWE = ::LoadCursor( NULL, IDC_SIZEWE);
					HCURSOR hCurrent = (HCURSOR)::GetClassLong( m_hWnd, GCL_HCURSOR);
										
					if( hCurrent == hCursor || hCurrent == hSizeWE )
					{
						bool bResult = pLVItem->OnMouseMove_EditMode( nFlags, point); 
						
						if( bResult)
						{
							hCursor = hSizeWE;
						}

						if( m_bLBDragging == false)
						{
							::SetCursor( hCursor);	//change that made by SetClassLong does not immediately apply to the cursor
							::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);
						}
					}
				}
				break;
			case LISTVIEW_MOUSEMSG_LDOWN:	
				{
					pLVItem->OnLButtonDown_EditMode( nFlags, point); 
				}
				break;
			case LISTVIEW_MOUSEMSG_LUP:		
				{
					pLVItem->OnLButtonUp_EditMode( nFlags, point); 
				}
				break;
			default: 
				break;
			}
			
			return true;
		}
	}

	i3GameNode* pChild = pRoot->getFirstChild();

	while( pChild != NULL)
	{
		bool bResult = _SendMouseMsgToLVItem( eMsg, pChild, nFlags, point);
		if( bResult == true)
			return true;

		pChild = pChild->getNext();
	}

	return false;
}

void Ci3UIEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_MouseMode == MOUSEMODE_NONE)
	{
		CPoint ptRel = _AbsToRel( point);

		HCURSOR hCursor = m_pGripManager.getCursor( ptRel);

		if( m_bLBDragging == false)
			::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);

		if( m_bBanding)
		{
			m_pGripManager.Banding( ptRel);

			Invalidate( FALSE);
		}

		// ListView
		if( getScene() != NULL)
		{
			_SendMouseMsgToLVItem( LISTVIEW_MOUSEMSG_MOVE, getScene(), nFlags, ptRel);	
		}

		if( m_bAddMode)
		{
			CView::OnMouseMove(nFlags, point);

			m_ptOldPos = ptRel;

			return;
		}

		if( m_pGripManager.OnMouseMove( ptRel, nFlags))
		{
			if( ptRel != m_ptOldPos)
			{
				m_pGripManager.RecalcLayout();

				::InvalidateRect( m_hWnd, &g_rtTarget, false);
			}
		}

		m_ptOldPos = ptRel;
	}
	else if( m_MouseMode == MOUSEMODE_SCROLL)
	{
		CPoint subPos;
		
		subPos = m_ScrollStart - point;

		if( subPos.x > 0)
		{
			UpdateScroll( false, SB_LINEDOWN, subPos.x, NULL);
		}
		else if( subPos.x < 0)
		{
			UpdateScroll( false, SB_LINEUP, subPos.x, NULL);
		}

		if( subPos.y > 0)
			UpdateScroll( true, SB_LINEDOWN, subPos.y, NULL);
		else if( subPos.y < 0)
			UpdateScroll( true, SB_LINEUP, subPos.y, NULL);

		m_ScrollStart = point;
	}

	CView::OnMouseMove(nFlags, point);
}

void Ci3UIEditorView::_AdjustRect( CPoint point1, CPoint point2, CRect * prtDrag)
{
	prtDrag->SetRect( MIN( point1.x , point2.x), MIN( point1.y, point2.y),
					MAX( point1.x , point2.x), MAX( point1.y, point2.y));
}


bool Ci3UIEditorView::_IsptInImage( CPoint point, i3UIControl * pControl)
{
	CRect rt;

	//ComboBox 예외 처리
	if( i3::kind_of<i3UIComboBase*>(pControl)) 
	{
		i3UIComboBox* pCombo = (i3UIComboBox*)pControl;
		REAL32 h = pCombo->getMinimumHeight();
		
		rt.SetRect( (INT32)pControl->getAccumulatedPos()->x, (INT32)pControl->getAccumulatedPos()->y, 
					(INT32)(pControl->getAccumulatedPos()->x + pControl->getWidth()), 
					(INT32)(pControl->getAccumulatedPos()->y + h));
	}
	else
	{
		rt.SetRect( (INT32)pControl->getAccumulatedPos()->x, (INT32)pControl->getAccumulatedPos()->y, 
					(INT32)(pControl->getAccumulatedPos()->x + pControl->getWidth()), 
					(INT32)(pControl->getAccumulatedPos()->y + pControl->getHeight()));
	}

	rt.NormalizeRect();

	if( rt.PtInRect( point))
		return true;
	else
		return false;
}

bool Ci3UIEditorView::_IsincludeRect( CRect *prtDrag, i3UIControl * pControl)
{
	CRect rt;
	rt.SetRect( (INT32)pControl->getAccumulatedPos()->x, (INT32)pControl->getAccumulatedPos()->y, 
				(INT32)(pControl->getAccumulatedPos()->x + pControl->getWidth()), 
				(INT32)(pControl->getAccumulatedPos()->y + pControl->getHeight()));

	INT32 includecnt = 0;

	rt.NormalizeRect();

	if( prtDrag->PtInRect( rt.TopLeft())) includecnt++;	
	if( prtDrag->PtInRect( rt.BottomRight())) includecnt++;
	if( prtDrag->PtInRect( CPoint( rt.right, rt.top))) includecnt++;
	if( prtDrag->PtInRect( CPoint( rt.left, rt.bottom))) includecnt++;

	if( includecnt > 1)
		return true;
	else 
		return false;
}

bool Ci3UIEditorView::IsPtInWindow( i3GameObj * pRoot, CPoint pt)
{
	if(  i3::kind_of<i3UIFrameWnd*>(pRoot)) //->IsTypeOf( i3UIFrameWnd::static_meta()))
	{
		i3UIFrameWnd * pThisWindow = (i3UIFrameWnd*)pRoot;

		if( pThisWindow->isEnable() && _IsptInImage( pt, (i3UIControl*)pThisWindow))
		{
			I3TRACE( "In FrameWindow..\n");
			return true;
		}
	}

	return false;
}

bool Ci3UIEditorView::InsideCtrl( i3UIControl * pCtrl, const CPoint & point)
{
	CRect rt;

	//ComboBox 예외 처리
	if( i3::kind_of<i3UIComboBase*>(pCtrl)) 
	{
		i3UIComboBox* pCombo = (i3UIComboBox*)pCtrl;
		REAL32 h = pCombo->getMinimumHeight();
		
		rt.SetRect( (INT32)pCtrl->getAccumulatedPos()->x, (INT32)pCtrl->getAccumulatedPos()->y, 
					(INT32)(pCtrl->getAccumulatedPos()->x + pCtrl->getWidth()), 
					(INT32)(pCtrl->getAccumulatedPos()->y + h));
	}
	else
	{
		rt.SetRect( (INT32)pCtrl->getAccumulatedPos()->x, (INT32)pCtrl->getAccumulatedPos()->y, 
					(INT32)(pCtrl->getAccumulatedPos()->x + pCtrl->getWidth()), 
					(INT32)(pCtrl->getAccumulatedPos()->y + pCtrl->getHeight()));
	}

	rt.NormalizeRect();

	if( rt.PtInRect( point))
		return true;
	else
		return false;
}

i3UIControl * Ci3UIEditorView::Rec_GetPickedControl( i3GameNode * pNode, const CPoint & point)
{
	i3GameNode * pChild = pNode->getFirstChild();
	i3::vector< i3GameNode *>	siblingList;

	// 같은 Child 안에서는 가장 나중의 것(화면 상의 위)이 선택되도록 해야 함.
	while( pChild != NULL )
	{
		siblingList.insert( siblingList.begin(), pChild );

		pChild = pChild->getNext();
	}

	for( size_t i = 0; i < siblingList.size(); i++)
	{
		// Child Control 우선 탐색
		pChild = siblingList[i];

		if( i3::kind_of( pChild, i3UIControl::static_meta() ) )
		{
			i3UIControl * pChildCtrl = (i3UIControl *) pChild;

			if( !pChildCtrl->isStyle( I3UI_STYLE_LOCK ) && !pChildCtrl->isStyle( I3UI_STYLE_SLAVE ) && pChildCtrl->isEnable() )
			{
				i3UIControl * pTemp = Rec_GetPickedControl( pChildCtrl, point );

				// Child가 아닌데, 무언가 Picking되었다면 바로 return.
				if( pTemp != NULL )
				{
					return pTemp;
				}
			}
		}
	}

	// 가장 마지막에 자기와 비교
	if( i3::kind_of( pNode, i3UIControl::static_meta()))
	{
		i3UIControl * pCtrl = (i3UIControl *) pNode;

		if( InsideCtrl( pCtrl, point))
		{
			return pCtrl;
		}
	}

	return NULL;
}

i3UIControl * Ci3UIEditorView::GetPickedControl( const CPoint & point)
{
	i3GameNode * pNode = getScene();
	if (pNode == NULL) return NULL;
	return Rec_GetPickedControl( pNode, point);
}

i3UIControl * Ci3UIEditorView::_rec_IsPtinControl( i3GameObj * pRoot, CPoint pt, bool bReverse, INT32 *pcnt)
{
	i3UIControl * pCtrl = NULL;

	if( i3::kind_of<i3UIControl*>(pRoot)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		i3UIControl* pUICtrl = (i3UIControl*)pRoot;

		if( pUICtrl->isEnable() && _IsptInImage( pt, pUICtrl))
		{
			pCtrl = pUICtrl;

			*pcnt += 1;
		}
	}

	i3GameObj * pChild = (i3GameObj*)pRoot->getFirstChild();
	while( pChild != NULL)
	{
		i3UIControl * pSel = _rec_IsPtinControl(pChild, pt, bReverse, pcnt);
		if( pSel != NULL)	pCtrl = pSel;

		pChild = (i3GameObj*)pChild->getNext();
	}

	return pCtrl;
}

void Ci3UIEditorView::_rec_IsControlinRect( i3GameNode * pRoot, CRect *prtDrag, bool bReverse, INT32 *pcnt)
{
	i3GameNode * pChild = (i3GameNode*) pRoot->getFirstChild();
	i3::vector< i3GameNode*> siblingList;

	while( pChild != NULL)
	{
		siblingList.insert( siblingList.begin(), pChild );		// 역순으로 돌기 위해...

		pChild = (i3GameObj*)pChild->getNext();
	}

	for( size_t i = 0; i < siblingList.size(); i++ )
	{
		pChild = siblingList[i];

		_rec_IsControlinRect( pChild, prtDrag, bReverse, pcnt );
	}

	if( i3::kind_of<i3UIControl*>(pRoot)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		i3UIControl* pUICtrl = (i3UIControl*)pRoot;

		if( pUICtrl->isEnable() && _IsincludeRect( prtDrag, pUICtrl))
		{
			bool bSelected = m_pGripManager.isSelected( pRoot);

			if( bReverse)
			{
				if( bSelected)
					UnSelect( (i3UIControl *) pRoot);
				else
					Select( (i3UIControl *) pRoot);
			}
			else
			{
				Select( (i3UIControl*)pRoot);
			}

			*pcnt += 1;
		}
	}
}

i3UIScene	*	Ci3UIEditorView::getScene(void)
{
	Ci3UIEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return NULL;

	return pDoc->getCurrentScene();
}

//이건 하위 윈도우까지 찾는거.
i3UIFrameWnd * Ci3UIEditorView::Rec_GetPickedFrameWnd( i3GameNode * pParent, const CPoint & pt)
{
	i3UIFrameWnd * pWindow = NULL;

	i3GameNode * pChild = pParent->getFirstChild();
	
	while( pChild != NULL)
	{
		if( pChild->isEnable() )
		{
			pWindow = Rec_GetPickedFrameWnd( pChild, pt);
			if( pWindow != NULL)
				return pWindow;

			pChild = (i3GameObj*)pChild->getNext();
		}
		else
		{
			pChild = (i3GameObj*)pChild->getNext();
		}
	}

	if( i3::kind_of( pParent, i3UIFrameWnd::static_meta()))
	{
		if( InsideCtrl( (i3UIControl *) pParent, pt))
			return (i3UIFrameWnd*) pParent;
	}

	return pWindow;
}

i3UIFrameWnd	*		Ci3UIEditorView::GetPickedFrameWnd( const CPoint & point)
{
	if( getScene() == NULL)
		return NULL;

	i3UIFrameWnd * pOwner = Rec_GetPickedFrameWnd( getScene(), point);

	return pOwner;
}

void Ci3UIEditorView::_SelectItem( CPoint point, bool bAddMode)
{
	// 여러 개의 Control이 겹쳐있는 경우
	// 가장 Child인 Control 중에서 트리상에 먼저 위치해 있는 아이템을 선택한다. 
	if( getScene() == NULL)
		return;

	if( !bAddMode)
	{
		UnSelectAll();
	}

	// scene의 child windows중에 최상위 윈도우만 가려서, 현재 클릭한 지점에 위치한 윈도우를 가려낸다.

	i3UIControl * pPicked = GetPickedControl( point);

	// 있다면 최상위로 보여지게 하고, 선택상태로.
	if( pPicked != NULL)
	{
		bool bSelected = m_pGripManager.isSelected( pPicked);

		if( bAddMode)
		{
			if( bSelected)
				UnSelect( pPicked);
			else
				Select( pPicked);
		}
		else
		{
			Select( pPicked);
		}

		if( !bSelected)
		{
			pPicked->ShowTop();
		}
	}
}

void Ci3UIEditorView::_SelectItemInRect( CRect *prtDrag, bool bReverse)
{
	if( getScene() == NULL)
		return;

	INT32 cnt = 0;

	// 가장 마지막의 윈도우안에 있는 컨트롤중에 선택될 아이템이 있는지 먼저 판단.

	i3GameNode * pFirstChild = getScene()->getFirstChild();
	while( pFirstChild != NULL)
	{
		_rec_IsControlinRect((i3GameObj*)pFirstChild, prtDrag, bReverse, &cnt);

		pFirstChild = pFirstChild->getNext();
	}

	if( !bReverse && cnt <= 0)
		UnSelectAll();
}

INT32 Ci3UIEditorView::Select( i3UIControl * pControl, bool bState)
{
	if (::GetAsyncKeyState(VK_SHIFT) & 0x8000)
		return 0;

	//ListView의 Cell이나 Seperator를 선택하면 Item을 선택하도록 강제한다.
	//또는 복합 컨트롤의 자식을 클릭하는 경우에도 부모를 선택하도록 강제한다.
	//(예외가 있을 수 있음)
	i3UIControl* pCtrl = pControl->OnSelect();
	
	INT32 rv = m_pGripManager.AddSelect( pCtrl, NULL, false);

	if( m_bNotifyUpdate || bState )
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pCtrl);

	return rv;
}

void Ci3UIEditorView::SelectAll(void)
{
	INT32 nCurrentlySelected = m_pGripManager.getSelectedItemCount();

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CUIPaneScreen* pPane = pMain->getScreenPane();
	
	if( nCurrentlySelected == 0)
	{
		//Select all controls on the screen
		i3::vector<i3UIControl*> list;
		pPane->getAllChildControls( NULL, list);

		for( size_t i=0; i<list.size(); i++)
		{
			i3UIControl* pControl = list[i];

			if( pControl->isStyle( I3UI_STYLE_LOCK) == false &&
				pControl->isEnable() )
			{
				i3UIControl* pSelectableControl = pControl->OnSelect();
				
				m_pGripManager.AddSelect( pSelectableControl, NULL, false);

				if( m_bNotifyUpdate)
					i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pSelectableControl);
			}
		}
	}
	else/* if( nCurrentlySelected > 0)*/
	{
		//Select all controls on the same frame window
		const i3::vector<CUIGripCtrl*>& List = m_pGripManager.getSelectedItemList();
		i3::vector<i3UIControl*> childList;

		for( size_t i=0; i< List.size(); i++)
		{
			CUIGripCtrl * pCtrl		= List[i];
			i3UIControl * pUICtrl	= (i3UIControl*)pCtrl->getObject();

			pPane->getAllChildControls( pUICtrl, childList);
		}

		for( size_t i=0; i<childList.size(); i++)
		{
			i3UIControl* pControl = childList[i];

			if( pControl->isStyle( I3UI_STYLE_LOCK) == false &&
				pControl->isEnable() )
			{
				i3UIControl* pSelectableControl = pControl->OnSelect();
				
				m_pGripManager.AddSelect( pSelectableControl, NULL, false);

				if( m_bNotifyUpdate)
					i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pSelectableControl);
			}
		}
	}
}

void Ci3UIEditorView::UnSelect( i3UIControl * pControl)
{
	if (::GetAsyncKeyState(VK_SHIFT) & 0x8000)
		return;

	m_pGripManager.RemoveSelect( pControl);

	if( m_bNotifyUpdate)
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pControl);
}

void Ci3UIEditorView::UnSelectAll(void)
{
	if (::GetAsyncKeyState(VK_SHIFT) & 0x8000)
		return;

	m_pGripManager.Reset();

	if( m_bNotifyUpdate)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3UIControl::static_meta());
	}
}

bool Ci3UIEditorView::OnDragMoveObjectTemplate( POINT point, bool bInsideFrame /*= false*/, i3ClassMeta * pInsideFrameMeta /*= NULL*/)
{
	CPoint ptRel = _AbsToRel( point);

	if( m_bSendToFramework == true)
		return false;

	UnSelectAll();

	i3UIFrameWnd * pWnd = GetPickedFrameWnd( ptRel);
	if( pWnd != NULL)
	{
		if( bInsideFrame)
		{
			// 가장 마지막의 윈도우안에 있는 컨트롤중에 선택될 아이템이 있는지 먼저 판단.
			INT32 cnt = 0;
			i3UIControl * pCtrl = _rec_IsPtinControl((i3GameObj*)pWnd, point, false, &cnt);
			if( (pCtrl != NULL) &&
				(pCtrl->isStyle( I3UI_STYLE_LOCK) == false) &&
				pCtrl->same_of( pInsideFrameMeta) )
			{
				Select( pCtrl);
			}
		}
		else
			Select( pWnd);
	}

	return false;
}

bool Ci3UIEditorView::DropObject( i3UITemplate * pTemplate, POINT scrPT)
{
	if( m_bSendToFramework == true)
		return false;

	i3GameNode* pParent = getScene();	//by default

	if( pParent == NULL)
	{
		::AfxMessageBox( "Scene을 생성하지 않았습니다. 새 문서를 생성하십시오.");
		return false;
	}

	I3ASSERT( pTemplate != NULL);
	
	i3::pack::RECT rt;
	pTemplate->GetControlRect(&rt);

	CPoint ptRel = _AbsToRel( scrPT);

	// Creates a control through i3UI
	{
		VEC3D	vPos;
		VEC2D	vSize;
		
		i3Vector::Set( &vPos, (REAL32)ptRel.x, (REAL32)ptRel.y, 0.0f);
		i3Vector::Set( &vSize, rt.right - rt.left, rt.bottom - rt.top);
		
		

		if( i3::same_of<i3UITemplate_LVItem*>(pTemplate) //->IsExactTypeOf( i3UITemplate_LVItem::static_meta()) 
			|| i3::same_of<i3UITemplate_LVHeader*>(pTemplate)) //->IsExactTypeOf( i3UITemplate_LVHeader::static_meta()))
		{
			bool bItem = i3::same_of<i3UITemplate_LVItem*>(pTemplate); //->IsExactTypeOf( i3UITemplate_LVItem::static_meta()) ? true : false;

			i3UIListView_Box* pBox = _getOwnerListViewBox( ptRel, bItem);
			if( pBox != NULL)
			{
				pParent = pBox;
				i3Vector::Set( &vPos, (REAL32)ptRel.x - pBox->getAccumulatedPos()->x, (REAL32)ptRel.y - pBox->getAccumulatedPos()->y, 0.0f);
			}
		}
		else
		{
			i3UIFrameWnd * pWnd = GetPickedFrameWnd( ptRel);
			
			if( pWnd != NULL)
			{
				pParent = pWnd;

				vPos.x -= pWnd->getAccumulatedPos()->x;
				vPos.y -= pWnd->getAccumulatedPos()->y;
			}
		}

		
		i3UIControl* pCtrl = i3UI::CreateControl( pTemplate, &vPos, &vSize, pParent);
		
		I3ASSERT( pCtrl != NULL);

		// scene에 직접 붙는 컨트롤인 경우에는 waitload를 빼주어야 정상적으로 보인다.( texture가 validate되기 전엔 이상할 수도 있음)
		if( pParent == getScene())
		{
			pCtrl->removeState( I3UI_STATE_WAITLOAD);
		}
		
		if( UIUndo::ADD_Create( pCtrl) == false)
		{
			if( m_bNotifyUpdate)
				i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pCtrl, pTemplate->GetInstanceMeta());
		}
	}

	return true;	
}

void Ci3UIEditorView::SetPrevMode( bool bSet)
{
	i3UIManager * pUIManager = i3UI::getManager();
	I3ASSERT( pUIManager != NULL);

	if( getScene() != NULL)
	{
		if( m_bSendToFramework == bSet)
			return;

		i3UIScene * pScene = getScene();
		if( pScene != NULL)
		{
			pScene->Detach();

			CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

			pFrame->returnToEditState();
			pFrame->setEnableOtherScene( false);
		}
		else
		{
			return;
		}

		if( bSet)
		{
			pScene->EnableScene(TRUE);

			bool bInitLua = true;
			if( pScene->getScriptFile() == NULL)
				bInitLua = false;

			if( bInitLua)
			{
				pScene->OpenLuaContext();
				if( pScene->getLuaContext() != NULL)
				{
					i3Framework * pFramework = i3Framework::getCurrentFramework();
					i3Stage * pStage = pFramework->GetCurrentStage();
					if((pStage == NULL) && (g_pInterface != NULL))
					{						
						g_pInterface->RegisterScript( pScene->getLuaContext());
					}
				}
			}

//			if( NULL != pScene->getLuaContext() )
//				i3Lua::RegisterFunc( pScene->getLuaContext(), "GAME_STRING", _GAME_STRING );

			if( pScene->Attach(bSet,theApp.m_hInstance) == true)
			{
				m_bSendToFramework = true;
					
				CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->setEnableOtherScene( true);
			}
			else
			{
				//Error. (Ex: Dofile에서 실패했을 경우)
				pScene->Detach();

				CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->returnToEditState();
			}
		}
		else
		{
			m_bSendToFramework = false;
			//I3TRACE("m_bSendToFramework : FALSE \n");
		}
	}
	else
	{
		m_bSendToFramework = true;
	}
}

BOOL Ci3UIEditorView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( m_bSendToFramework == true)
	{
		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

		/*if( pMsg->message == WM_KEYDOWN)
		{
			if( pMsg->wParam == VK_F5)
				pFrame->OnProjectStop();
		}*/

		CWnd * pWnd = pFrame->getDummyWindow();

		if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
			(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		{

		}
		else
		{
			::SendMessage( pWnd->m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);

			return TRUE;
		}
	}
	else
	{
		switch( pMsg->message)
		{
		case WM_KEYDOWN:
			if( GetKeyState( VK_CONTROL) & 0x8000)
			{
				BOOL bRet = FALSE;
				switch( pMsg->wParam)
				{
				case 'A':	SelectAll();	bRet = TRUE;	break;
				case 'D':	OnDelete();		bRet = TRUE;	break;
				case 'C':	OnCopy();		bRet = TRUE;	break;
				case 'X':	OnCut();		bRet = TRUE;	break;
				case 'V':	OnPaste();		bRet = TRUE;	break;
				default:
					break;
				}
				if( bRet)	return TRUE;
			}
			else
			{
				//if( pMsg->wParam == VK_F5)
				//{
				//	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
				//	pFrame->OnProjectRun();
				//}
			}
			break;
		default:
			break;
			
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

void Ci3UIEditorView::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	// i3TDK::Update가 중복적으로 호출되는 것을 방지하기 위함.
	m_bNotifyUpdate = false;

	if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		UnSelectAll();
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_BEFORE_PROP_CHANGE)
	{
		if( pInfo->m_pObject != NULL )
		{
			i3ElementBase* pBase = pInfo->m_pObject;
			if( i3::kind_of<i3UIControl*>(pBase)) //->IsTypeOf( i3UIControl::static_meta()))
			{
				i3UIControl* pControl = (i3UIControl*)pBase;

				UIUndo::ADD_Edit( pControl);

				pControl->AdjustTemplate();
				
				I3TRACE("============UIUndo::ADD_Edit( pControl)\n");
			}
//			else if( pBase->IsTypeOf( List::static_meta()))			// List 대신에 i3AnyData를 사용...
			else if (i3::kind_of<i3AnyData*>(pBase)) //->IsTypeOf( i3AnyData::static_meta()))
			{
				//List* pList = (List*)pBase;
				i3AnyData* pa = static_cast<i3AnyData*>(pBase);

				i3::vector<i3ElementBase*>* pList = AnyDataCast< i3::vector<i3ElementBase*> >(pa);
				I3ASSERT( pList != NULL);
				
				i3::vector<i3UIControl*> Target;
				i3::vu::copy_ptr_vector_force(*pList, Target);
				UIUndo::ADD_Edit( Target);

				I3TRACE("============UIUndo::ADD_Edit( pList)\n");
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		if((pInfo->m_pObject != NULL) && i3::kind_of<i3UIControl*>(pInfo->m_pMeta)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl * pControl = (i3UIControl*)pInfo->m_pObject;

			//pControl->ApplyChangesFromPropertyGrid();

			CWnd* pMain = AfxGetMainWnd();
			((CMainFrame*)pMain)->getPropertiesPane()->Update();

			 i3::vector<PROPERTYMSG*> msgList;
			pControl->GetMessagesForPropertyGrid( msgList);
			((CMainFrame*)pMain)->getPropertiesPane()->SendMessagesToPropertyGrid( msgList);
						
			m_pGripManager.RecalcLayout();
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_hwndFrom != NULL && pInfo->m_hwndFrom != m_hWnd)
		{
			if( pInfo->m_pObject != NULL)
			{
				if( i3::kind_of<i3UIControl*>(pInfo->m_pMeta)) //->IsTypeOf( i3UIControl::static_meta()))
				{
					i3UIControl * pControl = (i3UIControl*)pInfo->m_pObject;
					Select( pControl, true);
				}
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_UNSELECT)
	{
		// 자신의 업데이트 메시지는 무시한다.
		if( pInfo->m_hwndFrom != NULL && pInfo->m_hwndFrom != m_hWnd)
		{
			if( pInfo->m_pObject == NULL)
				UnSelectAll();
			else
			{
				if(i3::kind_of<i3UIControl*>(pInfo->m_pMeta)) //->IsTypeOf( i3UIControl::static_meta()))
				{
					i3UIControl * pControl = (i3UIControl*)pInfo->m_pObject;

					UnSelect( pControl);
				}
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SET)
	{
		if( i3::kind_of< i3UIScene*>( pInfo->m_pMeta))
		{
			UnSelectAll();
		}
	}

	m_bNotifyUpdate = true;
}

void Ci3UIEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	{
		HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW);
		::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);

		::SetClassLong( m_hWnd, GCL_HBRBACKGROUND, (LONG)g_hbrBackground);
	}

	// TODO: Add your specialized code here and/or call the base class
	UINT32 flag = I3_TDK_UPDATE_REMOVE | I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_UNSELECT | I3_TDK_UPDATE_BEFORE_PROP_CHANGE;
	i3TDK::RegisterUpdate( m_hWnd, i3UIControl::static_meta(), flag);
	i3TDK::RegisterUpdate( m_hWnd, i3UIScene::static_meta(), I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_UNSELECT | I3_TDK_UPDATE_BEFORE_PROP_CHANGE | I3_TDK_UPDATE_SET);
	i3TDK::RegisterUpdate( m_hWnd, i3AnyData::static_meta(), I3_TDK_UPDATE_BEFORE_PROP_CHANGE);
//	i3TDK::RegisterUpdate( m_hWnd, List::static_meta(), I3_TDK_UPDATE_BEFORE_PROP_CHANGE);
}

LRESULT Ci3UIEditorView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		if( IsWindowVisible())
			OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CView::WindowProc(message, wParam, lParam);
}

void Ci3UIEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 방향키
	if( nChar >= VK_LEFT && nChar <= VK_DOWN)	//VK_LEFT(0x25), VK_DOWN(0x28)
	{
		UINT32 flag = 0;

		if( (GetKeyState( VK_CONTROL) & 0x8000) != 0)
			flag |= GRIP_CTRL_PRESSED;

		if( (GetKeyState( VK_SHIFT) & 0x8000) != 0)
			flag |= GRIP_SHIFT_PRESSED;

		m_pGripManager.OnKeyDown( nChar, flag);

		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->UpdatePropertyPane();
	}
	
	//Accelerators
	/*if( (GetKeyState( VK_CONTROL) & 0x8000) != 0)
	{
		switch( nChar)
		{
		
		}
	}*/


	if( nChar == VK_DELETE)
	{
		if( m_pGripManager.getSelectedItemCount() > 0)
			OnDelete();
	}


	g_pViewer->OnKeyDown( nChar);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Ci3UIEditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	g_pViewer->OnKeyUp( nChar);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void Ci3UIEditorView::SpreadControl(const i3::vector<CUIGripCtrl*>& ItemList)
{
	CDlg_SplitValue Dlg;
	if( Dlg.DoModal() == IDOK)
	{
		//Add To UndoSystem
		{
			i3::vector<i3UIControl*> list;
			for( size_t i=0; i< ItemList.size(); i++)
			{
				CUIGripCtrl* pGripCtrl = ItemList[i];
				i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();
				
				list.push_back( pControl);
			}

			UIUndo::ADD_Transform( list);
		}

		INT32 col	= Dlg.m_Col;
		if( col <= 0) col = 1;

		INT32 Margin = Dlg.m_Margin;

		if( ItemList.size() > 1)
		{
			INT32 nBase = (INT32)ItemList.size()-1;
			CUIGripCtrl * pCtrl		= ItemList[nBase];
			i3UIControl * pUICtrl	= (i3UIControl*)pCtrl->getObject();

			REAL32 left = getX( pUICtrl->getPos()) + pUICtrl->getWidth() + Margin;
			REAL32 top = getY( pUICtrl->getPos());
			REAL32 originX = getX( pUICtrl->getPos());

			if( col == 1)
			{
				top += pUICtrl->getHeight() + Margin;
				left = originX;
			}

			for( INT32 i = nBase-1; i >= 0; --i)
			{
				pCtrl = ItemList[i];
				pUICtrl = (i3UIControl*)pCtrl->getObject();

				pUICtrl->setPos( left, top);

				pCtrl->RecalcLayout();
				
				left += pUICtrl->getWidth() + Margin;

				if((i%col) == (col-1))
				{
					left = originX;
					top += pUICtrl->getHeight() + Margin;
				}
			}
		}

		m_pGripManager.RecalcLayout();
	}
}

void Ci3UIEditorView::AlignLeft( const i3::vector<CUIGripCtrl*>& ItemList)
{
	//Add To UndoSystem
	{
		i3::vector<i3UIControl*> list;
		for( size_t i=0; i< ItemList.size(); i++)
		{
			CUIGripCtrl* pGripCtrl = ItemList[i];
			i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();
			
			list.push_back( pControl);
		}

		UIUndo::ADD_Transform( list);
	}

	INT32 nBase = (INT32)ItemList.size()-1;
	CUIGripCtrl * pCtrl		= ItemList[nBase];
	i3UIControl * pUICtrl	= (i3UIControl*)pCtrl->getObject();

	REAL32 left = getX( pUICtrl->getPos());

	VEC3D pos;
	for( INT32 i = nBase-1; i >= 0; --i)
	{
		pCtrl = ItemList[i];
		pUICtrl = (i3UIControl*)pCtrl->getObject();

		i3Vector::Copy( &pos, pUICtrl->getPos());

		pos.x = left;

		pUICtrl->setPos( &pos);

		pCtrl->RecalcLayout();
	}

	m_pGripManager.RecalcLayout();
}

void Ci3UIEditorView::AlignRight(const i3::vector<CUIGripCtrl*>& ItemList)
{
	//Add To UndoSystem
	{
		i3::vector<i3UIControl*> list;
		for( size_t i=0; i< ItemList.size(); i++)
		{
			CUIGripCtrl* pGripCtrl = ItemList[i];
			i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();
			
			list.push_back( pControl);
		}

		UIUndo::ADD_Transform( list);
	}

	INT32 nBase = (INT32)ItemList.size()-1;
	CUIGripCtrl * pCtrl		= ItemList[nBase];
	i3UIControl * pUICtrl	= (i3UIControl*)pCtrl->getObject();

	REAL32 right = getX( pUICtrl->getPos()) + pUICtrl->getWidth();

	VEC3D pos;
	
	for( INT32 i = nBase-1; i >= 0; --i)
	{
		pCtrl = ItemList[i];
		pUICtrl = (i3UIControl*)pCtrl->getObject();

		i3Vector::Copy( &pos, pUICtrl->getPos());

		pos.x = right - pUICtrl->getWidth();

		pUICtrl->setPos( &pos);

		pCtrl->RecalcLayout();
	}
}

void Ci3UIEditorView::AlignTop(const i3::vector<CUIGripCtrl*>& ItemList)
{
	//Add To UndoSystem
	{
		i3::vector<i3UIControl*> list;
		for( size_t i=0; i< ItemList.size(); i++)
		{
			CUIGripCtrl* pGripCtrl =ItemList[i];
			i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();
			
			list.push_back( pControl);
		}

		UIUndo::ADD_Transform( list);
	}

	INT32 nBase = (INT32)ItemList.size()-1;
	CUIGripCtrl * pCtrl		= ItemList[nBase];
	i3UIControl * pUICtrl	= (i3UIControl*)pCtrl->getObject();

	REAL32 top = getY( pUICtrl->getPos());

	VEC3D pos;
	
	for( INT32 i = nBase-1; i >= 0; --i)
	{
		pCtrl = ItemList[i];
		pUICtrl = (i3UIControl*)pCtrl->getObject();

		i3Vector::Copy( &pos, pUICtrl->getPos());

		pos.y = top;

		pUICtrl->setPos( &pos);

		pCtrl->RecalcLayout();
	}
}

void Ci3UIEditorView::AlignBottom(const i3::vector<CUIGripCtrl*>& ItemList)
{
	//Add To UndoSystem
	{
		i3::vector<i3UIControl*> list;
		for( size_t i=0; i< ItemList.size(); i++)
		{
			CUIGripCtrl* pGripCtrl = ItemList[i];
			i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();
			
			list.push_back( pControl);
		}

		UIUndo::ADD_Transform( list);
	}

	INT32 nBase = (INT32)ItemList.size()-1;
	CUIGripCtrl * pCtrl		= ItemList[nBase];
	i3UIControl * pUICtrl	= (i3UIControl*)pCtrl->getObject();

	REAL32 bottom = getY( pUICtrl->getPos()) + pUICtrl->getHeight();

	VEC3D pos;
	
	for( INT32 i = nBase-1; i >= 0; --i)
	{
		pCtrl = ItemList[i];
		pUICtrl = (i3UIControl*)pCtrl->getObject();

		i3Vector::Copy( &pos, pUICtrl->getPos());

		pos.y = bottom - pUICtrl->getHeight();

		pUICtrl->setPos( &pos);

		pCtrl->RecalcLayout();
	}
}

void Ci3UIEditorView::AlignCenter(const i3::vector<CUIGripCtrl*>& ItemList)
{
	//Add To UndoSystem
	{
		i3::vector<i3UIControl*> list;
		for( size_t i=0; i< ItemList.size(); i++)
		{
			CUIGripCtrl* pGripCtrl =ItemList[i];
			i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();

			list.push_back( pControl);
		}

		UIUndo::ADD_Transform( list);
	}

	INT32 nBase = (INT32)ItemList.size()-1;
	for( INT32 i = nBase; i >= 0; --i)
	{
		CUIGripCtrl * pCtrl = ItemList[i];
		i3UIControl * pUICtrl = (i3UIControl*)pCtrl->getObject();

		i3UIControl * pParentCtrl = (i3UIControl*)pUICtrl->getParent();
		
		VEC3D pos, posParent;
		if( NULL == pParentCtrl->getParent() )
		{
			i3UIManager * pUI = i3UI::getManager();
			I3ASSERT( NULL != pUI );
			INT32 width = pUI->getWidth();
			INT32 height = pUI->getHeight();
			
			pos.x = (width*0.5f) - (pUICtrl->getWidth()*0.5f) ;
			pos.y = (height*0.5f) - (pUICtrl->getHeight()*0.5f);
		}
		else
		{
			i3Vector::Copy( &pos, pUICtrl->getPos());
			i3Vector::Copy( &posParent, pParentCtrl->getPos());

			pos.x = (pParentCtrl->getWidth()*0.5f) - (pUICtrl->getWidth()*0.5f) ;
			pos.y = (pParentCtrl->getHeight()*0.5f) - (pUICtrl->getHeight()*0.5f);
		}

		pUICtrl->setPos( &pos);
		pCtrl->RecalcLayout();
	}
}

void Ci3UIEditorView::CascadeHorz(const i3::vector<CUIGripCtrl*>& ItemList)
{
	UINT32 count;
	VEC3D vmin, vmax;
	i3::vector<CUIGripCtrl *> tempList;

	if( ItemList.size() <= 2)
		return;

	count = ItemList.size();

	//Add To UndoSystem
	{
		i3::vector<i3UIControl*> list;

		for( size_t i=0; i < count; i++)
		{
			CUIGripCtrl* pGripCtrl =ItemList[i];
			i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();

			list.push_back( pControl);
			tempList.push_back( pGripCtrl);
		}

		UIUndo::ADD_Transform( list);
	}

	// 선택된 Ctrl들을 x 좌표별로 Sorting한다.
	for( UINT32 i = 0; i < count - 1; i++)
	{
		for( UINT32 j = i+1; j < count; j++)
		{
			CUIGripCtrl * pGrip0 = tempList[i];
			CUIGripCtrl * pGrip1 = tempList[j];

			i3UIControl * pCtrl0 = (i3UIControl *) pGrip0->getObject();
			i3UIControl * pCtrl1 = (i3UIControl *) pGrip1->getObject();

			const VEC3D * p0 = pCtrl0->getPos();
			const VEC3D * p1 = pCtrl1->getPos();

			if( p0->x > p1->x)
			{
				tempList[j] = pGrip0;
				tempList[i] = pGrip1;
			}
		}
	}

	// 전체 영역을 구한다.
	{
		CUIGripCtrl * pGrip = tempList[0];
		i3UIControl * pCtrl = (i3UIControl *) pGrip->getObject();		// 첫번째 Control

		vmin = * pCtrl->getPos();

		pGrip = tempList[ count-1];
		pCtrl = (i3UIControl *) pGrip->getObject(); // 마지막 Control

		vmax = * pCtrl->getPos();
	}


	// 영역을 고루 배분한다.
	REAL32 step = (vmax.x - vmin.x) / (count - 1);

	for(UINT32 i = 1; i < count-1; i++)
	{
		CUIGripCtrl * pGrip = tempList[i];
		i3UIControl * pCtrl = (i3UIControl *) pGrip->getObject();

		VEC3D pos = * pCtrl->getPos();

		pos.x = vmin.x + (step * i);

		pCtrl->setPos( &pos);
		pGrip->RecalcLayout();
	}
}

void Ci3UIEditorView::CascadeVert(const i3::vector<CUIGripCtrl*>& ItemList)
{
	UINT32 count;
	VEC3D vmin, vmax;
	i3::vector<CUIGripCtrl *> tempList;

	if( ItemList.size() <= 2)
		return;

	count = ItemList.size();

	//Add To UndoSystem
	{
		i3::vector<i3UIControl*> list;

		for( size_t i=0; i < count; i++)
		{
			CUIGripCtrl* pGripCtrl =ItemList[i];
			i3UIControl* pControl = (i3UIControl*)pGripCtrl->getObject();

			list.push_back( pControl);
			tempList.push_back( pGripCtrl);
		}

		UIUndo::ADD_Transform( list);
	}

	// 선택된 Ctrl들을 x 좌표별로 Sorting한다.
	for(UINT32 i = 0; i < count - 1; i++)
	{
		for( UINT32 j = i+1; j < count; j++)
		{
			CUIGripCtrl * pGrip0 = tempList[i];
			CUIGripCtrl * pGrip1 = tempList[j];

			i3UIControl * pCtrl0 = (i3UIControl *) pGrip0->getObject();
			i3UIControl * pCtrl1 = (i3UIControl *) pGrip1->getObject();

			const VEC3D * p0 = pCtrl0->getPos();
			const VEC3D * p1 = pCtrl1->getPos();

			if( p0->y < p1->y)
			{
				tempList[j] = pGrip0;
				tempList[i] = pGrip1;
			}
		}
	}

	// 전체 영역을 구한다.
	{
		CUIGripCtrl * pGrip = tempList[0];
		i3UIControl * pCtrl = (i3UIControl *) pGrip->getObject();		// 첫번째 Control

		vmin = * pCtrl->getPos();

		pGrip = tempList[ count-1];
		pCtrl = (i3UIControl *) pGrip->getObject(); // 마지막 Control

		vmax = * pCtrl->getPos();
	}


	// 영역을 고루 배분한다.
	REAL32 step = (vmax.y - vmin.y) / (count - 1);

	for( UINT32 i = 1; i < count-1; i++)
	{
		CUIGripCtrl * pGrip = tempList[i];
		i3UIControl * pCtrl = (i3UIControl *) pGrip->getObject();

		VEC3D pos = * pCtrl->getPos();

		pos.y = vmin.y + (step * i);

		pCtrl->setPos( &pos);
		pGrip->RecalcLayout();
	}
}

void Ci3UIEditorView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	m_bLBDragging = false;
}
// pt가 pGameObj내부에 있다면 return true( 단, pGameObj은 i3UIListView_Box* 일 경우)
bool Ci3UIEditorView::_isInsideEmptyListViewBox( i3GameObj* pGameObj, CPoint pt, bool bItem)
{
	if( i3::kind_of<i3UIListView_Box*>(pGameObj)) //->IsTypeOf( i3UIListView_Box::static_meta()))
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)pGameObj;
		if( pBox->isEnable() && _IsptInImage( pt, pBox))
		{
			if( bItem)	// item
			{
				if( pBox->getBoundItem() == NULL)
					return true;
			}
			else	// header
			{
				if( pBox->getHeader() == NULL)
					return true;
			}
		}
	}
		
	return false;
}

i3UIListView_Box* Ci3UIEditorView::_recFindEmptyListViewBox( i3GameObj* pRoot, CPoint pt, bool bItem)
{
	if( _isInsideEmptyListViewBox( pRoot, pt, bItem))
		return (i3UIListView_Box*)pRoot;

	i3GameObj* pChild = (i3GameObj*)pRoot->getFirstChild();
	while( pChild != NULL)
	{
		i3UIListView_Box* pBox = _recFindEmptyListViewBox( pChild, pt, bItem);
		
		if( pBox != NULL)
			return pBox;
		
		pChild = (i3GameObj*)pChild->getNext();
	}

	return NULL;
}

i3UIListView_Box* Ci3UIEditorView::_getOwnerListViewBox( POINT scrPT, bool bItem)
{
	if( getScene() == NULL)
		return NULL;

	i3GameObj * pChild = (i3GameObj*)getScene()->getFirstChild();
	while( pChild != NULL)
	{
		i3UIListView_Box* pBox = _recFindEmptyListViewBox( pChild, scrPT, bItem);

		if( pBox != NULL)
			return pBox;

		pChild = (i3GameObj*)pChild->getNext();
	}

	return NULL;
}

INT32 Ci3UIEditorView::getSelectedObjCount( void)
{
	return m_pGripManager.getSelectedItemCount();
}

void Ci3UIEditorView::getSelectedObjList( i3::vector<i3UIControl*>& Out)
{
	m_pGripManager.getSelectedObjList( Out);
}

void Ci3UIEditorView::SetSnapSize( INT32 size)
{
	m_pGripManager.SetSnapSize( size);
}

INT32 Ci3UIEditorView::GetSnapSize( void)
{
	return m_pGripManager.GetSnapSize();
}

void Ci3UIEditorView::SetupScroll( void)
{
	SCROLLINFO info;
	CRect rt;

	GetClientRect( &rt);

	// Horizonal Scroll
	info.cbSize = sizeof(info);
	info.fMask	= SIF_PAGE | SIF_RANGE;
	info.nMin	= 0;
	info.nMax	= (INT32)((g_rtTarget.right - g_rtTarget.left) * g_fZoom);
	info.nPage	= rt.Width();
	
	::SetScrollInfo( m_hWnd, SB_HORZ, &info, TRUE);

	// Vertical Scroll
	info.nMax	= (INT32)((g_rtTarget.bottom - g_rtTarget.top) * g_fZoom);
	info.nPage	= rt.Height();

	::SetScrollInfo( m_hWnd, SB_VERT, &info, TRUE);
}

void Ci3UIEditorView::UpdateScroll( bool bVert, UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO info;
	info.cbSize	= sizeof(info);
	info.fMask	= SIF_POS | SIF_TRACKPOS | SIF_PAGE | SIF_RANGE;
	INT32 nBar = bVert ? SB_VERT : SB_HORZ;		// bHorz or bVert
	::GetScrollInfo( m_hWnd, nBar, &info);

	INT32 nCurPos = info.nPos;

	switch( nSBCode)
	{
		case SB_TOP :			nCurPos = 0;			break;
		case SB_BOTTOM :		nCurPos = info.nMax;	break;
		case SB_LINEDOWN :		nCurPos ++;				break;
		case SB_LINEUP :		nCurPos --;				break;
		case SB_PAGEDOWN :		nCurPos += info.nPage;	break;
		case SB_PAGEUP :		nCurPos -= info.nPage;	break;
		case SB_THUMBPOSITION :
		case SB_THUMBTRACK :	nCurPos = nPos;			break;
	}

	info.nPos = nCurPos;
	info.nTrackPos = nCurPos;
	::SetScrollInfo( m_hWnd, nBar, &info, TRUE);

	// min, max 가 반영된 값을 다시 얻어옴
	AdjustShowXY();
}

void Ci3UIEditorView::AdjustShowXY( void)
{
	SCROLLINFO info;
	info.cbSize = sizeof( SCROLLINFO);
	info.fMask = SIF_POS;
	::GetScrollInfo( m_hWnd, SB_VERT, &info);
	m_ScrollY = info.nPos;
	m_ShowY = (INT32)(info.nPos / g_fZoom);

	::GetScrollInfo( m_hWnd, SB_HORZ, &info);
	m_ScrollX = info.nPos;
	m_ShowX = (INT32)(info.nPos / g_fZoom);

	Invalidate(false);
}

void Ci3UIEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	UpdateScroll( true, nSBCode, nPos, pScrollBar);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void Ci3UIEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	UpdateScroll( false, nSBCode, nPos, pScrollBar);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void Ci3UIEditorView::OnMButtonDown( UINT nFlags, CPoint point)
{
	SetFocus();

	SetCapture();

	m_MouseMode = MOUSEMODE_SCROLL;

	m_ScrollStart = point;

	HCURSOR hCursor = ::LoadCursor( NULL,IDC_HAND);

	::SetCursor( hCursor);
	::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);

	CView::OnMButtonDown( nFlags, point);
}

void Ci3UIEditorView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_MouseMode = MOUSEMODE_NONE;

	ReleaseCapture();

	HCURSOR hCursor = ::LoadCursor( NULL,IDC_ARROW);

	::SetCursor( hCursor);
	::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);

	CView::OnMButtonUp(nFlags, point);
}

BOOL Ci3UIEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	
	if( (GetKeyState( VK_CONTROL) & 0x8000) != 0)
	{
		//Ctrl을 누른 상태에선 확대/축소
		REAL32 mag = zDelta > 0 ? 0.25f : -0.25f;
			
		g_fZoom = MINMAX( 0.5f, g_fZoom + mag, 2.0f);

		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
		pMainFrm->UpdateStatusBar( true);

		SetupScroll();

		AdjustShowXY();
		AdjustShowXY();

		::InvalidateRect( m_hWnd, NULL, true);
	}
	else 
	{
		INT32 nScrollAmount = zDelta > 0 ? -5 : 5;
		nScrollAmount = (INT32)(nScrollAmount * g_fZoom);

		if( (GetKeyState( VK_SHIFT) & 0x8000) != 0)
		{
			//Shift를 누른 상태에선 수평스크롤
			OnHScroll( SB_THUMBTRACK, m_ScrollX + nScrollAmount, NULL);
		}
		else	
		{
			//수직스크롤
			OnVScroll( SB_THUMBTRACK, m_ScrollY + nScrollAmount, NULL);
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

REAL32 Ci3UIEditorView::getZoomRatio( void)
{
	return g_fZoom;
}


void Ci3UIEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	
	SetupScroll();
}

//마우스 커서의 좌표를 "Window에 대한 상대좌표"에서 "Zoom을 고려한 상대좌표"로 변환
CPoint Ci3UIEditorView::_AbsToRel( CPoint ptAbs)
{
	CPoint ptRel;
	
	ptRel.x = m_ShowX + (INT32)(ptAbs.x / g_fZoom);
	ptRel.y = m_ShowY + (INT32)(ptAbs.y / g_fZoom);

	return ptRel;
}

void _ArrangeList( i3::vector<i3UIControl*>& List)
{
	bool* pFlag = (bool*)i3MemAlloc( sizeof( bool) * List.size());

	size_t i=0;
	while( List.size() > i)
	{
		bool bHasNoParent = true;
		i3UIControl* pSrc = List[i];
		size_t j = 0;

		while( List.size() > j)
		{
			i3UIControl* pDest = List[j];
			
			if( i == j)
			{
				j++;
				continue;
			}
			
			if( pDest == pSrc->getParent())
			{
				bHasNoParent = false;
				break;
			}

			j++;
		}

		if( bHasNoParent)	//부모가 없다면
			pFlag[i] = true;
		else
			pFlag[i] = false;
		
		i++;
	}

	i3::vector<i3UIControl*> tempList;

	for( size_t j=0; j< List.size(); j++)
	{
		i3UIControl* pControl = List[j];
		if( pFlag[j] == true)
			tempList.push_back( pControl);
	}

	i3::swap( List, tempList);
/*
	pList->Clear();

	for( INT32 i=0; i< tempList.GetCount(); i++)
	{
		i3UIControl* pControl = (i3UIControl*)tempList.GetItem( i);
		pList->Add( pControl);
	}
*/
	I3MEM_SAFE_FREE( pFlag);
}

//부모와 자식을 동시에 선택한 상태에서 복사하려는 경우
//부모만 m_CopiedControls에 넣는다.
void Ci3UIEditorView::_ClearCopiedControlList(void)
{
	for( size_t i=0; i< m_CopiedControls.size(); i++)
	{
		CONTROL_COPY_INFO* pCopyInfo = m_CopiedControls[i];

		I3_SAFE_DELETE(pCopyInfo->m_pInfo);
		I3MEM_SAFE_FREE( pCopyInfo);
	}
	m_CopiedControls.clear();
}

void Ci3UIEditorView::OnCopy( void)
{
	_ClearCopiedControlList();

	const i3::vector<CUIGripCtrl*>& ItemList = m_pGripManager.getSelectedItemList();

	i3::vector<i3UIControl*> list;

	for( size_t i=0; i< ItemList.size(); i++)
	{
		CUIGripCtrl* pCtrl = ItemList[i];
		i3UIControl*	pUICtrl = (i3UIControl*)pCtrl->getObject();

		if( pUICtrl->hasName() == true
			&& (pUICtrl->getState() & I3UI_STYLE_SLAVE) == 0
			&& (pUICtrl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
		{
			list.push_back( pUICtrl);	
		}
	}

	if( list.size() > 0)
	{
		_ArrangeList( list);

		for( size_t i=0; i< list.size(); i++)
		{
			i3UIControl* pControl = list[i];

			UI_CONTROL_INFO* pInfo = new UI_CONTROL_INFO; //(UI_CONTROL_INFO*)i3MemAlloc( sizeof( UI_CONTROL_INFO));

			i3UI::FillControlInfo( pInfo, pControl);

			CONTROL_COPY_INFO* pCopyInfo = (CONTROL_COPY_INFO*)i3MemAlloc( sizeof( CONTROL_COPY_INFO));
			pCopyInfo->m_pControl = pControl;
			pCopyInfo->m_pInfo = pInfo;

			m_CopiedControls.push_back( pCopyInfo);
		}
	}
}

void Ci3UIEditorView::OnCut( void)
{
	OnCopy();

	OnDelete();
}

void Ci3UIEditorView::OnPaste( void)
{
	if( m_CopiedControls.size() < 1)
		return;

	UnSelectAll();

	i3::vector<i3UIControl*> list;

	for( size_t i=0; i< m_CopiedControls.size(); i++)
	{
		CONTROL_COPY_INFO* pCopyInfo = m_CopiedControls[i];

		
		UI_CONTROL_INFO* pInfo = pCopyInfo->m_pInfo;

		pInfo->m_vPos.x += 15.0f;
		pInfo->m_vPos.y += 15.0f;

		// new version
		i3UIControl* pSrc = pCopyInfo->m_pControl;
		i3GameNode* pParent = pInfo->m_pParent;

		I3ASSERT( pParent != NULL);

		i3UIControl* pDest = i3UI::CreateCloneControl( pSrc, true, pParent);

		I3ASSERT( pDest != NULL);

		pDest->EnableCtrl( true, true);

		pDest->setPos( &pInfo->m_vPos);

		list.push_back( pDest);	//child..
	}

	UIUndo::ADD_Create( list);
}

//void _ArrangeDeleteList( List* pList)
//{
//	//부모와 자식이 모두 삭제 대상이라면
//	//자식이 부모보다 앞에 오도록 재배치한다. 
//	List tempList;
//
//	INT32 i = 0;
//
//	while( pList->GetCount() > i)
//	{
//		bool bHasNoParent = true;
//		i3UIControl* pSrc = (i3UIControl*)pList->GetItem( i);
//		INT32 j = 0;
//
//		while( pList->GetCount() > j)
//		{
//			i3UIControl* pDest = (i3UIControl*)pList->GetItem( j);
//			
//			if( i == j)
//			{
//				j++;
//				continue;
//			}
//			
//			if( pDest == pSrc->getParent())
//			{
//				bHasNoParent = false;
//				break;
//			}
//
//			j++;
//		}
//
//		if( bHasNoParent)	//부모가 없다면
//		{
//			tempList.Insert( 0, pSrc);
//			pList->Delete( i);
//			
//			i = 0;
//			continue;
//		}
//		
//		i++;
//	}
//
//	I3ASSERT( pList->GetCount() == 0);
//
//	for( INT32 i=0; i< tempList.GetCount(); i++)
//	{
//		//tempList->CopyTo( pList);
//		i3UIControl* pControl = (i3UIControl*)tempList.GetItem( i);
//		pList->Add( pControl);
//	}
//}

void Ci3UIEditorView::OnDelete( void)
{
	const i3::vector<CUIGripCtrl*>& ItemList = m_pGripManager.getSelectedItemList();

	if( ItemList.size() < 1)
		return;

	i3::vector<i3UIControl*> list;	//list of i3UIControls

	for( size_t i=0; i< ItemList.size(); i++)
	{
		CUIGripCtrl* pCtrl = ItemList[i];
		i3UIControl*	pUICtrl = (i3UIControl*)pCtrl->getObject();

		//Add to List (delete later)
		list.push_back( pUICtrl);
	}

	// Child가 Parent보다 먼저 지워질 수 있도록 Arrange List
	//_ArrangeDeleteList( &list);

	// list안에 Parent를 가지고 있는 Control이 있다면 list에서 제거한다.
	// 어차피 Parent가 지워지면 자신도 지워지므로..
	_ArrangeList( list);

	//Add Undo Info
	if( list.size() == 1)
		UIUndo::ADD_Delete( list[0]);
	else if( list.size() > 1)
		UIUndo::ADD_Delete( list);

	for( size_t i=0; i< list.size(); i++)
	{
		i3UIControl* pControl = list[i];
		
		i3UI::DeleteControl( pControl);
	}

}

#ifdef _DEBUG
void Ci3UIEditorView::OnControlInfo( i3UIControl* pSelected)
{
	I3ASSERT( pSelected != NULL);

	I3TRACE( L"--------------------------------------------\n");
	
	I3TRACE( L"Name: %s\n", pSelected->GetName());
	
	i3GameNode* pParent = pSelected->getParent();
	if( pParent != NULL)
		I3TRACE( L"Parent: %s\n", pParent->GetName());
	else
		I3TRACE( L"Parent: NULL\n");

	I3TRACE( L"--------------------------------------------\n");
}
#endif

void Ci3UIEditorView::ViewTemplate( void)
{
	const i3::vector<CUIGripCtrl*>& ItemList = m_pGripManager.getSelectedItemList();

	if( ItemList.size() == 1)
	{
		CUIGripCtrl * pCtrl = ItemList[0];
		
		i3UIControl * pUICtrl = (i3UIControl*)pCtrl->getObject();

		CMainFrame * pFrame = (CMainFrame*) ::AfxGetMainWnd();
		pFrame->GotoViewTemplateEdit( pUICtrl->getTemplate());
	}
}
void Ci3UIEditorView::ShowListBox()
{
	const i3::vector<CUIGripCtrl*>& ItemList = m_pGripManager.getSelectedItemList();

	if (ItemList.size() == 1)
	{
		CUIGripCtrl * pCtrl = ItemList[0];

		i3UIControl * pUICtrl = (i3UIControl*)pCtrl->getObject();

		if (i3::kind_of<i3UIComboBase*>(pUICtrl)) //->IsTypeOf( i3UIControl::static_meta()) )
		{
			i3UIComboBase * pControl = (i3UIComboBase*)pUICtrl;
			i3UIListBox* pListBox = pControl->getListBox();
			pListBox->EnableCtrl(!pListBox->isEnable());
		}
	}
}
void Ci3UIEditorView::ShowDummyLine()
{
	const i3::vector<CUIGripCtrl*>& ItemList = m_pGripManager.getSelectedItemList();

	if (ItemList.size() == 1)
	{
		CUIGripCtrl * pCtrl = ItemList[0];

		i3UIControl * pUICtrl = (i3UIControl*)pCtrl->getObject();

		if (i3::kind_of<i3UIListView_Box*>(pUICtrl)) //->IsTypeOf( i3UIControl::static_meta()) )
		{
			i3UIListView_Box * pControl = (i3UIListView_Box*)pUICtrl;
			pControl->SetDummyLine(!pControl->IsDummyLine());
		}
	}
}
