#include "stdafx.h"
#include "i3LevelDesignFramework.h"

I3_CLASS_INSTANCE( i3LevelDesignFramework, i3ViewerFramework);

i3LevelDesignFramework::i3LevelDesignFramework(void)
{
	/*
	m_hCursorSel = ::LoadCursor( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDC_BANDSELECT));
	m_hCursorMove = ::LoadCursor( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDC_MOVE));
	m_hCursorRotate = ::LoadCursor( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDC_ROTATE));
	m_hCursorScale = ::LoadCursor( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDC_SCALE));
	m_hCursorBrush = ::LoadCursor( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDC_BRUSH));
	*/

	m_pScene = NULL;
}

i3LevelDesignFramework::~i3LevelDesignFramework(void)
{
	I3_SAFE_RELEASE( m_pScene);
	i3Level::SetScene( NULL);
}

void i3LevelDesignFramework::setScene( i3LevelScene * pScene)
{
	if( pScene != NULL)
		pScene->AddRef();

	I3_SAFE_RELEASE( m_pScene);
	m_pScene = pScene;

	SetSceneGraph( m_pScene->GetRoot());

	if( m_pScene != NULL)
	{
		m_pSceneRoot->AddChild( m_pScene->GetRoot());
		m_pScene->AddRef();

		{
			VEC3D * pPos;

			pPos = m_pScene->GetStartPosition();

			m_pWalkCamera->SetPos( i3Vector::GetX(pPos), i3Vector::GetY(pPos) + 2.0f, i3Vector::GetZ(pPos));
			m_pWalkCamera->SetRotate( 0.0f, m_pScene->GetStartRotation(), 0.0f);
			m_pWalkCamera->SetNear( m_pScene->GetCameraNear());
			m_pWalkCamera->SetFar( m_pScene->GetCameraFar());
		}

		m_pScene->AddElement( m_pWalkCamera);
	}

	SetCurrentCamera( m_pCurCamera);

	i3Level::SetScene( pScene);
}

void i3LevelDesignFramework::SetMouseCursor( HCURSOR hCursor)
{
	::SetClassLong( GetViewer()->GetWindowHandle(), GCL_HCURSOR, (LONG) hCursor);
	::SetCursor( hCursor);
}

void i3LevelDesignFramework::SetCurrentCamera( i3LevelCamera * pCam)
{
	if( m_pCurCamera != NULL)
	{
		m_pCurCamera->Release();
		m_pCurCamera = NULL;
	}

	m_pCurCamera = pCam;

	if( m_pCurCamera != NULL)
	{
		m_pCurCamera->AddRef();

		m_pCurCamera->SetViewport( this);
	}
}

void i3LevelDesignFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	{
		i3LevelScene * pScene;

		pScene = i3LevelScene::NewObjectRef();
		pScene->Create();
		
		setScene( pScene);
	}

}
