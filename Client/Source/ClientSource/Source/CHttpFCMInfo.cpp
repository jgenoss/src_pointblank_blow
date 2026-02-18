#include "pch.h"
#include "CHttpFCMInfo.h"
#include "GlobalVariables.h"

#include "i3Base/string/ext/mb_to_utf8.h"

I3_CLASS_INSTANCE(CHttpFCMInfo);//, i3NetworkHTTP ); 

CHttpFCMInfo::CHttpFCMInfo()
{
	m_pHttpFCMInfo	= nullptr;	
	m_nFCMInfoConnectHTTPRetryCount = 0;	
}

CHttpFCMInfo::~CHttpFCMInfo()
{
	Destroy();
}

bool CHttpFCMInfo::ConnectionCreate(char* URL)
{
	char szReturnString[MAX_PATH];
	m_pHttpFCMInfo = i3NetworkHTTP::new_object();
	m_pHttpFCMInfo->CreateMethod( 0, 0, 0, 1000, 1024 ); 
	if( false == m_pHttpFCMInfo->Connect( URL, i3NetworkHTTP::REQUEST_METHOD_GET ))
	{	
		sprintf_s( szReturnString, "Error : %s (code: %d)", m_pHttpFCMInfo->GetLastErrorString(), m_pHttpFCMInfo->GetLastErrorCode());
		return false;
	}

	return true;
}

bool CHttpFCMInfo::ConnectionDestroy()
{
	I3_SAFE_RELEASE( m_pHttpFCMInfo ); 
	
	return true;
}

INT32 CHttpFCMInfo::_WorkProcess(char* URL, RETURN_VALUE *RT)
{
	INT32 ECode = EVENT_ERROR_FAIL;
	m_pHttpFCMInfo->ResetArguments();


	// 정상적인 샘플
	// Request  -> http://114.80.132.152/IDCardAuthen?token= &appId=631 &appArea=999 &username=%E5%A7%9C%E6%99%BA%E5%85%83 &idCard=77362119871004123&endpointIP=122.199.144.231&email=hong@naver.com
    // Response -> {"result":"0","code":"2000","msg":"%E6%82%A8%E5%A1%AB%E5%86%99%E7%9A%84%E4%BF%A1%E6%81%AF%E4%B8%8D%E6%AD%A3%E7%A1%AE%EF%BC%8C%E8%AF%B7%E9%87%8D%E6%96%B0%E5%A1%AB%E5%86%99%EF%BC%81","guid":"{41bcb302fb}"}
	//sprintf_s(RT->username,50,"洪吉洞");	

	char strTemp[1024];
	char strUTF8UserName[1024];	

	i3::mb_to_utf8(RT->username, strUTF8UserName, 1024);

//	AnsiToUTF8(RT->username,strUTF8UserName,1024);				// AnsiToUTF8 함수를 완전 삭제하여, 커버리티 이슈를 제거함. (2016.07.05.수빈)

	i3::snprintf( strTemp, MAX_PATH, "%s&appId=%s&appArea=%s&username=%s&idcard=%s&endpointIP=%s&email=%s",
					  URL,RT->appId,RT->appArea,strUTF8UserName,RT->idCard,RT->IP,RT->Email);

	if( m_pHttpFCMInfo->SendRequestGet( strTemp ) )
	{
		m_pHttpFCMInfo->RecvRequest();
		if(_ParsingFCMInfo( m_pHttpFCMInfo->GetRequestResult()))
			ECode = EVENT_ERROR_SUCCESS;
		return ECode;
	}
	else 
	{// 일정횟수 이상 일어나면, Error처리해야함.
		m_nFCMInfoConnectHTTPRetryCount++;

		char strTemp2[1024];

		// Http ReConnect.
		{
			m_pHttpFCMInfo->DisConnect();

			if( !m_pHttpFCMInfo->Connect( URL, i3NetworkHTTP::REQUEST_METHOD_POST ))
			{	
				sprintf_s( strTemp2, "Error : %s (code: %d) - HTTP Connect Fail!",m_pHttpFCMInfo->GetLastErrorString(), m_pHttpFCMInfo->GetLastErrorCode());
			}
		}

		sprintf_s(strTemp2, "ReConnect HTTP Login. [RetryCount= %d]", m_nFCMInfoConnectHTTPRetryCount); 
	}
	return ECode;
}

bool CHttpFCMInfo::_ParsingFCMInfo(char* pszString)
{
	char Result[1024];

	if (pszString == nullptr)
		return false;
	const size_t lenString = i3::generic_string_size(pszString);

	if(lenString == 0 || lenString > 511 )
	{
		//ReturnValue->iReturnCode = EVENT_ERROR_EVENT_LOG_IN_PACKET;
		return false;
	}
	
	i3::safe_string_copy( Result, (pszString+11), 1024 );
	strtok(Result,"\"");
	
	return (!strcmp(Result, "0"));
}
