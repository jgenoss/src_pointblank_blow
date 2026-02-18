#include "stdafx.h"
#include "UdpReceiver.h"

I3_CLASS_INSTANCE(CUdpReceiver, i3Thread); 

CUdpReceiver::CUdpReceiver()
{
	m_bIsRun	 = FALSE; 
}

CUdpReceiver::~CUdpReceiver()
{
	m_bIsRun = FALSE; 
	OnDestroy(); 
	Sleep(1000); 
}

void CUdpReceiver::BeforeRunning( void * pUserData)
{
	return; 
}

UINT32 CUdpReceiver::OnRunning( void * pUserData)
{
	I3SOCKETADDR RecvAddr;
	INT32	size; 
	INT32	FromSize; 
	char	pBuffer[8190]; 
	SOCKADDR_IN	Addr; 
	while(m_bIsRun)
	{
		FromSize = sizeof(I3SOCKETADDR); 
		size = m_UdpSocket.Recv( pBuffer, 8190, 0, &RecvAddr, &FromSize);

		//i3mem::Copy(&Addr, &RecvAddr, sizeof( SOCKADDR_IN )); 
		//sprintf(pBuffer, "0x%x %d", Addr.sin_addr.s_addr, htons(Addr.sin_port)); 
		InsertList(0, 0.0f, pBuffer); 
	}
	return -1; 
}

void CUdpReceiver::AfterRunning( void * pUserData)
{
	return; 
}

BOOL CUdpReceiver::OnCreate(INT32 Port, CListCtrl	* pListControl)
{
	 m_pLog_Control = pListControl; 

	//Bind 
	m_UdpSocket.BindAny(Port); 	

	//Create 
	m_bIsRun = TRUE; 
	Create("CUdpReceiver", 0, 4096, 0, PRIORITY_LOW);
		
	return TRUE; 
}

BOOL CUdpReceiver::OnDestroy(void) 
{
	return TRUE; 
}

void CUdpReceiver::InsertList(INT32 LogLevel, REAL32 Time, char * pString)
{
	m_Mutex.Enter(); 
	{
		INT32 Idx; 
		Idx = m_pLog_Control->InsertItem(0, "Log"); 
		m_pLog_Control->SetItem( Idx, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0, 0);
		m_pLog_Control->SetItemText( Idx, 1, pString);
		m_pLog_Control->SetItemText( Idx, 2, "0.0f");
		
		char alltime[20];
		char cdate[9];
		char ctime[9];

		_strdate( cdate ); 
		_strtime( ctime ); 
		sprintf(alltime, "%s[%s]", ctime, cdate); 
		m_pLog_Control->SetItemText( Idx, 3, alltime);

	}
	m_Mutex.Leave(); 

	return; 
}

void CUdpReceiver::InsertBlank(void)
{
	//InsertList(INT32 LogLevel, REAL32 Time, char * pString)
	InsertList(0, 0.0f, "================================="); 
}

void CUdpReceiver::CleanList(void)
{
	m_Mutex.Enter(); 
	{
		m_pLog_Control->DeleteAllItems(); 
	}
	m_Mutex.Leave(); 
	return; 
}
