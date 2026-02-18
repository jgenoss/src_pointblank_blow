#include "i3InputType.h"
#include "i3InputDeviceManager.h"
#include "i3InputGlobalVariable.h"

#if defined( I3G_DX) || defined( I3G_XBOX)

i3InputDeviceManager::~i3InputDeviceManager(void)
{
	DestroyCommon();

	I3_SAFE_RELEASE( m_pTimer);
	I3_SAFE_RELEASE_NODBG( m_pInputObj);
	I3_SAFE_RELEASE( m_pKeyboard);
	I3_SAFE_RELEASE( m_pMouse);

	for( INT32 i = 0; i < I3I_CONTROLLER_MAX; i++)
	{
		I3_SAFE_RELEASE( m_pController[i]);
	}
}


static BOOL CALLBACK _EnumControllerCallback( const DIDEVICEINSTANCE * pDevice, void * pUserData)
{
	HRESULT Rv;
	IDirectInputDevice8 * pDIObj;
	i3InputDeviceManager * pManager = (i3InputDeviceManager *) pUserData;

	Rv = g_pDxInput8->CreateDevice( pDevice->guidInstance, &pDIObj, NULL);
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"_EnumControllerCallback()", Rv);
		return DIENUM_CONTINUE;
	}

	{
		i3InputController * pNew;

		pNew = i3InputController::new_object_ref();

		if( pNew->Create( (void *) pDIObj) == false)
		{
			i3Log( "_EnumControllerCallback()", "Could not create i3InputController Object.");
			I3_MUST_RELEASE( pNew);
			return DIENUM_CONTINUE;
		}

		pManager->SetController( pManager->GetControllerCount(), pNew);
	}

	return DIENUM_CONTINUE;
}

bool i3InputDeviceManager::Create(void * pPlatformData, UINT32 DeviceMask)
{
	HRESULT Rv;
	HINSTANCE hInst;
	IDirectInputDevice8 * pDIObj;

	g_hwndWnd = (HWND) pPlatformData;

	m_pTimer = i3Timer::new_object();

	hInst = (HINSTANCE) GetWindowLong( (HWND) pPlatformData, GWL_HINSTANCE);
	I3ASSERT( hInst != NULL);

	Rv = DirectInput8Create( GetModuleHandleA(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **) &m_pInputObj, NULL);
	if( FAILED( Rv))
	{
		DXTRACE_ERR(L"i3InputDeviceManager::Create()", Rv);
		return false;
	}

	g_pDxInput8 = m_pInputObj;

	// Controller
	if( DeviceMask & I3I_DEVICE_MASK_CONTROLLER)
	{
		m_ControllerCount = 0;

		Rv = m_pInputObj->EnumDevices( DI8DEVCLASS_GAMECTRL, _EnumControllerCallback, this, DIEDFL_ALLDEVICES);
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3InputDeviceManager::Create()", Rv);
			return false;
		}
	}

	// Keyboard
	if( DeviceMask & I3I_DEVICE_MASK_KEYBOARD)
	{
		i3InputKeyboard * pNew;

		pNew = i3InputKeyboard::new_object();

		if( pNew->Create( (void *) g_hwndWnd) == false)
		{
			i3Log( "i3InputDeviceManager::Create()", "Could not create i3InputKeyboard Object.");
		}
		else
		{
			SetKeyboard( pNew);
		}
		I3_MUST_RELEASE( pNew);
	}

	// Mouse
	if( DeviceMask & I3I_DEVICE_MASK_MOUSE)
	{
		// DirectInput의 Mouse와 Keyboard를 사용하지 않도록 조치.
		Rv = m_pInputObj->CreateDevice( GUID_SysMouse, &pDIObj, NULL);
		if( FAILED( Rv))
		{
			DXTRACE_ERR(L"i3InputDeviceManager::Create()", Rv);
			return DIENUM_CONTINUE;
		}

		{
			i3InputMouse * pNew;

			pNew = i3InputMouse::new_object();

			if( pNew->Create( (void *) pDIObj) == false)
			{
				i3Log( "i3InputDeviceManager::Create()", "Could not create i3InputMouse Object.");
			}
			else
			{
				SetMouse( pNew);
			}

			I3_MUST_RELEASE( pNew);
		}
	}

	return true;
}

void i3InputDeviceManager::OnLButtonDown( UINT nFlag, POINT point)
{
	//m_pMouse->OnLButtonDown( nFlag, point);
}

void i3InputDeviceManager::OnLButtonUp( UINT nFlags, POINT point)
{
	//m_pMouse->OnLButtonUp( nFlags, point);
}

void i3InputDeviceManager::OnMButtonDown( UINT nFlag, POINT point)
{
	//m_pMouse->OnMButtonDown( nFlag, point);
}

void i3InputDeviceManager::OnMButtonUp( UINT nFlag, POINT point)
{
	//m_pMouse->OnMButtonUp( nFlag, point);
}

void i3InputDeviceManager::OnRButtonDown( UINT nFlag, POINT point)
{
	//m_pMouse->OnRButtonDown( nFlag, point);
}

void i3InputDeviceManager::OnRButtonUp( UINT nFlag, POINT point)
{
	//m_pMouse->OnRButtonUp( nFlag, point);
}

void i3InputDeviceManager::OnMouseMove( UINT nFlag, POINT point)
{
	if( m_pMouse != nullptr)
	{
		m_pMouse->OnMouseMove( nFlag, point);
	}
}

void i3InputDeviceManager::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	//m_pMouse->OnMouseWheel( nFlag, zDelta, point);
}

void i3InputDeviceManager::OnKeyDown( UINT32 nKey)
{
	if( m_pKeyboard != nullptr)
	{
		m_pKeyboard->OnKeyDown( nKey);
	}
}

void i3InputDeviceManager::OnKeyUp( UINT32 nKey)
{
	if( m_pKeyboard != nullptr)
	{
		m_pKeyboard->OnKeyUp( nKey);
	}
}

void i3InputDeviceManager::OnRawInput( WPARAM wParam, LPARAM lParam)
{
	//m_pMouse->OnRawInput( wParam, lParam);
}

#endif
