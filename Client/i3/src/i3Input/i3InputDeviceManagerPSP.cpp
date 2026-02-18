#include "i3InputType.h"
#include "i3InputDeviceManager.h"
#include "i3InputGlobalVariable.h"

#if defined( I3G_PSP)

i3InputDeviceManager::i3InputDeviceManager(void)
{
	m_pKeyboard = NULL;
	m_pMouse = NULL;
	memset( m_pController, 0, sizeof(m_pController));

	m_ControllerCount = 0;

	InitCommon();
}

i3InputDeviceManager::~i3InputDeviceManager(void)
{
	DestroyCommon();

	if( m_pKeyboard != NULL)
	{
		m_pKeyboard->Release();
		m_pKeyboard = NULL;
	}

	if( m_pMouse != NULL)
	{
		m_pMouse->Release();
		m_pMouse = NULL;
	}

	for( INT32 i = 0; i < I3I_CONTROLLER_MAX; i++)
	{
		if( m_pController[i] != NULL)
		{
			m_pController[i]->Release();
			m_pController[i] = NULL;
		}
	}
}


BOOL i3InputDeviceManager::Create(void * pPlatformData, UINT32 DeviceMask)
{
	sceCtrlSetSamplingCycle( 0);
	sceCtrlSetSamplingMode( SCE_CTRL_MODE_DIGITALANALOG);


	m_pTimer = i3Timer::NewObject();

	// Controller
	if( DeviceMask & I3I_DEVICE_MASK_CONTROLLER)
	{
		i3InputController * pNew;

		pNew = i3InputController::NewObjectRef();

		if( pNew->Create( (void *) NULL) == FALSE)
		{
			i3Log( "_EnumControllerCallback()", "Could not create i3InputController Object.");
			pNew->Release();
			return FALSE;
		}

		SetController( 0, pNew);
	}

	return TRUE;
}


#endif
