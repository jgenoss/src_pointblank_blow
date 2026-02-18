// 스레드를 돌면서 처리한다
#include "pch.h"
#include "ModuleTask.h"
#include "ModuleFileManager.h"
#include "ModuleNetManager.h"
#include "ModuleConfig.h"

cModuleTask * g_pModuleTask = NULL; 

cModuleTask::cModuleTask()
{
	m_pFileManager	= nullptr;
	m_pNetManager	= nullptr;
}

cModuleTask::~cModuleTask()
{
	Destroy(); 
}

INT32 cModuleTask::Create(void)
{
	// Module DB Manager
	m_pFileManager = new CModuleFileManager;
	if( nullptr == m_pFileManager )																return SERVER_ERROR_MODULE_DATABASE;//return false;
	if( false   == m_pFileManager->Create( g_pConfig->m_ui32ServerCount, 
		g_pConfig->m_ui8WriteDBThreadCount, g_pConfig->m_strServiceLogFilePath ) )				return SERVER_ERROR_DATABASE;

	// Module NetWork Manager
	m_pNetManager		= new CModuleNetManager; 
	if( nullptr == m_pNetManager )																	return SERVER_ERROR_MODULE_NETWORK;
	if( false   == m_pNetManager->Create( g_pConfig->m_ui32Ip, g_pConfig->m_ui16Port, g_pConfig->m_ui32ServerCount,
		 g_pConfig->m_pui32ServerIP, m_pFileManager ) )									return SERVER_ERROR_NETWORK;

	return SERVER_ERROR_NO; 
}

void cModuleTask::Destroy(void)
{
	I3_SAFE_DELETE( m_pNetManager );
	I3_SAFE_DELETE( m_pFileManager );
}