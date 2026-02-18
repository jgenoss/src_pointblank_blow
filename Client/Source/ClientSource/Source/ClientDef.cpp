#include "pch.h"
#include "ClientDef.h"

#include "ServerInfo.h"
#include "Shop.h"

#include <vector>
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/format_string.h"

//UINT8	g_ui8PacketUpdateIdx = 0;;

void ResetInviteInfo(INVITE_INFO& out_info)
{
	out_info._wstr_nick.clear();
	out_info._idxRoom = 0;
	out_info._iUID = 0;
	
	::memset(out_info._pw, 0, sizeof(UINT8) * NET_ROOM_PW);
}

//-----------------------------------------------------------------
//		define 설명
//-----------------------------------------------------------------
//



//-----------------------------------------------------------------
// 키워드	:	EXPORT_AVALIABLE_CODE_VALUE
// 설명		:	코드에 삽입되어 있는 값을 외부파일로 뺄 수 있는 코드는 아래 키워드로
//				표시하였습니다. 틈틈히 외부 파일로 빼도록 하겠습니다. 영권
//
//
// 키워드	:	REFACTOR_TARGET_SHOTGUN
// 설명		:	샷건전 예외처리 변경해야될 부분
//
//
// 키워드	:	REFACTOR_TARGET_QUESTCARD
// 설명		:	임무카드 관련, 표장 등등 수정되어야할 부분
//-----------------------------------------------------------------



//wchar_t와 char을 비교하면 같은 문자열이라도 false를 리턴하기때문에 wchar_t로 바꿈
bool	IsGameMasterNickName(const wchar_t* pNickName)
{
	return i3::contain_string_head( pNickName, GAME_RCSTRING("STR_TBL_CHAT_GM_TEXT"));
}

//	CommonDef안에 모든 텍스트는 스트링 테이블을 사용하므로 텍스트 추가 및 변경시 각언어 스크립트 파일(StrTbl_Header.txt)에 추가 해야 하며
//	ApplyStringTable() 함수안에서 각언어 텍스트로 교체해주어야 한다. 그렇지 않으면 실제 게임상에서 다른 텍스트가 나올수 있다. komet
// #define MAX_ACTION_NAME				64

static i3::rc_wstring s_strActionName[] =
{
	L"조건 없음",
	L"폭탄을 설치하라",							// ACTION_ID_C4_PLANT,							// C4 설치하기
	L"폭탄을 해체하라",							// ACTION_ID_C4_DEFUSE,							// C4 해체하기
	L"머리 명중률 20% 달성",						// ACTION_ID_HIT_HEAD_20,						// 머리 명중률 20% 이상
	L"머리 명중률 30% 달성",						// ACTION_ID_HIT_HEAD_30,						// 머리 명중률 30% 이상
	L"가슴 명중률 20% 달성",						// ACTION_ID_HIT_CHEST_20,						// 가슴 명중률 20% 이상
	L"가슴 명중률 30% 달성",						// ACTION_ID_HIT_CHEST_30,						// 가슴 명중률 30% 이상
	L"팔 명중률 20% 달성",						// ACTION_ID_HIT_ARM_20,						// 팔 명중률 20% 이상
	L"팔 명중률 30% 달성",						// ACTION_ID_HIT_ARM_30,						// 팔 명중률 30% 이상
	L"다리 명중률 20% 달성",						// ACTION_ID_HIT_LEG_20,						// 다리 명중률 20% 이상
	L"다리 명중률 30% 달성",						// ACTION_ID_HIT_LEG_30,						// 다리 명중률 30% 이상
	L"입장하라",									// ACTION_ID_STAGE_ENTER,						// 입장
	L"난입하라",									// ACTION_ID_STAGE_INTERCEPT,					// 난입
	L"승리하라",									// ACTION_ID_WIN,								// 승리
	L"패배하라",									// ACTION_ID_DEFEAT,							// 패배
	L"Kill 달성",								// ACTION_ID_KILL,								// Kill
	L"Death 달성",								// ACTION_ID_DEATH,								// Death
	L"자살 달성",								// ACTION_ID_SUICIDE,							// 자살
	L"퇴장하라",									// ACTION_ID_ENFORCE_EXIT,						// 강제퇴장
	L"최종결과 1등 달성",						// ACTION_ID_RANK_1,							// 최종 결과 순위 1
	L"최종결과 2등 달성",						// ACTION_ID_RANK_2,							// 최종 결과 순위 2
	L"최종결과 3등 달성",						// ACTION_ID_RANK_3,							// 최종 결과 순위 3
	L"최종결과 4등 달성",						// ACTION_ID_RANK_4,							// 최종 결과 순위 4
	L"최종결과 5등 달성",						// ACTION_ID_RANK_5,							// 최종 결과 순위 5
	L"최종결과 6등 달성",						// ACTION_ID_RANK_6,							// 최종 결과 순위 6
	L"최종결과 7등 달성",						// ACTION_ID_RANK_7,							// 최종 결과 순위 7
	L"최종결과 꼴등 달성",						// ACTION_ID_RANK_8,							// 최종 결과 순위 8
	L"Head Shot 달성",							// ACTION_ID_HEADSHOT,							// Head shot 달성
	L"Double Kill 달성",							// ACTION_ID_DOUBLE_KILL,						// Double Kill 달성
	L"Piercing 달성",							// ACTION_ID_PIERCING							// Piercing Kill 달성
	L"Triple Kill 달성",							// ACTION_ID_TRIPLE_KILL,						// Triple Kill 달성
	L"Mass Kill 달성",							// ACTION_ID_MASS_KILL,							// Mass Kill
	L"명중률 40% 달성",							// ACTION_ID_HIT_RATE_40,						// 명중률 40%
	L"명중률 50% 달성",							// ACTION_ID_HIT_RATE_50,						// 명중률 50%
	L"먕중률 60% 달성",							// ACTION_ID_HIT_RATE_60,						// 명중률 60%
	L"킬 비율 40% 달성",							// ACTION_ID_KILL_RATE_40,						// 킬 비율 40%
	L"킬 비율 50% 달성",							// ACTION_ID_KILL_RATE_50,						// 킬 비율 50%
	L"킬 비율 60% 달성",							// ACTION_ID_KILL_RATE_60,						// 킬 비율 60%
	L"헤드 프로텍션 달성",						// ACTION_ID_HELMET_PROTECTION,					// 헬맷 방어
	L"상대 진영의 목표물 파괴",					// ACTION_ID_DESTROY,							// 목표물 파괴
	L"특정 적군 Kill 달성",						// ACTION_ID_KILL_WEAPONCLASS,					// 특정클래스 무기 사용중인 유저 킬
	L"특정 적군 Double Kill 달성",				// ACTION_ID_DOUBLE_KILL_WEAPONCLASS,			// 특정클래스 무기 사용중인 유저 더블킬
	L"특정 적군 Triple Kill 달성",				// ACTION_ID_TRIPLE_KILL_WEAPONCLASS,			// 특정클래스 무기 사용중인 유저 트리플킬
	L"사용",										// ACTION_ID_USE_WEAPON,						// 특정 무기 사용
	L"Chain Kill 달성",							// ACTION_ID_CHAINKILLER,						// 체인킬
	L"Chain Slugger 달성",						// ACTION_ID_CHAINSLUGGER,						// 체인 슬러거
	L"Chain Kill중인 적군 Kill",					// ACTION_ID_CHAINSTOPPER,						// 체인 스타퍼
	L"Chain HeadShot 달성",						// ACTION_ID_CHAINHEADSHOT,						// 체인 헤드샷
	L"아군 진영의 목표물 보호",					// ACTION_ID_PROTECT,							// 오브젝트 보호
	L"데스블로우",				//ACTION_ID_DEATHBLOW,						// 데스블로우
	L"터치다운",				//ACTION_ID_TOUCHDOWN,						// 터치다운
	L"에이스 어태커",				//ACTION_ID_TOUCHDOWN_ACE_ATTACKER,			// 에이스 어태커
	L"해트트릭",				//ACTION_ID_TOUCHDOWN_HATTRICK,				// 해트트릭
	L"게임메이커",				//ACTION_ID_TOUCHDOWN_GAME_MAKER,				// 게임메이커
	L"킬 어시스트",				//ACTION_ID_KILL_ASSIST,						// 킬 어시스트
	L"인간을 죽여라",				//ACTION_ID_KILL_MAN,							// 인간을 죽여라
};

i3::rc_wstring		getActionName2( ACTION_ID id)
{
	I3ASSERT(GET_ARRAY_COUNT(s_strActionName) == ACTION_ID_COUNT);

	return s_strActionName[ id];
}





void		FormatLocation(UINT32 State, i3::rc_wstring& out)
{

	INT32 Server = GET_FRIEND_SERVER(State);
	INT32 Channel = GET_FRIEND_CHANNEL(State) + 1;
	INT32 RoomNum = GET_FRIEND_ROOMIDX(State) + 1;

	if( g_pServerInfo->IsUseServerSelectPage() )
	{
		Server--;
	}

	INT32	DisplayServerNumber = (Server*10)+Channel;

	out = i3::format_string( GAME_RCSTRING("STBL_IDX_CLAN_DECLARE_ROOM"), DisplayServerNumber, RoomNum);
}


//	Increase grenade slot 캐시템에 적용되는 무기인지 판단
bool IsWeaponByIncreaseGrenadeSlot( WEAPON_CLASS_TYPE nClass, UINT32 nNumber)
{
	switch( nClass)
	{
	case WEAPON_CLASS_THROWING_GRENADE:		//	투척무기
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( nClass, (WEAPON::RES_ID) nNumber, true);
			if( pWeaponInfo->GetLoadBullet() == 1)
				return true;	// 수류탄 계열 클래스가 분리되어 클래스만 체크하면 됨. swoongoo
		}
		break;
	}

	return false;
}

//	Increase grenade slot 캐시템에 적용되는 무기인지 판단
bool IsWeaponByIncreaseDualGrenadeSlot( WEAPON_CLASS_TYPE nClass, UINT32 nNumber)
{
	switch( nClass)
	{
	case WEAPON_CLASS_THROWING_GRENADE:		//	투척무기
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( nClass, (WEAPON::RES_ID) nNumber, true);
			if( pWeaponInfo->GetLoadBullet() == 2)
				return true;
		}
		break;
	}

	return false;
}


UINT32 ConvertSecToUseDay(UINT32 sec)
{	//	상점 또는 인벤토리에서 사용 기간을 나타낼때 초단위를 시간단위로 변환해줍니다.
	INT32 time = sec;

	//	초를 시간단위로 변경.
	INT32 remainder = time % 86400;
	INT32 accm = time / 86400;

	//	올림으로 계산합니다.
	return (UINT32)(accm + (remainder > 0 ? 1 : 0));
}

UINT32 ConvertSecToUseHour( UINT32 sec)
{
	INT32 time = sec;

	// 초를 시간으로
	INT32 remainder = time % 3600;
	INT32 accm = time / 3600;

	return (UINT32)(accm + (remainder > 0 ?1:0));
}

UINT32 ConvertSecToUseMinute(UINT32 sec)
{
	INT32 time = sec;

	INT32 remainder = time % 60;
	INT32 accum = time / 60;
	
	return (UINT32)(accum + (remainder > 0 ? 1 : 0));
}

UINT32 ConvertHourToUseDay(UINT32 hour)
{	//	상점 또는 인벤토리에서 사용 기간을 나타낼때 시간단위를 일단위로 변환해줍니다.
	return (UINT32)(hour / 24);
}



INT32 GetCashItemByGroup( T_ItemID ItemID)
{
	if ( GET_ITEM_TYPE(ItemID) == ITEM_TYPE_MAINTENANCE || GET_ITEM_TYPE(ItemID) == ITEM_TYPE_WRAP_COUNT )
		return ItemID;

	INT32 ItemType		= GET_ITEM_SUBTYPE(CASHITEM_DAY_NO);
	INT32 ItemNumber	= GET_ITEM_NUMBER(ItemID);
	INT32 UseItemID		= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, ItemType, ItemNumber);

	return UseItemID;
}



//다른 나라 일때 스트링 테이블에서 읽어 오는것으로 만듬

void ApplyClientStringTable(void)
{
	//	빌드 심볼로 NO_STR_TBL를 사용하면 스트링 테이블을 사용하지 않습니다. (스트링 테이블을 사용하지 않는 서버에서 사용)
#if !defined( NO_STR_TBL)

	// Mission Type
	// 클라이언트에서만 쓰는 것 같습니다.
	// 추후 이 함수에서 다른 함수로 옮길것입니다. 영권(09-08-26)
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_NA], GAME_RCSTRING("STR_HEADER_NA"));							//	"NA",
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_DEATHMATCH], GAME_RCSTRING("STR_HEADER_MODE_TEAM_DEATH_MATCH"));	//	"단체전",
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_BOMB], GAME_RCSTRING("STR_HEADER_MODE_EXPLOSION"));				//	"폭파미션",
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_DESTROY], GAME_RCSTRING("STR_HEADER_MODE_DESTROY"));				//	"파괴미션",
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_ANNIHILATION], GAME_RCSTRING("STR_HEADER_MODE_ANNIHILATION"));	//	"섬멸전",
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_DEFENCE], GAME_RCSTRING("STR_HEADER_MODE_DEFENCE"));				//	"방어미션"
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_ESCAPE], GAME_RCSTRING("STR_HEADER_MODE_BOSS"));					//	"보스전",
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_SUDDEN_DEATH], GAME_RCSTRING("STR_HEADER_MODE_SUDDEN_DEATH"));	//	"서든데쓰",
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_HEAD_HUNTER], GAME_RCSTRING("STR_HEADER_MODE_HEAD_HUNTER"));
	//i3::safe_string_copy( s_pStageTypeName[ STAGE_MODE_HEAD_KILLER], GAME_RCSTRING("STR_HEADER_MODE_HEAD_KILLER"));

	// Action Name
	// 클라이언트에서만 쓰는 것 같습니다.
	// 추후 이 함수에서 다른 함수로 옮길것입니다. 영권(09-08-26)
	s_strActionName[0] = GAME_RCSTRING("STR_HEADER_NO_CONDITION"); 			//	조건없음
	s_strActionName[1] = GAME_RCSTRING("STR_HEADER_ACTIVE_BOMB"); 			// ACTION_ID_C4_PLANT,					// C4 설치하기
	s_strActionName[2] = GAME_RCSTRING("STR_HEADER_DISAM_BOMB"); 				// ACTION_ID_C4_DEFUSE,					// C4 해체하기
	s_strActionName[3] = GAME_RCSTRING("STR_HEADER_HIT_RATE_HEAD_20"); 		// ACTION_ID_HIT_HEAD_20,				// 머리 명중률 20% 이상
	s_strActionName[4] = GAME_RCSTRING("STR_HEADER_HIT_RATE_HEAD_30"); 		// ACTION_ID_HIT_HEAD_30,				// 머리 명중률 30% 이상
	s_strActionName[5] = GAME_RCSTRING("STR_HEADER_HIT_CHEST_20"); 			// ACTION_ID_HIT_CHEST_20,				// 가슴 명중률 20% 이상
	s_strActionName[6] = GAME_RCSTRING("STR_HEADER_HIT_CHEST_30"); 			// ACTION_ID_HIT_CHEST_30,				// 가슴 명중률 30% 이상
	s_strActionName[7] = GAME_RCSTRING("STR_HEADER_HIT_ARM_20"); 				// ACTION_ID_HIT_ARM_20,				// 팔 명중률 20% 이상
	s_strActionName[8] = GAME_RCSTRING("STR_HEADER_HIT_ARM_30"); 				// ACTION_ID_HIT_ARM_30,				// 팔 명중률 30% 이상
	s_strActionName[9] = GAME_RCSTRING("STR_HEADER_HIT_LEG_20"); 				// ACTION_ID_HIT_LEG_20,				// 다리 명중률 20% 이상
	s_strActionName[10] = GAME_RCSTRING("STR_HEADER_HIT_LEG_30"); 			// ACTION_ID_HIT_LEG_30,				// 다리 명중률 30% 이상
	s_strActionName[11] = GAME_RCSTRING("STR_HEADER_DO_ENTERANCE"); 			// ACTION_ID_STAGE_ENTER,				// 입장
	s_strActionName[12] = GAME_RCSTRING("STR_HEADER_STAGE_INTERCEPT"); 		// ACTION_ID_STAGE_INTERCEPT,			// 난입
	s_strActionName[13] = GAME_RCSTRING("STR_HEADER_DO_WIN"); 				// ACTION_ID_WIN,						// 승리
	s_strActionName[14] = GAME_RCSTRING("STR_HEADER_DO_DEFEAT"); 				// ACTION_ID_DEFEAT,					// 패배
	s_strActionName[15] = GAME_RCSTRING("STR_HEADER_DO_KILL"); 				// ACTION_ID_KILL,						// Kill
	s_strActionName[16] = GAME_RCSTRING("STR_HEADER_DO_DEATH"); 				// ACTION_ID_DEATH,						// Death
	s_strActionName[17] = GAME_RCSTRING("STR_HEADER_DO_KILL_SELF"); 			// ACTION_ID_SUICIDE,					// 자살
	s_strActionName[18] = GAME_RCSTRING("STR_HEADER_DO_OUT"); 				// ACTION_ID_ENFORCE_EXIT,				// 강제퇴장
	s_strActionName[19] = GAME_RCSTRING("STR_HEADER_DO_RANK_1"); 				// ACTION_ID_RANK_1,					// 최종 결과 순위 1
	s_strActionName[20] = GAME_RCSTRING("STR_HEADER_DO_RANK_2"); 				// ACTION_ID_RANK_2,					// 최종 결과 순위 2
	s_strActionName[21] = GAME_RCSTRING("STR_HEADER_DO_RANK_3"); 				// ACTION_ID_RANK_3,					// 최종 결과 순위 3
	s_strActionName[22] = GAME_RCSTRING("STR_HEADER_DO_RANK_4"); 				// ACTION_ID_RANK_4,					// 최종 결과 순위 4
	s_strActionName[23] = GAME_RCSTRING("STR_HEADER_DO_RANK_5"); 				// ACTION_ID_RANK_5,					// 최종 결과 순위 5
	s_strActionName[24] = GAME_RCSTRING("STR_HEADER_DO_RANK_6"); 				// ACTION_ID_RANK_6,					// 최종 결과 순위 6
	s_strActionName[25] = GAME_RCSTRING("STR_HEADER_DO_RANK_7"); 				// ACTION_ID_RANK_7,					// 최종 결과 순위 7
	s_strActionName[26] = GAME_RCSTRING("STR_HEADER_DO_RANK_WORST"); 			// ACTION_ID_RANK_8,					// 최종 결과 순위 8
	s_strActionName[27] = GAME_RCSTRING("STR_HEADER_DO_HEADSHOT"); 			// ACTION_ID_HEADSHOT,					// Head shot 달성
	s_strActionName[28] = GAME_RCSTRING("STR_HEADER_DO_DOUBLE_KILL"); 		// ACTION_ID_DOUBLE_KILL,				// Double Kill 달성
	s_strActionName[29] = GAME_RCSTRING("STR_HEADER_DO_PIERCING"); 			// ACTION_ID_PIERCING					// Piercing Kill 달성
	s_strActionName[30] = GAME_RCSTRING("STR_HEADER_DO_TRIPLE_KILL"); 		// ACTION_ID_TRIPLE_KILL,				// Triple Kill 달성
	s_strActionName[31] = GAME_RCSTRING("STR_HEADER_DO_MASS_KILL"); 			// ACTION_ID_MASS_KILL,					// Mass Kill
	s_strActionName[32] = GAME_RCSTRING("STR_HEADER_HIT_RATE_40"); 			// ACTION_ID_HIT_RATE_40,				// 명중률 40%
	s_strActionName[33] = GAME_RCSTRING("STR_HEADER_HIT_RATE_50"); 			// ACTION_ID_HIT_RATE_50,				// 명중률 50%
	s_strActionName[34] = GAME_RCSTRING("STR_HEADER_HIT_RATE_60"); 			// ACTION_ID_HIT_RATE_60,				// 명중률 60%
	s_strActionName[35] = GAME_RCSTRING("STR_HEADER_KILL_RATE_40"); 			// ACTION_ID_KILL_RATE_40,				// 킬 비율 40%
	s_strActionName[36] = GAME_RCSTRING("STR_HEADER_KILL_RATE_50"); 			// ACTION_ID_KILL_RATE_50,				// 킬 비율 50%
	s_strActionName[37] = GAME_RCSTRING("STR_HEADER_KILL_RATE_60"); 			// ACTION_ID_KILL_RATE_60,				// 킬 비율 60%
	s_strActionName[38] = GAME_RCSTRING("STR_HEADER_HELMET_PROTECTION"); 		// ACTION_ID_HELMET_PROTECTION,			// 헬맷 방어
	s_strActionName[39] = GAME_RCSTRING("STR_HEADER_DO_DESTROY_ENEMY"); 		// ACTION_ID_DESTROY,					// 목표물 파괴
	s_strActionName[40] = GAME_RCSTRING("STR_HEADER_DO_KILL_ENEMY"); 			//ACTION_ID_KILL_WEAPONCLASS,			// 특정클래스 무기 사용중인 유저 킬
	s_strActionName[41] = GAME_RCSTRING("STR_HEADER__DO_DOUBLE_KILL"); 		//ACTION_ID_DOUBLE_KILL_WEAPONCLASS,	// 특정클래스 무기 사용중인 유저 더블킬
	s_strActionName[42] = GAME_RCSTRING("STR_HEADER_DO_TRIPLE_KILL_ENEMY"); 	//ACTION_ID_TRIPLE_KILL_WEAPONCLASS,	// 특정클래스 무기 사용중인 유저 트리플킬
	s_strActionName[43] = GAME_RCSTRING("STR_HEADER_DO_USE"); 				//ACTION_ID_USE_WEAPON,					// 특정 무기 사용
	s_strActionName[44] = GAME_RCSTRING("STR_HEADER_DO_CHAINKILLER"); 		//ACTION_ID_CHAINKILLER,				// 체인킬
	s_strActionName[45] = GAME_RCSTRING("STR_HEADER_DO_CHAINSLUGGER"); 		//ACTION_ID_CHAINSLUGGER,				// 체인 슬러거
	s_strActionName[46] = GAME_RCSTRING("STR_HEADER_DO_CHAINSTOPPER"); 		//ACTION_ID_CHAINSTOPPER,				// 체인 스타퍼
	s_strActionName[47] = GAME_RCSTRING("STR_HEADER_DO_CHAINHEADSHOT"); 		//ACTION_ID_CHAINHEADSHOT,				// 체인 헤드샷
	s_strActionName[48] = GAME_RCSTRING("STR_HEADER_DO_PROTECT_OBJECT"); 		//ACTION_ID_PROTECT,					// 오브젝트 보호
	s_strActionName[49] = GAME_RCSTRING("STR_HEADER_DO_DEATHBLOW"); 			//ACTION_ID_DEATHBLOW,					// 데스블로우
	s_strActionName[50] = GAME_RCSTRING("STR_HEADER_DO_TCH_DN"); 				//ACTION_ID_TOUCHDOWN,					// 터치다운
	s_strActionName[51] = GAME_RCSTRING("STR_HEADER_DO_TCH_DN_ACE_ATTACKER"); //ACTION_ID_TOUCHDOWN_ACE_ATTACKER,		// 에이스 어태커
	s_strActionName[52] = GAME_RCSTRING("STR_HEADER_DO_TCH_DN_HATTRICK"); 	//ACTION_ID_TOUCHDOWN_HATTRICK,			// 해트트릭
	s_strActionName[53] = GAME_RCSTRING("STR_HEADER_DO_TCH_DN_GAME_MAKER"); 	//ACTION_ID_TOUCHDOWN_GAME_MAKER,		// 게임메이커
	s_strActionName[54] = GAME_RCSTRING("STR_HEADER_DO_KILL_ASSIST"); 		//ACTION_ID_KILL_ASSIST,				// 킬 어시스트
	s_strActionName[55] = GAME_RCSTRING("STR_HEADER_DO_KILL_MAN"); 			//ACTION_ID_KILL_MAN,						// 인간을 죽여라

#endif
}


#pragma region void GetCurrentPcSpecs(LPDIRECT3D9 pD3d, USER_INFO_PCSPECS& tInfo)
// 2016-11-02 User PC Specs 추가
void GetCurrentPcSpecs(LPDIRECT3D9 pD3d, USER_INFO_PCSPECS& tInfo)
{
	SYSTEM_INFO si;
	D3DADAPTER_IDENTIFIER9 id;
	OSVERSIONINFOEX osvi;
	MEMORYSTATUSEX statex;

	memset(&tInfo, 0, sizeof(USER_INFO_PCSPECS));
	memset(&si, 0, sizeof(SYSTEM_INFO));
	memset(&id, 0, sizeof(D3DADAPTER_IDENTIFIER9));
	memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
	memset(&statex, 0, sizeof(MEMORYSTATUSEX));

	GetSystemInfo(&si);
	pD3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &id);

	// Cpu ID
	int CPUInfo[4] = { -1 };
	char szCpuString[0x40];
	memset(szCpuString, 0, sizeof(szCpuString));

	__cpuid(CPUInfo, 0);

	*((int*)szCpuString) = CPUInfo[1];
	*((int*)(szCpuString + 4)) = CPUInfo[3];
	*((int*)(szCpuString + 8)) = CPUInfo[2];

	strupr(szCpuString);


	// 유저 옵션 사용정보 추가. 
	// tInfo._cpuId 8bit씩 순서대로 cpu종류, 풀스크린 여부, Hz, Depth 옵션값 수집. 

	UINT8 cpuType = 0;
	UINT8 screenType = 0;
	UINT8 screenHz = 0;
	UINT8 screenDepth = 0;

	if (0 < strstr(szCpuString, "INTEL")) //Intel
	{
		cpuType = (UINT8)1;
	}
	else if (0 < strstr(szCpuString, "AMD")) // AMD
	{
		cpuType = (UINT8)2;
	}
	else // Unknown
	{
		cpuType = (UINT8)0;
	}	

	screenType = (g_pEnvSet->m_bFullScreen) ? (UINT8)1 : (UINT8)0;
	screenHz = (UINT8)g_pEnvSet->m_nScreenHz;
	screenDepth = (UINT8)g_pEnvSet->m_ScreenDepth;
	
	tInfo._cpuId = (tInfo._cpuId | cpuType) << 8;
	tInfo._cpuId = (tInfo._cpuId | screenType) << 8;
	tInfo._cpuId = (tInfo._cpuId | screenHz) << 8;
	tInfo._cpuId = (tInfo._cpuId | screenDepth);

	// CPU core count
	tInfo._cpuCoreCnt = (si.dwNumberOfProcessors & 0xFF);

	// CPU hz 
	DWORD BufSize = _MAX_PATH;
	DWORD dwMHz = _MAX_PATH;
	HKEY hKey;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_READ, &hKey);
	::RegQueryValueEx(hKey, "~MHz", NULL, NULL, (LPBYTE)&dwMHz, &BufSize);
	tInfo._cpuHz = (dwMHz & 0xFFFF);

	// RAM 크기
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	tInfo._ramSize = ((statex.ullTotalPhys / (UINT64)1048576) & 0xFFFF);

	// GPU 제조사 ID
	tInfo._gpuVendorId = id.VendorId;
	// GPU Device ID
	tInfo._gpuDevId = id.DeviceId;
	// GPU Driver ID
#ifdef _WIN32
	tInfo._gpuDriverId = id.DriverVersion.HighPart;
#else
	tInfo._gpuDriverId = id.DriverVersionHighPart;
#endif // _WIN32
	// VRAM Size
	tInfo._dvm = ((i3RenderContext::GetCaps()->getVRAMSize() >> 20) & 0xFFFF);

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (NULL != GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		// OS Version
		// 10 일 경우 제대로 체크 못해서 8 이상이면 레지스트리에서.
		if ((osvi.dwMajorVersion > 6) ||
			(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 2))
		{
			int buffer = 0;

			// open Regstry Key
			RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
				0, KEY_QUERY_VALUE, &hKey);

			// Registry Key가 없는 경우가 있다....
			// Read Regstry Key
			if (ERROR_SUCCESS == RegQueryValueEx(hKey, _T("CurrentMajorVersionNumber"), 0, &dwMHz, (LPBYTE)&buffer, &BufSize))
			{
				osvi.dwMajorVersion = buffer;

				if (ERROR_SUCCESS == RegQueryValueEx(hKey, _T("CurrentMinorVersionNumber"), 0, &dwMHz, (LPBYTE)&buffer, &BufSize)) {
					osvi.dwMinorVersion = buffer;
				}
			}
			RegCloseKey(hKey);
		}
		tInfo._osVer = ((osvi.dwMajorVersion * 10) + osvi.dwMinorVersion) & 0xFF;

		// SP version
		if (osvi.szCSDVersion[0] == '\0')
			tInfo._spVer = 0x00;
		else
		{
			int i = 0;
			WCHAR szSp[2] = { 0, };
			for (i = 0; i < 128; ++i)
			{
				if (osvi.szCSDVersion[i] == '\0')
				{
					szSp[0] = osvi.szCSDVersion[i - 1];
					tInfo._spVer = (_wtoi(szSp) & 0xFF);
					break;
				}
			}
		}
	}

	// 64bit 여부
#if defined(_WIN64)
	tInfo._64bit = 0x01;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows
	// so must sniff
	BOOL is64Bit = FALSE;
	BOOL f64 = FALSE;
	is64Bit = ::IsWow64Process(GetCurrentProcess(), &f64) && f64;
	tInfo._64bit = (is64Bit & 0xFF);
#else
	tInfo._64bit = 0xFF; // Win64 does not support Win16
#endif

	HDC dc = GetDC(NULL);
	// Screen Width
	tInfo._screenW = (GetDeviceCaps(dc, HORZRES) & 0xFFFF);
	// Screen Height
	tInfo._screenH = (GetDeviceCaps(dc, VERTRES) & 0xFFFF);
	// Screen CX
	tInfo._screenCX = ((tInfo._screenW >> 6) & 0xFF);
	// Screen CY
	tInfo._screenCY = ((tInfo._screenH >> 6) & 0xFF);

	ReleaseDC(NULL, dc);

}

#pragma endregion


