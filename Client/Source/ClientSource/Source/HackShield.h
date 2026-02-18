#if !defined __HACK_SHIELD_H
#define __HACK_SHIELD_H

#if defined (_HACK_SHIELD_)

#include "HShield.h"
#include "HSUpChk.h"

#pragma comment (lib, "version.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "HShield.lib")		//	V3 HackShield 라이브러리
#pragma comment (lib, "HsUpChk.lib")		//	V3 HackShield 라이브러리

#define PB_HSHIELD_GAME_CODE		5350							//	고유 게임 코드 (안랩에서 지급받음, 4자리 숫자)
#define PB_HSHIELD_LICENSE_KEY		"557ADA9BF431C2F034AA31A8"		//	라이선스 키 (안랩에서 지급받음, 24자리 문자열)

namespace HackShield
{
	BOOL RunUpdater(void);						//	스마트 업데이터를 사용할 경우 Init함수보다 선행되어야 한다.
	BOOL Init();								//	초기화
	BOOL StartService( BOOL bErrorHandle);		//	해킹 방지 서비스 시작
	BOOL StopService( BOOL bErrorHandle);		//	해킹 방지 서비스 정지
	BOOL Release( BOOL bErrorHandle);			//	해제

	//	서버에서 전달한 암호화된 버전 요청을 메시지를 복호화하고 현재 클라이언트 파일의 버전을 암호화하여 응답 메시지를 만듭니다.
	BOOL MakeResponse( AHNHS_TRANS_BUFFER * pRequest, AHNHS_TRANS_BUFFER * pResponse);	

	//	BUG_TRAP 사용안 할 경우만...
	//	게임이 비정상 종료되었을 경우에도 핵쉴드는 종료루틴이 실행되어야 하므로 exception시 이 함수를 호출하도록 한다.
	void SetUnhandledException(void);
	void UnhandledExceptionHandler(void);		
}

#endif	// _HACK_SHIELD_
#endif
