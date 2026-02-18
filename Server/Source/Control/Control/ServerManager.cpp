#include "pch.h"
#include "Servermanager.h"


CServerControlManager * g_pServerControlManager = NULL; 

I3_CLASS_INSTANCE( CServerControlManager, i3ElementBase);

CServerControlManager::CServerControlManager()
{

}

CServerControlManager::~CServerControlManager()
{
	OnDestroy(); 
}

BOOL CServerControlManager::OnCreate(void)
{
	return TRUE; 
}

BOOL CServerControlManager::OnDestroy(void)
{
	return TRUE; 
}

/*----------------------------------------------------------
Name : OnUpdate 
Desc : 유저들을 상태를확인 합니다. 
----------------------------------------------------------*/
void CServerControlManager::OnUpdate(void)
{
	return; 
}
