#include "i3FrameworkPCH.h"
#include "i3Viewer.h"
#include "i3ViewerModeDefault.h"

I3_CLASS_INSTANCE( i3ViewerModeDefault, i3ViewerMode);

i3ViewerModeDefault::i3ViewerModeDefault(void)
{
	i3Vector::Set( &m_BasePos, __RT_ZERO, __RT_ZERO, __RT_ZERO);

	m_CamDist = __RT( 28000.0f);
	m_CamNear = __RT( 10.0f);
	m_CamFar = __RT( 10000.0f);
	m_ZoomRate = __RT( 10.0f);

	m_CamModeFlag = 0;

#if defined( I3_WINDOWS)
	m_CamMode = I3_VIEWER_CAM_NONE;
#endif
}

i3ViewerModeDefault::~i3ViewerModeDefault(void)
{
}


void i3ViewerModeDefault::PivotRotate( RT_REAL32 RotX, RT_REAL32 RotY, RT_REAL32 /*RotZ*/)
{
    // Camera matrix
    RT_MATRIX * pCamera;
	RT_VEC3D vecRight, localPos;
	i3Camera * pCurCam = m_pViewer->GetCurrentCamera();

	#if defined( I3_FIXED_POINT)
		RotX = i3FXD::Mul( RotX, __RT(5.0f));
		RotY = i3FXD::Mul( RotY, __RT(5.0f));
	#else
		RotX *= 5.0f;
		RotY *= 5.0f;
	#endif

	pCamera = pCurCam->GetMatrix();

	i3Matrix::Copy( pCamera, &m_DragStartMatrix);
	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCamera), &m_BasePos);

	// make it in local space
	i3Matrix::SetPos( pCamera, &localPos);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCamera));
	i3Vector::Normalize( &vecRight, &vecRight);

	i3Matrix::PostRotateAxis( pCamera, (const RT_VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCamera, (const RT_VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCamera), &m_BasePos);
	i3Matrix::SetPos( pCamera, &localPos);
}

void i3ViewerModeDefault::SetPivot( VEC3D * pVec)
{
	i3Vector::Copy( &m_BasePos, pVec);
}

void i3ViewerModeDefault::Rotate( RT_REAL32 RotX, RT_REAL32 RotY, RT_REAL32 /*RotZ*/)
{
	RT_MATRIX * pOldCam = &m_DragStartMatrix;
	RT_MATRIX * pCam = m_pViewer->GetCurrentCamera()->GetMatrix();

	i3Matrix::Copy( pCam, pOldCam);

	i3Matrix::PreRotateAxis( pCam, i3Matrix::GetUp( pOldCam), RotY);
	i3Matrix::PreRotateAxis( pCam, i3Matrix::GetRight( pOldCam), RotX);
}

void i3ViewerModeDefault::Pan( RT_REAL32 dx, RT_REAL32 dy, RT_REAL32 dz)
{
	RT_VEC3D vecRight, vecUp, vecAt;
	RT_MATRIX * pCam;

	pCam = &m_DragStartMatrix;

	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCam));
	i3Vector::Copy( &vecUp, i3Matrix::GetUp( pCam));
	i3Vector::Copy( &vecAt, i3Matrix::GetAt( pCam));

	i3Vector::Normalize( &vecRight, &vecRight);
	i3Vector::Normalize( &vecUp, &vecUp);
	i3Vector::Normalize( &vecAt, &vecAt);

	i3Vector::Scale( &vecRight, &vecRight, (-dx));
	i3Vector::Scale( &vecUp, &vecUp, ( dy));
	i3Vector::Scale( &vecAt, &vecAt, ( dz));

	i3Vector::Add( &vecRight, &vecRight, &vecUp);
	//i3Vector::Add( &vecRight, &vecRight, &vecAt);

	{
		RT_MATRIX tmp;
		
		i3Matrix::Identity( &tmp);

		i3Matrix::SetPos( &tmp, &vecRight);

		i3Matrix::Mul( m_pViewer->GetCurrentCamera()->GetMatrix(), &m_DragStartMatrix, &tmp);
	}
}

void i3ViewerModeDefault::OnResetCameraPosition(void)
{
	RT_MATRIX * pCamera = m_pViewer->GetDefaultCamera()->GetMatrix();

	i3Matrix::Identity( pCamera);	
	i3Matrix::SetTranslate( pCamera, getX(&m_BasePos), getY(&m_BasePos), getZ(&m_BasePos) + m_CamDist);	
}

void i3ViewerModeDefault::OnChangeBound(void)
{
	RT_VEC3D * pMin, * pMax;
	RT_VEC3D vec;

	pMin = m_pViewer->GetBoundMin();
	pMax = m_pViewer->GetBoundMax();

	i3Vector::Sub( &vec, pMax, pMin);
	m_CamDist = i3Math::Mul( i3Vector::Length( &vec), __RT(1.3f));

	#if defined( I3_FIXED_POINT)
		m_CamNear = i3FXD::Mul( m_CamDist, __RT( 0.0001f));
		m_CamFar =	i3FXD::Mul( m_CamDist, __RT( 5.0f));
		m_ZoomRate = i3FXD::Mul( m_CamDist, __RT( 0.01f));
	#else
		m_CamNear = m_CamDist * 0.0001f;
		m_CamFar = m_CamDist * 5.0f;
		m_ZoomRate = m_CamDist * 0.01f;
	#endif

	if( m_CamFar < __RT( 10000.0f))
		m_CamFar = __RT( 10000.0f);

	if((m_CamFar > __RT_ONE) &&(m_CamNear < __RT_ONE))
		m_CamNear = __RT_ONE;

	m_pViewer->SetCameraProperty( __PT( m_CamDist), __PT( m_CamNear), __PT(m_CamFar));

	i3Vector::Add( &vec, pMax, pMin);

	i3Vector::Scale( &m_BasePos, &vec, __RT( 0.5f));
}

void i3ViewerModeDefault::OnUpdateCamera(void)
{
	i3RenderTarget * pRT;
	i3Camera * pCam;

	pRT = m_pViewer->GetRenderContext()->GetRenderTarget( 0);
	I3ASSERT( pRT != NULL);

	pCam = m_pViewer->GetCurrentCamera();

	switch( pCam->GetMode())
	{
		case i3Camera::I3_CAMERA_MODE_ORTHOGONAL :
			{
				REAL32 w, h, l;
				RT_MATRIX * pCamera = pCam->GetMatrix();
				VEC3D vec, pc;

				i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera));
				i3Vector::Normalize( &vec, &vec);
				i3Vector::Scale( &vec, &vec, -1.0f);

				#if defined( I3_FIXED_POINT)
					VEC3D campos, basepos;

					i3Vector::Copy( &basepos, &m_BasePos);
					i3Vector::Copy( &campos, i3Matrix::GetPos( pCamera));
					l = NearestPlanePoint( &basepos, &vec, &campos, &pc);
				#else
					l = NearestPlanePoint( &m_BasePos, &vec, i3Matrix::GetPos( pCamera), &pc);
				#endif

				l = i3Math::abs( l);

				w = l;
				h = l * ((REAL32) pRT->GetHeight() / pRT->GetWidth());

				pCam->SetOrthogonal( __RT(w), __RT(h), pCam->GetNear(), pCam->GetFar());
			}
			break;

		default :
			{
				RT_REAL32 w, h;
				
				#if defined( I3_FIXED_POINT)
					w = i3FXD::FromInt( pRT->GetWidth());
					h = i3FXD::FromInt( pRT->GetHeight());
				#else
					w = (REAL32)pRT->GetWidth();
					h = (REAL32)pRT->GetHeight();
				#endif

				pCam->SetPerspective( pCam->GetFOV(), w, h, pCam->GetNear(), pCam->GetFar());
			}
			break;
	}
}

void i3ViewerModeDefault::OnInput(void)
{
	i3InputController * pCtrl;
	i3InputDeviceManager * pInputManager;
	i3InputKeyboard * pKB;
	i3Camera * pCam;

	pInputManager = m_pViewer->GetInputDeviceManager();
	pCam = m_pViewer->GetCurrentCamera();

	if( pInputManager->GetKeyboard() != NULL)
	{
		pKB = pInputManager->GetKeyboard();

		#if defined( I3_WIPI)
		if( pKB->GetStrokeState( I3I_KEY_1))
		{
			m_pViewer->ReportPerformance();
			REPORT_PROFILE();
		}
		
		if( pKB->GetStrokeState( I3I_KEY_MINUS))
		{
			m_pViewer->SetDefaultLightEnable( ! m_pViewer->GetDefaultLightEnable());
		}
		#endif

		if( pKB->GetStrokeState( I3I_KEY_EQUAL))
		{
			ToggleDebugControl();
		}

		if( m_bDebugControl)
		{
			if( pKB->GetPressState( I3I_KEY_LEFT))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT( I3_DEG2RAD(-0.5f)), __RT_0, __RT_0);
			}

			if( pKB->GetPressState( I3I_KEY_RIGHT))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT(I3_DEG2RAD(0.5f)), __RT_0, __RT_0);
			}

			if( pKB->GetPressState( I3I_KEY_UP))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT_0, __RT(I3_DEG2RAD(-0.5f)), __RT_0);
			}

			if( pKB->GetPressState( I3I_KEY_DOWN))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT_0, __RT(I3_DEG2RAD(0.5f)), __RT_0);
			}

			if( pKB->GetPressState( I3I_KEY_Z))
			{
				m_pViewer->Zoom( __RT( 0.5f));
			}

			if( pKB->GetPressState( I3I_KEY_X))
			{
				m_pViewer->Zoom( __RT( -0.5f));
			}
		}
	}

	if( pInputManager->GetControllerCount() > 0)
	{
		pCtrl = pInputManager->GetController( 0);

		if((pCtrl->GetPressState() & (I3I_BTN_L1 | I3I_BTN_R1)) == (I3I_BTN_L1 | I3I_BTN_R1))
		{
			// L1과 R1이 동시에 눌려진 상태
			// Debug 기능들을 나열한다.

			if( pCtrl->GetStrokeState() & I3I_BTN_RLEFT)
			{
				m_pViewer->ReportPerformance();
			}
			else if( pCtrl->GetStrokeState() & I3I_BTN_RRIGHT)
			{
				REPORT_PROFILE();
			}
			else if( pCtrl->GetStrokeState() & I3I_BTN_RUP)
			{
				m_pViewer->SetDefaultLightEnable( ! m_pViewer->GetDefaultLightEnable());
			}
			else if( pCtrl->GetStrokeState() & I3I_BTN_RDOWN)
			{
				m_pViewer->SetDumpState( true);
			}
		}
		else
		{
			REAL32 axis;

			axis = pCtrl->GetAnalogValue( I3I_ANALOG_DIR_X);
			if( i3Math::abs(axis) > I3I_ANALOG_DEAD_MAX )
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT(I3_DEG2RAD( axis)), __RT_0, __RT_0);
			}

			axis = pCtrl->GetAnalogValue( I3I_ANALOG_DIR_Y);
			if( i3Math::abs(axis) > I3I_ANALOG_DEAD_MAX )
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT_0, __RT(I3_DEG2RAD( axis)), __RT_0);
			}

			if(pCtrl->GetPressState() & I3I_BTN_LLEFT)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT( I3_DEG2RAD( -0.5f)), __RT_0, __RT_0);
			}

			if( pCtrl->GetPressState() & I3I_BTN_LRIGHT)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT(I3_DEG2RAD( 0.5f)), __RT_0, __RT_0);
			}

			if( pCtrl->GetPressState() & I3I_BTN_LUP)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT_0, __RT(I3_DEG2RAD( -0.5f)), __RT_0);
			}

			if( pCtrl->GetPressState() & I3I_BTN_LDOWN)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( __RT_0, __RT(I3_DEG2RAD( 0.5f)), __RT_0);
			}

			if( pCtrl->GetPressState() & I3I_BTN_RLEFT)
			{
				m_pViewer->Zoom( i3Math::Mul( __RT( 0.5f), m_ZoomRate));
			}
			if( pCtrl->GetPressState() & I3I_BTN_RDOWN)
			{
				m_pViewer->Zoom( i3Math::Mul( __RT( -0.5f), m_ZoomRate));
			}

			if( pCtrl->GetPressState() & I3I_BTN_L1)
			{
			}

			if( pCtrl->GetPressState() & I3I_BTN_R1)
			{
			}

			if( pCtrl->GetPressState() & I3I_BTN_START)
			{
				do
				{
					pInputManager->Update();
				} while( pCtrl->GetPressState() & I3I_BTN_START);
			}
		}
	}
}

#if defined( I3_WINDOWS)
void i3ViewerModeDefault::OnLButtonDown( UINT /*nFlag*/, POINT point)
{
	HWND hwnd = m_pViewer->GetWindowHandle();
	i3Camera * pCam = m_pViewer->GetCurrentCamera();

	I3ASSERT( hwnd != NULL);

	::SetCapture( hwnd);
	m_CamMode = I3_VIEWER_CAM_PIVOT_ROTATE;

	i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
	m_DragStartPt = point;
}

void i3ViewerModeDefault::OnLButtonUp( UINT /*nFlags*/, POINT /*point*/)
{
	m_CamMode = I3_VIEWER_CAM_NONE;

	::ReleaseCapture();
}

void i3ViewerModeDefault::OnMButtonDown( UINT /*nFlags*/, POINT point)
{
	HWND hwnd = m_pViewer->GetWindowHandle();
	i3Camera * pCam = m_pViewer->GetCurrentCamera();

	I3ASSERT( hwnd != NULL);

	::SetCapture( hwnd);
	m_CamMode = I3_VIEWER_CAM_PAN;

	i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
	m_DragStartPt = point;
}

void i3ViewerModeDefault::OnMButtonUp( UINT /*nFlags*/, POINT /*point*/)
{
	m_CamMode = I3_VIEWER_CAM_NONE;

	::ReleaseCapture();
}

void i3ViewerModeDefault::OnRButtonDown( UINT /*nFlags*/, POINT point)
{
	HWND hwnd = m_pViewer->GetWindowHandle();
	i3Camera * pCam = m_pViewer->GetCurrentCamera();

	I3ASSERT( hwnd != NULL);

	::SetCapture( hwnd);
	m_CamMode = I3_VIEWER_CAM_ROTATE;

	i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
	m_DragStartPt = point;
}

void i3ViewerModeDefault::OnRButtonUp( UINT /*nFlags*/, POINT /*point*/)
{
	m_CamMode = I3_VIEWER_CAM_NONE;

	::ReleaseCapture();
}

void i3ViewerModeDefault::OnMouseMove( UINT nFlags, POINT point)
{
	REAL32 amount = 0.4f;
	REAL32 xDeg, yDeg;
	i3Camera * pCurCam = m_pViewer->GetCurrentCamera();

	switch( m_CamMode)
	{
		case I3_VIEWER_CAM_PIVOT_ROTATE :
			if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE) == 0)
			{
				amount = 0.1f;
				if( nFlags & MK_SHIFT)
					amount = 0.8f;
				else if( nFlags & MK_ALT)
					amount = 0.05f;

				xDeg = (REAL32)(m_DragStartPt.x - point.x) * amount;
				yDeg = (REAL32)(m_DragStartPt.y - point.y) * amount;

				PivotRotate( __RT( I3_DEG2RAD(xDeg)), __RT(I3_DEG2RAD(yDeg)), __RT_0);
			}
			break;

		case I3_VIEWER_CAM_PAN :
			if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_PAN) == 0)
			{
				RT_VEC3D vec1;
				RT_VEC3D end1;
				RT_VEC3D start1;
				RT_REAL32 dist;

				i3Vector::Sub( &vec1, i3Matrix::GetPos( pCurCam->GetMatrix()), &m_BasePos);

				dist = i3Vector::Length( &vec1);
				if( dist <= __RT( I3_EPS))
					dist = __RT_ONE;

				m_pViewer->ScreenToWorld( point.x, point.y, &end1, dist);
				m_pViewer->ScreenToWorld( m_DragStartPt.x, m_DragStartPt.y, &start1, dist);

				i3Vector::Sub( &vec1, &end1, &start1);

				Pan( getX(&vec1), getY(&vec1), getZ(&vec1));
			}
			break;

		case I3_VIEWER_CAM_ROTATE :
			if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_ROTATE) == 0)
			{
				RT_REAL32 amount = __RT(0.1f);
				RT_REAL32 xDeg, yDeg;

				if( nFlags & MK_SHIFT)
					amount = __RT(0.2f);
				else if( nFlags & MK_ALT)
					amount = __RT(0.01f);

				#if defined( I3_FIXED_POINT)
					xDeg = i3FXD::Mul( __RT(-(REAL32)(point.y - m_DragStartPt.y)), amount);
					yDeg = i3FXD::Mul( __RT( (REAL32)(point.x - m_DragStartPt.x)), amount);
				#else
					xDeg = -(REAL32)(point.y - m_DragStartPt.y) * amount;
					yDeg =  (REAL32)(point.x - m_DragStartPt.x) * amount;
				#endif

				Rotate( __RT( I3_DEG2RAD(-xDeg)), __RT(I3_DEG2RAD(yDeg)), __RT_0);
			}
			break;
	}
}

void i3ViewerModeDefault::OnMouseWheel( UINT /*nFlag*/, short zDelta, POINT /*point*/)
{
	VEC3D vec, pc;
	i3Camera * pCam = m_pViewer->GetCurrentCamera();
	RT_MATRIX * pCamera = pCam->GetMatrix();
	RT_MATRIX tmp;
	REAL32 len;

	i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera));
	i3Vector::Normalize( &vec, &vec);
	i3Vector::Scale( &vec, &vec, -1.0f);

	#if defined( I3_FIXED_POINT)
		VEC3D campos, basepos;

		i3Vector::Copy( &campos, i3Matrix::GetPos( pCamera));
		i3Vector::Copy( &basepos, &m_BasePos);

		len = NearestPlanePoint( &basepos, &vec, &campos, &pc);
	#else
		len = NearestPlanePoint( &m_BasePos, &vec, i3Matrix::GetPos( pCamera), &pc);
	#endif
	
	if( zDelta > 0)
		len = len / -6.0f;
	else
		len = len / 6.0f;

	i3Vector::Scale( &vec, &vec, len);

	i3Matrix::SetTranslate( &tmp, __RT(getX(&vec)), __RT(getY(&vec)), __RT(getZ(&vec)));

	i3Matrix::Mul( pCamera, pCamera, &tmp);

	if( pCam->GetMode() == i3Camera::I3_CAMERA_MODE_ORTHOGONAL)
	{
		m_pViewer->UpdateCamera();
	}
}

void i3ViewerModeDefault::OnKeyDown( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_F2 :	m_pViewer->ReportPerformance();	break;

		case VK_F3 :
			#if defined( I3_DEBUG)
			REPORT_PROFILE();
			#endif
			break; 

		case 'L' :
			m_pViewer->SetDefaultLightEnable( ! m_pViewer->GetDefaultLightEnable());
			break;

		case 'G' :
			m_pViewer->SetGridShow( !m_pViewer->GetGridShow());
			break;

		case 'D' :			m_pViewer->SetDumpState( true);	break;

	}
}
#endif
