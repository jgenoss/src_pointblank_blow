
#include "stdafx.h"
#include <process.h>
#include "Patroller_Thread.h"
#include "Patroller_Client.h"
#include "Patroller_Shared_Struct.h"

Patroller_Thread::Patroller_Thread()
{
	::InitializeCriticalSection( &m_cs );

	m_bExit = false;
	m_CurrentProtocol = PAT::PROTOCOL_LIVE_CHECK;
	memset(m_SendBuf,0,sizeof(m_SendBuf));

	m_fElapsedTime = 0.0f;
	m_uiOldTime = 0;
	m_bTimerInit = false;
}

Patroller_Thread::~Patroller_Thread()
{
	::DeleteCriticalSection( &m_cs );
}

float Patroller_Thread::GetDeltaSec(void)
{
	float dt;
	UINT32 uiTime = timeGetTime();

	if( m_bTimerInit == false)
	{
		dt = 0.0f;
		m_bTimerInit = true;
	}
	else
	{
		dt = (float)((uiTime - m_uiOldTime) * 0.001f);
	}

	m_uiOldTime = uiTime;

	return dt;
}

UINT32 Patroller_Thread::OnRunning()
{
	UINT32 nSize = 0;
	DWORD dwWrite = 0;
	UINT8 nDisConnectCnt = 0;
	HANDLE hPipe = NULL;
	HANDLE hEvent = NULL;
	float dt = 0.0f;

	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTNAME);

	hPipe = CreateFile(
		PIPENAME,
		GENERIC_WRITE, 
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 

	if ( hEvent == INVALID_HANDLE_VALUE || hPipe == INVALID_HANDLE_VALUE )
	{
		SendErrorLog(ZGuard::CREATE_HANDLE_FAIL);
		ZG_cbProc(ZGuard::CREATE_HANDLE_FAIL);
		//return 0;
	}

	while(!m_bExit)
	{
		dt = GetDeltaSec();
		m_fElapsedTime += dt;
		if ( m_fElapsedTime > TIME_STAMP_FOR_DLL_LIST )
		{
			if ( m_CurrentProtocol == PAT::PROTOCOL_LIVE_CHECK )
				m_CurrentProtocol = PAT::PROTOCOL_DLL_LIST;
			m_fElapsedTime = 0.0f;
		}

		nSize = MakeSendData();

		WriteFile(hPipe,&m_SendBuf,nSize,&dwWrite,NULL);

		if ( dwWrite == 0 )
		{
			// Launcher 의 Patroller 는 이미 켜진 상태일 것이므로 이곳에서 Disconnect 처리 가능

 			if ( nDisConnectCnt >= 3 )
			{
				SendErrorLog(ZGuard::LAUNCHER_DISCONNECTED);
				ZG_cbProc(ZGuard::LAUNCHER_DISCONNECTED);
			//	return 0;
			}

			nDisConnectCnt ++;
		}

		SetEvent(hEvent);

		m_CurrentProtocol = PAT::PROTOCOL_LIVE_CHECK;

		WaitForSingleObject(GetHandle(),1000);
	}

	return 0;
}

UINT32 Patroller_Thread::MakeSendData(void)
{
	_AutoCs _auto(&m_cs);

	UINT32 nDataSize = 0;
	switch (m_CurrentProtocol)
	{
	case PAT::PROTOCOL_LIVE_CHECK:
		{
			IPC_DATA_LIVE_CHECK tData;
			tData._nSendTime = timeGetTime();
			nDataSize = tData.CopyToBuffer(m_SendBuf);
		}
		break;

	case PAT::PROTOCOL_ACCOUNT_INFO:
		{
			IPC_DATA_ACCOUNT_INFO tData;
			tData._nTUID = m_nUID;
			nDataSize = tData.CopyToBuffer(m_SendBuf);
		}
		break;

	case PAT::PROTOCOL_DLL_LIST:
		{
			IPC_DATA_DLL_LIST tData;
			SetDLLPath(&tData);
			nDataSize = tData.CopyToBuffer(m_SendBuf);
		}
		break;

	}
	return nDataSize;
}

void Patroller_Thread::SetProtocolOnce(UINT32 nProtocol)
{
	_AutoCs _auto(&m_cs);
	m_CurrentProtocol = nProtocol;
}

void Patroller_Thread::SetDLLPath(IPC_DATA_DLL_LIST* pData)
{

	DWORD dwPEB;
	__asm
	{
		push eax
			mov eax,fs:[0x30]
		mov dwPEB, eax
			pop eax
	}

	PPEB pPEB = (PPEB)dwPEB;


	PPEB_LDR_DATA pLDR;
	PLDR_MODULE pLM;

	pLDR = (PEB_LDR_DATA*)pPEB->Ldr;

	PLIST_ENTRY pLE = pLDR->InLoadOrderModuleList.Flink;
	INT32 Count =0;

	while(pLM!= NULL){

		pLM = CONTAINING_RECORD(pLE, _LDR_MODULE, InLoadOrderLinks);
		char Temp[MAX_PATH];
		WideCharToMultiByte(CP_ACP,0,pLM->FullDllName.Buffer,-1,Temp,sizeof(Temp),NULL,NULL);


		strcpy_s(pData->_szDLLList[Count],MAX_PATH,Temp);
		Count++;

		if (pLM->BaseAddress == 0) break;

		pLE = pLM->InLoadOrderLinks.Flink;
	}
}

void Patroller_Thread::SendErrorLog(ZGuard::ERROR_CODE eErr)
{

	char tErrorCode[TAG_SIZE] = {0,};
	sprintf(tErrorCode,"%#010X",eErr);
	tErrorCode[1] = 'x';


	RemoteLog::InitErrorLog(
		"PB",
		tErrorCode
		);

	if (RemoteLog::IsAvailableSendState(RemoteLog::ERROR_LOG))
	{
		RemoteLog::SendRemoteLog(RemoteLog::ERROR_LOG, "Patroller Error", 10.0f, 0x0000);
	}

}


