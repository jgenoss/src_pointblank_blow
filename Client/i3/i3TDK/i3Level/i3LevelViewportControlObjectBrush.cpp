#include "stdafx.h"
#include "i3LevelViewportControlObjectBrush.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelStaticObject.h"
#include "../i3TDKGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelViewportControlObjectBrush);

void i3LevelViewportControlObjectBrush::Create(void)
{
	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_BRUSH));
}

void	i3LevelViewportControlObjectBrush::PlaceNewObject( I3_LEVEL_EDIT_CONTEXT * pCtx, BOOL bClone, CPoint point)
{
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelRes * pRes = pFramework->GetSelectedRes();

	if( pRes == nullptr)
		return;

	{
		i3LevelElement3D * pInstance;

		pInstance = pRes->CreateInstance( g_pScene);

		if( pInstance != nullptr)
		{
			VEC3D pos;

			pFramework->GetPickedPos( point, &pos);

			pInstance->setPos( &pos);
		}
	}
}

bool i3LevelViewportControlObjectBrush::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	if( nFlags & MK_SHIFT)
		PlaceNewObject( pCtx, TRUE, point);
	else
		PlaceNewObject( pCtx, FALSE, point);

	return false;
}

bool i3LevelViewportControlObjectBrush::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewport * pViewport = pCtx->m_pViewport;

	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta());

	return false;
}


bool i3LevelViewportControlObjectBrush::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{

	//i3Level::GetScene()->Scaling( pCtx->m_DragStartPt, point, &pCtx->m_AxisMask);

	return false;
}
