#if !defined( __QPI_H)
#define __QPI_H

extern "C"
{
	// 현재 Quest의 Argument Value를 얻어온다.
	// idx : 0~5
	INT32				QPI_GetValue( INT32 idx);

	// 현재 Quest의 Count Value를 설정한다.
	// idx : 0~5
	void				QPI_SetValue( INT32 idx, UINT8 value);

	// 현재 유저가 들고 있는 무기의 카테고리를 반환한다.
	WEAPON_CLASS_TYPE	QPI_GetWeaponClassType(void);

	// 현재 사용자가 들고 있는 무기의 종류를 반환한다.
	// WEAPON_CLASS_TYPE에 따라 아래와 같은 다양한 카테고리의 결과값이 반환된다.
	//
	// GetItemIndex("ASSAULT")
	// GetItemIndex("SMG")
	// GetItemIndex("SNIPER")
	// GetItemIndex("HANDGUN")
	// GetItemIndex("SHOTGUN")
	// GetItemIndex("KNIFE")
	// GetItemIndex("THROWING")
	// GetItemIndex("MISSION")
	// GetItemIndex("MG")
	INT32				QPI_GetWeaponType(void);

	//현재 사용자가 들고있는 무기의 시조 무기종류를 반환한다 ( GetItemIndex("ASSAULT_M4A1_SCOPE2")나 GetItemIndex("ASSAULT_M4A1_CAMO") 의 경우는 GetItemIndex("ASSAULT_M4A1")을 반환)
	INT32				QPI_GetWeaponTypeEx(void);

	INT32				QPI_GetWeapon_DerivedFrom( WEAPON_CLASS_TYPE type, INT32 num);

	// 주인공이 다른 캐릭터를 맞힌 경우, 총알을 맞은 상대 캐릭터의 부위를 반환한다.
	HIT_CHARA_PART		QPI_GetHitPart(void);

	// 다른 캐릭터에 의해 주인공이 공격을 당했을 경우, 주인공의 피격 부위를 반환한다.
	HIT_CHARA_PART		QPI_GetHittedPart(void);

	// 현재 Stage에서의 총 Kill Count를 반환한다.
	INT32				QPI_GetKillCount(void);

	// 최후의 Respawn 이후부터의 Kill Count를 반환한다.
	INT32				QPI_GetKillCountInALife(void);

	// 현재까지의 누계 Kill Count를 반환한다.
	INT32				QPI_GetKillCountTotal(void);

	// 현재 Stage에서의 Death Count를 반환한다.
	INT32				QPI_GetDeathCount(void);

	// 현재까지의 누계 Death Count를 반환한다.
	INT32				QPI_GetDeathCountTotal(void);

	// 현재 Stage의 Type
	STAGE_TYPE			QPI_GetStageType(void);

	// 현재 Stage의 ID
	STAGE_ID			QPI_GetStage(void);

	// 난입 여부를 반환한다.
	BOOL				QPI_IsIntercept(void);

	// Round 제의 Stage인 경우, 승리 Round 회수를 반환한다.
	INT32				QPI_GetWinRoundCount(void);

	// Round 제의 Stage인 경우, 패배 Round 회수를 반환한다.
	INT32				QPI_GetDefeatRoundCount(void);

	// Round 제의 Stage인 경우, 전체 Round 회수를 반환한다.
	INT32				QPI_GetTotalRoundCount(void);

	// 만약 Round 제의 Stage가 아닌 경우, 전체 시간을 초 단위로 반환한다. 그렇지 않다면, 각 Round의 시간 제한을 초 단위로 반환한다.
	INT32				QPI_GetLimitTime(void);

	// 만약 Kill Count가 승리 조건에 있다면, 해당 조건을 반환한다. 그렇지 않다면 0을 반환한다.
	INT32				QPI_GetLimitKillCount(void);

	// 사용자의 계급을 반환한다. (0~51)
	INT32				QPI_GetPlayerRank(void);

	// 사용자의 스테이지 종료시 소속팀내 최종 랭킹을 반환한다.
	INT32				QPI_GetPlayerStageRank(void);

}

#endif
