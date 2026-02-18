#include "stdafx.h"
#include "i3LevelViewport.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelStaticObject.h"
#include "../resource.h"
#include "i3LevelElementList.h"
#include "i3LevelFramework.h"

IMPLEMENT_DYNCREATE(i3LevelViewport, i3TDKViewerCtrl)

static i3LevelViewport * s_pCurViewport = nullptr;

i3LevelViewport::i3LevelViewport(void)
{
	s_pCurViewport = this;
}

i3LevelViewport::~i3LevelViewport(void)
{
	s_pCurViewport = nullptr;
}

i3LevelViewport * i3LevelViewport::GetCurrentFocusViewport(void)
{
	return s_pCurViewport;
}

VEC3D i3LevelViewport::GetScreenPos( const VEC3D & wsPos)
{
	VEC3D pos;
	MATRIX mtx;

	m_pFramework->GetViewProjectionMatrix( &mtx);

	i3Vector::TransformCoord( &pos, &wsPos, &mtx);

	pos.x = ((pos.x * 0.5f) + 0.5f) * m_pViewer->GetViewWidth();
	pos.y = ((pos.y * -0.5f) + 0.5f) * m_pViewer->GetViewHeight();

	return pos;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//

void i3LevelViewport::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	i3LevelFramework * pFramework = (i3LevelFramework *) getViewer()->GetFramework();

	pFramework->OnUpdateEvent( pInfo);

	Invalidate();
}

BEGIN_MESSAGE_MAP(i3LevelViewport, i3TDKViewerCtrl)
	ON_WM_DESTROY()					// 이런게 필요함.(2012.09.13.수빈)
END_MESSAGE_MAP()

BOOL i3LevelViewport::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID, i3LevelScene * pScene)
{
	i3TDKViewerCtrl::Create( dwStyle, rect, pParentWnd, nID);

	{
		m_pFramework = i3LevelFramework::new_object();

		m_pFramework->setViewport( this);
		m_pFramework->Create( getViewer());
		m_pFramework->SetScene( pScene);
	}

	i3Level::RegisterUpdateNotify( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_ALL);
	i3Level::RegisterUpdateNotify( m_hWnd, i3LevelRes::static_meta(), I3_TDK_UPDATE_REMOVE | I3_TDK_UPDATE_SELECT);
	i3Level::RegisterUpdateNotify( m_hWnd, i3LevelScene::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;
}

void i3LevelViewport::OnDestroy()
{
	i3LevelViewport::Cleanup();

	i3TDKViewerCtrl::OnDestroy();	
}

void	i3LevelViewport::Cleanup()
{
	I3_SAFE_RELEASE( m_pFramework);			// 소멸자에서 이쪽으로 이동..소멸자엔 넣지 말것..(호출안되면 그냥 릭을 보고하는게 낫다..)
}											// 대신에, OnDestroy함수를 사용하며, i3Viewer보다 일찍 삭제되도록 유도..2012.09.13.수빈.

// CDummyWnd message handlers

LRESULT i3LevelViewport::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
	case WM_TDK_UPDATE :
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);	break;
	}

	return i3TDKViewerCtrl::WindowProc(message, wParam, lParam);
}

void i3LevelViewport::OnSave( i3::pack::Level_VIEWPORT * pInfo)
{
	memset( pInfo, 0, sizeof(i3::pack::Level_VIEWPORT));

	memcpy( pInfo->m_magic, "TVI1", 4);

	pInfo->m_ID = ::GetWindowLong( m_hWnd, GWL_ID);

	m_pFramework->OnSaveInfo( pInfo);
}

void i3LevelViewport::OnLoad( i3::pack::Level_VIEWPORT * pInfo)
{
	m_pFramework->OnLoadInfo( pInfo);
}
