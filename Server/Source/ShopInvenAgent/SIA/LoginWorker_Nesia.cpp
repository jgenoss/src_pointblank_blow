#include "pch.h"
#include "LoginWorker.h"
#include "LoginWorker_Nesia.h"

I3_CLASS_INSTANCE( CLoginWorker_Nesia, CLoginWorker);

CLoginWorker_Nesia::CLoginWorker_Nesia()
{
	m_pHttpLogIn	= NULL;
	m_pHttpBilling	= NULL; 

	m_nLoginConnectHTTPRetryCount = 0;
	m_nBillingConnectHTTPRetryCount = 0;	
}

CLoginWorker_Nesia::~CLoginWorker_Nesia()
{
	OnDestroy();
}

BOOL CLoginWorker_Nesia::ConnectionCreate()
{
	char szReturnString[MAX_PATH];

	m_pHttpLogIn = i3NetworkHTTP::NewObject();
	m_pHttpLogIn->CreateMethod( 0, 0, 0, 1000, 1024 ); 
	if( FALSE == m_pHttpLogIn->Connect( g_pConfig->m_LoginHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
	{	
		i3String::Format( szReturnString, MAX_PATH, "Error : %s (code: %d)", m_pHttpLogIn->GetLastErrorString(), m_pHttpLogIn->GetLastErrorCode());
		g_pModuleLogFile->Write_Log_Main(szReturnString );
		return FALSE;
	}

	m_pHttpBilling = i3NetworkHTTP::NewObject(); 
	m_pHttpBilling->CreateMethod( 0, 0, 0, 1000, 1024 ); 
	if( FALSE == m_pHttpBilling->Connect( g_pConfig->m_BillingHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
	{
		i3String::Format( szReturnString, MAX_PATH, "Error : %s (code: %d)", m_pHttpBilling->GetLastErrorString(), m_pHttpBilling->GetLastErrorCode());
		g_pModuleLogFile->Write_Log_Main(szReturnString ); 
		return FALSE;
	}

	return TRUE;
}

BOOL CLoginWorker_Nesia::ConnectionDestroy()
{
	I3_SAFE_RELEASE( m_pHttpLogIn ); 
	I3_SAFE_RELEASE( m_pHttpBilling); 

	return TRUE;
}

bool CLoginWorker_Nesia::_WorkProcess( LOGIN_INPUT *pData, LOGIN_OUTPUT* pLoginOut )
{
	pLoginOut->_CommandType	= pData->_CommandType;		// LogIn	LogOut
	pLoginOut->_iServerIdx	= pData->_iServerIdx;		// LogIn	LogOut
	pLoginOut->_iSessionIdx	= pData->_iSessionIdx;		// LogIn	LogOut	
	pLoginOut->_iAuthKey		= pData->_iAuthKey;			// LogIn	LogOut	
	pLoginOut->_Custom[0]		= '\0';

	switch( pData->_CommandType )
	{
	case TYPE_LOGIN_WEB		: pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN; break; //Error 
	case TYPE_LOGIN_ID		: 
		{
			pLoginOut->_i64DbIdx	= 0; 
			pLoginOut->_iResult	= EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
			char szPWMD5[MAX_PATH];
			char szKeyMD5[MAX_PATH];
			char szTemp[MAX_PATH];
			char szDate[MAX_PATH];
			struct in_addr	ConIp;
							
			i3String::Format( szDate, MAX_PATH, "%04d%02d%02d%02d%02d%02d", g_pConfig->m_Year, g_pConfig->m_Month, g_pConfig->m_Day, g_pConfig->m_Hour, g_pConfig->m_Minute, g_pConfig->m_Second);
			ConIp.s_addr	=	pData->_Ip;						//아이피 
			m_pHttpLogIn->ConvertMD5( szPWMD5, pData->_Passwd );//비밀번호
			i3String::Format( szTemp, MAX_PATH, "%s%s%s%s%s", inet_ntoa(ConIp), pData->_Id, szDate, NESIA_GAME_CODE, szPWMD5);
			m_pHttpLogIn->ConvertMD5( szKeyMD5, szTemp);

			m_pHttpLogIn->ResetArguments();
			m_pHttpLogIn->AddArgument( "id", pData->_Id);
			m_pHttpLogIn->AddArgument( "pw", szPWMD5);
			m_pHttpLogIn->AddArgument( "ip", inet_ntoa( ConIp ));
			m_pHttpLogIn->AddArgument( "dt", szDate);
			m_pHttpLogIn->AddArgument( "svc_code", NESIA_GAME_CODE);
			m_pHttpLogIn->AddArgument( "key", szKeyMD5);
			m_pHttpLogIn->SetUpPostArguments();
			if( m_pHttpLogIn->SendRequestPost() )
			{
				m_pHttpLogIn->RecvRequest();

				LOGIN_RETURN_VALUE_BY_NESIA ReturnValue;
				_ParsingLogInByNesia( m_pHttpLogIn->GetRequestResult(), &ReturnValue);
				pLoginOut->_iResult	= ReturnValue.iReturnCode; 
				if( EV_SUCCESSED( pLoginOut->_iResult ) )
				{
					pLoginOut->_iResult				= EVENT_ERROR_SUCCESS;
					pLoginOut->_i64DbIdx				= 0;//ReturnValue.iGameIdx;
					pLoginOut->_birthday				= ReturnValue.iBirth; 						
					pLoginOut->_ui8Gender				= 1;					// 인도네시아의 경우 전부 남자로 고정합니다.
					pLoginOut->_pcCafe				= PC_NOT_CAFE; 
					pLoginOut->_ncCoin				= ReturnValue.iCash;
					pLoginOut->m_ui8ApplingAdult		= APPLING_ADULT_YES;
					pLoginOut->_ui64UserBillingIdx	= ReturnValue.iGameIdx;						
				
					i3mem::Copy( pLoginOut->_Id, pData->_Id, NET_ID_SIZE );
				}
			}
			else 
			{// 일정횟수 이상 일어나면, Error처리해야함.
				m_nLoginConnectHTTPRetryCount++;

				// 현재 로그인 유저에 대해서는 실패 처리합니다.
				pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_HTTP;

				char strTemp[1024];

				// Http ReConnect.
				{
					m_pHttpLogIn->DisConnect();

					if( !m_pHttpLogIn->Connect( g_pConfig->m_LoginHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
					{	
						i3String::Format( strTemp, 1024, "Error[%d] : %s (code: %d) - HTTP Connect Fail!", m_Idx, m_pHttpLogIn->GetLastErrorString(), m_pHttpLogIn->GetLastErrorCode());
						g_pModuleLogFile->Write_Log_Login(m_Idx, strTemp, WARNING_AUTH);							
					}
				}
				
				i3String::Format(strTemp, 1024, "ReConnect HTTP Login. [RetryCount= %d]", m_nLoginConnectHTTPRetryCount); 
				g_pModuleLogFile->Write_Log_Login( m_Idx, strTemp, WARNING_AUTH); 
			}
		}
		break; 			
	case TYPE_LOGIN_LOGOUT	: 
		pLoginOut->_iResult	= EVENT_ERROR_SUCCESS; 
		pLoginOut->_i64DbIdx	= pData->_i64DbIdx;			
		break; 
	case TYPE_LOGIN_CASH_CHECK:
		{
			pLoginOut->_iServerIdx	= pData->_iServerIdx;
			pLoginOut->_iSessionIdx	= pData->_iSessionIdx;
			pLoginOut->_i64DbIdx		= pData->_i64DbIdx;
			i3mem::Copy( pLoginOut->_Id, pData->_Id, NET_ID_SIZE );
			pLoginOut->_Id[ NET_ID_SIZE - 1 ] = '\0';
			_GetExtensionInfoNesia( pLoginOut );
		}
		break;
	default: pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN; break; 
	}

	_PushReult( pLoginOut ); 

	return true; 
}

void CLoginWorker_Nesia::_GetExtensionInfoNesia( LOGIN_OUTPUT * pRecvData)
{
	char szKey[MAX_PATH];
	char szKeyMD5[MAX_PATH];
	char strCN[MAX_PATH]; 

	// Custromer Number 변환
	i3String::Format(strCN, MAX_PATH, "%d", pRecvData->_ui64UserBillingIdx  );

	// A-KEY 생성 ( SHOP-KEY + CN + SHOP-ID )
	//i3String::Format(szKey, MAX_PATH, "%s%s%s", NESIA_SHOP_KEY, strCN, NESIA_GAME_CODE );
	// A-KEY 생성 ( SHOP-KEY + UID + SHOP-ID )
	i3String::Format(szKey, MAX_PATH, "%s%s%s", NESIA_SHOP_KEY, pRecvData->_Id, NESIA_GAME_CODE );
	m_pHttpBilling->ConvertMD5( szKeyMD5, szKey);

	m_pHttpBilling->ResetArguments();
	m_pHttpBilling->AddArgument( "SID", NESIA_GAME_CODE);
	//m_pHttpBilling->AddArgument( "CN", strCN);
	m_pHttpBilling->AddArgument( "UID", pRecvData->_Id);
	m_pHttpBilling->AddArgument( "A-KEY", szKeyMD5);
	m_pHttpBilling->SetUpPostArguments();

	if(m_pHttpBilling->SendRequestPost())
	{
		m_pHttpBilling->RecvRequest();

		BALANCE_RETURN_VALUE_BY_NESIA ReturnValue;

		_ParsingBillingByNesia( m_pHttpBilling->GetRequestResult(), &ReturnValue);


		if( 0 == i3String::Compare( "S000", ReturnValue.szReturnCode ) )	// 성공
		{
			pRecvData->_iResult	= EVENT_ERROR_SUCCESS;
			pRecvData->_ncCoin	= ReturnValue.nCash + ReturnValue.nGiftCash; 
		}
		else // 실패
		{
			char strTemp[256]; 
 
			i3String::Format(strTemp, 256, "HTTP Billing Error Cord(Cash Balance) = %s", ReturnValue.szReturnCode); 
			g_pModuleLogFile->Write_Log_Login( m_Idx, strTemp );
 
			// 에러처리를 위해 0으로 초기화 합니다.
			pRecvData->_ncCoin = 0;
		}
	}
	else 
	{
		// 재접속.
		// 일정횟수 이상 일어나면, Error처리해야함.
		m_nBillingConnectHTTPRetryCount++;
		m_pHttpBilling->DisConnect();
		m_pHttpBilling->Connect( g_pConfig->m_BillingHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ); 
		g_pModuleLogFile->Write_Log_Login( m_Idx, "ReConnect HTTP Billing", WARNING_AUTH ); 
	}
}

void CLoginWorker_Nesia::_ParsingLogInByNesia(char* pszString, LOGIN_RETURN_VALUE_BY_NESIA * ReturnValue)
{
	char szTemp[1024] = {0,};

	INT32 nStep = 0;		
	INT32 iCount = 0; 
	INT32	iBufferCount = 0; 

	if( i3String::Length( pszString) == 0 || pszString == NULL || i3String::Length( pszString) > 512)
	{
		//에러 리턴 
		g_pModuleLogFile->Write_Log_Login( m_Idx,  "Return Parameter is wrong by Creon!!");
		ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_PACKET;
		return;
	}

	//512넘으면 에러 입니다. 
	while( pszString[iBufferCount] != NULL)
	{
		szTemp[iCount] = pszString[iBufferCount];

		if(szTemp[iCount] == 9) //구분자 '	'
		{
			nStep++;
			szTemp[iCount] = 0; 
			switch(nStep)
			{
			case 1:	//결과값
				if( i3String::CompareCase( szTemp, "00") != 0)
				{//실패 
					if(		 i3String::CompareCase( szTemp, "E01") == 0)ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;	// Wrong PassWord
					else if( i3String::CompareCase( szTemp, "E02") == 0)ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;	// Wrong ID
					else if( i3String::CompareCase( szTemp, "E03") == 0)ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_NOTAVAILABLE;		// Not available
					else if( i3String::CompareCase( szTemp, "E04") == 0)ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_SYS;				// System error
					else if( i3String::CompareCase( szTemp, "E05") == 0)ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_MD5;				// Key value is not match
					else												
					{// error etc.
						char strTemp[256]; 
						i3String::Format(strTemp, 256, "LogIn UnKnown Error Error Code = %s", szTemp);
						g_pModuleLogFile->Write_Log_Login( m_Idx,  strTemp   );
						ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;			
					}
					goto loginreturn; 
				}
				break;
			
			case 2:	//DBIDx == CN(CustomerNumber )
				ReturnValue->iGameIdx = i3String::ToInt(szTemp);		break;
			case 3:	//게임 ID
				i3String::Copy( ReturnValue->szGameID, szTemp, sizeof(ReturnValue->szGameID) );			break;
			case 4:	//Billing ID
				i3String::Copy( ReturnValue->szBillingID, szTemp, sizeof(ReturnValue->szBillingID));		break;
			case 5:	//Name
				i3String::Copy( ReturnValue->szName, szTemp, sizeof(ReturnValue->szName));			break;
			case 6:	//성별
				{
					if(		i3String::CompareCase( szTemp, "M") == 0)	ReturnValue->iGender = 1;	// 남자
					else if(i3String::CompareCase( szTemp, "F") == 0)	ReturnValue->iGender = 2;	// 남자
					else 
					{
						ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_PACKET;
						goto loginreturn; 
					}
				}
				break;


			case 7: // 생년월일
				{
					//성공이면 10 바이트 카피 ****-**-**
					//생년 월일 읽고 리턴					

					//년 
					i3mem::Copy( szTemp, &pszString[iBufferCount + 1], 4);	szTemp[4] = 0; 
					ReturnValue->iBirth = i3String::ToInt(szTemp) * 10000; 
					//월
					i3mem::Copy( szTemp, &pszString[iBufferCount + 6], 2);	szTemp[2] = 0; 
					ReturnValue->iBirth += i3String::ToInt(szTemp) * 100;
					//일 
					i3mem::Copy( szTemp, &pszString[iBufferCount + 9], 2);	szTemp[2] = 0; 
					ReturnValue->iBirth += i3String::ToInt(szTemp); 
				}
				break;
			case 8: // G-Cash
				{
					ReturnValue->iCash = i3String::ToInt(szTemp);

					//G-Cash까지 성공하면 다음 부터는 Active 상태입니다. 
					//성공이면 1 바이트 카피
					i3mem::Copy( szTemp, &pszString[iBufferCount + 1], 1);	szTemp[1] = 0; 
					ReturnValue->iActive = (UINT8)i3String::ToInt(szTemp); 

					ReturnValue->iReturnCode = 1;
					goto loginreturn; 
				}
			default:
				//에러 리턴 
				ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_PACKET;
				goto loginreturn; 
				break; 
			}

			iCount = -1;
		}

		iBufferCount++;
		iCount++;

		if(iBufferCount > 512)
		{
			ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_PACKET;
			goto loginreturn; 
		}
	}

	if( iBufferCount == 0)
	{
		// 내용이 없다는 것임..
		char strTemp[256]; 
		i3String::Format(strTemp, 256, "LogIn Error.(HTTP Return Value is nothing)");
		g_pModuleLogFile->Write_Log_Login( m_Idx,  strTemp  );
		ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;			
	}

loginreturn:
	return; 
}

void CLoginWorker_Nesia::_ParsingBillingByNesia(char* pszString, BALANCE_RETURN_VALUE_BY_NESIA * ReturnValue)
{
#ifdef OLD_VER
	i3mem::FillZero( ReturnValue, sizeof( BALANCE_RETURN_VALUE_BY_NESIA));

	INT32 nStep = 0;
	char * pszTemp = pszString;
	char szTemp[MAX_PATH] = {0,};
	char StoreString[MAX_PATH] = {0,};

	while( *pszTemp)
	{
		if( *pszTemp != 9/*'	'*/)
		{
			INT32 nLength = i3String::Length(szTemp);
			szTemp[nLength] = *pszTemp;
		}
		else
		{
			if( i3String::Length( szTemp) > 0)
			{
				nStep++;

				i3String::Copy( StoreString, szTemp);
				i3mem::FillZero( szTemp, sizeof(char)*MAX_PATH );

				switch(nStep)
				{
				case 1:			
					i3String::Copy( ReturnValue->szReturnCode, StoreString);
					break;
				case 2:
					i3String::Copy( ReturnValue->szServiceCode, StoreString);
					break;
				case 3:
					i3String::Copy( ReturnValue->szID, StoreString);
					break;
				case 4:
					ReturnValue->nGiftCash = i3String::ToInt(StoreString);
					break;
				case 5:
					ReturnValue->nCash = i3String::ToInt(StoreString);
					break;			
				}
			}
		}

		pszTemp++;
	}

	ReturnValue->nDate += i3String::ToInt(szTemp);
#else
	i3mem::FillZero( ReturnValue, sizeof( BALANCE_RETURN_VALUE_BY_NESIA));

	char strReturnCode[MAX_PATH];
	char *pReturnCode;
	pReturnCode = strReturnCode;

	char strCash[MAX_PATH];
	char *pCash;
	pCash = strCash;
	INT32 i32QuoteCount = 0;	// "의 갯수
	INT32 i32ColonCount = 0;	// : 의 갯수
	INT32 i32BraceCount = 0;	// } 의 갯수

	// Format : {"RESULT-CODE":"S000","RESULT-MESSAGE":"Success","SID":"PTBLK","CN":"549087","CASH-BALANCE":404122} 
	while( '\0' != *pszString )
	{
		if( 34 == *pszString )	 i32QuoteCount++;	// 구분자 "
		else if( 58 == *pszString )	 i32ColonCount++;	// 구분자 :
		else if( 125 == *pszString )	// 구분자 }
		{	// 캐쉬 잔액 끝
			*pCash = '\0';
			ReturnValue->nCash = i3String::ToInt(strCash); // 캐쉬 잔액 저장
		}
		else
		{
			switch ( i32QuoteCount ) 
			{
			case 3: // 리턴코드 시작
				*pReturnCode = *pszString;
				pReturnCode++;
				break;
			case 4: // 리턴코드 끝
				*pReturnCode = '\0';
				i3String::Copy( ReturnValue->szReturnCode, strReturnCode, MAX_PATH ); // 리턴 코드 저장 
				break;
			case 22: // 캐쉬 잔액
				{
					if ( 6 == i32ColonCount )
					{	
						if( 1 != i32BraceCount )	// 캐쉬 잔액 시작
						{							
							*pCash = *pszString;
							pCash++;
						}
					}
				}
				break;
			default:
				break;
			} 
		}
		pszString++;
	}
#endif
}
