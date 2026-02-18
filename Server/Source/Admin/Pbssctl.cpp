// Pbssctl.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Control.h"
#include "LogFile.h"
void Install(char* pPath, char* pName);
void UnInstall(char* pName);
BOOL RunService(char* pName);
BOOL StopService(char* pName);

void StartService(void); 
void MainThread(void *);

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);

char g_strCurrentPath[512];
char pServiceName[512];
BOOL bProcessStarted = FALSE; 
SERVICE_STATUS_HANDLE   hServiceStatusHandle; 
SERVICE_STATUS          ServiceStatus; 

SERVICE_TABLE_ENTRY		lpServiceStartTable[] = 
{
	{pServiceName, ServiceMain},
	{NULL, NULL}
};

int _tmain(int argc, _TCHAR* argv[])
{
	char lpCmdLineData[512];
	char pModuleFile[512];
	char pExeFile[512];

	//Set Current Directory; 
	{
		INT32 StrLen; 		

		i3String::Copy(g_strCurrentPath, argv[0], 512 );
		StrLen = i3String::Length(g_strCurrentPath); 
		for(INT32 i = (StrLen - 1); i > 0; i--)
		{
			if( g_strCurrentPath[i] == '\\')
			{
				g_strCurrentPath[i] = 0; 
				break; 
			}
		}		
	}

	//1. Pre Set Value 
	if(argc >= 2)	strcpy(lpCmdLineData, argv[1]);
	else			strcpy(lpCmdLineData, ""); 	
	DWORD dwSize = GetModuleFileName(NULL, pModuleFile, 512);
	pModuleFile[dwSize] = 0;
	if(dwSize>4 && pModuleFile[dwSize-4] == '.')
	{
		sprintf(pExeFile,"%s",pModuleFile);
		pModuleFile[dwSize-4] = 0;	
	}

	strcpy(pServiceName,"ServiceAppController");

	//2. Run 

	if(		_stricmp("-i",lpCmdLineData) == 0 || _stricmp("-I",lpCmdLineData) == 0)	Install(pExeFile, pServiceName);	//1. 인스톨	
	else if(_stricmp("-u",lpCmdLineData) == 0 || _stricmp("-U",lpCmdLineData) == 0)	UnInstall(pServiceName);			//2. 언인스톨
	else if(_stricmp("-s",lpCmdLineData) == 0 || _stricmp("-S",lpCmdLineData) == 0)	RunService(pServiceName);			//3. 서비스 시작.
	else if(_stricmp("-e",lpCmdLineData) == 0 || _stricmp("-E",lpCmdLineData) == 0)	StopService(pServiceName);			//4. 서비스 중단.
	else if(_stricmp("-h",lpCmdLineData) == 0 || _stricmp("-H",lpCmdLineData) == 0)	
	{
		printf("PB ServiceAppController Help!\n\n"); 
		printf("ServiceAppController.exe -i  : Service를 인스톨 합니다. \n"); 
		printf("ServiceAppController.exe -u  : Service를 언인스톨 합니다. \n"); 
		printf("ServiceAppController.exe -s  : Service를 시작합니다. \n");
		printf("ServiceAppController.exe -e  : Service를 종료합니다. \n");
		printf("ServiceAppController.exe		: 사용하지 못합니다. \n");
	}
	else		StartService();						//6. 메인 함수 

	return 0;
}

void StartService(void) 
{
	char Temp[256];
	//Create Memory
	i3MemoryInit(0); 	

	//Create Network 
	if(!i3Net::Init() )return; 

	g_pControl = CControl::NewObject(); 
	i3String::Copy( Temp, g_strCurrentPath, 256);
	i3String::Concat(Temp, "\\Config.ini"); 

	if( !g_pControl->Create(Temp) )
	{		
		return; 
	}

	g_pLogFile = CLogFile::NewObject(); 
	i3String::Copy( Temp, g_strCurrentPath, 256);
	i3String::Concat(Temp, "\\PBssCtrlLog.txt"); 
	if(!g_pLogFile->OnCreate(Temp) )return; 

	bProcessStarted =  TRUE; 
	if(_beginthread(MainThread, 0, NULL) == -1) 
	{
		bProcessStarted =  FALSE; 
		return; 
	}

	if(!StartServiceCtrlDispatcher(lpServiceStartTable))
	{
		bProcessStarted =  FALSE; 
		return; 
	}	
	return; 
}

void MainThread(void *)
{
	UINT32 IntervalForFrame = 500; 
	HANDLE hSyncEvent = CreateEvent( NULL, TRUE, TRUE, NULL);
	timeSetEvent( IntervalForFrame, 2, (LPTIMECALLBACK) hSyncEvent, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	I3ASSERT( hSyncEvent != NULL );

	INT32 Rv;
	while(bProcessStarted)
	{	
		Rv = g_pControl->Update(); 
		switch(Rv)
		{
		case 1: //실행 
			RunService( g_pControl->m_pServiceName ); 
			break; 
		case 2: //종료
			StopService( g_pControl->m_pServiceName ); 
			break; 
		}		
		WaitForSingleObject( hSyncEvent, INFINITE);
		ResetEvent( hSyncEvent);
	}
	return; 
}

void Install(char* pPath, char* pName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[128];
		sprintf(pTemp, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",pTemp); 		
	}
	else
	{
		SC_HANDLE schService = CreateService
		( 
			schSCManager,				/* SCManager database      */ 
			pName,						/* name of service         */ 
			pName,						/* service name to display */ 
			SERVICE_ALL_ACCESS,			/* desired access          */ 
			SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
			SERVICE_AUTO_START,			/* start type              */ 
			SERVICE_ERROR_NORMAL,		/* error control type      */ 
			pPath,						/* service's binary        */ 
			NULL,						/* no load ordering group  */ 
			NULL,						/* no tag identifier       */ 
			NULL,						/* no dependencies         */ 
			NULL,						/* LocalSystem account     */ 
			NULL						/* no password             */ 
		); 

		if (schService == 0) 
		{
			long nError =  GetLastError();
			char pTemp[128];
			sprintf(pTemp, "Failed to create service %s, error code = %d\n", pName, nError);
			printf("%s\n",pTemp); 
		}
		else
		{
			//성공 
			char Des[] = "Control PointBlank Server service."; 
			SERVICE_DESCRIPTION lpDes;
			lpDes.lpDescription = Des;
			ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &lpDes);

			char pTemp[128];
			sprintf(pTemp, "Service %s installed\n", pName);
			printf("%s\n",pTemp); 			
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);
	}	
}

void UnInstall(char* pName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[128];
		sprintf(pTemp, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",pTemp); 
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[128];
			sprintf(pTemp, "OpenService failed, error code = %d\n", nError);
			printf("%s\n",pTemp); 			
		}
		else
		{
			if(!DeleteService(schService)) 
			{
				char pTemp[128];
				sprintf(pTemp, "Failed to delete service %s\n", pName);
				printf("%s\n",pTemp); 
			}
			else 
			{
				char pTemp[128];
				sprintf(pTemp, "Service %s removed\n",pName);
				printf("%s\n",pTemp); 
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);	
	}	
}


BOOL RunService(char* pName)
{
	// run service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[128];
		sprintf(pTemp, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",pTemp); 
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[128];
			sprintf(pTemp, "OpenService failed, error code = %d\n", nError);
			printf("%s\n",pTemp); 
		}
		else
		{
			// call StartService to run the service
			if(StartService(schService, 0, (const char**)NULL))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTemp[128];
				sprintf(pTemp, "StartService failed, error code = %d\n", nError);
				printf("%s 1111 \n",pTemp); 
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

BOOL StopService(char* pName)
{
	// Stop service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[128];
		sprintf(pTemp, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[128];
			sprintf(pTemp, "OpenService failed, error code = %d\n", nError);
			printf("%s\n",pTemp);			
		}
		else
		{
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTemp[128];
				sprintf(pTemp, "ControlService failed, error code = %d\n", nError);
				printf("%s\n",pTemp);				
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	DWORD   status = 0; 
    DWORD   specificError = 0xfffffff; 
 
    ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    ServiceStatus.dwWin32ExitCode      = 0; 
    ServiceStatus.dwServiceSpecificExitCode = 0; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0; 
 
    hServiceStatusHandle = RegisterServiceCtrlHandler(pServiceName, ServiceHandler); 
    if (hServiceStatusHandle==0) 
    {
		Beep( 750, 100 );
        return; 
    } 
 
    // Initialization complete - report running status 
    ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0;  
    if(!SetServiceStatus(hServiceStatusHandle, &ServiceStatus)) 
    { 
		Beep( 750, 100 );
	} 
}

VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	switch(fdwControl) 
	{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:			
			bProcessStarted = FALSE;
			ServiceStatus.dwWin32ExitCode = 0; 
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
			ServiceStatus.dwCheckPoint    = 0; 
			ServiceStatus.dwWaitHint      = 0;
			// terminate all processes
			g_pControl->Destroy(); 
			I3_SAFE_RELEASE(g_pControl); 			
			break; 
		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
			break;
		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
		default:
			break; 	
	};

    if (!SetServiceStatus(hServiceStatusHandle,  &ServiceStatus)) 
	{ 
		Beep( 750, 100 );
    } 
}

