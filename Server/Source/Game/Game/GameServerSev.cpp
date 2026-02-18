// PbServerss.cpp : Defines the entry point for the console application.
// Server Service Program 

#include "pch.h"

#ifdef BUILDMODE_SEV

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

i3Timer					g_i3Timer;

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
	// QA 일때만 서비스 이름을 바꿉니다.
	strcpy(pServiceName,"_PBGameServerService");

	//2. Run 
	if(		_stricmp("-i",lpCmdLineData) == 0 || _stricmp("-I",lpCmdLineData) == 0)	Install(pExeFile, pServiceName);	//1. 인스톨	
	else if(_stricmp("-u",lpCmdLineData) == 0 || _stricmp("-U",lpCmdLineData) == 0)	UnInstall(pServiceName);			//2. 언인스톨
	else if(_stricmp("-s",lpCmdLineData) == 0 || _stricmp("-S",lpCmdLineData) == 0)	RunService(pServiceName);			//3. 서비스 시작.
	else if(_stricmp("-e",lpCmdLineData) == 0 || _stricmp("-E",lpCmdLineData) == 0)	StopService(pServiceName);			//4. 서비스 중단.
	else if(_stricmp("-h",lpCmdLineData) == 0 || _stricmp("-H",lpCmdLineData) == 0)	
	{
		printf("PB Server Help!\n\n"); 
		printf("PBGames.exe -i : Service Install \n"); 
		printf("PBGames.exe -u : Service Uninstall. \n"); 
		printf("PBGames.exe -s : Service Start. \n");
		printf("PBGames.exe -e : Service End. \n");
		printf("PBGames.exe : It does not use. \n");
	}
	else																			StartService();						//6. 메인 함수 

	return 0;
}
 
void MainThread(void *)
{
	//UINT32 ui32IntervalForFrame = g_pContextMain->m_i32UpdateMainTime;

	INT32 i32Count = 10;	// 타임 체크를 위한 임시 코드입니다.
	INT32 i32Time;
	while(bProcessStarted)
	{
		g_i3Timer.GetDeltaSec();

		g_UpdateServer();

		i32Time = g_pContextMain->m_i32UpdateMainTime-((INT32)(g_i3Timer.GetDeltaSec() * 1000));
		if( i32Time > 0 )	::Sleep( i32Time );
		else
		{
			i32Count++;
			if( i32Count > 10 )
			{
				g_pLog->WriteLog( L"---------UPDATE TIME Check---------"); 
				i32Count = 0;
			}
		}
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
		printf("%s 1111 \n",pTemp);
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
			printf("%s 2222 \n",pTemp);			
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
				printf("%s 3333 \n",pTemp);				
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}


void StartService(void)
{	
	//InitServer
	i3MemoryInit(0); 
	INT32 Rv = g_InitServer(); 
	if(Rv != EVENT_ERROR_SUCCESS )
	{
		switch( Rv )
		{
		case SERVER_ERROR_NETWORK			: Beep( 750, 300 );																											break; 
		case SERVER_ERROR_CONFIGINI			: Beep( 750, 600 );																											break; 
		case SERVER_ERROR_LOGFILE			: Beep( 750, 1000 );																										break; 
		case SERVER_ERROR_CONTROL			: if( g_pLog != NULL )	g_pLog->WriteLog( L"Server Start Fail : Control Error!");							break; 
		default								: if( g_pLog != NULL )	g_pLog->WriteLog( L"Server Start Fail : UnKnown Error!");							break;
		}
		Sleep(500);
		g_DestroyServer(Rv);
		i3mem::Destroy();
	}
	else
	{	
		bProcessStarted =  TRUE; 
		if(_beginthread(MainThread, 0, NULL) == -1)
		{
			g_pLog->WriteLog( L"Server Start Fail : BeginThread Error!"); 
			bProcessStarted =  FALSE; 
			return; 
		}

		if(!StartServiceCtrlDispatcher(lpServiceStartTable))
		{
			g_pLog->WriteLog( L"Server Start Fail : tartServiceCtrlDispatcher Error!"); 
			bProcessStarted =  FALSE; 
			return; 
		}
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
			g_DestroyServer(SERVER_ERROR_CONTROLSERVER); 
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
			i3String::Copy( Des, "Point Blank Game Server", MAX_STRING_COUNT );
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

#endif