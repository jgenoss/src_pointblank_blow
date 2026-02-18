#include "pch.h"
#include "LoginWorker_Latin.h"

#define RECONNECT_TIME	10

static INT32 _DefCallBackProc(void* pAgent, char* pPacket, INT32 i32Size)
{
	return ((CLoginWorker_Latin*)pAgent)->RecvMessage(pPacket, i32Size); //중남미 리시브 쓰레드
}

CLoginWorker_Latin::CLoginWorker_Latin()
{
}

CLoginWorker_Latin::~CLoginWorker_Latin()
{
	I3_SAFE_RELEASE(m_pOutBuffer);
	OnDestroy();
}

BOOL	CLoginWorker_Latin::OnCreate(INT32 i32Idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx)
{
	m_i32Idx				= i32Idx;
	m_ui32BufferMaxCount	= ui32BufferMaxCount;
	m_ui8MethodIdx			= ui8MethodIdx;

	m_pOutBuffer = i3RingBuffer::NewObject();
	if (NULL == m_pOutBuffer)															return FALSE;
	if (FALSE == m_pOutBuffer->Create(sizeof(CTPacket), ui32BufferMaxCount))			return FALSE;

	m_ui32IP = g_pConfig->m_aui32LoginIp[ui8MethodIdx];
	m_ui16Port = g_pConfig->m_aui16LoginPort[ui8MethodIdx];

	m_pSocket = i3NetworkClientSocket2::NewObject();
	if (NULL == m_pSocket)																return FALSE;
	if (FALSE == m_pSocket->OnCreate(m_ui32IP, m_ui16Port, _DefCallBackProc, this))		return FALSE;	

	m_ui32ReconnectTime = i3ThreadTimer::GetServerTime();

	return CLoginBase::OnCreate(m_i32Idx, ui32BufferMaxCount, ui8MethodIdx);
}

BOOL CLoginWorker_Latin::_WorkProcess( LOGIN_INPUT *pData, LOGIN_OUTPUT* pLoginOut )
{
	switch( pData->m_eCommandType)
	{	
	case TYPE_LOGIN_ID		:
		{
			char strID[STR_ID_SIZE+1];
			i3mem::FillZero(strID, STR_ID_SIZE+1);
			strncpy(strID, pData->m_strID, STR_ID_SIZE);

			struct	in_addr ConIP;
			ConIP.s_addr = ntohl(pData->m_ui32IP);
			char *	strConIP = inet_ntoa( ConIP );

			UINT8 ui8Tmp = (UINT8)pData->m_i32MainLoginBufferIdx;

			CTPacket sendPacket( LATIN_PACKET_ID_LOGIN_REQUEST2 );
			sendPacket << strID << strConIP << GAME_CODE << pData->m_strPW;
			sendPacket << (INT16)(pData->m_i32ServerIdx) << (INT32)(pData->m_i32SessionIdx) <<(pData->m_ui8Country)<< ui8Tmp <<(INT32)(pData->m_ui32AuthKey);

			// 로그인 서버 접속 유지 코드 (0: 50초 후 연결 종료, 3: 접속 유지)
			sendPacket << (BYTE) 3;

			sendPacket.Encrypt(ENCRYPT_KEY);	

			if (0 == SendMessage((char *)sendPacket.GetBuffer(), sendPacket.GetSize())) return FALSE;		
					
		}
		break; 			
	case TYPE_LOGIN_LOGOUT	: 
		{
			//pLoginOut->m_TUID = pData->m_TUID; 
			char strID[STR_ID_SIZE+1];
			i3mem::FillZero(strID, STR_ID_SIZE+1);
			strncpy(strID, pData->m_strID, STR_ID_SIZE);

			CTPacket sendPacket( LATIN_PACKET_ID_LOGOUT_REQUEST );
			sendPacket << strID << GAME_CODE;

			sendPacket.Encrypt(ENCRYPT_KEY);
			//g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L" latin log out");
			if (0 == SendMessage((char *)sendPacket.GetBuffer(), sendPacket.GetSize())) return FALSE;
			//g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L" latin log out2");
			return FALSE;
		}
		break;
	case TYPE_LOGIN_CASH_CHECK:
		{			
			
		}
		break;
	default: pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN; break;
	}

	return FALSE; 
}

bool CLoginWorker_Latin::_WorkProcess(CTPacket *pData)
{
	LOGIN_OUTPUT*	pLoginOut;
	pLoginOut = (LOGIN_OUTPUT*)m_pPopBuffer->PushPointer();

	if (NULL == pLoginOut)
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[%d] Login OutBuffer Full", m_i32Idx);		
		return false;
	}

	switch( pData->GetID() )
	{
	case LATIN_PACKET_ID_LOGIN_ACK		:
		{
			WORD	usResult = 0xFFFF;
			INT64	ui64GameMoney = 0;
			char	bSex;				// 'm'(0x6D) : male, 'f'(0x66) : female
			WORD	usNation;
			BYTE	bPCBangIP = 0;		// 0:일반, 1: PC방
			char	strID[STR_ID_SIZE+1];
			i3mem::FillZero(strID, STR_ID_SIZE+1);
			UINT32	ui32UserIdx = 0;

			*pData >> usResult >> ui64GameMoney >> bSex >> usNation >> bPCBangIP >> strID;
			
			pLoginOut->m_eCommandType = TYPE_LOGIN_ID;
			pLoginOut->m_TUID = 0;			
			if( ui64GameMoney > 0x7FFFFFFF )
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Error : Game Money over Cash / %I64d ", ui64GameMoney ); //종혁 테스트 후 삭제
			}
			pLoginOut->m_i32Cash = (INT32) ui64GameMoney;
	 
			pLoginOut->m_ui8PCCafe	= bPCBangIP;
			pLoginOut->m_ui32Birthday = 19800101;
			strncpy(pLoginOut->m_strID, strID, NET_ID_SIZE);

			pLoginOut->m_i8UIDCreateType	= TYPE_UID_CREATE_INTER;
			pLoginOut->m_i32ServerIdx		= 0;
			pLoginOut->m_i32SessionIdx		= 0;
			pLoginOut->m_ui32AuthKey		= 0;
			pLoginOut->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_SEND;

			UINT8 ui8Tmp = 0;
			INT16 i16ServerIdx = 0; 

			*pData >> (INT16&)(i16ServerIdx) >> (INT32&)(pLoginOut->m_i32SessionIdx) >>(UINT8&)pLoginOut->m_ui8Country>> (UINT8&)ui8Tmp>>(INT32&)(pLoginOut->m_ui32AuthKey);
			pLoginOut->m_i32ServerIdx = i16ServerIdx;
			pLoginOut->m_i32MainLoginBufferIdx = ui8Tmp;
			*pData >> (INT32&)ui32UserIdx;
			i3String::Format((char *)pLoginOut->m_strBillingID, TURKEY_UID_SIZE, "%u", ui32UserIdx); 
		
			switch( usResult )
			{
				case 0 :	pLoginOut->m_i32Result = EVENT_ERROR_SUCCESS;							break;
				case 1 :	pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;		break;
				case 2 :	pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN;			break;
				case 3 :	pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_SERVER_INSPECT_TIME;	break;
				case 5 :	pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT;			break;
				case 6 :	pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_NO_SERVICE_GAME;		break;
				case 7 :	pLoginOut->m_i32Result = EVENT_ERROR_DATABASE;							break;
				default :	pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
			}
			
		}
		break; 			
	default: pLoginOut->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN; break;
	}
	m_pPopBuffer->PushPointerIdx();

	return true; 
}

INT32 CLoginWorker_Latin::RecvMessage(char* pPacket, INT32 i32Size)
{
	if (SOCKET_HEAD_DATASIZE_SIZE > i32Size)
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"Size LOW SOCKET_HEAD_DATASIZE_SIZE %d ", i32Size);
		return 0;
	}

	INT32 PacketSize = *((INT16*)(pPacket));

	if (0 >= PacketSize || PACKETBUFFERSIZE < PacketSize)
	{
		I3TRACE("[WARNING] i3NetworkClientSocket2::OnReceive() - Break packet size(%d bytes)!\n", PacketSize);
		return PacketSize;
	}

	if (i32Size < PacketSize)
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L" PacketSize %d ", PacketSize);
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L" bufferRemainsize %d ", i32Size);
		return 0;
	}	

	// 패킷 로드
	CTPacket recvPacket;
	recvPacket.SetBuffer(pPacket, (WORD)PacketSize);

	PacketParsing(&recvPacket, i32Size);

	return PacketSize;
}

void CLoginWorker_Latin::PacketParsing(CTPacket* packet, INT32 i32Size)
{
	INT32 i32CTPacketSize = packet->GetSize();
	if ( i32CTPacketSize != i32Size )
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"CTPacket CTSize : %d / Size : %d ", i32CTPacketSize, i32Size);
	}

	if ( i32CTPacketSize  < DF_PACKET_HEADER_SIZE  || i32CTPacketSize > DF_DEFAULT_BUFFER_SIZE)
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"CTPacket Size ERROR CTSize : %d / Size : %d ", i32CTPacketSize, i32Size);
		return;
	}

	packet->Decrypt(ENCRYPT_KEY);
	
	// 패킷을 응답 큐에 넣는다.
	CTPacket *pPacketInput = (CTPacket *)m_pOutBuffer->PushPointer();
	if (pPacketInput == NULL) return;	// 2012.05.08 서버 크래쉬로 인한 방어코드 추가. 2012.05.09 안국정
	
	pPacketInput->Clone(packet);
	m_pOutBuffer->PushPointerIdx();	
}

BOOL CLoginWorker_Latin::OnRunningEx()
{
	BOOL bWorking = FALSE;
	if (m_pSocket->IsConnected())
	{
		bWorking = m_pSocket->SelectEvent();			
	}
	else
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Socket latin DisConnect ");
		// reconnect
		if (i3ThreadTimer::GetServerTime() - m_ui32ConnectTime > RECONNECT_TIME)
		{	// 10초에 한번씩 재 접속을 요청합니다.
			m_ui32ConnectTime = i3ThreadTimer::GetServerTime();;

			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Socket latin NULL ");
			m_pSocket->OnDestroy();
			if (m_pSocket->OnCreate(m_ui32IP, m_ui16Port, _DefCallBackProc, this))
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Reconnect latin Success");
			}
			else
			{
				m_pSocket->OnDestroy();
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Reconnect  latin Fail");
			}
			bWorking = TRUE;
		}
	}

	INT32 iWorkCount = PopOutBufferCount();
	for (INT32 i = 0; i < iWorkCount; i++)
	{		
		_WorkProcess(PopOutBufferPointer());
		PopOutBufferIdx();
		bWorking = TRUE;
	}

	return bWorking;
}