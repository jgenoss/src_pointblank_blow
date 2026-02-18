// Pbssctl.cpp : Defines the entry point for the console application.
//
#include "pch.h"

#ifdef BUILDMODE_SEV

#include "../ServerDef.h"

void Install(char* pstrPath, char* pstrName);
void UnInstall(char* pstrName);
BOOL RunService(char* pstrName);
BOOL StopService(char* pstrName);

void StartService(void); 
void MainThread(void *);

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);

char g_strServiceName[512];
BOOL bProcessStarted = FALSE; 
SERVICE_STATUS_HANDLE	hServiceStatusHandle;
SERVICE_STATUS			ServiceStatus;

SERVICE_TABLE_ENTRY		lpServiceStartTable[] = 
{
	{g_strServiceName, ServiceMain},
	{NULL, NULL}
};

int _tmain(int argc, _TCHAR* argv[])
{
	char lpCmdLineData[512];
	char pModuleFile[512];
	char strExeFile[512];
 
	//Set Current Directory; 
	{
		INT32 StrLen;

		i3String::Copy( g_strCCurrentPath, argv[0], PATH_STRING_COUNT);
		StrLen = i3String::Length(g_strCCurrentPath);
		for(INT32 i = (StrLen - 1); i > 0; i--)
		{
			if( g_strCCurrentPath[i] == '\\')
			{
				g_strCCurrentPath[i] = 0; 
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
		i3String::Format(strExeFile, 512,"%s",pModuleFile);
		pModuleFile[dwSize-4] = 0;	
	}

	strcpy(g_strServiceName,"_PBSIAServerService");

	//2. Run 

	if(		_stricmp("-i",lpCmdLineData) == 0 || _stricmp("-I",lpCmdLineData) == 0)	Install(strExeFile, g_strServiceName);	//1. 인스톨	
	else if(_stricmp("-u",lpCmdLineData) == 0 || _stricmp("-U",lpCmdLineData) == 0)	UnInstall(g_strServiceName);			//2. 언인스톨
	else if(_stricmp("-s",lpCmdLineData) == 0 || _stricmp("-S",lpCmdLineData) == 0)	RunService(g_strServiceName);			//3. 서비스 시작.
	else if(_stricmp("-e",lpCmdLineData) == 0 || _stricmp("-E",lpCmdLineData) == 0)	StopService(g_strServiceName);			//4. 서비스 중단.
	else if(_stricmp("-h",lpCmdLineData) == 0 || _stricmp("-H",lpCmdLineData) == 0)	
	{
		printf("Shop Inventory Agent!\n\n"); 
		printf("siass.exe -i  : Service Install \n"); 
		printf("siass.exe -u  : Service Uninstall. \n"); 
		printf("siass.exe -s  : Service Start. \n");
		printf("siass.exe -e  : Service End. \n");
		printf("siass.exe	  : It does not use. \n");
	}
	else		StartService();						//6. 메인 함수 

	return 0;
}

void StartService(void) 
{
	INT32 i32InitValue = g_InitServer();
	if( EVENT_SUCCESS != i32InitValue )
	{
		switch(i32InitValue)
		{
		case SERVER_ERROR_CONFIGINI			:			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"EVENT_FAIL_CREATE_INI" );			break; 
		case SERVER_ERROR_LOGFILE			:			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"EVENT_FAIL_CREATE_LOG" );			break; 
		case SERVER_ERROR_INI_VALUE			: 			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"EVENT_FAIL_CREATE_INI_VALUE" );		break;
		case SERVER_ERROR_CONTROLSERVER		: 			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"EVENT_FAIL_CREATE_CONTROL" );		break; 
		case SERVER_ERROR_SYSTEMLOG			: 			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"EVENT_FAIL_CREATE_SYSTEMLOG");		break;
		//g_InitServer에서 -> g_StartServer가 사용하는 부분
		//case SERVER_ERROR_TASK			:			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, "EVENT_FAIL_CREATE_TASK" );			break; 
		//case SERVER_ERROR_DATABASE		: 			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, "EVENT_FAIL_CREATE_DATABASE" );		break; 
		//case SERVER_ERROR_LINK			:			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, "EVENT_FAIL_CREATE_LINK" );			break; 
		//case SERVER_ERROR_NETWORK			:			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, "EVENT_FAIL_CREATE_NETWORK" );		break; 
		default								: 			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"ERROR UNKNOWN" );					break; 
		}

		// 에러가 났다면 컨트롤 서버로 전달
		/*NET_I3_PACKET_BUFFER	SendErrorPacket(PROTOCOL_ASC_SERVER_LOADING_RESULT);
		SendErrorPacket.WriteData(&iInitValue, sizeof(INT32));
		g_pModuleControl->SendPacketMessage((char*)&SendErrorPacket, SendErrorPacket._ui16TotalSize);*/

		Sleep(500);
		g_DestroyServer(i32InitValue);
		return; 
	}

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
	UINT32 ui32IntervalForFrame = SERVER_MAIN_UPDATE_TIME;
	HANDLE hSyncEvent = CreateEvent( NULL, TRUE, TRUE, NULL);
	timeSetEvent( ui32IntervalForFrame, 2, (LPTIMECALLBACK) hSyncEvent, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	//_ASSERT( hSyncEvent != NULL );

	while(bProcessStarted)
	{
		g_UpdateServer(); 
		WaitForSingleObject( hSyncEvent, INFINITE);		
		ResetEvent( hSyncEvent);
	}
	return; 
}

void Install(char* pstrPath, char* pstrName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char strTemp[128];
		i3String::Format(strTemp, 128, "OpenSCManager failed, error code = %d\n", nError);
		printf("%s\n", strTemp);
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
			printf("%s\n", strTemp);
		}
		else
		{
			//성공 
			char strDes[MAX_PATH]; 
			i3String::Copy( strDes, "Point Blank ShopInventoryAgent Server", MAX_PATH );
			SERVICE_DESCRIPTION lpDes;
			lpDes.lpDescription = strDes;
			ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &lpDes);

			char strTemp[128];
			i3String::Format(strTemp, 128, "Service %s installed\n", pstrName);
			printf("%s\n", strTemp);
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
		printf("%s\n", strTemp);
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pstrName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char strTemp[128];
			i3String::Format(strTemp, 128, "OpenService failed, error code = %d\n", nError);
			printf("%s\n", strTemp);
		}
		else
		{
			if(!DeleteService(schService)) 
			{
				char strTemp[128];
				i3String::Format(strTemp, 128, "Failed to delete service %s\n", pstrName);
				printf("%s\n", strTemp);
			}
			else 
			{
				char strTemp[128];
				i3String::Format(strTemp, 128, "Service %s removed\n",pstrName);
				printf("%s\n", strTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);
	}	
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
		printf("%s\n", strTemp);
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
			printf("%s\n", strTemp);
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
				printf("%s\n", strTemp);
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
		printf("%s\n", strTemp);
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
			printf("%s\n", strTemp);
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
				printf("%s\n", strTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	ServiceStatus.dwServiceType			= SERVICE_WIN32;
	ServiceStatus.dwCurrentState		= SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted	= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	ServiceStatus.dwWin32ExitCode		= 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint			= 0;
	ServiceStatus.dwWaitHint			= 0;

	hServiceStatusHandle = RegisterServiceCtrlHandler(g_strServiceName, ServiceHandler);
	if (hServiceStatusHandle==0) 
	{
		Beep( 750, 100 );
		return;
	}

	// Initialization complete - report running status
	ServiceStatus.dwCurrentState		= SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint			= 0;
	ServiceStatus.dwWaitHint			= 0;
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
			ServiceStatus.dwWin32ExitCode	= 0;
			ServiceStatus.dwCurrentState	= SERVICE_STOPPED;
			ServiceStatus.dwCheckPoint		= 0;
			ServiceStatus.dwWaitHint		= 0;
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
#endif