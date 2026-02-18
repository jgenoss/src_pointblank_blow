// Relayss.cpp : Defines the entry point for the console application.
// Server Service Program 

#include "pch.h"
#include "../LogFile.h"

//Install Uninstall 
void Install(char* pPath, char* pName);
void UnInstall(char* pName);
BOOL RunService(char* pName);
BOOL StopService(char* pName);

//Run
void StartService(void); 
void MainThread(void *);

//Service Main And Handler 
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);

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
	//Set Current Directory; 
	{
		INT32 StrLen; 
		char Temp[512]; 

		i3String::Copy(Temp, argv[0], 512);
		StrLen = i3String::Length(Temp); 
		for(INT32 i = (StrLen - 1); i > 0; i--)
		{
			if( Temp[i] == '\\')
			{
				Temp[i] = 0; 
				break; 
			}
		}
		g_SetCurrentPath(Temp); 
	}

	char lpCmdLineData[512];
	char pModuleFile[512];
	char pExeFile[512];

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
	strcpy(pServiceName,"PBRelayServerService");

	//2. Run 
	if(		_stricmp("-i",lpCmdLineData) == 0 || _stricmp("-I",lpCmdLineData) == 0)	Install(pExeFile, pServiceName);	//1. 인스톨	
	else if(_stricmp("-u",lpCmdLineData) == 0 || _stricmp("-U",lpCmdLineData) == 0)	UnInstall(pServiceName);			//2. 언인스톨
	else if(_stricmp("-s",lpCmdLineData) == 0 || _stricmp("-S",lpCmdLineData) == 0)	RunService(pServiceName);			//3. 서비스 시작.
	else if(_stricmp("-e",lpCmdLineData) == 0 || _stricmp("-E",lpCmdLineData) == 0)	StopService(pServiceName);			//4. 서비스 중단.
	else if(_stricmp("-h",lpCmdLineData) == 0 || _stricmp("-H",lpCmdLineData) == 0)	
	{
		printf("Relay Server Help!\n\n"); 
		printf("Relayss.exe -i : Service Install \n"); 
		printf("Relayss.exe -u : Service Uninstall. \n"); 
		printf("Relayss.exe -s : Service Start. \n");
		printf("Relayss.exe -e : Service End. \n");
		printf("Relayss.exe : It does not use. \n");
	}
	else																			StartService();						//6. 메인 함수 

	return 0;
}
 
void MainThread(void *)
{
	UINT32 IntervalForFrame = SERVER_MAIN_UPDATE_TIME; 
	HANDLE hSyncEvent = CreateEvent( NULL, TRUE, TRUE, NULL);
	timeSetEvent( IntervalForFrame, 2, (LPTIMECALLBACK) hSyncEvent, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	I3ASSERT( hSyncEvent != NULL );

	while(bProcessStarted)
	{		
		g_UpdateServer(); 
		WaitForSingleObject( hSyncEvent, INFINITE);
		ResetEvent( hSyncEvent);
	}
	return; 
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
				printf("%s\n",pTemp); 
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


void StartService(void)
{	
	bProcessStarted =  TRUE; 

	//InitServer
	INT32 Rv = g_InitServer(); 
	if(Rv != EVENT_ERROR_SUCCESS )
	{
		switch( Rv )
		{
		case SERVER_ERROR_NETWORK:			Beep( 750, 300 );							break; 
		case SERVER_ERROR_CONFIG:			Beep( 750, 600 );							break; 
		case SERVER_ERROR_LOGFILE:			Beep( 750, 1000 );							break; 
		case SERVER_ERROR_SYSTEMLOG:		WRITE_LOG_MAIN("SystemLog Error!");			break;
		case SERVER_ERROR_CONTROLSERVER:	WRITE_LOG_MAIN("Control Server Error!");	break;
		case SERVER_ERROR_UDPMODULE:		WRITE_LOG_MAIN("UdpModule Error!");			break;
		case SERVER_ERROR_GROUPMAKER:		WRITE_LOG_MAIN("GroupMaker Error!");		break;
		default:
			WRITE_LOG_MAIN("UnKnown Error!");
			break;	
		}
		Sleep(500);
		g_DestroyServer();
		i3mem::Destroy();

		return;
	}

	if(_beginthread(MainThread, 0, NULL) == -1)
	{
		Beep( 750, 100 );
		return; 
	}

	if(!StartServiceCtrlDispatcher(lpServiceStartTable))
	{
		Beep( 750, 100 );
	}	
	return; 
}


VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
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
			g_DestroyServer(); 
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
			SERVICE_DEMAND_START,		/* start type              */ 
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
			char Des[MAX_STRING_COUNT];
			i3String::Copy( Des, "Point Blank Relay Server", MAX_STRING_COUNT );
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
