#include "i3InputType.h"
#include "i3InputDeviceManager.h"
#include "i3InputGlobalVariable.h"

#if defined( I3_WIPI)

i3InputDeviceManager::i3InputDeviceManager(void)
{
	m_pKeyboard = NULL;
	m_pMouse = NULL;
	m_pTimer = NULL;
	memset( m_pController, 0, sizeof(m_pController));

	m_ControllerCount = 0;

	InitCommon();
}

i3InputDeviceManager::~i3InputDeviceManager(void)
{
	DestroyCommon();

	I3_SAFE_RELEASE( m_pKeyboard);
	I3_SAFE_RELEASE( m_pMouse);
	I3_SAFE_RELEASE( m_pTimer);

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
	m_pTimer = i3Timer::NewObject();

	// Controller
	if( DeviceMask & I3I_DEVICE_MASK_KEYBOARD)
	{
		i3InputKeyboard * pNew;

		pNew = i3InputKeyboard::NewObjectRef();

		if( pNew->Create( (void *) NULL) == FALSE)
		{
			i3Log( "i3InputDeviceManager::Create()", "Could not create i3InputKeyboard Object.");
			pNew->Release();
			return FALSE;
		}

		SetKeyboard( pNew);
	}
	
	return TRUE;
}


#endif
