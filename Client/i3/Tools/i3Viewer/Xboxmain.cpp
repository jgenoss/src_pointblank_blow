#include "stdafx.h"

#ifdef I3_XBOX

#include "i3Viewer.h"
#include "XBInput.h"

XDEVICE_PREALLOC_TYPE* m_InputDeviceTypes;
DWORD                  m_dwNumInputDeviceTypes;
XBGAMEPAD*             m_Gamepad;
XBGAMEPAD              m_DefaultGamepad;

//-----------------------------------------------------------------------------
// Name: RebootToDash()
// Desc: Reboots to the Dash
//-----------------------------------------------------------------------------
HRESULT RebootToDash()
{
    LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
    XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );

    // XLaunchNewImage should not return
    return E_FAIL;
}


void UpdateInput()
{
	// Deadzone for the gamepad inputs
	const SHORT XINPUT_DEADZONE = (SHORT)( 0.24f * REAL32(0x7FFF) );

	//-----------------------------------------
	// Handle input
	//-----------------------------------------

	// Read the input for all connected gamepads
	XBInput_GetInput( m_Gamepad );

	// Lump inputs of all connected gamepads into one common structure.
	// This is done so apps that need only one gamepad can function with
	// any gamepad.
	ZeroMemory( &m_DefaultGamepad, sizeof(m_DefaultGamepad) );
	INT nThumbLX = 0;
	INT nThumbLY = 0;
	INT nThumbRX = 0;
	INT nThumbRY = 0;

	for( DWORD i=0; i<4; i++ )
	{
		if( m_Gamepad[i].hDevice )
		{
			// Only account for thumbstick info beyond the deadzone
			if( m_Gamepad[i].sThumbLX > XINPUT_DEADZONE ||
				m_Gamepad[i].sThumbLX < -XINPUT_DEADZONE )
				nThumbLX += m_Gamepad[i].sThumbLX;
			if( m_Gamepad[i].sThumbLY > XINPUT_DEADZONE ||
				m_Gamepad[i].sThumbLY < -XINPUT_DEADZONE )
				nThumbLY += m_Gamepad[i].sThumbLY;
			if( m_Gamepad[i].sThumbRX > XINPUT_DEADZONE ||
				m_Gamepad[i].sThumbRX < -XINPUT_DEADZONE )
				nThumbRX += m_Gamepad[i].sThumbRX;
			if( m_Gamepad[i].sThumbRY > XINPUT_DEADZONE ||
				m_Gamepad[i].sThumbRY < -XINPUT_DEADZONE )
				nThumbRY += m_Gamepad[i].sThumbRY;

			m_DefaultGamepad.fX1 += m_Gamepad[i].fX1;
			m_DefaultGamepad.fY1 += m_Gamepad[i].fY1;
			m_DefaultGamepad.fX2 += m_Gamepad[i].fX2;
			m_DefaultGamepad.fY2 += m_Gamepad[i].fY2;
			m_DefaultGamepad.wButtons        |= m_Gamepad[i].wButtons;
			m_DefaultGamepad.wPressedButtons |= m_Gamepad[i].wPressedButtons;
			m_DefaultGamepad.wLastButtons    |= m_Gamepad[i].wLastButtons;

			for( DWORD b=0; b<8; b++ )
			{
				m_DefaultGamepad.bAnalogButtons[b]        |= m_Gamepad[i].bAnalogButtons[b];
				m_DefaultGamepad.bPressedAnalogButtons[b] |= m_Gamepad[i].bPressedAnalogButtons[b];
				m_DefaultGamepad.bLastAnalogButtons[b]    |= m_Gamepad[i].bLastAnalogButtons[b];
			}
		}
	}

	// Clamp summed thumbstick values to proper range
	m_DefaultGamepad.sThumbLX = SHORT( nThumbLX );
	m_DefaultGamepad.sThumbLY = SHORT( nThumbLY );
	m_DefaultGamepad.sThumbRX = SHORT( nThumbRX );
	m_DefaultGamepad.sThumbRY = SHORT( nThumbRY );

	// Handle special input combo to trigger a reboot to the Xbox Dashboard
	if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0 )
	{
		if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0 )
		{
			if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
			{
				RebootToDash();
			}
		}
	}
}

void FrameMove()
{
	static UINT32 LastTime = timeGetTime();
    UINT32 CurTime = timeGetTime();
	REAL32 fElapsedTime = ( CurTime - LastTime ) / 1000.0f;
	LastTime = CurTime;

	REAL32 fRotationScale = 1.0f * fElapsedTime;
	REAL32 fTranslationScale = 100.f * fElapsedTime;

	MoveCamera( fTranslationScale, fRotationScale,
				m_DefaultGamepad.fX1, 0.0f, m_DefaultGamepad.fY1,
				m_DefaultGamepad.fX2, m_DefaultGamepad.fY2, FALSE);

	/*
	CAMERA_DIR Dir = CAMDIR_NONE;
	if (m_DefaultGamepad.fY1 > 0)
		Dir = CAMDIR_ZOOMIN;
	if (m_DefaultGamepad.fY1 < 0)
		Dir = CAMDIR_ZOOMOUT;
	
	if (m_DefaultGamepad.fX2 > 0)
		Dir = CAMDIR_RIGHT;
	if (m_DefaultGamepad.fX2 < 0)
		Dir = CAMDIR_LEFT;

	if (m_DefaultGamepad.fY2 > 0)
		Dir = CAMDIR_UP;
	if (m_DefaultGamepad.fY2 < 0)
		Dir = CAMDIR_DOWN;

	MoveCamera(Dir);
	*/
}

void Render()
{
	if( g_pContext->IsReady())
	{
		if( g_pContext->Begin())
		{
			g_pContext->Clear();

			if( g_pContext->BeginRender())
			{
				// something to render

				RenderView();

				g_pContext->EndRender();
			}

			g_pContext->End();
		}
	}
}

void main()
{
	HRESULT hr;

    // Initialize core peripheral port support. Note: If these parameters
    // are 0 and NULL, respectively, then the default number and types of 
    // controllers will be initialized.
    XInitDevices( m_dwNumInputDeviceTypes, m_InputDeviceTypes );

    // Create the gamepad devices
    OutputDebugString( "XBApp: Creating gamepad devices...\n" );
    if( FAILED( hr = XBInput_CreateGamepads( &m_Gamepad ) ) )
    {
        OutputDebugString( "XBApp: Call to CreateGamepads() failed!\n" );
        return;
    }

	InitRenderContext();
	Resize( 640, 480 );

	while (1)
	{
		UpdateInput();
		FrameMove();
		Render();
	}
}

#endif