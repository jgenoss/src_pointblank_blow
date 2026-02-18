#include "stdafx.h"
#include "i3PerfFramework.h"
#include "GlobalVar.h"

I3_CLASS_INSTANCE( i3PerfFramework, i3ViewerFramework);

i3PerfFramework::i3PerfFramework(void)
{
	m_bCamDrag = false;

	m_pInfoText = NULL;
	m_timeInfo = 0.0f;

	m_pPerfText = NULL;
	m_PerfState = PERF_STATE_NONE;
	m_PerfCount = 0;
	m_timePerf	= 0.0f;
}

i3PerfFramework::~i3PerfFramework(void)
{
	I3_SAFE_RELEASE( m_pInfoText);
	I3_SAFE_RELEASE( m_pPerfText);
}

void i3PerfFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	m_pInfoText = i3TextNodeDX2::NewObject();
	m_pInfoText->Create( "±¼¸²Ã¼", 256, 9, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE);
	m_pInfoText->setAutoWrapEnable( TRUE);			
	m_pInfoText->DeactivateNode();
	m_pInfoText->SetText( "Information");
	m_pInfoText->setPos( 5, 155);
	m_pInfoText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
	m_pInfoText->SetColor(255, 255, 255, 255);				

	Get2DRoot()->AddChild( m_pInfoText);

	m_pPerfText = i3TextNodeDX2::NewObject();
	m_pPerfText->Create( "±¼¸²Ã¼", 256, 9, FW_NORMAL, FALSE, FALSE, CLEARTYPE_QUALITY, TRUE);
	m_pPerfText->setAutoWrapEnable( TRUE);			
	//m_pPerfText->DeactivateNode();
	m_pPerfText->SetText( "Performance Report");
	m_pPerfText->setPos( 5, 155);
	m_pPerfText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
	m_pPerfText->SetColor(255, 255, 255, 255);				

	Get2DRoot()->AddChild( m_pPerfText);
}

void	i3PerfFramework::OnSize( POINT2D * pSize)
{
	i3ViewerFramework::OnSize( pSize);

	m_pInfoText->setPos( 5, pSize->y - 20);
	m_pInfoText->setSize( pSize->x - 180, 20);

	m_pPerfText->setPos( pSize->x - 180, pSize->y - 100);
}

void	i3PerfFramework::OnChangeScene(void)
{
	i3ViewerFramework::OnChangeScene();

	if( GetLoadedScene() != NULL)
	{
		GetLoadedScene()->Instantiate();

		i3Scene::ModifyDynamicState( GetLoadedScene(), I3_PHYSIX_STATE_KINEMATIC, 0);
	}

	if( m_szFileName[0] != 0)
	{
		g_pConfig->setLastI3S( m_szFileName);

		g_pConfig->Save();
	}
}

void	i3PerfFramework::_SetInfo( const char * pszText)
{
	m_timeInfo = 2.0f;

	m_pInfoText->SetText( (char *) pszText);
	m_pInfoText->ActivateNode();
}

void	i3PerfFramework::OnUpdate( REAL32 tm)
{
	i3ViewerFramework::OnUpdate( tm);

	if( m_timeInfo > 0.0f)
	{
		m_timeInfo -= tm;

		if( m_timeInfo <= 0.0f)
		{
			m_pInfoText->DeactivateNode();
			m_timeInfo = 0.0f;
		}
		else
		{
			//COLOR col;

			//i3Color::Set( &col, 255, 255, 200, (UINT8)((m_timeInfo * 0.5f) * 255));
			//m_pInfoText->SetColor( &col);
		}
	}

	if( m_PerfState == PERF_STATE_MEASURE)
	{
		m_timePerf += tm;

		if( m_timePerf >= 1.0f)
		{
			m_timePerf = 0.0f;

			_AddPerfReport();
		}
	}
}

#define		SPEED			5.0f

void	i3PerfFramework::_MoveToAt( REAL32 speed, REAL32 tm)
{
	i3Camera * pCam = GetCurrentCamera();
	MATRIX * pCamMatrix = pCam->GetMatrix();
	
	VEC3D dir;

	i3Vector::Normalize( &dir, i3Matrix::GetAt( pCamMatrix));

	i3Vector::Scale( &dir, &dir, speed * tm);

	i3Matrix::PostTranslate( pCamMatrix, &dir);
}

void	i3PerfFramework::_MoveToRight( REAL32 speed, REAL32 tm)
{
	i3Camera * pCam = GetCurrentCamera();
	MATRIX * pCamMatrix = pCam->GetMatrix();
	
	VEC3D dir;

	i3Vector::Normalize( &dir, i3Matrix::GetRight( pCamMatrix));

	i3Vector::Scale( &dir, &dir, speed * tm);

	i3Matrix::PostTranslate( pCamMatrix, &dir);
}

void	i3PerfFramework::_MoveRecordedCam( INT32 idx)
{
	i3Camera * pCam = GetCurrentCamera();

	i3Matrix::Copy( pCam->GetMatrix(), g_pConfig->getCamRecordPos( idx));

	char conv[256];

	sprintf( conv, "Move to %dth Camera", idx + 1);
	_SetInfo( conv);
}

void	i3PerfFramework::_RecordCam( INT32 idx)
{
	i3Camera * pCam = GetCurrentCamera();

	g_pConfig->setCamRecordPos( idx, pCam->GetMatrix());

	char conv[256];
	sprintf( conv, "Record to %d camera", idx + 1);
	_SetInfo( conv);

	g_pConfig->Save();
}

void i3PerfFramework::_SetPerfState( PERF_STATE state)
{
	m_PerfState = state;

	if( m_PerfState == PERF_STATE_MEASURE)
	{
		m_timePerf = 0.0f;
		m_PerfCount = 0;

		m_pPerfText->SetText( "Measuring...");
	}
}

void i3PerfFramework::_AddPerfReport(void)
{
	INT32 i;
	char conv[512], temp[128];
	REAL32 sum;
	i3GfxPerformanceReport * pReport = GetViewer()->GetRenderContext()->GetPerformanceReport();

	if( m_PerfCount >= PERF_COUNT)
	{
		_SetPerfState( PERF_STATE_NONE);
	}

	m_FPSTable[ m_PerfCount] = pReport->GetFPS();
	m_PerfCount ++;

	conv[0] = 0;

	for( i = 0, sum = 0.0f; i < m_PerfCount; i++)
	{
		sprintf( temp, "%d : %.1f\n", i, m_FPSTable[i]);

		strcat( conv, temp);

		sum += m_FPSTable[i];
	}

	sprintf( temp, "Avg : %.1f\n", sum / m_PerfCount);
	strcat( conv, temp);

	m_pPerfText->SetText( conv);
}

void i3PerfFramework::OnInput(void)
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

	if( pKbd->GetPressState( I3I_KEY_LCTRL))
	{
		if( pKbd->GetStrokeState( I3I_KEY_P))
		{
			_SetPerfState( PERF_STATE_MEASURE);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_R))
		{
			_RebuildAllShaders();
		}

		if( pKbd->GetStrokeState( I3I_KEY_1))
		{
			_RecordCam( 0);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_2))
		{
			_RecordCam( 1);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_3))
		{
			_RecordCam( 2);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_4))
		{
			_RecordCam( 3);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_5))
		{
			_RecordCam( 4);
		}
	}
	else
	{
		if( pKbd->GetStrokeState( I3I_KEY_1))
		{
			_MoveRecordedCam( 0);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_2))
		{
			_MoveRecordedCam( 1);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_3))
		{
			_MoveRecordedCam( 2);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_4))
		{
			_MoveRecordedCam( 3);
		}
		else if( pKbd->GetStrokeState( I3I_KEY_5))
		{
			_MoveRecordedCam( 4);
		}
	}

	i3ViewerFramework::OnInput();
}

/*
void	i3PerfFramework::OnLButtonDown( UINT32 nFlag, POINT point)
{
}

void	i3PerfFramework::OnLButtonUp( UINT32 nFlag, POINT point)
{
}
*/

void	i3PerfFramework::OnMButtonDown( UINT32 nFlag, POINT point)
{
	HWND hwnd = GetViewer()->GetWindowHandle();
	i3Camera * pCam = GetCurrentCamera();

	m_bCamDrag = true;

	::SetCapture( hwnd);

	m_DragStartPt = point;
	i3Matrix::Copy( &m_DragStartTx, pCam->GetMatrix());
	i3Matrix::SetPos( &m_DragStartTx, 0.0f, 0.0f, 0.0f, 1.0f);
}

void	i3PerfFramework::OnMButtonUp( UINT32 nFlag, POINT point)
{
	m_bCamDrag = false;

	::ReleaseCapture();
}

/*
void	i3PerfFramework::OnRButtonDown( UINT32 nFlag, POINT point)
{
}

void	i3PerfFramework::OnRButtonUp( UINT32 nFlag, POINT point)
{
}
*/

void	i3PerfFramework::OnMouseMove( UINT32 nFlag, POINT point)
{
	//i3ViewerFramework::OnMouseMove( nFlag, point);

	if( m_bCamDrag)
	{
		i3Camera * pCam = GetCurrentCamera();
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

void	i3PerfFramework::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	i3ViewerFramework::OnMouseWheel( nFlag, zDelta, point);
}

/*
void	i3PerfFramework::OnKeyDown( UINT32 nKey)
{
}

void	i3PerfFramework::OnKeyUp( UINT32 nKey)
{
}

*/

void i3PerfFramework::_RebuildAllShaders(void)
{
	i3RenderContext * pCtx=  GetViewer()->GetRenderContext();
	i3ShaderCache * pCache = pCtx->getShaderCache();
	INT32 i;
	i3Shader * pShader;

	for( i = 0; i < pCache->getShaderCount(); i++)
	{
		pShader = pCache->getShader( i);

		pShader->Rebuild();
	}

	_SetInfo( "All shaders are rebuilt.");
}
