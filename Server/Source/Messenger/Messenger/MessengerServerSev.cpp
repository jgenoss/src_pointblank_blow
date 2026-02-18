// Messenger.cpp : Defines the entry point for the console application.
// Server Service Program 

#include "pch.h"

#ifdef BUILDMODE_SEV

#include "LogFile.h"

//Install Uninstall 
void Install(char* pstrPath, char* pstrName);
void UnInstall(char* pstrName);
BOOL RunService(char* pstrName);
BOOL StopService(char* pstrName);

//Run
void StartService(void);
void MainThread(void *);

//Service Main And Handler 
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);

char g_strServiceName[512];
BOOL g_bProcessStarted = FALSE;
SERVICE_STATUS_HANDLE   g_hServiceStatusHandle;
SERVICE_STATUS          g_ServiceStatus;

SERVICE_TABLE_ENTRY		lpServiceStartTable[] = 
{
	{g_strServiceName, ServiceMain},
	{NULL, NULL}
};

int _tmain(int argc, _TCHAR* argv[])
{	
	//Set Current Directory; 
	{
		INT32 i32StrLen;
		char strTemp[512];

		i3String::NCopy(strTemp, argv[0], 512);
		i32StrLen = i3String::Length(strTemp);
		for(INT32 i = (i32StrLen - 1); i > 0; i--)
		{
			if( strTemp[i] == '\\')
			{
				strTemp[i] = 0; 
				break; 
			}
		}
		g_SetCurrentPath(strTemp);
	}

	char strCmdLineData[512];
	char strModuleFile[512];
	char strExeFile[512];

	//1. Pre Set Value 
	if(argc >= 2) {
		i3String::NCopy(strCmdLineData, argv[1], 512);
	}
	else {
		i3String::NCopy(strCmdLineData, "", 512);
	}

	DWORD dwSize = GetModuleFileName(NULL, strModuleFile, 512);
	strModuleFile[dwSize] = 0;
	if(dwSize>4 && strModuleFile[dwSize-4] == '.')
	{
		i3String::Format(strExeFile, 512, "%s", strModuleFile);
		strModuleFile[dwSize-4] = 0;	
	}
	i3String::NCopy(g_strServiceName,"_PBMessengerServerService", 512);

	//2. Run 
	if(		_stricmp("-i",strCmdLineData) == 0 || _stricmp("-I",strCmdLineData) == 0)	Install(strExeFile, g_strServiceName);	//1. 인스톨	
	else if(_stricmp("-u",strCmdLineData) == 0 || _stricmp("-U",strCmdLineData) == 0)	UnInstall(g_strServiceName);			//2. 언인스톨
	else if(_stricmp("-s",strCmdLineData) == 0 || _stricmp("-S",strCmdLineData) == 0)	RunService(g_strServiceName);			//3. 서비스 시작.
	else if(_stricmp("-e",strCmdLineData) == 0 || _stricmp("-E",strCmdLineData) == 0)	StopService(g_strServiceName);			//4. 서비스 중단.
	else if(_stricmp("-h",strCmdLineData) == 0 || _stricmp("-H",strCmdLineData) == 0)
	{
		printf("PbMessenger Server Help!\n\n"); 
		printf("PbMessengerss.exe -i : Service Install \n"); 
		printf("PbMessengerss.exe -u : Service Uninstall. \n"); 
		printf("PbMessengerss.exe -s : Service Start. \n");
		printf("PbMessengerss.exe -e : Service End. \n");
		printf("PbMessengerss.exe : It does not use. \n");
	}
	else {
		StartService();						//6. 메인 함수 
	}

	return 0;
}
 
void MainThread(void *)
{
	UINT32 ui32IntervalForFrame = SERVER_MAIN_UPDATE_TIME; 
	HANDLE hSyncEvent = CreateEvent( NULL, TRUE, TRUE, NULL);
	timeSetEvent( ui32IntervalForFrame, 2, (LPTIMECALLBACK) hSyncEvent, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	I3ASSERT( hSyncEvent != NULL );

	while(g_bProcessStarted)
	{
		g_UpdateServer();
		WaitForSingleObject( hSyncEvent, INFINITE);
		ResetEvent( hSyncEvent);
	}
	return; 
}

BOOL RunService(char* pstrName)
{
	// run service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char strTemp[128];
		i3String::Format(strTemp, 128, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",strTemp); 
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pstrName, SERVICE_ALL_ACCESS);
		if (schService==0)
		{
			long nError = GetLastError();
			char strTemp[128];
			i3String::Format(strTemp, 128, "OpenService failed, error code = %d\n", nError);
			printf("%s\n",strTemp); 
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
				char strTemp[128];
				i3String::Format(strTemp, 128, "StartService failed, error code = %d\n", nError);
				printf("%s\n",strTemp); 
			}
			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schSCManager);
	}
	return FALSE;
}

BOOL StopService(char* pstrName)
{
	// Stop service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char strTemp[128];
		i3String::Format(strTemp, 128, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",strTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pstrName, SERVICE_ALL_ACCESS);
		if (schService==0)
		{
			long nError = GetLastError();
			char strTemp[128];
			i3String::Format(strTemp, 128, "OpenService failed, error code = %d\n", nError);
			printf("%s\n",strTemp);
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
				char strTemp[128];
				i3String::Format(strTemp, 128, "ControlService failed, error code = %d\n", nError);
				printf("%s\n",strTemp);
			}
			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schSCManager);
	}
	return FALSE;
}


void StartService(void)
{	
	g_bProcessStarted =  TRUE;

	//InitServer	
	INT32 i32Rv = g_InitServer(); 
	if(i32Rv != EVENT_ERROR_SUCCESS )
	{
		switch( i32Rv )
		{
		case SERVER_ERROR_NETWORK		: Beep( 750, 300 );																			break; 
		case SERVER_ERROR_CONFIGINI		: Beep( 750, 600 );																			break; 
		case SERVER_ERROR_LOGFILE		: Beep( 750, 1000 );																		break; 
		case SERVER_ERROR_SYSTEMLOG		: if( g_pLog != NULL )	g_pLog->WriteLog( L"SystemLog Error!");				break; 
		case SERVER_ERROR_CONTROL		: if( g_pLog != NULL )	g_pLog->WriteLog( L"ControlInfo Error!");			break; 
		case SERVER_ERROR_CONTROLSERVER	: if( g_pLog != NULL )	g_pLog->WriteLog( L"ControlServer Error!");			break;  		
		default							: Beep( 750, 1000 );																		break;	
		//g_InitServer에서 g_StartServer로 작업위치 변경
		//case SERVER_ERROR_DATABASE		: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"Database Error!");			break; 
		//case SERVER_ERROR_CONTEXT			: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"Context Error!");				break; 
		//case SERVER_ERROR_PATH			: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"Path Error!");				break; 
		//case SERVER_ERROR_LOG				: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"Log Error!");					break; 
		//case SERVER_ERROR_CONTENT			: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"Content Error!");				break; 
		//case SERVER_ERROR_PACKETLOCKER	: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"PacketLocker Error!");		break; 
		//case SERVER_ERROR_GATEWAY			: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"GateWay Error!");				break; 
		//case SERVER_ERROR_TASK			: if( g_pLog != NULL )	g_pLog->WriteLog(g_i32MainLogIdx,"Task Error!");				break;
		}
		Sleep(500);
		g_DestroyServer( i32Rv );
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
    g_ServiceStatus.dwServiceType        = SERVICE_WIN32;
    g_ServiceStatus.dwCurrentState       = SERVICE_START_PENDING;
    g_ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
    g_ServiceStatus.dwWin32ExitCode      = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint         = 0;
    g_ServiceStatus.dwWaitHint           = 0;
 
    g_hServiceStatusHandle = RegisterServiceCtrlHandler(g_strServiceName, ServiceHandler);
    if (g_hServiceStatusHandle==0)
    {
		Beep( 750, 100 );
        return; 
    } 
 
    // Initialization complete - report running status 
    g_ServiceStatus.dwCurrentState       = SERVICE_RUNNING;
    g_ServiceStatus.dwCheckPoint         = 0;
    g_ServiceStatus.dwWaitHint           = 0;
    if(!SetServiceStatus(g_hServiceStatusHandle, &g_ServiceStatus))
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
			g_bProcessStarted = FALSE;
			g_ServiceStatus.dwWin32ExitCode = 0;
			g_ServiceStatus.dwCurrentState	= SERVICE_STOPPED;
			g_ServiceStatus.dwCheckPoint	= 0;
			g_ServiceStatus.dwWaitHint		= 0;

			// terminate all processes
			g_DestroyServer(SERVER_ERROR_CONTROLSERVER); 
			break; 
		case SERVICE_CONTROL_PAUSE:
			g_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			break;
		case SERVICE_CONTROL_CONTINUE:
			g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
		default:
			break;
	};

	if (!SetServiceStatus(g_hServiceStatusHandle,  &g_ServiceStatus))
	{
		Beep( 750, 100 );
	}
}


void Install(char* pstrPath, char* pstrName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char strTemp[128];
		i3String::Format(strTemp, 128, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",strTemp);
	}
	else
	{
		SC_HANDLE schService = CreateService
		( 
			schSCManager,				/* SCManager database      */ 
			pstrName,					/* name of service         */ 
			pstrName,					/* service name to display */ 
			SERVICE_ALL_ACCESS,			/* desired access          */ 
			SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
			SERVICE_DEMAND_START,		/* start type              */ 
			SERVICE_ERROR_NORMAL,		/* error control type      */ 
			pstrPath,					/* service's binary        */ 
			NULL,						/* no load ordering group  */ 
			NULL,						/* no tag identifier       */ 
			NULL,						/* no dependencies         */ 
			NULL,						/* LocalSystem account     */ 
			NULL						/* no password             */ 
		); 

		if (schService == 0) 
		{
			long nError =  GetLastError();
			char strTemp[128];
			i3String::Format(strTemp, 128, "Failed to create service %s, error code = %d\n", pstrName, nError);
			printf("%s\n",strTemp); 
		}
		else
		{
			//성공 
			char strDes[MAX_STRING_COUNT];
			i3String::Copy( strDes, "Point Blank Messenger Server", MAX_STRING_COUNT );
			SERVICE_DESCRIPTION lpDes;
			lpDes.lpDescription = strDes;
			ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &lpDes);

			char strTemp[128];
			i3String::Format(strTemp, 128, "Service %s installed\n", pstrName);
			printf("%s\n",strTemp);
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);
	}	
}

void UnInstall(char* pstrName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char strTemp[128];
		i3String::Format(strTemp, 128, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n",strTemp);
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pstrName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char strTemp[128];
			i3String::Format(strTemp, 128, "OpenService failed, error code = %d\n", nError);
			printf("%s\n",strTemp);
		}
		else
		{
			if(!DeleteService(schService))
			{
				char strTemp[128];
				i3String::Format(strTemp, 128, "Failed to delete service %s\n", pstrName);
				printf("%s\n",strTemp);
			}
			else 
			{
				char strTemp[128];
				i3String::Format(strTemp, 128, "Service %s removed\n",pstrName);
				printf("%s\n",strTemp);
			}
			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schSCManager);
	}
}

#endif