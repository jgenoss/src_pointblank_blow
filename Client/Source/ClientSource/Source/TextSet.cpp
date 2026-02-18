#include "pch.h"
#include "TextSet.h"

//#define EMPTY_TEXTW L""
#include "ClientStageInfoUtil.h"

UINT32 TextSet::GetTime_Second(UINT32 GameTime)
{
	switch(GameTime)
	{
//	case BATTLE_TIME_TYPE_3:	//3분
	case BATTLE_TIME_MISSION_TYPE_3:	return 3*60;	// 3분
	case BATTLE_TIME_MISSION_TYPE_5:	return 5*60;	// 5분
	case BATTLE_TIME_MISSION_TYPE_7:	return 7*60;	// 7분
	case BATTLE_TIME_TYPE_5:			return 5*60;	// 5분
	case BATTLE_TIME_TYPE_10:			return 10*60;	// 10분
	case BATTLE_TIME_TYPE_15:			return 15*60;	// 15분
	case BATTLE_TIME_TYPE_20:			return 20*60;	// 20분
	case BATTLE_TIME_TYPE_25:			return 25*60;	// 25분
	case BATTLE_TIME_TYPE_30:			return 30*60;	// 30분
	}

	return 0;
}

i3::rc_wstring TextSet::GameTime(UINT32 GameTime)
{
	switch(GameTime)
	{
//	case BATTLE_TIME_TYPE_3:	//3분
	case BATTLE_TIME_MISSION_TYPE_3:	return GAME_RCSTRING("STBL_IDX_ROOM_TIME1");	// 3분
	case BATTLE_TIME_MISSION_TYPE_5:	return GAME_RCSTRING("STBL_IDX_ROOM_TIME2");	// 5분
	case BATTLE_TIME_MISSION_TYPE_7:	return GAME_RCSTRING("STBL_IDX_ROOM_TIME3");	// 7분
	case BATTLE_TIME_TYPE_5:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME2");	// 5분
	case BATTLE_TIME_TYPE_10:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME4");	// 10분
	case BATTLE_TIME_TYPE_15:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME5");	// 15분
	case BATTLE_TIME_TYPE_20:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME6");	// 20분
	case BATTLE_TIME_TYPE_25:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME7");	// 25분
	case BATTLE_TIME_TYPE_30:			return GAME_RCSTRING("STBL_IDX_ROOM_TIME8");	// 30분
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::KillCount( UINT32 KillCount )
{
	// Kill 숫자
	switch( KillCount)
	{		
	case BATTLE_KILL_TYPE_60:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT1");	// 60Kill
	case BATTLE_KILL_TYPE_80:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT2");	// 80Kill
	case BATTLE_KILL_TYPE_100:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT3");	// 100Kill
	case BATTLE_KILL_TYPE_120:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT4");	// 120Kill
	case BATTLE_KILL_TYPE_140:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT5");	// 140Kill
	case BATTLE_KILL_TYPE_160:	return GAME_RCSTRING("STBL_IDX_KILL_COUNT6");	// 160Kill
	}

	return i3::rc_wstring();
}


i3::rc_wstring TextSet::Round( UINT32 Round )
{
	switch(Round)
	{
	case BATTLE_ROUND_TYPE_1:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE1");	// 1 Round
	case BATTLE_ROUND_TYPE_3:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE2");	// 3 Rounds
	case BATTLE_ROUND_TYPE_5:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE3");	// 5 Rounds
	case BATTLE_ROUND_TYPE_7:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE4");	// 7 Rounds
	case BATTLE_ROUND_TYPE_9:	return GAME_RCSTRING("STBL_IDX_ROUND_COUNT_TYPE5");	// 9 Rounds
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::AIModeLevel(UINT32 level)
{
	switch(level)
	{
	case 1:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_J");	// 1등급(쉬움)
	case 2:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_I");	// 2등급
	case 3:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_H");	// 3등급 
	case 4:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_G");	// 4등급
	case 5:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_F");	// 5등급
	case 6:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_E");	// 6등급
	case 7:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_D");	// 7등급
	case 8:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_C");	// 8등급
	case 9:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_B");	// 9등급
	case 10:	return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_A");	// 10등급
	}

	return i3::rc_wstring();
}

//제압 미션 난이도 스트링( 이후 String.txt 로 만들어야 합니다. )
const i3::string TextSet::domi_script( UINT32 level)
{
	//switch(level)
	//{
	//case 1:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_EASY"); /*"Easy"*/
	//case 2:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_NORMAL");/*"Normal"*/
	//case 3:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_HARD");/*"Hard"*/
	//case 4:		return GAME_RCSTRING("STR_TBL_GUI_READY_GRADE_VERYHARD");/*"very Hard"*/
	//default:	return "not found";
	//}
	i3::string str = "script_";

	char strNum[4] = {0,};
	itoa( level, strNum, 10);

	str += strNum;

	if( level == 6) str = "script_random";
	return str;
}

i3::rc_wstring TextSet::Mode( UINT32 Mode )
{
	i3::rc_wstring ret;
	switch(Mode)
	{
		//STBL_IDX_MODE_TYPE1 : 공수교대
	case STAGE_MODE_CROSSCOUNT:
	case STAGE_MODE_ESCAPE:	

		ret = GAME_RCSTRING("STBL_IDX_MODE_TYPE1");// 공수교대

		if( i3::generic_is_equal( ret, L"STBL_IDX_MODE_TYPE1" ) )
			return i3::rc_wstring();

		return ret;
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::VictoryCondition( STAGE_MODE StageType, UINT32 GameType )
{
	switch( StageType )
	{
	case STAGE_MODE_DESTROY			:
	case STAGE_MODE_DEFENCE			:
	case STAGE_MODE_ANNIHILATION	:
	case STAGE_MODE_BOMB			:
	case STAGE_MODE_CONVOY			:
		{
			return TextSet::Round(GameType);
		}
		break;
	case STAGE_MODE_CROSSCOUNT:
	case STAGE_MODE_ESCAPE			:
		{
			return TextSet::Mode(StageType);
		}
		break;
	case STAGE_MODE_DEATHMATCH		:
	default							:
		{
			return TextSet::KillCount(GameType);
		}
		break;
	}

	//return EMPTY_TEXT;
}


i3::rc_wstring TextSet::PCCafeMessage( UINT32 PCCafe )
{
	switch(PCCafe)
	{
	case PC_NORMAL_CAFE			:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PCROOM");			/*PC방에서 접속하셨습니다.*/
	case PC_PREMIUM_CAFE		:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PRIMIUM_PCROOM");	/*프리미엄 PC방에서 접속하셨습니다.*/
	case PC_VACATION_EVENT_ITEM	:	return L"ID Bonus Package";
	case PC_GARENA_GCA			:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PCROOM");			/*PC방에서 접속하셨습니다.*/
	case PC_GARENA_GCA_PLUS		:	return GAME_RCSTRING("STR_TBL_GUI_LOBBY_CONNECT_PCROOM");			/*PC방에서 접속하셨습니다.*/
	}

	return i3::rc_wstring();
}

i3::rc_wstring TextSet::ItemType(UINT8 ItemType)
{
	switch(ItemType)
	{
	case ITEM_TYPE_UNKNOWN:			return i3::rc_wstring();										
	case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ASSAULT");	// 돌격소총 전용
	//// 1.5 Check ClassType
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SMG");		// 기관단총 전용
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SNIPER");		// 저격총 전용
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SHOTGUN");	// 산탄총 전용
	//case ITEM_TYPE_PRIMARY:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_MG");			// 기관총 전용
	case ITEM_TYPE_SECONDARY:		return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_HANDGUN");	// 보조 무기 전용
	case ITEM_TYPE_MELEE:			return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_KNIFE");		// 근접 무기 전용
	case ITEM_TYPE_THROWING1:		return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_THROWING");	// 투척 무기 전용
	case ITEM_TYPE_THROWING2:		return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ITEM");		// 특수 무기 전용
	case ITEM_TYPE_CHARA:			return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_CHARA");		// 캐릭터 전용
	case ITEM_TYPE_HEADGEAR:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_HEAD");		// 머리 전용
	case ITEM_TYPE_MAINTENANCE:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_TERM");		// 지속성 전용
	case ITEM_TYPE_WRAP_PERIOD:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_COUNT");		// 소모성 전용
	case ITEM_TYPE_WRAP_COUNT:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_COUNT");		// 소모성 전용
	case ITEM_TYPE_INGAMEITEM:																// 야전상점 전용템(예비)
	default:						return L"NONE_TYPE";
	}
}

i3::rc_wstring TextSet::ClanJoinLimitToRank(UINT8 Rank)
{
	if(Rank >= RANK_46)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_4");
	else if (Rank >= RANK_31)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_3");
	else if( Rank >= RANK_17)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_2");
	else if ( Rank >= RANK_04)
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_1");
	else
		return GAME_RCSTRING("STBL_IDX_CLAN_RANK_UNLIMIT");
}

i3::rc_wstring TextSet::ClanJoinLimitToHighAge(UINT8 Age, bool bUpper)
{
	if(Age <= 0)
		return GAME_RCSTRING("STBL_IDX_CLAN_AGE_UNLIMIT");

	UINT32 nConvertAgeToIdx = 0;

	if(Age > 20)
		nConvertAgeToIdx = 3;	// 30세
	else if (Age > 15)
		nConvertAgeToIdx = 2;	// 20세
	else 
		nConvertAgeToIdx = 1;	// 15세

	i3::string strTemp;

	if(bUpper)
		i3::sprintf(strTemp, "STBL_IDX_CLAN_AGE_UPPER_%d", nConvertAgeToIdx);
	else
		i3::sprintf(strTemp, "STBL_IDX_CLAN_AGE_LOWER_%d", nConvertAgeToIdx);

	return GAME_RCSTRING(strTemp);
}

void TextSet::ClanJoinLimitText(i3::wstring& strOut, CLAN_DETAIL_INFO * pInfo)
{
	// 초기화 부분의 예외처리로 인하여 추가
	if(pInfo == nullptr)
	{
		i3::sprintf(strOut, L"%s %s\n%s %s", 
			GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_RANK"), TextSet::ClanJoinLimitToRank(0), 
			GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_AGE"), TextSet::ClanJoinLimitToHighAge(0));
		return;
	}

//	const INT32 TEMP_STRING_SIZE = 64;

	i3::rc_wstring wstrRankLock = TextSet::ClanJoinLimitToRank(pInfo->m_ranklock);		// 계급 제한 설정

	// 나이 제한 설정
	//char szAgeLock[64] = {0};
	i3::rc_wstring	wstrAgeLock;
	{
		// 최소 나이 제한 설정
		i3::rc_wstring wstrLAgeLock = TextSet::ClanJoinLimitToHighAge(pInfo->m_lagelock, true);
		// 최대 나이 제한 설정
		i3::rc_wstring wstrHAgeLock = TextSet::ClanJoinLimitToHighAge(pInfo->m_hagelock);

		if(pInfo->m_lagelock > 0 && pInfo->m_hagelock > 0)
		{
			// X살 이상 ~ X살 이하
			i3::sprintf(wstrAgeLock, L"%s ~ %s", wstrLAgeLock, wstrHAgeLock);
		}
		else if (pInfo->m_lagelock > 0)
		{
			// X살 이상
			wstrAgeLock = wstrLAgeLock;
		}
		else if (pInfo->m_hagelock > 0)
		{
			// X살 이하
			wstrAgeLock = wstrHAgeLock;
		}
		else
		{
			// 제한 없음
			wstrAgeLock = GAME_RCSTRING("STBL_IDX_CLAN_AGE_UNLIMIT");
		}
	}

	i3::sprintf(strOut, L"%s %s\n%s %s", 
		GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_RANK"), wstrRankLock, GAME_RCSTRING("STR_IDX_CLAN_JOIN_LIMIT_AGE"), wstrAgeLock);

}

void	 TextSet::GetWinObjectText(i3::wstring& strOut, CSI_STAGE* stage, UINT8 ui8GameSetPrim, UINT8 ui8GameSetSec)
{
	switch(	stage->GetStageMode())
	{
	case STAGE_MODE_CROSSCOUNT		:
		{
			strOut = TextSet::Mode(STAGE_MODE_CROSSCOUNT);	// CrossCount
		}
		break;

	case STAGE_MODE_ESCAPE			:
		{
			strOut = TextSet::Mode(STAGE_MODE_ESCAPE);	// Escape
		}
		break;
	default							:					
		{
			if( CStageInfoUtil::IsModeRoundType(*stage))
				strOut = TextSet::Round(ui8GameSetPrim);		// Round Type
			else
				strOut = TextSet::KillCount(ui8GameSetPrim); // No Round Type	
		}
		break;		
	}

	strOut += L" /";
	// Time Count
	i3::rc_wstring	wstrTime = TextSet::GameTime( ui8GameSetSec);
	strOut.append(wstrTime.begin(), wstrTime.end());

}

void TextSet::StarPlayerEntranceMessage(i3::rc_wstring & wstr_out, const UINT8 rank, const i3::rc_wstring wstr_nick)
{
	switch (rank)
	{
	case RANK_56: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_ASSAULT"),	wstr_nick);	break;
	case RANK_57: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_SMG"),		wstr_nick);	break;
	case RANK_58: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_MG"),		wstr_nick);	break;
	case RANK_59: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_SHOTGUN"),	wstr_nick);	break;
	case RANK_60: i3::sprintf(wstr_out, GAME_RCSTRING("STBL_IDX_STARPLAYER_SNIPER"),	wstr_nick);	break;
	}
}
