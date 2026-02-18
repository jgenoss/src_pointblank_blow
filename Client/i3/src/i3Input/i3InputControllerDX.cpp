#include "i3InputType.h"
#include "i3InputController.h"
#include "i3InputGlobalVariable.h"

#if defined( I3G_DX)
#define		ANALOG_MAX			1000
#define		ANALOG_MIN			-1000

// SVN 테스트
i3InputController::~i3InputController(void)
{
	I3_SAFE_RELEASE_NODBG( m_pDevice);
}

void i3InputController::ResetAll(void)
{
	m_PressState = 0;
	m_RepeatState = 0;	
	m_StrokeState = 0;

	i3mem::Fill( m_RepeatCount, 0, sizeof(m_RepeatCount));
	i3mem::Fill( m_TimeStamp, 0, sizeof( m_TimeStamp));
	i3mem::Fill( m_Analog, 0, sizeof(m_Analog));
}

static BOOL CALLBACK _EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE * pObj, void * pUserData)
{
    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls
	IDirectInputDevice8 *	pDevice = (IDirectInputDevice8 *) pUserData;

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pObj->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pObj->dwType; // Specify the enumerated axis
        diprg.lMin              = ANALOG_MIN; 
        diprg.lMax              = ANALOG_MAX; 
    
        // Set the range for the axis
        if( FAILED( pDevice->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;
    }

    // Set the UI to reflect what objects the joystick supports
	/*
    if (pdidoi->guidType == GUID_XAxis)
    {
    }
    if (pdidoi->guidType == GUID_YAxis)
    {
    }
    if (pdidoi->guidType == GUID_ZAxis)
    {
    }
    if (pdidoi->guidType == GUID_RxAxis)
    {
    }
    if (pdidoi->guidType == GUID_RyAxis)
    {
    }
    if (pdidoi->guidType == GUID_RzAxis)
    {
    }
	*/

    return DIENUM_CONTINUE;
}

bool i3InputController::Create( void * pPlatformData)
{
	HRESULT Rv;

	m_pDevice = (IDirectInputDevice8 *) pPlatformData;

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	Rv = m_pDevice->SetDataFormat( &c_dfDIJoystick2 );
    if( FAILED( Rv))
	{
		DXTRACE_ERR( L"i3InputController::Create()", Rv);
        return false;
	}

	HWND hwndTop;

	hwndTop = g_hwndWnd;
	while( ::GetParent( hwndTop) != NULL)
		hwndTop = ::GetParent( hwndTop);

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
	Rv = m_pDevice->SetCooperativeLevel( hwndTop, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3InputController::Create()", Rv);
        return false;
	}

    // Enumerate the joystick objects. The callback function enabled user
    // interface elements for objects that are found, and sets the min/max
    // values property for discovered axes.
	Rv = m_pDevice->EnumObjects( _EnumObjectsCallback, (void*) m_pDevice, DIDFT_ALL );
    if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3InputController::Create()", Rv);
        return false;
	}

	return true;
}

static UINT32	s_BtnMask[] =
{
	I3I_BTN_RUP,
	I3I_BTN_RRIGHT,
	I3I_BTN_RDOWN,
	I3I_BTN_RLEFT,
	I3I_BTN_L2,
	I3I_BTN_R2,
	I3I_BTN_L1,
	I3I_BTN_R1,
	I3I_BTN_START,
	I3I_BTN_SELECT,
	I3I_BTN_SYS1,
	I3I_BTN_SYS2,
	0
};

void i3InputController::Update( REAL32 time)
{
    HRESULT     Rv;
    DIJOYSTATE2 js;           // DInput joystick state 
	UINT32	NewState = 0;
	UINT32	i;
	REAL32 timeGap;

    // Poll the device to read the current state
    Rv = m_pDevice->Poll(); 
    if( FAILED( Rv))  
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        Rv = m_pDevice->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return; 
    }

    // Get the input's device state
	Rv = m_pDevice->GetDeviceState( sizeof(DIJOYSTATE2), &js);
    if( FAILED( Rv))
        return; // The device should have been acquired during the Poll()

    // Display joystick state to dialog

    // Analog Axes
	m_Analog[ I3I_ANALOG_DIR_X] =	REAL32( js.lX  ) / ANALOG_MAX; // -1.0f ~ 1.0f (오른쪽이 +)
	m_Analog[ I3I_ANALOG_DIR_Y] =	REAL32( js.lY  ) / ANALOG_MAX; // -1.0f ~ 1.0f (아래쪽이 +)
	m_Analog[ I3I_ANALOG_DIR_U]	=	REAL32( js.lRy ) / ANALOG_MAX;
	m_Analog[ I3I_ANALOG_DIR_V] =	REAL32( js.lRx ) / ANALOG_MAX;

	//I3TRACE( "x,y = %6f, %6f\n", m_Analog[ I3I_ANALOG_DIR_X], m_Analog[ I3I_ANALOG_DIR_Y] );

	// Remove Dead Zone
	for( i = 0; i < I3I_ANALOG_DIR_MAX; i++ )
	{
		if( ( I3I_ANALOG_DEAD_MIN <= m_Analog[i] ) && ( m_Analog[i] <= I3I_ANALOG_DEAD_MAX ) )
		{
			m_Analog[i] = 0.0f;
		}
	}

	//I3TRACE( "x,y = %6f, %6f\n", m_Analog[ I3I_ANALOG_DIR_X], m_Analog[ I3I_ANALOG_DIR_Y] );

	// Arrow Buttons (POV)
	// -1		: no button pressed
	//
	// 0		: UP
	// 2250
	// 4500		: UP + RIGHT
	// 6750
	// 9000		: RIGHT
	// 11250
	// 13500	: DOWN + RIGHT
	// 15750
	// 18000	: DOWN
	// 20250
	// 22500	: DOWN + LEFT
	// 24750
	// 27000	: LEFT
	// 29250
	// 31500	: UP + LEFT
	// 33750
	// 36000	: N/A
	//
	DWORD dwPOV = js.rgdwPOV[0];
	if( dwPOV != 0xFFFFFFFF )
	{
		if( dwPOV < 2250 || dwPOV >= 33750 )
		{
			NewState |= I3I_BTN_LUP;
		}
		else if( dwPOV < 6750 )
		{
			NewState |= I3I_BTN_LUP;
			NewState |= I3I_BTN_LRIGHT;
		}
		else if( dwPOV < 11250 )
		{
			NewState |= I3I_BTN_LRIGHT;
		}
		else if( dwPOV < 15750 )
		{
			NewState |= I3I_BTN_LDOWN;
			NewState |= I3I_BTN_LRIGHT;
		}
		else if( dwPOV < 20250 )
		{
			NewState |= I3I_BTN_LDOWN;
		}
		else if( dwPOV < 24750 )
		{
			NewState |= I3I_BTN_LDOWN;
			NewState |= I3I_BTN_LLEFT;
		}
		else if( dwPOV < 29250 )
		{
			NewState |= I3I_BTN_LLEFT;
		}
		else if( dwPOV < 33750 )
		{
			NewState |= I3I_BTN_LUP;
			NewState |= I3I_BTN_LLEFT;
		}
	}

	for( i = 0; s_BtnMask[i] > 0; i++)
    {
        if ( js.rgbButtons[i] & 0x80 )
		{
			NewState |= s_BtnMask[i];
		}
    }

	// Stroke State
	m_StrokeState = (m_PressState ^ NewState) & NewState;

	// Repeat State
	UINT32 mask;

	for( i = 0, mask = 1; mask != 0; mask = mask << 1, i++)
	{
		if( NewState & mask)
		{
			if( m_RepeatCount[i] == 0)
			{
				m_TimeStamp[i] = 0.0f;
				m_RepeatState |= mask;
				m_RepeatCount[i] = 1;
			}
			else
			{
				if( m_RepeatCount[i] == 1)
					timeGap = 0.250f;
				else
					timeGap = 0.033f;

				m_TimeStamp[i] += time;

				if(m_TimeStamp[i] > timeGap)
				{
					m_RepeatCount[i] ++;
					m_RepeatState |= mask;
					m_TimeStamp[i] = 0.0f;
				}
				else
				{
					m_RepeatState &= ~mask;
				}
			}
		}
		else
		{
			m_RepeatCount[i] = 0;
			m_RepeatState &= ~mask;
		}
	}

	// Press State
	m_PressState = NewState;
}

void i3InputController::Reset(void)
{
}


void	i3InputController::SetAccessRight( bool bGet)
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

#endif