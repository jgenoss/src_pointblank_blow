#include "pch.h"
#include "UIDefine.h"
#include "UIUtil.h"

#include "ClientStageInfoUtil.h"

void SortRoom::InitPriority()
{

	priority[128] = 0;
	memset(priority, 4, sizeof(priority));

	INT32 i = 0;
	for(i = 'A'; i <= 'Z'; ++i) priority[i] = 3;
	for(i = 'a'; i <= 'z'; ++i) priority[i] = 3;
	for(i = '0'; i <= '9'; ++i) priority[i] = 2;
	for(i = '!'; i <= '/'; ++i) priority[i] = 1;
	for(i = ':'; i <= '@'; ++i) priority[i] = 1;
	for(i = '['; i <= '`'; ++i) priority[i] = 1;
	for(i = '{'; i <= '}'; ++i) priority[i] = 1;
}

SortRoom::SORT_LIMIT_TYPE SortRoom::GetSortLimitType(UINT8 weaponFlag)
{
	if		( GET_ROOM_INFO_WEAPON_SHOTGUN ( weaponFlag )	== 1 )	return SLT_SHOTGUN;	// 샷건전
	else if ( GET_ROOM_INFO_WEAPON_SNIPER( weaponFlag )		== 1 )	return SLT_SNIPER;	// 스나전
	else if ( GET_ROOM_INFO_WEAPON_KNUCKLE( weaponFlag)		== 1 )	return SLT_KNUCKLE;	// 너클전
	else if ( GET_ROOM_INFO_WEAPON_00( weaponFlag )			== 0 )						// 주무기 Lock
	{
		if( GET_ROOM_INFO_WEAPON_01( weaponFlag) == 0 )									// 보조무기 Lock
			return SLT_KNIFE;

		return SLT_PISTOL;
	}

	return SLT_NONE;
}

INT32 SortRoom::value_compare(INT32 lValue, INT32 rValue)
{
	if( lValue == rValue) 
		return 0;

	return lValue < rValue ? -1 : 1;
}

INT32 SortRoom::priority_compare(const TTCHAR* lhs, const TTCHAR* rhs)
{
	const INT32 len_s1 = i3::generic_string_size(lhs);
	const INT32 len_s2 = i3::generic_string_size(rhs);

	const INT32 min_len = (len_s1 < len_s2) ? len_s1 : len_s2;

	for(INT32 i = 0; i < min_len; ++i)
	{
		const INT32 str1 = ((lhs[i] > 0)&&(lhs[i] < 128)) ? lhs[i] : 128;
		const INT32 str2 = ((rhs[i] > 0)&&(rhs[i] < 128)) ? rhs[i] : 128;

		if( priority[str1] != priority[str2]) 
			return (priority[str1] > priority[str2]) ? -1 : 1;
	}

	return 0;
}

INT32 SortRoom::SortbyNumber(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs)
{
	return value_compare(lhs->_Idx, rhs->_Idx);
}

INT32 SortRoom::SortbyRoomName(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs)
{
	const TTCHAR * pszStr1 = lhs->_Title;
	const TTCHAR * pszStr2 = rhs->_Title;

	// 이름이 같으면 번호 순서로 정렬
	//const int res_1st = i3::generic_icompare( pszStr1, pszStr2 );
	INT32 res_1st = priority_compare(pszStr1, pszStr2);

	if( res_1st == 0 )
		res_1st = i3::generic_icompare( pszStr1, pszStr2 );

	if( res_1st == 0 )
		return value_compare(lhs->_Idx, rhs->_Idx);

	return res_1st;
}

INT32 SortRoom::SortbyMode(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs)
{
	INT32 i32ModeType[2] = {0};

	i32ModeType[0] = CStageInfoUtil::GetStageData(lhs->_StageID)->m_i32RuleIdx;
	i32ModeType[1] = CStageInfoUtil::GetStageData(rhs->_StageID)->m_i32RuleIdx;

	if( i32ModeType[0] == i32ModeType[1] )
	{
		INT32 i32LimitType[2] = {0};

		i32LimitType[0] = GetSortLimitType(lhs->_WeaponFlag);
		i32LimitType[1] = GetSortLimitType(rhs->_WeaponFlag);

		return value_compare(i32LimitType[0], i32LimitType[1]);
	}		

	return value_compare(i32ModeType[0], i32ModeType[1]);
}

INT32 SortRoom::SortbyUIMode(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs)
{
	INT32 i32ModeType[2] = {0};

	i32ModeType[0] = CStageInfoUtil::GetStageData(lhs->_StageID)->GetStageMode();
	i32ModeType[1] = CStageInfoUtil::GetStageData(rhs->_StageID)->GetStageMode();
	
	if( i32ModeType[0] == i32ModeType[1] )
	{
		INT32 i32LimitType[2] = {0};

		i32LimitType[0] = GetSortLimitType(lhs->_WeaponFlag);
		i32LimitType[1] = GetSortLimitType(rhs->_WeaponFlag);

		return value_compare(i32LimitType[0], i32LimitType[1]);
	}		

	return value_compare(i32ModeType[0], i32ModeType[1]);
}

INT32 SortRoom::SortbyStageName(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs)
{
	i3::wstring wstrStageName[2];

	if( GET_ROOM_INFO_RANDOM_MODE( lhs->_InfoFlag) )
	{
		wstrStageName[0] = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
	}
	else
	{
		wstrStageName[0] = CStageInfoUtil::GetMapData(lhs->_StageID)->getDisplayName();
	}

	if( GET_ROOM_INFO_RANDOM_MODE( rhs->_InfoFlag) )
	{
		wstrStageName[1] = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
	}
	else
	{
		wstrStageName[1] = CStageInfoUtil::GetMapData(rhs->_StageID)->getDisplayName();
	}

	return i3::generic_icompare(wstrStageName[0], wstrStageName[1]);			// 맵이름 순
}

INT32 SortRoom::SortbyRoomState(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs)
{
	INT8 nRoomState[2] = {0};

	if( (lhs->_State != ROOM_STATE_READY && GET_ROOM_INFO_INTERENTER(lhs->_InfoFlag) > 0) ||
		lhs->_PersonNow == lhs->_PersonPlayerMax)
	{
		nRoomState[0] = 1;
	}

	if( (rhs->_State != ROOM_STATE_READY && GET_ROOM_INFO_INTERENTER(rhs->_InfoFlag) > 0) ||
		rhs->_PersonNow == rhs->_PersonPlayerMax)
	{
		nRoomState[1] = 1;
	}

	return value_compare(nRoomState[0], nRoomState[1]);	// 대기중 < 게임중
}

INT32 SortRoom::SortbyPerson(const ROOM_INFO_BASIC * lhs, const ROOM_INFO_BASIC * rhs)
{
	INT32 nPerson[2] = {0};

	nPerson[0] = lhs->_PersonPlayerMax - lhs->_PersonNow;
	nPerson[1] = rhs->_PersonPlayerMax - rhs->_PersonNow;

	if (0 == nPerson[0]) nPerson[0] = SLOT_MAX_COUNT + 1;
	if (0 == nPerson[1]) nPerson[1] = SLOT_MAX_COUNT + 1;

	return value_compare(nPerson[0], nPerson[1]);	 // (MAX - CUR), 단 0은 무조건 큼.
}