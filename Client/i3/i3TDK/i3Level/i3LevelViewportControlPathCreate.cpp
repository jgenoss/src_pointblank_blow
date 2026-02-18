#include "stdafx.h"
#include "i3LevelViewportControlPathCreate.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelStaticObject.h"
#include "../i3TDKGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelViewportControlPathCreate);

i3LevelViewportControlPathCreate::~i3LevelViewportControlPathCreate(void)
{
	I3_SAFE_RELEASE( m_pDragLink);
}

void i3LevelViewportControlPathCreate::Create(void)
{
	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_PATH_EDIT));

	m_pDragLink = i3Billboard::new_object();

	m_pDragLink->SetCount( 1);
	m_pDragLink->SetEnable( 0, TRUE);
	m_pDragLink->SetSize( 0, 0.2f);
	m_pDragLink->SetSrcBlend( I3G_BLEND_SRCALPHA);
	m_pDragLink->SetDestBlend( I3G_BLEND_INVSRCALPHA);
	m_pDragLink->SetColor( 0, 255, 255, 0, 128);
}

bool	i3LevelViewportControlPathCreate::OnStart( I3_LEVEL_EDIT_CONTEXT * pCtx)
{
	i3::vector<i3LevelElement3D*> list;

	i3LevelElement3D * pElm;

	m_bDragging = false;
	m_pStartLink = nullptr;

	// 현재 선택된 녀석들 중에 i3LevelPath가 있는지 확인한다.
	g_pScene->GetSelectedElements( list);

	for( size_t i = 0; i < list.size(); i++)
	{
		pElm = list[i];

		if( i3::kind_of<i3LevelPath*>(pElm))
		{
			m_pPath = (i3LevelPath *) pElm;
			return true;
		}
	}

	list.clear();

	g_pScene->FindElementByType( list, i3LevelPath::static_meta());

	if( list.size() >= 1)
	{
		m_pPath = (i3LevelPath *) list[0];

		return true;
	}
	
	return false;
}

bool i3LevelViewportControlPathCreate::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelViewportControl::OnLButtonDown( pCtx, nFlags, point );

	if( m_pPath == nullptr)
		return true;

	VEC3D pos;

	pFramework->GetPickedPos( point, &pos);

	if( nFlags & MK_SHIFT)
	{
		m_pPath->AddPoint( &pos);
	}
	else
	{
		i3LevelElement3D * pPicked = g_pScene->GetPickedElement( point, true, SEL_EXCLUSIVE);

		m_pStartLink = nullptr;
		m_pLastLink = nullptr;

		if(  i3::kind_of<i3LevelPathPoint* >(pPicked))
		{
			i3LevelPathPoint * pIcon = (i3LevelPathPoint *) pPicked;

			if((pIcon->GetOwner() != nullptr) && (pIcon->GetOwner() == m_pPath))
			{
				pIcon->Targeted();

				m_pStartLink = pIcon;
			}
		}

		{
			pFramework->Get3DRoot()->AddChild( m_pDragLink);

			m_pDragLink->SetStart( 0, &pos);
			m_pDragLink->SetEnd( 0, &pos);
		}

		::SetCapture( pViewport->m_hWnd);

		m_bDragging = true;
	}

	return false;
}

bool i3LevelViewportControlPathCreate::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonUp( pCtx, nFlags, point );

//	i3LevelViewport * pViewport = pCtx->m_pViewport;

	if( m_bDragging)
	{
		m_bDragging = false;
		::ReleaseCapture();

		m_pDragLink->RemoveFromParent();

		// 해당 위치에 놓인 Path Point를 찾는다.
		if( m_pStartLink != nullptr)
		{
			i3LevelElement3D * pPicked = g_pScene->GetPickedElement( point, true, SEL_EXCLUSIVE);

			if( i3::kind_of<i3LevelPathPoint*>(pPicked))
			{
				i3LevelPathPoint * pIcon = (i3LevelPathPoint *) pPicked;

				if( pIcon == m_pStartLink)
					return true;

				if((pIcon->GetOwner() != nullptr) && (pIcon->GetOwner() == m_pPath))
				{
					i3LevelPathLink * pLink = m_pPath->FindLinkByPoint( m_pStartLink, pIcon);

					if( pLink == nullptr)
					{

						m_pPath->AddLine( m_pStartLink, pIcon);
					}
				}
			}

			m_pStartLink->Untargeted();

			if( m_pLastLink != nullptr)
			{
				m_pLastLink->Untargeted();
				m_pLastLink = nullptr;
			}
		}
	}

	return false;
}

bool i3LevelViewportControlPathCreate::OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnRButtonDown( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;
	pViewport->Invalidate();

	return true;
}

bool i3LevelViewportControlPathCreate::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnMouseMove( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;

	if( m_bDragging)
	{
		i3LevelFramework * pFramework = pCtx->m_pFramework;

		VEC3D pos;

		pFramework->GetPickedPos( point, &pos);

		m_pDragLink->SetEnd( 0, &pos);


		{
			i3LevelElement3D * pPicked = g_pScene->GetPickedElement( point, true, SEL_EXCLUSIVE);

			if(  i3::kind_of<i3LevelPathPoint* >(pPicked))
			{
				i3LevelPathPoint * pIcon = (i3LevelPathPoint *) pPicked;

				if( pIcon != m_pStartLink)
				{

					if( m_pLastLink != nullptr)
						m_pLastLink->Untargeted();

					m_pLastLink = pIcon;
					m_pLastLink->Targeted();
				}
			}
		}
	}

	pViewport->Invalidate();

	return false;
}

