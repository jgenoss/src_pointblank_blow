#include "i3InputType.h"
#include "i3InputDeviceManager.h"
#include "i3InputGlobalVariable.h"

#if defined( I3_WINCE)

i3InputDeviceManager::i3InputDeviceManager(void)
{
	m_pTimer = NULL;
	memset( m_pController, 0, sizeof(m_pController));

	m_ControllerCount = 0;

	InitCommon();
}

i3InputDeviceManager::~i3InputDeviceManager(void)
{
	DestroyCommon();

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

	return TRUE;
}


#endif
