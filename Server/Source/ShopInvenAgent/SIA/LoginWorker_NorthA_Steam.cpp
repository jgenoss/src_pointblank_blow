#include "pch.h"
#include "LoginWorker_NorthA_Steam.h"

CLoginWorker_NorthA_Steam::CLoginWorker_NorthA_Steam()
{
}

CLoginWorker_NorthA_Steam::~CLoginWorker_NorthA_Steam()
{
#ifndef USE_STEAM_API
	I3_SAFE_RELEASE(m_pHttpSteam)
#endif
	OnDestroy();
}
BOOL CLoginWorker_NorthA_Steam::OnCreate( INT32 i32Idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx )
{

#ifndef USE_STEAM_API
	m_pHttpSteam	= i3NetworkHTTP::NewObject();
	m_pHttpSteam->CreateMethod( 0, 0, 0, 10000, 1024 ); 
	if( FALSE == m_pHttpSteam->Connect( NULL , i3NetworkHTTP::REQUEST_METHOD_GET ) )	return FALSE;
#endif

	return CLoginHTTP::OnCreate( i32Idx, ui32BufferMaxCount, ui8MethodIdx );
}

BOOL CLoginWorker_NorthA_Steam::_WorkProcess( LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput )
{
	pOutput->m_eCommandType			= pInput->m_eCommandType;		// LogIn	LogOut
	pOutput->m_i32TransServerIdx	= pInput->m_i32TransServerIdx;	// LogIn	LogOut
	pOutput->m_i32SessionIdx		= pInput->m_i32SessionIdx;		// LogIn	LogOut	
	pOutput->m_ui32AuthKey			= pInput->m_ui32AuthKey;		// LogIn	LogOut
	pOutput->m_ui32LoginTime		= GetTickCount();
	pOutput->m_ui8LinkThreadIdx		= pInput->m_ui8LinkThreadIdx;
	pOutput->m_strBillingID[0]		= '\0';
	pOutput->m_ui8Country			= pInput->m_ui8Country;


	switch( pInput->m_eCommandType )
	{	
	case TYPE_LOGIN_ID			: 
		{
			
			pOutput->m_i32Result	= EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
			
#ifndef USE_STEAM_API
			char strTicket[2048];
			char strUrl[4096];
			_ConvertHexKey( strTicket, 2048, (char*)pInput->m_pSessionKey, pInput->m_ui16SessionKeySize );

			//////////////Steam Authentication//////////////

			i3String::Format( strUrl, 4096, "https://api.steampowered.com/ISteamUserAuth/AuthenticateUserTicket/V1/?format=xml&ticket=%s&appid=382850&key=BED785FB746F34C44018DCE58E69C84B", strTicket );
			
			DWORD Time1 = GetTickCount();

			if( m_pHttpSteam->SendRequestGet(strUrl) )
			{
				m_pHttpSteam->RecvRequest();

				_ParsingLogInBySteam( m_pHttpSteam->GetRequestResult(), pOutput );
			}
			else
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"HTTP REQUEST ERROR - STEAM" );
				pOutput->m_i32Result	= EVENT_ERROR_EVENT_LOG_IN_STEAM_HTTP;
			}

			DWORD Time2 = GetTickCount();
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Response Time : %d\n", Time2-Time1 );
#endif

#ifdef USE_STEAM_API
			UINT8 DecryptedTicket[1024];
			UINT32 ui32DecrytedTicketSize = sizeof( DecryptedTicket );
			
			//////////////Steam Authentication//////////////

			// included is the "secret" key for spacewar. normally this is secret
			const uint8 rgubKey[k_nSteamEncryptedAppTicketSymmetricKeyLen] = {
				0xe8, 0x76, 0xdf, 0xbd, 0x73, 0xb0, 0xe7, 0xa4, 
				0xa4, 0x73, 0xc3, 0x82, 0xea, 0x1a, 0xba, 0xf9, 
				0xc1, 0x20, 0x8e, 0xed, 0x0b, 0xd5, 0x95, 0xe5, 
				0x83, 0x7f, 0x9b, 0xef, 0x41, 0x30, 0x1e, 0x98 };

			if( !g_pSteam_BDecryptTicket( pInput->m_pSessionKey, static_cast<UINT32>(pInput->m_ui16SessionKeySize), DecryptedTicket, &ui32DecrytedTicketSize, rgubKey, sizeof( rgubKey ) ))
			{				
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"Steam App Ticket Decrypt Error." );
				pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_STEAM_KEY_DECRYPT;
				return true;
			}

			if( !g_pSteam_BIsTicketForApp( DecryptedTicket, ui32DecrytedTicketSize, 382850 ) )
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Steam App Ticket error - IsTicketForApp" );
				pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_STEAM_NOT_PB_KEY;
				return true;
			}

			CSteamID steamIDFromTicket;
			g_pSteam_GetTicketSteamID( DecryptedTicket, ui32DecrytedTicketSize, &steamIDFromTicket );

			if( steamIDFromTicket.IsValid() )
			{
				pOutput->m_i32Result = EVENT_ERROR_SUCCESS;
				pOutput->m_TExtUID = steamIDFromTicket.ConvertToUint64();
			}
			else
			{
				return true;
			}

#endif

			//스팀 연동이 정상적으로 처리된 경우에만 Web Middle서버로 로그인 인증을 던집니다.
			if( EV_SUCCESSED(pOutput->m_i32Result) )
			{
				struct in_addr	ConIp;
				ConIp.s_addr			= pInput->m_ui32IP;
				m_pHttp->ResetArguments();
				m_pHttp->AddArgument( "STEAM_UID",	pOutput->m_TExtUID );			
				m_pHttp->AddArgument( "IP",			inet_ntoa( ConIp ));
				m_pHttp->SetUpPostArguments();

				if( m_pHttp->SendRequestPost() )
				{
					m_pHttp->RecvRequest();

					if( _ParsingLogInByWeb( m_pHttp->GetRequestResult(), pOutput ) )
					{
						pOutput->m_ui32Birthday		= 19800101;
						pOutput->m_i32Cash			= 0;
						pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_EXTER;
						pOutput->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_NOT;
						pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE;
						i3mem::FillZero( pOutput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE );
						
						_LoginExtension( pInput, pOutput );
					}
					else
					{						
						g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"HTTP PARSING ERROR. [IN]([UID = %I64d]),[Out]([Rv= %d])", pOutput->m_TExtUID, pOutput->m_i32Result);

						pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_HTTP;
					}
				}
				else 
				{// 일정횟수 이상 일어나면, Error처리해야함.
					ReConnect();
				}
			}
		}
		break;
	}

	return true; 
}

#define STR_RV_CODE 32

BOOL CLoginWorker_NorthA_Steam::_ParsingLogInBySteam( char * pszString, LOGIN_OUTPUT * pRecvData )
{
	INT32 iCount = 0; 
	BOOL bUID = FALSE; 
	BOOL bRv  = FALSE; 
		
	INT32 iUidCount = 0; 	
	INT32 iRvCount	= 0;
		
	char pTemp[STR_RV_CODE] = {0, };
	
	char LinkId[50];

	//INT32 i32Length = i3String::Length( pszString ) ;
	//if(i3String::Length( pszString ) > 1000)return FALSE; 
	
	while(pszString[iCount] != NULL)
	{
		iCount++;
		
		if(bUID)
		{
			if( pszString[iCount] == '<' )
			{
				LinkId[iUidCount] = NULL; 
				bUID = FALSE; 
			}
			else 
			{
				LinkId[iUidCount] = pszString[iCount];   
				iUidCount++; 
			}

			//버그 코드 
			if(iUidCount >= TURKEY_UID_SIZE)
			{
				return FALSE; 
			}
		}
		
		if(bRv)
		{
			if( pszString[iCount] == '<' )
			{
				pTemp[iRvCount] = NULL; 
				bRv = FALSE; 
			}else
			{
				pTemp[iRvCount] = pszString[iCount];
				iRvCount++; 
			}

			//버그 코드 
			if(iRvCount >= STR_RV_CODE)
			{
				return FALSE; 
			}
		}		
		
		if( pszString[iCount] != '<' )
		{
			continue;
		}
		
		//UID
		if( pszString[iCount+1] == 's' &&
			pszString[iCount+2] == 't' &&
			pszString[iCount+3] == 'e' &&
			pszString[iCount+4] == 'a' &&
			pszString[iCount+5] == 'm' &&
			pszString[iCount+6] == 'i' &&
			pszString[iCount+7] == 'd' &&			
			pszString[iCount+8] == '>' )
		{
			bUID = TRUE; 
			iCount = iCount + 8;
		}
				
		//spt_rtn
		if( pszString[iCount+1] == 'e' &&
			pszString[iCount+2] == 'r' &&
			pszString[iCount+3] == 'r' &&
			pszString[iCount+4] == 'o' &&
			pszString[iCount+5] == 'r' &&
			pszString[iCount+6] == 'c' &&
			pszString[iCount+7] == 'o' &&
			pszString[iCount+8] == 'd' &&
			pszString[iCount+9] == 'e' &&
			pszString[iCount+10] == '>' )
		{
			bRv = TRUE; 
			iCount = iCount + 10;
		}
	}
	 
	//i3String::Copy
	switch( atoi(pTemp) ) // 원본
	{
	case 0		: pRecvData->m_i32Result = EVENT_ERROR_SUCCESS;							break;  //정상 로그인 
	case 3		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_STEAM_PARAMETER;	break;  //파라미터 값 불량
	case 101	: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_STEAM_AUTH_FAIL;	break;  //유효하지 않은 티켓
	default		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;			break;  //에러 
	}
		
	pRecvData->m_TExtUID		= _atoi64(LinkId);

	return TRUE; 
}

BOOL CLoginWorker_NorthA_Steam::_ParsingLogInByWeb( char * pszString, LOGIN_OUTPUT * pRecvData )
{
	INT32 iCount = 0; 
	BOOL bUID = FALSE; 
	BOOL bRv  = FALSE; 
	BOOL bTempKey = FALSE; 
	BOOL bSteamUID = FALSE;
	BOOL bLoginKey = FALSE;
	
	INT32 iUidCount = 0; 
	INT32 iTempKey = 0; 
	INT32 iRvCount	= 0;
	INT32 i32SteamUidCount = 0;
	INT32 i32LoginKeyCount = 0;
	char pTemp[STR_RV_CODE]; 

	char LinkId[50];

	//INT32 i32Length = i3String::Length( pszString ) ;
	//if(i3String::Length( pszString ) > 1000)return FALSE; 
	
	while(pszString[iCount] != NULL)
	{
		iCount++;

		if(bSteamUID)
		{
			if( pszString[iCount] == '<' )
			{
				pRecvData->m_strExtUID[i32SteamUidCount] = NULL;
				bSteamUID = FALSE; 
			}
			else 
			{
				pRecvData->m_strExtUID[i32SteamUidCount] = pszString[iCount];
				i32SteamUidCount++; 
			}

			if( i32SteamUidCount >= EXT_UID_SIZE )
			{
				return FALSE; 
			}
		}

		if(bUID)
		{
			if( pszString[iCount] == '<' )
			{
				LinkId[iUidCount] = NULL; 
				bUID = FALSE; 
			}
			else 
			{
				LinkId[iUidCount] = pszString[iCount];   
				iUidCount++; 
			}

			//버그 코드 
			if(iUidCount >= TURKEY_UID_SIZE)
			{
				return FALSE; 
			}
		}

		if(bTempKey)
		{
			if( pszString[iCount] == '<' )
			{
				pRecvData->m_strBillingID[iTempKey] = NULL;
				bTempKey = FALSE; 
			}
			else 
			{
				pRecvData->m_strBillingID[iTempKey] = pszString[iCount];
				iTempKey++; 
			}

			if(iTempKey >= BILLING_ID_SIZE)
			{
				return FALSE; 
			}
		}

		if(bLoginKey)
		{
			if( pszString[iCount] == '<' )
			{
				pRecvData->m_strWebLoginKey[i32LoginKeyCount] = NULL;
				bLoginKey = FALSE; 
			}
			else 
			{
				pRecvData->m_strWebLoginKey[i32LoginKeyCount] = pszString[iCount];
				i32LoginKeyCount++; 
			}

			if( i32LoginKeyCount >= WEB_LOGINKEY_SIZE )
			{
				return FALSE; 
			}
		}

		if(bRv)
		{
			if( pszString[iCount] == '<' )
			{
				pTemp[iRvCount] = NULL; 
				bRv = FALSE; 
			}else
			{
				pTemp[iRvCount] = pszString[iCount];
				iRvCount++; 
			}

			//버그 코드 
			if(iRvCount >= STR_RV_CODE)
			{
				return FALSE; 
			}
		}		
		
		if( pszString[iCount] != '<' )
		{
			continue;
		}

		//SteamUID(암호화)
		if( pszString[iCount+1] == 'S' &&
			pszString[iCount+2] == 'T' &&
			pszString[iCount+3] == 'E' &&
			pszString[iCount+4] == 'A' &&
			pszString[iCount+5] == 'M' &&
			pszString[iCount+6] == '_' &&
			pszString[iCount+7] == 'U' &&
			pszString[iCount+8] == 'I' &&
			pszString[iCount+9] == 'D' &&
			pszString[iCount+10] == '>' )
		{
			bSteamUID = TRUE; 
			iCount = iCount + 10;
		}

		//UID
		if( pszString[iCount+1] == 'U' &&
			pszString[iCount+2] == 'I' &&
			pszString[iCount+3] == 'D' &&
			pszString[iCount+4] == '>' )
		{
			bUID = TRUE; 
			iCount = iCount + 4;
		}

		//tempKey
		if( pszString[iCount+1] == 't' &&
			pszString[iCount+2] == 'e' &&
			pszString[iCount+3] == 'm' &&
			pszString[iCount+4] == 'p' &&
			pszString[iCount+5] == 'K' &&
			pszString[iCount+6] == 'e' &&
			pszString[iCount+7] == 'y' &&
			pszString[iCount+8] == '>' )
		{
			bTempKey = TRUE; 
			iCount = iCount + 8;
		}

		//LoginKey
		if( pszString[iCount+1] == 'L' &&
			pszString[iCount+2] == 'o' &&
			pszString[iCount+3] == 'g' &&
			pszString[iCount+4] == 'i' &&
			pszString[iCount+5] == 'n' &&
			pszString[iCount+6] == 'K' &&
			pszString[iCount+7] == 'e' &&
			pszString[iCount+8] == 'y' &&
			pszString[iCount+9] == '>' )
		{
			bLoginKey = TRUE; 
			iCount = iCount + 9;
		}

		//spt_rtn
		if( pszString[iCount+1] == 's' &&
			pszString[iCount+2] == 'p' &&
			pszString[iCount+3] == 't' &&
			pszString[iCount+4] == '_' &&
			pszString[iCount+5] == 'r' &&
			pszString[iCount+6] == 't' &&
			pszString[iCount+7] == 'n' &&
			pszString[iCount+8] == '>' )
		{
			bRv = TRUE; 
			iCount = iCount + 8;
		}
	}

	if(iRvCount == 0) return FALSE; 
 
	//i3String::Copy
	switch( atoi(pTemp) ) // 원본
	{
	case 0		: pRecvData->m_i32Result	 = EVENT_ERROR_SUCCESS;						break;  //정상 로그인 
	case -1		: pRecvData->m_i32Result	 = EVENT_ERROR_EVENT_LOG_IN_ID_FAIL;		break;  //존재하지않는 ID
	case -2		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_PW_FAIL;			break;  //비밀번호 불일치 
	case -3		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT;		break;  //삭제된 계정
	case -99	: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_HTTP;				break;  //로그인 페이지 오류	
	default		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;			break;  //에러 
	}

	i3String::Format( pRecvData->m_strID, NET_ID_SIZE, "%s@ST", LinkId );
	pRecvData->m_TUID				= _atoi64(LinkId);

	return TRUE; 
}

void CLoginWorker_NorthA_Steam::_ConvertHexKey( char* pstrOut, INT32 i32OutLength, char* pstrIn, INT32 i32InLength )
{
	char const hexval[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	
	if( i32InLength * 2 > i32OutLength ) return;

	INT32 i32Outpos = 0;	

	for( INT32 i32Inpos = 0 ; i32Inpos < i32InLength ; ++i32Inpos )
	{
		pstrOut[i32Outpos] = hexval[(pstrIn[i32Inpos] & 0xF0 ) >> 4];
		i32Outpos++;
		pstrOut[i32Outpos] = hexval[pstrIn[i32Inpos] & 0x0F];
		i32Outpos++;
	}

	pstrOut[i32Outpos] = '\0';

}