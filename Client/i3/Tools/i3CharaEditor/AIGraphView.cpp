// AIGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "AIGraphView.h"
#include "GICShapeAI.h"
#include "DlgRenameFolder.h"
#include "AI.h"

// CAIGraphView

IMPLEMENT_DYNAMIC(CAIGraphView, i3GICDiagramView)
CAIGraphView::CAIGraphView()
{
	m_pAI = NULL;

	m_pGDC = NULL;
}

CAIGraphView::~CAIGraphView()
{
	I3_SAFE_RELEASE( m_pGDC);
}

void CAIGraphView::SetAI( AI * pAI)
{
	if( m_pAI != NULL)
	{
		m_pAI->OnSelect( false);
		m_pAI->getDiagram()->RemoveAllView();
	}

	m_pAI = pAI;

	if( m_pAI != NULL)
	{
		setDiagram( m_pAI->getDiagram());
		m_pAI->getDiagram()->AddView( this);
		m_pAI->OnSelect( true);
	}
	else
	{
		setDiagram( NULL);
	}

	

	Invalidate();
}

bool	CAIGraphView::OnCreateShape( i3GICShape * pShape)
{
	if( i3::kind_of<GICShapeAI*>(pShape)) //->IsTypeOf( GICShapeAI::static_meta()))
	{
		i3AIState * pState = i3AIState::new_object_ref();
		GICShapeAI * pShapeAI = (GICShapeAI *) pShape;

		pShapeAI->setAIState( pState);
		
		m_pAI->getAI()->AddAIState( pState);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pState);
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pAI);
	}

	return i3GICDiagramView::OnCreateShape( pShape);
}

void	CAIGraphView::OnSelectShape( i3GICShape * pShape, GIC_MOUSEMODE mode)
{
	if( m_pAI != NULL)
	{
		GICShapeAI * pState = (GICShapeAI *) pShape;

		m_pAI->getAIContext()->removeGNodeState( I3_GAMENODE_STATE_LOCKED);
		m_pAI->getAIContext()->setCurrentAIState( pState->getAIState());
		//m_pAI->getAIContext()->OnChangeTime( 0.0f);
		m_pAI->getAIContext()->addGNodeState( I3_GAMENODE_STATE_LOCKED);
		m_pAI->OnSelect( true);
	}
}

void	CAIGraphView::OnBeforePasteFromClipboard( i3::vector<i3PersistantElement*>& List)
{
	GICShapeAI * pAIShape;
	i3AI * pAI;

	pAI = m_pAI->getAI();

	i3::vector<i3PersistantElement*> temp;

	for(size_t i = 0; i < List.size(); i++)
	{
		if(i3::same_of<GICShapeAI*>(List[i]))
		{
			pAIShape = (GICShapeAI *) List[i];
			pAI->AddAIState( pAIShape->getAIState());

			temp.push_back(List[i]);
		}
	}
	List = temp;
}

BEGIN_MESSAGE_MAP(CAIGraphView, i3GICDiagramView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


int CAIGraphView::OnCreate( LPCREATESTRUCT lpCreateStruct)
{
	int rv = i3GICDiagramView::OnCreate( lpCreateStruct);

	m_pGDC = i3GICDeviceContext::new_object();
	m_pGDC->setView( this);

	/*HDC dc = ::GetDC( m_hWnd);

	HDC hDrawDC = ::CreateCompatibleDC( dc);
	I3ASSERT( hDrawDC != NULL);

	::ReleaseDC( m_hWnd, dc);*/

	m_pGDC->setDC( ::GetDC( m_hWnd));

	return rv;
}

void CAIGraphView::OnDraw( void)
{
	/*RECT rect;

	rect.left = (INT32) 0;
	rect.top = (INT32)0;
	rect.right = (INT32)m_ClientWidth;
	rect.bottom = (INT32)m_ClientHeight;*/

	_DrawShapes( NULL);
}

// CAIGraphView message handlers
void CAIGraphView::OnRButtonUpProcess( UINT nFlags, CPoint point)
{
	CMenu menu;
	CPoint pt;
	UINT32 cmd;

	menu.CreatePopupMenu();

	menu.AppendMenu( MF_STRING, 50, "&Select all...");
	menu.AppendMenu( MF_STRING, 51, "&Unselect all...");
	menu.AppendMenu( MF_SEPARATOR, 0, "");
	/*menu.AppendMenu( MF_STRING, 53, "&Copy");
	menu.AppendMenu( MF_STRING, 54, "&Paste");*/
	menu.AppendMenu( MF_STRING, 52, "&Delete");
	menu.AppendMenu( MF_STRING, 55, "&RenameAnim");

	OnQueryPopupMenu( &menu);

	GetCursorPos( &pt);

	cmd = menu.TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	if( cmd != 0)
	{
		switch( cmd)
		{
			case 50 :		// Select all
				if( getDiagram() != NULL)
					getDiagram()->SelectAllShapes();
				break;

			case 51 :		// Unselect all
				if( getDiagram() != NULL)
					getDiagram()->UnselectAllShapes();
				break;

			case 52 :		// Delete
				if( getDiagram() != NULL)
					getDiagram()->RemoveSelectedShapes();
				break;

			case 53 :		// Copy
				/*if( getDiagram() != NULL)
					getDiagram()->CopyToClipboard( this);*/
				break;

			case 54 :		// Paste
				/*if( getDiagram() != NULL)
					getDiagram()->PasteFromClipboard( this);*/
				break;

			case 55 :
				{
					CDlgRenameFolder dlg;
					dlg.setAI( m_pAI);
					dlg.setRenameType( RENAME_TYPE_AUTOAISTATEANIM);
					dlg.DoModal();
				}
				break;
		}

		OnSelectPopupMenu( cmd);
	}
}
