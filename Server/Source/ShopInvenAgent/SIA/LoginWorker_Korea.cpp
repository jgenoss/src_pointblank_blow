#include "pch.h"
#include "LoginWorker_Korea.h"


CLoginWorker_Korea::CLoginWorker_Korea()
{
	
}

CLoginWorker_Korea::~CLoginWorker_Korea()
{
	OnDestroy();
}

BOOL CLoginWorker_Korea::_WorkProcess( LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput )
{
	
	pOutput->m_eCommandType			= pInput->m_eCommandType;		// LogIn	LogOut
	pOutput->m_i32TransServerIdx	= pInput->m_i32TransServerIdx;	// LogIn	LogOut
	pOutput->m_ui8PBVer				= pInput->m_ui8PBVer;			// LogIn	LogOut	
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
			struct in_addr	ConIp;
			pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
			pOutput->m_ui8PCCafe = PC_NOT_CAFE;
			ConIp.s_addr		=	pInput->m_ui32IP;
			m_pHttp->ResetArguments();
			
			

			//암호화된 세션키로 통신하도록 변경 (Parameter명은 추후 연동문서보고 수정해야됨) - 2014.04.24 임준현
			m_pHttp->AddArgument( "data", (char*)pInput->m_pSessionKey );			
			m_pHttp->AddArgument( "ip", inet_ntoa( ConIp ));
			m_pHttp->SetUpPostArguments();
			
			if( m_pHttp->SendRequestPost() )
			{
				m_pHttp->RecvRequest();

				if( _ParsingLogInByKorea( m_pHttp->GetRequestResult(), pOutput ) )
				{					
					pOutput->m_i32Cash		= 0;					
					
					_LoginExtension( pInput, pOutput );
				}
				else
				{
					char strTemp[SERVER_STRING_COUNT];
					i3String::Format(strTemp, SERVER_STRING_COUNT, "HTTP PARSING ERROR. [IN]([ID = %s],[IP = %s]),[Out]([Rv= %x])", pInput->m_strID, inet_ntoa(ConIp), pOutput->m_i32Result);

					wchar_t wstrTemp[SERVER_STRING_COUNT];
					MultiByteToWideChar(CP_ACP, NULL, strTemp, -1, wstrTemp, SERVER_STRING_COUNT);

					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, wstrTemp );

					pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_HTTP;
				}
			}
			else 
			{// 일정횟수 이상 일어나면, Error처리해야함.
				ReConnect();
			}
		}
		break; 	
	}

	return true; 
}

#define STR_RV_CODE 32

BOOL CLoginWorker_Korea::_ParsingLogInByKorea( char * pszString, LOGIN_OUTPUT * pRecvData )
{
	INT32 i32Count = 0; 
	BOOL bUID = FALSE; 
	BOOL bRv  = FALSE; 
	BOOL bTempKey = FALSE; 
	BOOL bBirthDay = FALSE;
	BOOL bServiceCode = FALSE;
	BOOL bShutDownTime = FALSE;
	BOOL bAccountID		= FALSE;
	BOOL bPcCafe		= FALSE;
	
	INT32 iUidCount		= 0; 
	INT32 iTempKey		= 0; 
	INT32 iRvCount		= 0; 
	INT32 iBirthDay		= 0;
	INT32 iServiceCode	= 0;
	INT32 iShutDownTime	= 0;
	INT32 iAccountID	= 0;
	INT32 iPcCafe		= 0;

	char LinkId[KOREA_UID_SIZE];
	char pBirthday[KOREA_BIRTHDAY_SIZE];
	char ServiceCode[KOREA_SERVICE_CODE_SIZE];
	char PcCafe[KOREA_PC_CAFE_SIZE];
	char pRetval[STR_RV_CODE];
	
	while(pszString[i32Count] != NULL)
	{
		i32Count++; 

		if(bUID)
		{
			if( pszString[i32Count] == '<' )
			{
				LinkId[iUidCount] = NULL; 
				bUID = FALSE; 
			}
			else 
			{
				LinkId[iUidCount] = pszString[i32Count];
				iUidCount++; 
			}

			//버그 코드 
			if(iUidCount >= KOREA_UID_SIZE)
			{
				return FALSE; 
			}
		}

		if(bAccountID)
		{
			if( pszString[i32Count] == '<' )
			{
				pRecvData->m_strID[iAccountID] = NULL;
				bAccountID = FALSE; 
			}
			else 
			{
				pRecvData->m_strID[iAccountID] = pszString[i32Count];
				iAccountID++; 
			}

			if(iAccountID >= KOREA_ACCOUNTID_SIZE)
			{
				return FALSE; 
			}
		}

		if(bTempKey)
		{
			if( pszString[i32Count] == '<' )
			{
				pRecvData->m_strBillingID[iTempKey] = NULL;
				bTempKey = FALSE; 
			}
			else 
			{
				pRecvData->m_strBillingID[iTempKey] = pszString[i32Count];
				iTempKey++; 
			}

			if(iTempKey >= KOREA_ENCRIPT_UID_SIZE)
			{
				return FALSE; 
			}
		}

		if(bBirthDay)
		{
			if( pszString[i32Count] == '<' || iBirthDay == 8 )
			{
				pBirthday[iBirthDay] = NULL; 
				bBirthDay = FALSE; 
			}else
			{
				pBirthday[iBirthDay] = pszString[i32Count];

				//smalldatetime에서 '-'은 빼고 받습니다.
				if( '-' != pBirthday[iBirthDay] )
				{
					iBirthDay++; 
				}
			}

			//버그 코드 
			if(iBirthDay >= KOREA_BIRTHDAY_SIZE)
			{
				return FALSE; 
			}
		}

		if(bServiceCode)
		{
			if( pszString[i32Count] == '<' )
			{
				ServiceCode[iServiceCode] = NULL; 
				bServiceCode = FALSE; 
			}
			else 
			{
				ServiceCode[iServiceCode] = pszString[i32Count];
				iServiceCode++; 
			}

			//버그 코드 
			if(iServiceCode >= KOREA_SERVICE_CODE_SIZE)
			{
				return FALSE; 
			}
		}

		if(bShutDownTime)
		{
			if( pszString[i32Count] == '<' )
			{				
				bShutDownTime = FALSE; 
			}else
			{
				//웹서버에서 로그인 가능한 시간일 경우 1로 표시해서 내려줍니다.
				//게임서버에서는 로그인할 수 없는시간을 1로 간주하기 때문에 로그인 결과값을 받는 시점에서
				//결과값을 반전시켜서 게임서버에 내려줍니다.
				pRecvData->m_aui8ShutDownTime[iShutDownTime] = (pszString[i32Count] - '0' > 0 ? 0 : 1) ;
				iShutDownTime++; 
			}

			//버그 코드 
			if( iShutDownTime >= SHUTDOWN_TIME_SIZE + 1 )
			{
				return FALSE; 
			}
		}

		if(bPcCafe)
		{
			if( pszString[i32Count] == '<' )
			{
				PcCafe[iPcCafe] = NULL; 
				bPcCafe = FALSE; 
			}
			else 
			{
				PcCafe[iPcCafe] = pszString[i32Count];
				iPcCafe++; 
			}

			//버그 코드 
			if(iPcCafe >= KOREA_PC_CAFE_SIZE)
			{
				return FALSE; 
			}
		}

		if(bRv)
		{
			if( pszString[i32Count] == '<' )
			{
				pRetval[iRvCount] = NULL; 
				bRv = FALSE; 
			}else
			{
				pRetval[iRvCount] = pszString[i32Count];
				iRvCount++; 
			}

			//버그 코드 
			if(iRvCount >= STR_RV_CODE)
			{
				return FALSE; 
			}
		}
		
		if( pszString[i32Count] != '<' )
		{
			continue;
		}

		if( pszString[i32Count+1] == 'U' &&
			pszString[i32Count+2] == 'I' &&
			pszString[i32Count+3] == 'D' &&
			pszString[i32Count+4] == '>' )
		{
			bUID = TRUE; 
			i32Count = i32Count + 4;
		}

		//AccountID
		if( pszString[i32Count+1] == 'A' &&
			pszString[i32Count+2] == 'c' &&
			pszString[i32Count+3] == 'c' &&
			pszString[i32Count+4] == 'o' &&
			pszString[i32Count+5] == 'u' &&
			pszString[i32Count+6] == 'n' &&
			pszString[i32Count+7] == 't' &&
			pszString[i32Count+8] == 'I' &&
			pszString[i32Count+9] == 'D' &&
			pszString[i32Count+10] == '>' )
		{
			bAccountID = TRUE; 
			i32Count = i32Count + 10;
		}

		//tempKey
		if( pszString[i32Count+1] == 't' &&
			pszString[i32Count+2] == 'e' &&
			pszString[i32Count+3] == 'm' &&
			pszString[i32Count+4] == 'p' &&
			pszString[i32Count+5] == 'K' &&
			pszString[i32Count+6] == 'e' &&
			pszString[i32Count+7] == 'y' &&
			pszString[i32Count+8] == '>' )
		{
			bTempKey = TRUE; 
			i32Count = i32Count + 8;
		}

		//birthday
		if( pszString[i32Count+1] == 'b' &&
			pszString[i32Count+2] == 'i' &&
			pszString[i32Count+3] == 'r' &&
			pszString[i32Count+4] == 't' &&
			pszString[i32Count+5] == 'h' &&
			pszString[i32Count+6] == 'd' &&
			pszString[i32Count+7] == 'a' &&
			pszString[i32Count+8] == 'y' &&
			pszString[i32Count+9] == '>')
		{
			bBirthDay = TRUE; 
			i32Count = i32Count + 9;
		}

		//serviceCode
		if( pszString[i32Count+1] == 's' &&
			pszString[i32Count+2] == 'e' &&
			pszString[i32Count+3] == 'r' &&
			pszString[i32Count+4] == 'v' &&
			pszString[i32Count+5] == 'i' &&
			pszString[i32Count+6] == 'c' &&
			pszString[i32Count+7] == 'e' &&
			pszString[i32Count+8] == 'C' &&
			pszString[i32Count+9] == 'o' &&
			pszString[i32Count+10] == 'd' &&
			pszString[i32Count+11] == 'e' &&
			pszString[i32Count+12] == '>')
		{
			bServiceCode = TRUE; 
			i32Count = i32Count + 12;
		}

		//ShutDownTime
		if( pszString[i32Count+1] == 'S' &&
			pszString[i32Count+2] == 'h' &&
			pszString[i32Count+3] == 'u' &&
			pszString[i32Count+4] == 't' &&
			pszString[i32Count+5] == 'D' &&
			pszString[i32Count+6] == 'o' &&
			pszString[i32Count+7] == 'w' &&
			pszString[i32Count+8] == 'n' &&
			pszString[i32Count+9] == 'T' &&
			pszString[i32Count+10] == 'i' &&
			pszString[i32Count+11] == 'm' &&
			pszString[i32Count+12] == 'e' &&
			pszString[i32Count+13] == '>')
		{
			bShutDownTime = TRUE; 
			i32Count = i32Count + 13;
		}

		//PcBang
		if( pszString[i32Count+1] == 'P' &&
			pszString[i32Count+2] == 'c' &&
			pszString[i32Count+3] == 'B' &&
			pszString[i32Count+4] == 'a' &&
			pszString[i32Count+5] == 'n' &&
			pszString[i32Count+6] == 'g' &&			
			pszString[i32Count+7] == '>')
		{
			bPcCafe = TRUE; 
			i32Count = i32Count + 7;
		}

		//spt_rtn
		if( pszString[i32Count+1] == 's' &&
			pszString[i32Count+2] == 'p' &&
			pszString[i32Count+3] == 't' &&
			pszString[i32Count+4] == '_' &&
			pszString[i32Count+5] == 'r' &&
			pszString[i32Count+6] == 't' &&
			pszString[i32Count+7] == 'n' &&
			pszString[i32Count+8] == '>' )
		{
			bRv = TRUE; 
			i32Count = i32Count + 8;
		}
	}

	if( iRvCount == 0) return FALSE; 
 
	//i3String::Copy
	switch( atoi(pRetval) ) // 원본
	{
	case 0		: pRecvData->m_i32Result = EVENT_ERROR_SUCCESS;							break;  //정상 로그인 
	case -4		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_AUTHKEY_FAIL;		break;  //AuthKey Fail
	case -11		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_BLOCK_COUNTRY;		break;  //IP제한	
	case -99		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_HTTP;				break;  //로그인 페이지 오류	
	case -999	: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_AUTHKEY_TIMEOUT;		break;  //로그인 페이지 오류	
	default		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;				break;  //에러 
	}

	pRecvData->m_TUID				= _atoi64(LinkId);
	pRecvData->m_ui32Birthday		= atoi(pBirthday);
	pRecvData->m_i8UIDCreateType	= TYPE_UID_CREATE_EXTER;
	pRecvData->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_SEND;

	switch(atoi(ServiceCode))
	{
	case 0:	//자체서비스로 로그인한 경우
		pRecvData->m_ui8LocaleCode	= LOCALE_CODE_KOREA;
		break;
	case 1:	//다음 채널링으로 로그인한 경우
		pRecvData->m_ui8LocaleCode	= LOCALE_CODE_KOREA_DAUM;
		break;
	case 2:	//피카온 채널링으로 로그인한 경우
		pRecvData->m_ui8LocaleCode	= LOCALE_CODE_KOREA_PICAON;
		break;
	default:
		{
			wchar_t wstrID[NET_ID_SIZE];
			MultiByteToWideChar(CP_ACP, NULL, pRecvData->m_strID, -1, wstrID, NET_ID_SIZE);

			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[Service Code Error]Service Code is %c, AccountInfo(ID:%s, UID:%I64d)",
				ServiceCode[0], wstrID, pRecvData->m_TUID );

			pRecvData->m_ui8LocaleCode	= LOCALE_CODE_KOREA;
		}
		break;
	}
	

	switch(atoi(PcCafe))
	{
	case 1	:	//피씨방 유료사용자
		pRecvData->m_ui8PCCafe = PC_NORMAL_CAFE;
		break;	
	case 0	:	//피씨방 무료사용자
	case 2	:	//개인
	case 9	:	//웹서비스 접근권환 오류
	case -1	:	//기타 오류
	default	:
		pRecvData->m_ui8PCCafe = PC_NOT_CAFE;		
		break;
	}

	return TRUE; 
}