#include "i3InputType.h"
#include "i3InputMouse.h"
#include "i3InputGlobalVariable.h"
#include "dxerr.h"

#if defined( I3G_DX)

//#define __BUFFERED__
#define I3I_BUFFER_SIZE		64  // arbitrary number of buffer elements

i3InputMouse::~i3InputMouse(void)
{
	I3_SAFE_RELEASE_NODBG( m_pDevice);
}

bool i3InputMouse::Create( void * pPlatformData)
{
	HRESULT Rv;
	DIDATAFORMAT fmt;

	m_pDevice = (IDirectInputDevice8 *) pPlatformData;

	memcpy( &fmt, &c_dfDIMouse2, sizeof(fmt));

	fmt.dwFlags = DIDF_RELAXIS;

	Rv = m_pDevice->SetDataFormat( &fmt);
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3InputMouse::Create()", Rv);
		return false;
	}

	{
		DIPROPDWORD prop;

		prop.diph.dwSize = sizeof(prop);
		prop.diph.dwHeaderSize = sizeof( prop.diph);
		prop.diph.dwHow = DIPH_DEVICE;
		prop.diph.dwObj = 0;

		prop.dwData = DIPROPAXISMODE_REL;

		Rv = m_pDevice->SetProperty( DIPROP_AXISMODE, (LPCDIPROPHEADER) &prop);
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3InputMouse::Create()", Rv);
			return false;
		}

		#if defined( __BUFFERED__)
			prop.dwData = I3I_BUFFER_SIZE;

			Rv = m_pDevice->SetProperty( DIPROP_BUFFERSIZE, (LPCDIPROPHEADER) &prop);
			if( FAILED( Rv))
			{
				DXTRACE_ERR( "i3InputMouse::Create()", Rv);
				return false;
			}
		#endif
	}

	// Set Coorperative Level
	{
		HWND  hwndTop;

		hwndTop = g_hwndWnd;

		while( GetParent( hwndTop) != nullptr)
			hwndTop = GetParent( hwndTop);

		Rv = m_pDevice->SetCooperativeLevel( hwndTop, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3InputMouse::Create()", Rv);
			return false;
		}
	}

	m_pDevice->Acquire();

	return true;
}

void	i3InputMouse::SetAccessRight( bool bGet)
{
	if( bGet)
	{
		m_pDevice->Acquire();
	}
	else
	{
		m_pDevice->Unacquire();
	}
}

//Mouse Update Immediate Mode
void i3InputMouse::Update( REAL32 time)
{
	HRESULT	hr;

	if( m_pDevice == nullptr)
		return;

	m_StrokeButton = 0;

	if( FAILED( hr = m_pDevice->Poll()))
	{
		// DirectInput may be telling us that the input stream has been
        // interrupted.  We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done.
        // We just re-acquire and try again.
        
        // If input is lost then acquire and keep trying 
        hr = m_pDevice->Acquire();
        while( hr == DIERR_INPUTLOST )
		{
            hr = m_pDevice->Acquire();
		}

		//I3TRACE( "Input Lost : NAME:%s\nDESC:%s\n", DXGetErrorString( hr), DXGetErrorDescription( hr));

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return; 
	}
	
	UINT32 btn = 0;	
	m_DeltaX = m_DeltaY = m_DeltaZ = 0.0f;

	{
		#if defined( __BUFFERED__)
			static DIDEVICEOBJECTDATA s_MouseData[ I3I_BUFFER_SIZE];
			INT32 i, cnt;

			cnt = I3I_BUFFER_SIZE;
			hr = m_pDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), s_MouseData, (LPDWORD) &cnt, 0);
			if( SUCCEEDED( hr))
			{
				for( i = 0; i < cnt; i++)
				{
					switch( s_MouseData[i].dwOfs)
					{
						case DIMOFS_BUTTON0 :	if( s_MouseData[i].dwData & 0x80)		btn |= I3I_MOUSE_LBUTTON;	break;
						case DIMOFS_BUTTON1 :	if( s_MouseData[i].dwData & 0x80)		btn |= I3I_MOUSE_RBUTTON;	break;
						case DIMOFS_BUTTON2 :	if( s_MouseData[i].dwData & 0x80)		btn |= I3I_MOUSE_MBUTTON;	break;
						case DIMOFS_X :			m_DeltaX = (REAL32) *((INT32 *) &s_MouseData[i].dwData);	break;
						case DIMOFS_Y :			m_DeltaY = (REAL32) *((INT32 *) &s_MouseData[i].dwData);	break;
						case DIMOFS_Z :			
							m_DeltaZ = (REAL32) *((INT32 *) &s_MouseData[i].dwData);	

							m_Z += m_DeltaZ;

							if( m_DeltaZ > 0.0f)				btn |= I3I_MOUSE_WHEELUP;
							if( m_DeltaZ < 0.0f)				btn |= I3I_MOUSE_WHEELDOWN;
							break;

					}
				}
			}

		#else
			DIMOUSESTATE2 dims2;

			i3mem::FillZero( &dims2, sizeof(dims2));

			hr = m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2);
			if( SUCCEEDED(hr))
			{			
				m_DeltaX = (REAL32)dims2.lX;
				m_DeltaY = (REAL32)dims2.lY;
				m_DeltaZ = (REAL32)dims2.lZ;

				m_Z += m_DeltaZ;

				if(m_bUseLeftMouse)
				{
					if( dims2.rgbButtons[0] & 0x80)		btn |= I3I_MOUSE_RBUTTON;
					if( dims2.rgbButtons[1] & 0x80)		btn |= I3I_MOUSE_LBUTTON;
				}
				else
				{
					if( dims2.rgbButtons[0] & 0x80)		btn |= I3I_MOUSE_LBUTTON;
					if( dims2.rgbButtons[1] & 0x80)		btn |= I3I_MOUSE_RBUTTON;
				}
				

				if( dims2.rgbButtons[2] & 0x80)		btn |= I3I_MOUSE_MBUTTON;
				if( m_DeltaZ > 0.0f)				btn |= I3I_MOUSE_WHEELUP;
				if( m_DeltaZ < 0.0f)				btn |= I3I_MOUSE_WHEELDOWN;
			}
		#endif
	}	

	m_StrokeButton = (m_PressButton ^ btn) & btn;
	m_PressButton = btn;


	////////////////////////////////////////////////////
	/*
	아래의 코드는 매우 심각한 누적 증상을 가져온다.
	- TCP의 속도가 느려짐.
	- 회를 거듭할수록 전반적인 속도가 느려짐.
	MSG msg;

	while( ::PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
	{
		::TranslateMessage( &msg );
        ::DispatchMessage( &msg );
	}
	*/
}

void i3InputMouse::Reset(void)
{
}

void i3InputMouse::SetButtonState(UINT32 mask, bool bPress)
{
	if (bPress)
	{
		m_PressButton |= mask;
	}
	else
	{
		m_PressButton &= ~mask;
	}
}

void i3InputMouse::SetStrokeButtonState(UINT32 mask, bool bPress)
{
	if (bPress)
	{
		m_StrokeButton |= mask;
	}
	else
	{
		m_StrokeButton &= ~mask;
	}
}

void i3InputMouse::SetMousePos( REAL32 fX, REAL32 fY, REAL32 fZ)
{
	m_X = fX;	m_Y = fY; m_Z = fZ;
}

/*//Mouse Update Buffer Mode
void i3InputMouse::Update( REAL32 time)
{
	DIDEVICEOBJECTDATA didod[ I3I_BUFFER_SIZE ];  // Receives buffered data 
    DWORD              count;
    DWORD              i;
	UINT32			key;
    HRESULT            hr;

    count = I3I_BUFFER_SIZE;
    hr = m_pDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &count, 0 );
    if( hr != DI_OK ) 
    {
        // We got an error or we got DI_BUFFEROVERFLOW.
        //
        // Either way, it means that continuous contact with the
        // device has been lost, either due to an external
        // interruption, or because the buffer overflowed
        // and some events were lost.
        //
        // Consequently, if a button was pressed at the time
        // the buffer overflowed or the connection was broken,
        // the corresponding "up" message might have been lost.
        //
        // But since our simple sample doesn't actually have
        // any state associated with button up or down events,
        // there is no state to reset.  (In a real game, ignoring
        // the buffer overflow would result in the game thinking
        // a key was held down when in fact it isn't; it's just
        // that the "up" event got lost because the buffer
        // overflowed.)
        //
        // If we want to be cleverer, we could do a
        // GetDeviceState() and compare the current state
        // against the state we think the device is in,
        // and process all the states that are currently
        // different from our private state.
        m_pDevice->Acquire();

        // Update the dialog text 

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return; 
    }

    if( FAILED(hr) )  
        return;

    // Study each of the buffer elements and process them.
    //
    // Since we really don't do anything, our "processing"
    // consists merely of squirting the name into our
    // local buffer.
    for( i = 0; i < count; i++ ) 
    {
		key = 0;

        switch( didod[ i ].dwOfs )
        {
            case DIMOFS_BUTTON0:		key = I3I_MOUSE_LBUTTON;		break;
            case DIMOFS_BUTTON1:		key = I3I_MOUSE_LBUTTON;		break;
            case DIMOFS_BUTTON2:		key = I3I_MOUSE_MBUTTON;		break;

            case DIMOFS_X:				m_X += didod[ i ].dwData;		break;
            case DIMOFS_Y:				m_Y += didod[ i ].dwData;		break;
            case DIMOFS_Z:				m_Z += didod[ i ].dwData;		break;
        }

		if( key != 0)
		{
			if( didod[i].dwData & 0x80)
				m_Button |= key;
			else
				m_Button &= ~key;
		}
    }
}
*/

void i3InputMouse::OnMouseMove( UINT nFlag, POINT pt)
{
	m_X = (REAL32) pt.x;
	m_Y = (REAL32) pt.y;
}

#endif
