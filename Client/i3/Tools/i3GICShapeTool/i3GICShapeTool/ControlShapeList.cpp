#include "stdafx.h"
#include "ControlShapeList.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CControlShapeList, CListCtrl)

CControlShapeList::CControlShapeList()
{
}

CControlShapeList::~CControlShapeList()
{
	INT32 i;

	for( i = 0; i < m_ShapeList.GetCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*) m_ShapeList.Items[ i];
		I3_SAFE_RELEASE( pShape);
	}
}

const char * CControlShapeList::_GetShapeTypeName( i3GICShape * pShape)
{
	return pShape->getTypeName();
}

INT32 CControlShapeList::_GetIconIndex( i3GICShape * pShape)
{
	INT32 nIdx = -1;

	if( pShape->IsExactTypeOf( i3GICShapeRect::GetClassMeta() ) )
	{
		nIdx = 0;
	}
	else if( pShape->IsExactTypeOf( i3GICShapeLine::GetClassMeta() ) )
	{
		nIdx = 1;
	}
	else if( pShape->IsExactTypeOf( i3GICShapeBezierLine::GetClassMeta()) )
	{
		nIdx = 2;
	}
	else if( pShape->IsExactTypeOf( i3GICShapeRenderTexture::GetClassMeta()) )
	{
		nIdx = 3;
	}

	return nIdx;
}


void CControlShapeList::_SetShapeInfo( LVITEM * pInfo, i3GICShape * pShape)
{
	I3ASSERT( pShape != NULL);

	memset( pInfo, 0, sizeof( LVITEM));

	pInfo->mask				= TVIF_CHILDREN | TVIF_PARAM | LVIF_IMAGE | TVIF_TEXT;
	pInfo->cColumns			= 0;
	pInfo->lParam			= (LPARAM) pShape;

	pInfo->pszText			= (LPSTR) _GetShapeTypeName( pShape);
	pInfo->iImage			= _GetIconIndex( pShape);
}

INT32 CControlShapeList::AddShape( i3GICShape * pShape)
{
	LVITEM itemInfo;

	_SetShapeInfo( &itemInfo, pShape);

	I3_SAFE_ADDREF( pShape);
	m_ShapeList.Add( pShape);

	return InsertItem( &itemInfo);
}

BEGIN_MESSAGE_MAP(CControlShapeList, CListCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CControlShapeList::OnMouseMove( UINT nFlags, CPoint point)
{
	CListCtrl::OnMouseMove( nFlags, point);

	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->MouseMove( point);
}

void CControlShapeList::OnLButtonUp( UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonUp( nFlags, point);

	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
	pMainWnd->LButtonUp( nFlags, point, false);
}