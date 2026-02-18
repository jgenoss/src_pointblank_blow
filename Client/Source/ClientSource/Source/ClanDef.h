#ifndef _CLAN_DEF_H
#define _CLAN_DEF_H

#include "ClanMark.h"

//	클랜 등급
extern CLAN_RANK		GetClanRank(UINT32 nClanExp);

//	클랜 최대 인원수
extern INT32			GetClanStaffMax(CLAN_UNIT nUnit);

//	클랜 접미사
extern CLAN_UNIT		GetClanUnit(INT32 nClanStaffNum);

//	클랜 레벨업 필요 경험치
extern UINT32			GetClanNextRankExp(UINT32 nCurExp);

//	클랜 레벨업 된 경험치
extern UINT32			GetClanBeforeRankExp(UINT32 nCurExp);

//	클랜 접두사 가져오기
extern void				GetClanRankPreName(i3::rc_wstring& out, UINT32 nClanExp);
extern void				GetClanRankPreName2(i3::rc_wstring& out, UINT8 nClanExpRank);

//	클랜이름 접미사 가져오기
extern void				GetClanRankPostName(i3::rc_wstring& out, INT32 nClanStaffNum);
extern void				GetClanRankPostName2(i3::rc_wstring& out, UINT8 nClanPerRank);

//	클랜 등급별 풀네임 가져오기
extern void				GetClanFullName(i3::wstring& out, UINT32 nClanExp, INT32 nClanStaffNum);
extern void				GetClanFullName2(i3::wstring& out, UINT8 nClanExpRank, UINT8 nClanPerRank);

//	계급 설정
extern void				SetRankImage(i3GuiStatic * pRankStatic, UINT8 nRank);

extern void				SetClanRankImage(i3GuiStatic * pRankStatic, UINT8 nRank);

// 클랜원 등급
extern CLAN_MEMBER_LEVEL_TYPE	GetMemberLevel(UINT8 nLevel); 

// 클랜원 등급네임 반환
extern void				GetMemberLevelName(CLAN_MEMBER_LEVEL_TYPE level, i3::wstring& out);

// 클랜원 등급네임 반환
extern void				GetMemberLevelName2(UINT8 nLevel, i3::wstring& out);

//	클랜원 접속 상태
extern FRIEND_STATE		GetMemberConnectState(UINT32 nState); 

//	클랜원 접속 채널
extern INT32			GetMemberConnectChannel(UINT32 nState);

//	클랜원 접속 방
extern INT32			GetMemberConnectRoomNum(UINT32 nState);

//	클랜원 접속 상태 문자열 반환
extern void				GetMemberConnectStateName(UINT32 nState, i3::rc_wstring& out );

//	접속 상태로 온라인 여부
extern bool				IsOnlineUser(UINT32 nState);

//	신청일 날짜(년)
extern UINT32			GetEntryYear(UINT32 nDate);

//	신청일 날짜(달)
extern UINT32			GetEntryMonth(UINT32 nDate);

//	신청일 날짜(일)
extern UINT32			GetEntryDay(UINT32 nDate);

//	체크박스 
extern void				SetCheckBox(i3GuiCheckBox * pControl, bool bChecked);

//	클랜 경험치 텍스트
extern void				SetClanExpText(i3GuiEditBox * pControl, UINT32 nCurExp, UINT32 nMaxExp);

//	클랜 경험치 게이지
extern void				SetClanExpGauge(i3GuiStatic * pExpBar, REAL32 fResolutionWidth, UINT32 nCurExp, UINT32 nMaxExp);

//	클랜 인원수
extern void				SetClanStaff(i3GuiEditBox * pControl, INT32 nCurrentNum, INT32 nTotalNum);

//	클랜 마스터
// extern void				SetClanMaster(i3GuiEditBox * pName, i3GuiStatic * pRank, const char * pszName, UINT8 nRank);		//쓰이지 않는듯 해 일단 주석..(2014.07.10.수빈)

//	클랜 등급
// extern void				SetClanRank(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank);	//쓰이지 않는듯 해 일단 주석..(2014.07.10.수빈)
//extern void				SetClanRank2(i3GuiEditBox * pRankName, i3GuiStatic * pRankImage, UINT8 nClanExpRank, UINT8 nClanPerRank, UINT8 nClanRank);

//	클랜 순위
extern void				SetClanOrder(i3GuiEditBox * pControl, INT32 nCurOrder, INT32 nPrevOrder);

//	클랜 포인트
extern void				SetClanPoint(i3GuiEditBox * pControl, REAL32 fPoint);

//	클랜 마크
extern bool				SetClanMarkTex(i3GuiStatic * pControl, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

extern bool				SetClanMarkTex(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

extern bool				SetClanMarkTexEx(i3GuiStatic * pControl, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag);

extern bool				SetClanMarkTexEx(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag);

extern bool				SetClanMarkTex(i3GuiStatic * pControl, SLOT_INFO * pInfo);

extern bool				SetDefaultClanMarkTex( i3GuiStatic * pControl, CLAN_MARK_SIZE type);

extern bool				SetDefaultClanMarkTex( i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type);

extern bool				MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, 
										INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx, 
										INT32 u = 0, INT32 v = 0);

extern bool				MakeClanMarkTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, UINT32 markFlag, INT32 u = 0, INT32 v = 0);

extern bool				MakeClanMarkShapeTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, CLAN_MARK_LAYER_TYPE LayerType, 
											  INT32 nImageIdx, INT32 nColorIdx, INT32 u = 0, INT32 v = 0);

//	BinList에서 Compare에 쓰임
extern INT32			CompareString( const char * str1, const char * str2, bool bAsc);

//	클랜 팀 네이밍 반환
extern void				GetBattleTeamName(i3::wstring& out, INT32 idx, bool bIncludeTeamString = true);

//	문자열이 포함되어 있는지 여부 (대소문자 안가림)
//extern INT32			ContainString( const char * pStr1, const char * pStr2);

enum CLAN_MATCH_TYPE
{
	CLAN_MATCH_UNKNOWN = -1,
	CLAN_MATCH_4 = 0,			//	0번으로 무조건 시작해야 함. // revision 37397 클랜매치 4vs4 제거
	CLAN_MATCH_5,
	CLAN_MATCH_6,
	CLAN_MATCH_7,	
	CLAN_MATCH_8,

	CLAN_MATCH_MAX
};

extern const wchar_t*	GetClanBattleMatchType(CLAN_MATCH_TYPE type);		//	클랜전 팀 타입 문자열

extern UINT8			GetClanBattleMatchMember(CLAN_MATCH_TYPE type);		//	클랜전 팀 타입 맴버수

extern CLAN_MATCH_TYPE	ConvertClanBattleMatchType(INT32 nMax);		//	팀 최대수로 팀 구성 타입 반환

bool	CanClanInvite();
bool	CanClanMemberSendNoteAll();


//	클랜 가입 계급 제한
enum CLAN_ADMISSION_LIMIT_RANK
{
	CLAN_LIMIT_RANK_1 = 0,		//	전체 허용
	CLAN_LIMIT_RANK_2 = 4,		//	병장 이상 계급
	CLAN_LIMIT_RANK_3 = 17,		//	소위 이상 계급
	CLAN_LIMIT_RANK_4 = 31,		//	소령 이상 계급
	CLAN_LIMIT_RANK_5 = 46,		//	준장 이상 계급
};

//	클랜 가입 나이 제한
enum CLAN_ADMISSION_LIMIT_AGE
{
	CLAN_LIMIT_AGE_1 = 0,		//	전체 허용
	CLAN_LIMIT_AGE_2 = 15,		//	15세 이상
	CLAN_LIMIT_AGE_3 = 20,		//	20세 이상
	CLAN_LIMIT_AGE_4 = 30,		//	30세 이상
};

//	클랜 스텝 권한
enum CLAN_STEP_AUTHORITY 
{
	CLAN_STEP_AUTHORITY_FALSE = 0,		//	권한 없슴
	CLAN_STEP_AUTHORITY_TRUE = 1,		//	권한 있슴
};

#define ADMISSION_LIMIT_CONTROL_COUNT	2	//	클랜 나이 제한 컨트롤 갯수 (현재 2개)

#define COMPARE_ASC(a, b)			(a == b ? 0 : (a > b ? 1 : -1))
#define COMPARE_DESC(a, b)			(a == b ? 0 : (a > b ? -1 : 1))

#endif // _CLAN_DEF_H