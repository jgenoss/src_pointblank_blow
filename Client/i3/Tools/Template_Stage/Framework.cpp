#include "stdafx.h"
#include "Framework.h"
#include "GlobalVar.h"

I3_CONCRETE_CLASS_INSTANCE( CFramework, i3Framework);

CFramework::CFramework(void)
{
	m_bCamDrag = false;

	m_pInfoText = NULL;
	m_timeInfo = 0.0f;
}

CFramework::~CFramework(void)
{
	I3_SAFE_RELEASE( m_pInfoText);
}

void CFramework::OnCreate(void)
{
	i3Framework::OnCreate();

	m_pInfoText = i3TextNodeDX2::NewObject();
	m_pInfoText->Create( "±¼¸²Ã¼", 256, 9, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE);
	m_pInfoText->setAutoWrapEnable( TRUE);			
	m_pInfoText->DeactivateNode();
	m_pInfoText->SetText( "Information");
	m_pInfoText->setPos( 5, 155);
	m_pInfoText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
	m_pInfoText->SetColor(255, 255, 255, 255);				

	Get2DRoot()->AddChild( m_pInfoText);
}

void	CFramework::OnSize( POINT2D * pSize)
{
	i3Framework::OnSize( pSize);

	m_pInfoText->setPos( 5, pSize->y - 20);
	m_pInfoText->setSize( pSize->x - 180, 20);
}

void	CFramework::OnChangeScene(void)
{
	i3Framework::OnChangeScene();

	/*
	if( GetLoadedScene() != NULL)
	{
		GetLoadedScene()->Instantiate();

		i3Scene::ModifyDynamicState( GetLoadedScene(), I3_PHYSIX_STATE_KINEMATIC, 0);
	}

	if( m_szFileName[0] != 0)
	{
		//g_pConfig->setLastI3S( m_szFileName);

		//g_pConfig->Save();
	}
	*/
}

void	CFramework::_SetInfo( const char * pszText)
{
	m_timeInfo = 2.0f;

	m_pInfoText->SetText( (char *) pszText);
	m_pInfoText->ActivateNode();
}

void	CFramework::OnUpdate( REAL32 tm)
{
	i3Framework::OnUpdate( tm);

	if( m_timeInfo > 0.0f)
	{
		m_timeInfo -= tm;

		if( m_timeInfo <= 0.0f)
		{
			m_pInfoText->DeactivateNode();
			m_timeInfo = 0.0f;
		}
	}
}

#define		SPEED			5.0f

void	CFramework::_MoveToAt( REAL32 speed, REAL32 tm)
{
	i3Camera * pCam = GetDefaultCamera();
	MATRIX * pCamMatrix = pCam->GetMatrix();
	
	VEC3D dir;

	i3Vector::Normalize( &dir, i3Matrix::GetAt( pCamMatrix));

	i3Vector::Scale( &dir, &dir, speed * tm);

	i3Matrix::PostTranslate( pCamMatrix, &dir);
}

void	CFramework::_MoveToRight( REAL32 speed, REAL32 tm)
{
	i3Camera * pCam = GetDefaultCamera();
	MATRIX * pCamMatrix = pCam->GetMatrix();
	
	VEC3D dir;

	i3Vector::Normalize( &dir, i3Matrix::GetRight( pCamMatrix));

	i3Vector::Scale( &dir, &dir, speed * tm);

	i3Matrix::PostTranslate( pCamMatrix, &dir);
}

void CFramework::OnInput(void)
{
	REAL32 tm = GetViewer()->getDeltaTime();
	i3InputDeviceManager * pManager = GetViewer()->GetInputDeviceManager();
	i3InputKeyboard * pKbd = pManager->GetKeyboard();

	if( pKbd->GetPressState( I3I_KEY_W))
	{
		_MoveToAt( -SPEED, tm);
	}
	
	if( pKbd->GetPressState( I3I_KEY_S))
	{
		_MoveToAt( SPEED, tm);
	}

	if( pKbd->GetPressState( I3I_KEY_A))
	{
		_MoveToRight( -SPEED, tm);
	}

	if( pKbd->GetPressState( I3I_KEY_D))
	{
		_MoveToRight( SPEED, tm);
	}

	if( pKbd->GetStrokeState( I3I_KEY_LCTRL))
	{
		_SetInfo( "[{col:255,0,0,255}12345{/col}:Camera Set]  [{col:255,0,0,255}P{/col}:Start Measure]  [{col:255,0,0,255}R{/col}:Shader Reuild]  [{col:255,0,0,255}M{/col}:Shadow On/Off]");
	}

	i3Framework::OnInput();
}

/*
void	CFramework::OnLButtonDown( UINT32 nFlag, POINT point)
{
}

void	CFramework::OnLButtonUp( UINT32 nFlag, POINT point)
{
}
*/

void	CFramework::OnMButtonDown( UINT32 nFlag, POINT point)
{
	HWND hwnd = GetViewer()->GetWindowHandle();
	i3Camera * pCam = GetDefaultCamera();

	m_bCamDrag = true;

	::SetCapture( hwnd);

	m_DragStartPt = point;
	i3Matrix::Copy( &m_DragStartTx, pCam->GetMatrix());
	i3Matrix::SetPos( &m_DragStartTx, 0.0f, 0.0f, 0.0f, 1.0f);
}

void	CFramework::OnMButtonUp( UINT32 nFlag, POINT point)
{
	m_bCamDrag = false;

	::ReleaseCapture();
}

/*
void	CFramework::OnRButtonDown( UINT32 nFlag, POINT point)
{
}

void	CFramework::OnRButtonUp( UINT32 nFlag, POINT point)
{
}
*/

void	CFramework::OnMouseMove( UINT32 nFlag, POINT point)
{
	if( m_bCamDrag)
	{
		i3Camera * pCam = GetDefaultCamera();
		REAL32 amount = 0.4f;
		REAL32 xDeg, yDeg;
		VEC3D oldPos;

		xDeg = (REAL32)-(point.x - m_DragStartPt.x) * amount;
		yDeg = (REAL32)-(point.y - m_DragStartPt.y) * amount;

		MATRIX rot;

		i3Matrix::SetRotateXDeg( &rot, yDeg);
		i3Matrix::PostRotateYDeg( &rot, xDeg);

		i3Vector::Copy( &oldPos, i3Matrix::GetPos( pCam->GetMatrix()));
		i3Matrix::Mul( pCam->GetMatrix(), &rot, &m_DragStartTx);

		i3Matrix::SetPos( pCam->GetMatrix(), &oldPos);

		OnUpdateCameraTransform();
	}
}

void	CFramework::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	i3Framework::OnMouseWheel( nFlag, zDelta, point);
}
