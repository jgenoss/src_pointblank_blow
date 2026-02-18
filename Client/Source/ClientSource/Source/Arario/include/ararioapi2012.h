#ifndef __ARAUTH4CLI_H__
#define __ARAUTH4CLI_H__

#include "ararioApiCommon.h"
#include "ARTPAuth.h"
#include "ARTPAuthError.h"
#include "AuthFunctor.h"
#include "ArGameInfo.h"
#include <string>

extern "C" 
{
	//\brief Auth by id & password with sync. 
	ARAPI_DLL int  __cdecl AraAuthLoginB( LPCWSTR userId_, LPCWSTR password_, LPSTR sesKey_, DWORD waitTime_ = 5000 );

	//\brief Auth by id & password with async.
	ARAPI_DLL void __cdecl AraAuthLoginNB( LPCWSTR userId_, LPCWSTR password_ );

	//\breif Auth by sessionKey with sync. 
	ARAPI_DLL int __cdecl AraAuthLoginWithSesKeyB( LPCSTR oldSesKey_, LPSTR newSesKey_ );
	
	//\breif Auth by sessionKey with async. 
	ARAPI_DLL int __cdecl AraAuthLoginWithSesKeyNB( LPCSTR oldSesKey_ );

	//\brief Get User Penalty Info, Only support blocking mode
	ARAPI_DLL int __cdecl GetPenaltyInfoB( LPCWSTR userId_, LPCWSTR gameId_, int& penalty_, WCHAR* endDate, DWORD waitTime_ = 5000 );		

	//\brief 아라런쳐가 열어놓은 파이프를 통해서 게임실행시 사용할 파라미터를 넘겨 받는다
	ARAPI_DLL bool __cdecl GetGameParameter( ArGameStartInfo* pGameStartInfo_ );

	//\brief 넷카페 유저인지 체크하는 함수 1
	//\param userIp_	유저의 IP
	ARAPI_DLL int __cdecl IsNetcafeUserB( LPCWSTR userIp_, DWORD waitTime_ = 5000 );		

	//\brief	인코딩된 스트링에서 게임 시작 파라미터를 얻어오는 함수.
	//			ArGameStartInfo 구조체의 아래 변수에만 값이 채워짐
	//			유저아이디			: ArGameStartInfo::userId
	//			유저인덱스			: ArGameStartInfo::userIndex
	//			인증용세션키		: ArGameStartInfo::ticket
	//			패치파일 받을 URL	: ArGameStartInfo::downloadUrl
	//			풀모드실행인가?		: ArGameStartInfo::isFullScreen
	//			QA 버젼인가?		: ArGameStartInfo::optGameInfo::isAlpha
	//\param	gameStartInfo_	게임 시작시 사용할 정보
	//\return 0 보다 작으면 에러. 에러값은 별도 확인 필요
	//\see ARTPError.h
	ARAPI_DLL int __cdecl GetGameParameterFromEncStr( IN LPCWSTR encStr_, OUT ArGameStartInfo& gameStartInfo_ );		

	//\brief	넷카페 유저인지 체크하는 함수 2 (2010.8.22)
	//\param	userIp_	유저의 IP
	//\param	gameId_ 게임 아이디
	ARAPI_DLL int __cdecl IsNetcafeUserB2( LPCWSTR userIp_, LPCWSTR gameId_, DWORD waitTime_ = 5000 );		

	///
	//\brief	회원 정보를 확인하고, 세션키와 회원 USN을 받아온다.
	//\param[in]	userId_		유저아이디
	//\param[in]	pwStr_		암호
	//\param[in]	gmId_		게임아이디
	//\param[in]	extInfoStr_	로그인에 필요한 추가정보, 게임마다 다르지만 아이피와 MacAddress를 로그인 체크용으로 저장한다.
	//\param[out]	seskeyStr_	로그인 후에 받게되는 세션키	
	//\param[out]	userIndex_	로그인후에 받게되는 유저 인덱스 번호
	//\param[in]	waitTime_	네트워크 통신하는 타임아웃시간.
	//\return 0 보다 작으면 에러. 에러값은 별도 확인 필요
	//\see ARTPError.h
	ARAPI_DLL int __cdecl ArarioUserAuthB( LPCWSTR userId_, IN LPCWSTR pwStr_, IN LPCWSTR gmId_, IN LPCWSTR extInfoStr_, OUT LPSTR seskeyStr_, OUT int& userIndex_, IN DWORD waitTime_ = 5000 );

	///
	//\brief	세션키를 갱신하는 API. 
	//\details	추가로 설명
	//			(1)	
	//			변경된 암호문을 사용하는 세션키를 갱신하는 인터페이스 함수로,
	//			추가 파라미터를 받아 아이피와 맥주소를 사용해서 
	//			이전에 로그인한 적이 있는지 확인하는 기능도 추가된다.
	//			> 입력된 상태에 따라 각각의 체크 여부는 다르다.
	//			(2)
	//			단순 세션키 갱신만 사용하려면, extInfoStr_ 변수에 0 (또는 NULL)를 넘겨주면 세션키만 갱신된다.
	//			갱신시에는 세션키의 유효시간을 검사해서 유효하지 않으면 에러를 리턴하고 세션키는 갱신되지 않는다.
	//			> 서버의 설정 상태에 따라 유효시간은 각 서비스마다 다르다.
	//
	//			<example, 추가 파라미터를 만들어서 사용하기>
	//			///-- extInfoStr_ 만들기
	//			// 클래스 선언 후, 파라미터 추가
	//			ArtpParam _param;
	//			_param.Add( ARKWD_PC_MACADDRESS1, _myMacAddr );
	//			// 추가된 파라미터를 한개의 스트링으로 변환
	//			CString _paramStr;
	//			ArtpParam::ParamToURL( _param, _paramStr );
	//			/// 만들어진 스트링 타입의 파라미터는( _paramStr ) 아래 함수에 함수 파라미터로 패스
	//			///-- 함수 콜
	//			WCHAR* _newSesKey[ 2048 ];
	//			int _ret = ArarioUserAuthBySesionKeyB( {저장된세션키}, _paramStr, _newSesKey );
	//			if( _ret < 0 ) // 실패 - 상세한 이유는 _ret의 에러코드값에 있다.
	//			{ 실패 처리 }
	//\param[in]	seskeyStr_	유저 확인을 하고 나서 발급된 세션키. ( 보통 ArarioUserAuthNB() 함수를 통해 받게 되는. )
	//\param[in]	extInfoStr_	유저의 추가 정보 (IP와 맥주소)를 전달한다. 값이 NULL이나 0이면 무시하고 세션키만 갱신한다.
	//\param[out]	seskeyOut_	새로 발급된 세션키. 실패하는 경우에는 값이 없거나 이전과 같다.
	//\return 0 보다 작으면 에러. 에러값은 별도 확인 필요
	//\see ARTPError.h
	ARAPI_DLL int __cdecl ArarioUserAuthBySesionKeyB( IN LPCSTR seskeyIn_, OUT LPSTR seskeyOut_ );


	//\brief ip와 mac주소로 스트링을 만든다.
	ARAPI_DLL int __cdecl MakeExtraParameter1( IN LPCWSTR ipAddr_, IN LPCWSTR macAddr_, OUT LPWSTR outputStr_, IN int outputBufLen_ );
}

#endif //__ARAUTH4CLI_H__