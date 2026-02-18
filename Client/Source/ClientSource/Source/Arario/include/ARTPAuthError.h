#ifndef __ARTPAUTHERROR_H__
#define __ARTPAUTHERROR_H__


#include "ARTPError.h"

#define ARTPRET_AUTH_MULTIPLELOGIN		-10		// multiple login
#define ARTPRET_AUTH_NOUSERINFO			-11		// no user info
#define ARTPRET_AUTH_WRONGPASSWORD		-12		// wrong password
#define ARTPRET_AUTH_UNKNOWNREASON		-13		// as words, unknown error (out of expected)
#define ARTPRET_AUTH_INTERNALERROR		-14		// internal error. cause will be various
#define ARTPRET_AUTH_SESKEYNOTISSUED	-15		// failed to issue new session key
#define ARTPRET_AUTH_SESKEYTOOOLD		-16		// session key looks old one
#define ARTPRET_AUTH_SESKEYNOTREFRESHED	-17		// failed to refresh session key
#define ARTPRET_AUTH_ATABROAD			-18		
#define ARTPRET_AUTH_INVALIDKEY			-19

// V2.0
#define ARTPRET_AUTH_NOGAMEINFO			-20

#define ARTPRET_EVT_NOENTERPERMISSION	-21		// No Permission to entering to game 

#define ARTPRET_AUTH_CUSTOM				-30		// custom error
#define ARTPRET_AUTH_CUSTOMERROR1		-31		// no immigrant user (for aceonline)
#define ARTPRET_AUTH_NOCBTMEMBER		-32		// no cbt member(for souten)


// V3.0 -- from 2010.4.20
#define ARTPRET_V3_AUTH_DBPROCERROR				-10001	// 프로시져 내부 에러
#define ARTPRET_V3_AUTH_IDNOTEXIST				-10002	// 아이디 없음
#define ARTPRET_V3_AUTH_WRONGPASSWORD			-10003	// 패스워드틀림
#define ARTPRET_V3_AUTH_BLOCKIP					-10004	// 블럭 아이피
#define ARTPRET_V3_AUTH_BLOCKID					-10005	// 블럭 아이디
#define ARTPRET_V3_AUTH_NOTALLOWEDCOUNTRY		-10006	// 허용국가 아님
#define ARTPRET_V3_AUTH_NOTALLOWLOGIN			-10007	// 접속대상이 아님. 오픈전 이벤트 해당자만 로그인 하도록 할때 사용
#define ARTPRET_V3_AUTH_NOTALLOWMACADDRESS		-10008	// 1개의 MacAddress에서 10분이내에 5개이상의 계정 로그인시 자동 차단
#define ARTPRET_V3_AUTH_5TIMES_WRONGPASSWORD	-10009	// 5번 암호 틀림
#define ARTPRET_V3_AUTH_LOGIN_ABUSE				-10010	// ?분동안 ?회 로그인 시도 - 어뷰져로 간주
#define ARTPRET_V3_AUTH_PW_UPDATE_NEED			-10011	// パスワード変更必要(패스워드변경필요)
#define ARTPRET_V3_AUTH_PW_USED_30DAYS			-10012	// 正常(정상) - パスワード変更日超過(패스워드변경일30일초과) 
#define ARTPRET_V3_AUTH_BLOCKIDBYHACKING		-10013	// 해킹으로 인한 도난된 아이디라서 차단, 웹에서 차단을 풀어야만 사용 가능
#define ARTPRET_V3_AUTH_MENTE					-10098	// 점검 중
#define ARTPRET_V3_AUTH_NETWORKERROR			-10099	// 통신에러?


#endif //__ARTPAUTHERROR_H__