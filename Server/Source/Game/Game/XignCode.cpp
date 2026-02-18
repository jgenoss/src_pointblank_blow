#include "pch.h"
#include "XignCode.h"

CXignCode::CXignCode()
{
	m_pXignServer		= NULL;
}

CXignCode::~CXignCode()
{
	OnDestroy();
}
 
BOOL CXignCode::OnCreate(char* strPath)
{
	CreateXigncodeServer2	CXProc = LoadHelperDll2(strPath);
	if( !CXProc ) 
	{
		g_pLog->WriteLog( L"[START] Error GameGuard XIGNCODE / LoadHelperDll2 ");
		return FALSE;
	}

	if( !CXProc( &m_pXignServer, g_XignSendPacket, g_XignDisconnect ) )
	{
		g_pLog->WriteLog( L"[START] Error GameGuard XIGNCODE / CXProc() ");
		return FALSE;
	}

	if( !m_pXignServer->OnBegin( XIGNCODE_PACKET_SIZE ) )
	{
		g_pLog->WriteLog( L"[START] Error GameGuard XIGNCODE / OnBegin() ");
		return FALSE;
	}

	return TRUE;
}

void	CXignCode::OnDestroy(void)
{

}

void	CXignCode::Release(void)
{
	if( !m_pXignServer ) return;

	m_pXignServer->OnEnd();
	m_pXignServer->Release();
}
  
void		CXignCode::Connect(T_UID TUID, INT32 i32SessionIdx, UINT32 ui32IpAddress, char* strID)
{
	if( -1 == TUID ) return;	// 서버 이동입니다.

	m_pXignServer->OnAccept(TUID, (void*)&i32SessionIdx);
	m_pXignServer->SetUserInformationA( TUID, ui32IpAddress, strID );

	/*char strTemp[1024];
	i3String::Format( strTemp, 1024, "#### CXignCode::Connect() / Time:%d / uid : %I64d, ip : %d, ID : %s \n",
		 g_pContextMain->GetServerTime(),  T_UID, ui32IpAddress, strID );
	I3TRACE(strTemp);*/
}

void		CXignCode::DisConnect(T_UID TUID)
{
	if( -1 == TUID ) return;	// 서버 이동입니다.
 
	m_pXignServer->OnDisconnect(TUID);

	/*char strTemp[1024];
	i3String::Format( strTemp, 1024, "#### CXignCode::DisConnect() / Time:%d / uid : %I64d \n",
		   g_pContextMain->GetServerTime(), T_UID );
	I3TRACE(strTemp);*/
}

void		CXignCode::Recive(T_UID TUID, const char* pData, INT16 i16DataSize )
{
	m_pXignServer->OnReceive(TUID, pData, (INT32)i16DataSize);

		/*char strTemp[1024];
	i3String::Format( strTemp, 1024, "#### CXignCode::Recive() / Time:%d / uid : %I64d, size : %d, buf : %s \n",
		  g_pContextMain->GetServerTime(), T_UID, i16DataSize, pData );
	I3TRACE(strTemp);*/
}
