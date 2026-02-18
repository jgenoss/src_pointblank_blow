#include "pch.h"
#include "QPI.h"
#include "GlobalVariables.h"
#include "WeaponBase.h"
#include "StageBattle.h"
#include "GameCharaWeaponContext.h"

#include "./StageBattle/ScoreContext.h"

extern "C"
{
	/*
	// 6 --> SG550
	// 7 --> AK47
	// 8 --> M4A1
	static INT32 s_WeaponDerivation_Assault[GetItemIndex("ASSAULT_MAX")] = 
	{//총기가 사라지면서 숫자로 예외처리 들어감
		GetItemIndex("ASSAULT_UNKNOWN"),			//GetItemIndex("ASSAULT_UNKNOWN")			= 0,
	
		6,									//GetItemIndex("ASSAULT_SG550_DOTSIGHT")		= 1,	//	SG550_DOTSIGHT		( Extension)
		7,									//GetItemIndex("ASSAULT_AK47_DUALMAG")		= 2,	//	AK47_DUALMAGAZINE	( Extension)
		GetItemIndex("ASSAULT_M4A1_SCOPE2"),		//GetItemIndex("ASSAULT_M4A1_SCOPE2")		= 3,	//	M4A1_SCOPE2			( Extension)
		GetItemIndex("ASSAULT_K2"),				//GetItemIndex("ASSAULT_K2")					= 4,	//	K2
		GetItemIndex("ASSAULT_F2000_SCOPE2"),	//GetItemIndex("ASSAULT_F2000_SCOPE2")		= 5,	//	F2000_SCOPE2		( Extension)

		6,									//GetItemIndex("ASSAULT_SG550")				= 6,	//	SG550
		7,									//GetItemIndex("ASSAULT_AK47")				= 7,	//	AK47
		8,									//GetItemIndex("ASSAULT_M4A1")				= 8,	//	M4A1

		7,									//GetItemIndex("ASSAULT_AK47_GOLD")			= 9,
		8,									//GetItemIndex("ASSAULT_M4A1_CAMO")			= 10,	// M4A1_Camo
		GetItemIndex("ASSAULT_K201_GL"),			//GetItemIndex("ASSAULT_K201_GL")			= 11,	// K201_GrenadeLauncher
		GetItemIndex("ASSAULT_K201_GL"),			//GetItemIndex("ASSAULT_K201_EVENT")			= 12,	// K201_Event

		GetItemIndex("ASSAULT_G36C_SCOPE2"),		// GetItemIndex("ASSAULT_G36C_SCOPE2")		= 13,
		6,									// GetItemIndex("ASSAULT_SG550_SE")			= 14,
		7,									// GetItemIndex("ASSAULT_AK_SOPMOD")			= 15,
		7,									// GetItemIndex("ASSAULT_AK_SOPMOD")			= 16,	// Event

		//////////////////////////////////////////////////////////////////////////// 2008. 12. 19
		7,									// GetItemIndex("ASSAULT_AK47_SILVER")		= 17
		6,									// GetItemIndex("ASSAULT_SG550_MB")			= 18
		6,									// GetItemIndex("ASSAULT_SG550_SILVER")		= 19
		6,									// GetItemIndex("ASSAULT_SG550_WHITE")		= 20
		8,									// GetItemIndex("ASSAULT_M4A1_SILVER")		= 21
		8,									// GetItemIndex("ASSAULT_M4A1_WHITE")		= 22
		8,									// GetItemIndex("ASSAULT_M4A1_GOLD")			= 23

		8,									// GetItemIndex("ASSAULT_M4A1_G_EXP_PT_30")	= 24
		7,									// GetItemIndex("ASSAULT_AK47_S_POINT15")	= 25,
		6,									// GetItemIndex("ASSAULT_SG550_MB_EXP_PT_10") = 26,
	};

	static INT32 s_WeaponDerivation_SMG[GetItemIndex("SMG_MAX")] = 
	{
		GetItemIndex("SMG_UNKNOWN"),				//GetItemIndex("SMG_UNKNOWN")			= 0,
		
		GetItemIndex("SMG_MP5K_SILENCER"),		//GetItemIndex("SMG_MP5K_SILENCER")		= 1,	//	MP5K_Silencer		( Extension)
		GetItemIndex("SMG_SPECTRE_STEADYAIM"),	//GetItemIndex("SMG_SPECTRE_STEADYAIM")	= 2,	//	Spectre_SteadyAim	( Extension)
		GetItemIndex("SMG_K1"),					//GetItemIndex("SMG_K1_ZOOM")			= 3,	//	K1_Zoom				( Extension)
		GetItemIndex("SMG_MP7_DOTSIGHT"),		//GetItemIndex("SMG_MP7_DOTSIGHT")		= 4,	//	MP7_DOTSIGHT		( Extension)
		GetItemIndex("SMG_MP5K_SILENCER"),		//GetItemIndex("SMG_MP5K")				= 5,	//	MP5K
		GetItemIndex("SMG_K1"),					//GetItemIndex("SMG_K1")					= 6,	//	K1
		GetItemIndex("SMG_MP5K_SILENCER"),		//GetItemIndex("SMG_MP5K_SERIF")			= 7,
		GetItemIndex("SMG_UMP45_DUALMAG"),		//GetItemIndex("SMG_UMP45_DUALMAG")		= 8,
		GetItemIndex("SMG_SPECTRE_STEADYAIM"),	//GetItemIndex("SMG_SPECTRE_SE")			= 9,
		GetItemIndex("SMG_P90MC_DOTSIGHT"),		// GetItemIndex("SMG_P90MC_DOTSIGHT")	= 10,
		GetItemIndex("SMG_P90_DOTSIGHT"),		// GetItemIndex("SMG_P90_DOTSIGHT")		= 11,
		GetItemIndex("SMG_KRISSSUPERV"),			// GetItemIndex("SMG_KRISSSUPERV_DUMMY")	= 12,
		GetItemIndex("SMG_KRISSSUPERV"),			// GetItemIndex("SMG_KRISSSUPERV")		= 13,

		/////////////////////////////////////////////////////////////////////////////////// 2008. 12. 19
		GetItemIndex("SMG_MP5K_SILENCER"),		// GetItemIndex("SMG_MP5K_SILVER")		= 14
		GetItemIndex("SMG_MP5K_SILENCER"),		// GetItemIndex("SMG_MP5K_WHITE")		= 15
		GetItemIndex("SMG_SPECTRE_STEADYAIM"),	// GetItemIndex("SMG_SPECTRE_WHITE")		= 16

		GetItemIndex("SMG_UNKNOWN"),				// 		= 17
		GetItemIndex("SMG_UNKNOWN"),				// 		= 18
		GetItemIndex("SMG_KRISSSUPERV"),			// GetItemIndex("SMG_SPECTRE_WHITE")		= 19
		GetItemIndex("SMG_SPECTRE_SILVER"),		//= 20,

		GetItemIndex("SMG_K1_SILVER"),			//= 21
		GetItemIndex("SMG_MP7_SILVER"),			//= 22
		GetItemIndex("SMG_UMP45_SILVER"),		//= 23

		GetItemIndex("SMG_MP5K_SERIF_D"),		//= 24	// MP5K Silver D
		GetItemIndex("SMG_SPECTRE_SE_D"),		//= 25
		GetItemIndex("SMG_KRISSSUPERV_GOLD"),	// GetItemIndex("SMG_KRISSSUPERV_GOLD")= 26
		GetItemIndex("SMG_P90MC_DOTSIGHT_CAMO"),	//=27
		GetItemIndex("SMG_KRISSSUPERV_GOLD"),	// GetItemIndex("SMG_KRISSSUPERV_GOLD_DUMMY")
		GetItemIndex("SMG_P90_DOTSIGHT_D"),		//= 29,
	};

	static INT32 s_WeaponDerivation_Sniper[ GetItemIndex("SNIPER_MAX")] = 
	{
		GetItemIndex("SNIPER_UNKNOWN"),			//GetItemIndex("SNIPER_UNKNOWN")			= 0,
		
		GetItemIndex("SNIPER_DRAGUNOV"),			//GetItemIndex("SNIPER_DRAGUNOV")		= 1,	//	SVD Dragunov
		GetItemIndex("SNIPER_PSG1"),				//GetItemIndex("SNIPER_PSG1")			= 2,	//	HK-PSG1
		GetItemIndex("SNIPER_SSG69"),			//GetItemIndex("SNIPER_SSG69")			= 3,	//	SSG69
		
		GetItemIndex("SNIPER_SSG69"),			//GetItemIndex("SNIPER_SSG69_CAMO")		= 4,	//	SSG69_CAMO
		GetItemIndex("SNIPER_L115A1"),			//GetItemIndex("SNIPER_L1115A1")			= 5
		GetItemIndex("SNIPER_DRAGUNOV"),			//GetItemIndex("SNIPER_DRAGUNOV_GOLD")	= 6
		GetItemIndex("SNIPER_PSG1"),				//GetItemIndex("SNIPER_PSG1_S")			= 7

		///////////////////////////////////////////////////////////////////////////// 2008. 12. 19
		GetItemIndex("SNIPER_SSG69"),			//GetItemIndex("SNIPER_SSG69_SILVER")	= 8,
		GetItemIndex("SNIPER_PSG1"),				//GetItemIndex("SNIPER_PSG1_SILVER")		= 9
		GetItemIndex("SNIPER_DRAGUNOV"),			//GetItemIndex("SNIPER_DRAGUNOV_SILVER") = 10

	};

	static INT32 s_WeaponDerivation_HandGun[ GetItemIndex("HANDGUN_MAX")] =
	{
		GetItemIndex("HANDGUN_UNKNOWN"),			//GetItemIndex("HANDGUN_UNKNOWN")			= 0,
		
		GetItemIndex("HANDGUN_DESERTEAGLE"),		//GetItemIndex("HANDGUN_DESERTEAGLE")		= 1,	//	Desert Eagle
		GetItemIndex("HANDGUN_MK23_SILENCER"),	//GetItemIndex("HANDGUN_MK23_SILENCER")		= 2,	//	MK23_Silencer		( Extension)
		GetItemIndex("HANDGUN_K5"),				//GetItemIndex("HANDGUN_K5")					= 3,	//	K5
		GetItemIndex("HANDGUN_MK23_SILENCER"),	//GetItemIndex("HANDGUN_MK23")				= 4,	//	MK23			// not used
		
		GetItemIndex("HANDGUN_DESERTEAGLE"),		//GetItemIndex("HANDGUN_DESERTEAGLE_REF")	= 5,	// DesertEagle S

		GetItemIndex("HANDGUN_P99"),				// GetItemIndex("HANDGUN_P99")				= 6			// DUMMY
		GetItemIndex("HANDGUN_COLT_PYTHON"),		// GetItemIndex("HANDGUN_COLT_PYTHON")		= 7			// DUMMY
		GetItemIndex("HANDGUN_DESERTEAGLE"),		// GetItemIndex("HANDGUN_P99")				= 8			// DUMMY
	};

	static INT32 s_WeaponDerivation_ShotGun[ GetItemIndex("SHOTGUN_MAX")] = 
	{
		GetItemIndex("SHOTGUN_UNKNOWN"),			//GetItemIndex("SHOTGUN_UNKNOWN")		= 0,
		
		GetItemIndex("SHOTGUN_870MCS"),			//GetItemIndex("SHOTGUN_870MCS")			= 1,	//	870MCS Remington
		GetItemIndex("SHOTGUN_870MCS"),			//GetItemIndex("SHOTGUN_870MCS_EVENT")	= 2,	//	870MCS Remington SE		//Event
		GetItemIndex("SHOTGUN_SPAS15"),			//GetItemIndex("SHOTGUN_SPAS15")			= 3,
		GetItemIndex("SHOTGUN_870MCS"),			//GetItemIndex("SHOTGUN_870MCS_W")		= 4,
		GetItemIndex("SHOTGUN_M1887"),			//GetItemIndex("SHOTGUN_M1887")			= 5,
	};

	static INT32	s_WeaponDerivation_Knife[ GetItemIndex("KNIFE_MAX")] =
	{
		GetItemIndex("KNIFE_UNKNOWN"),			//GetItemIndex("KNIFE_UNKNOWN")		= 0,
		
		GetItemIndex("KNIFE_M7"),				//GetItemIndex("KNIFE_M7")			= 1,	//	M7 Knife
		GetItemIndex("KNIFE_M9"),				//GetItemIndex("KNIFE_M9")			= 2,	//	M9 Knife

		GetItemIndex("KNIFE_HAK"),				// GetItemIndex("KNIFE_HAK")			= 3,	// DUMMY
		GetItemIndex("KNIFE_KUKRII"),			// GetItemIndex("KNIFE_KUKRII")		= 4,

		GetItemIndex("KNIFE_KUKRII"),			// GetItemIndex("KNIFE_KUKRII_EVENT")= 5,	// Event
		GetItemIndex("KNIFE_MINIAXE"),			// GetItemIndex("KNIFE_MINIAXE_DUMMY")	= 6,	// 실제적인 MiniAxe
		GetItemIndex("KNIFE_M7"),				//GetItemIndex("KNIFE_MINIAXE")		= 7,		// M7입니다.
		GetItemIndex("KNIFE_MINIAXE"),			//GetItemIndex("KNIFE_MINIAXE_EVENT")	= 8,	// 이벤트

		////////////////////////////////////////////////////////////////////////////// 2008. 12. 19추가
		GetItemIndex("KNIFE_M7"),				//GetItemIndex("KNIFE_M7_GOLD")		= 9,
	};

	static INT32 s_WeaponDerivation_Throwing[ GetItemIndex("THROWING_MAX")] = 
	{
		GetItemIndex("THROWING_UNKNOWN"),		//GetItemIndex("THROWING_UNKNOWN")	= 0,
		
		GetItemIndex("THROWING_K400"),			//GetItemIndex("THROWING_K400")		= 1,	//	Grenade
		GetItemIndex("THROWING_SMOKE"),			//GetItemIndex("THROWING_SMOKE")		= 2,	//	Smoke Grenade
		GetItemIndex("THROWING_FLASHBANG"),		//GetItemIndex("THROWING_FLASHBANG")	= 3,	//	Flash Bang
		GetItemIndex("THROWING_C5"),				// GetItemIndex("THROWING_C5")		= 4,
		GetItemIndex("THROWING_WP_SMOKE"),		// GetItemIndex("THROWING_WP_SMOKE") = 5
	};

	static INT32 s_WeaponDerivation_Item[ GetItemIndex("ITEM_MAX")] = 
	{
		GetItemIndex("ITEM_UNKNOWN"),
	};

	static INT32 s_WeaponDerivation_Mission[GetItemIndex("MISSION_MAX")] = 
	{
		GetItemIndex("MISSION_UNKNOWN"),			//GetItemIndex("MISSION_UNKNOWN")		= 0,
		GetItemIndex("MISSION_C4"),				//GetItemIndex("MISSION_C4")			= 1,
	};

	static INT32	s_WeaponDerivation_MG[ GetItemIndex("MG_MAX")] = 
	{
		GetItemIndex("MG_UNKNOWN"),
		GetItemIndex("MG_MK46"),
	};

	static INT32 s_WeaponDerivation_GrenadeShell[GetItemIndex("GRENADESHELL_MAX")] = 
	{
		GetItemIndex("GRENADESHELL_UNKNOWN"),
		GetItemIndex("GRENADESHELL_K201"),
		GetItemIndex("GRENADESHELL_K201_BOSS"),	
		GetItemIndex("GRENADESHELL_RPG7"),			
		GetItemIndex("GRENADESHELL_RPG7_FOR_DINO")	
	};

	static INT32 s_WeaponDerivation_BombTrigger[GetItemIndex("BOMBTRIGGER_MAX")] = 
	{
		GetItemIndex("BOMBTRIGGER_UNKNOWN"),
		GetItemIndex("BOMBTRIGGER_DEFAULT"),
	};

	static INT32 s_WeaponDerivation_CIC[GetItemIndex("CIC_MAX")] = 
	{
		GetItemIndex("CIC_UNKNOWN"),
		GetItemIndex("CIC_P99"),
	};

	static INT32 s_WeaponDerivation_DualHandGun[GetItemIndex("DUALHANDGUN_MAX")] = 
	{
		GetItemIndex("DUALHANDGUN_UNKNOWN"),
		GetItemIndex("DUALHANDGUN_MK23_K5"),
		GetItemIndex("DUALHANDGUN_DESERTEAGLE"),
		GetItemIndex("DUALHANDGUN_UNKNOWN"),
		GetItemIndex("DUALHANDGUN_DESERTEAGLE_GOLD"),
	};

	static INT32 s_WeaponDerivation_DualKnife[GetItemIndex("DUALKNIFE_MAX")] = 
	{
		GetItemIndex("DUALKNIFE_UNKNOWN"),
		GetItemIndex("DUALKNIFE_M9"),
	};

	static INT32 s_WeaponDerivation_RocketLauncher[GetItemIndex("ROCKET_LAUNCHER_MAX")] = 
	{
		GetItemIndex("ROCKET_LAUNCHER_UNKNOWN"),
		GetItemIndex("ROCKET_LAUNCHER_RPG7_OUTPOST"),
		GetItemIndex("ROCKET_LAUNCHER_RPG7"),
		GetItemIndex("ROCKET_LAUNCHER_RPG7_BOSS")
	};

	static INT32 s_WeaponDerivation_ObjectGun[GetItemIndex("ON_OBJECT_MAX")] = 
	{
		GetItemIndex("ON_OBJECT_UNKNOWN"),
		GetItemIndex("ON_OBJECT_M197"),
	};

	static INT32 s_WeaponDerivation_DualSMG[GetItemIndex("DUALSMG_MAX")] = 
	{
		GetItemIndex("DUALSMG_UNKNOWN"),
		GetItemIndex("DUALSMG_KRISSSUPERV"),
		GetItemIndex("DUALSMG_KRISSSUPERV_GOLD"),
	};

	typedef struct __tagWeaponinfo
	{
		INT32	*	pNumber;	//무기 유래 정보배열 포인터
		INT32		nMaxCnt;	//무기 유래 정보 최대값,
	}WPN_DERIVATION_INFO;

	static WPN_DERIVATION_INFO s_WpnDrvInfo[WEAPON_CLASS_COUNT] = 
	{
		{	NULL,	0},
		{	s_WeaponDerivation_Knife,		GetItemIndex("KNIFE_MAX")},
		{	s_WeaponDerivation_HandGun,		GetItemIndex("HANDGUN_MAX")},
		{	s_WeaponDerivation_Assault,		GetItemIndex("ASSAULT_MAX")},
		{	s_WeaponDerivation_SMG,			GetItemIndex("SMG_MAX")},
		{	s_WeaponDerivation_Sniper,		GetItemIndex("SNIPER_MAX")},
		{	s_WeaponDerivation_ShotGun,		GetItemIndex("SHOTGUN_MAX")},
		{	s_WeaponDerivation_Throwing,	GetItemIndex("THROWING_MAX")},
		{	s_WeaponDerivation_Item,		GetItemIndex("ITEM_MAX")},
		{	s_WeaponDerivation_Mission,		GetItemIndex("MISSION_MAX")},
		{	s_WeaponDerivation_MG,			GetItemIndex("MG_MAX")},
		{	s_WeaponDerivation_GrenadeShell,GetItemIndex("GRENADESHELL_MAX")},
		{	s_WeaponDerivation_BombTrigger,	WEAPON_CLASS_BOMBTRIGGER},
		{	s_WeaponDerivation_CIC,			WEAPON_CLASS_CIC},
		{	s_WeaponDerivation_DualHandGun,	WEAPON_CLASS_DUALHANDGUN},
		{	s_WeaponDerivation_DualKnife,	WEAPON_CLASS_DUALKNIFE},
		{	s_WeaponDerivation_RocketLauncher,WEAPON_CLASS_ROCKET_LAUNCHER},
		{	s_WeaponDerivation_ObjectGun,	WEAPON_CLASS_OBJECT_GUN},
		{	s_WeaponDerivation_DualSMG,		WEAPON_CLASS_DUALSMG}
	};
	*/
// 현재 Mission의 Argument Value를 얻어온다.
// idx : 0~5
INT32				QPI_GetValue( INT32 idx)
{
	return 0;
}

// 현재 Mission의 Count Value를 설정한다.
// idx : 0~5
void				QPI_SetValue( INT32 idx, UINT8 value)
{
}

static CGameCharaBase * getPlayer( void)
{
	return g_pCharaManager->getCharaByNetIdx( g_pGameContext->getMySlotIdx());
}

// 현재 유저가 들고 있는 무기의 카테고리를 반환한다.
WEAPON_CLASS_TYPE	QPI_GetWeaponClassType(void)
{
	CGameCharaBase * pPlayer = getPlayer();

	if( pPlayer != NULL && pPlayer->getWeaponContext() != NULL)
	{
		WeaponBase * pWeapon = pPlayer->getWeaponContext()->getCurrent();
		if( pWeapon != NULL)
		{
			if( pWeapon->getWeaponInfo() != NULL)
			{
				return pWeapon->getWeaponInfo()->GetTypeClass();
			}
		}
	}

	return WEAPON_CLASS_UNKNOWN;
}

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
INT32				QPI_GetWeaponType(void)
{
	CGameCharaBase * pPlayer = getPlayer();

	if( pPlayer != NULL)
	{
		WeaponBase * pWeapon = pPlayer->getWeaponContext()->getCurrent();

		return pWeapon->getWeaponInfo()->GetNumber();
	}

	return 0;
}

INT32				QPI_GetWeaponTypeEx(void)
{
	/*WeaponBase * pWeapon = g_pPlayer->getWeaponContext()->getCurrent();

	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();

	I3_BOUNDCHK( pInfo->GetTypeClass(), WEAPON_CLASS_COUNT);

	WPN_DERIVATION_INFO * pDInfo = &s_WpnDrvInfo[pInfo->GetTypeClass()];

	I3_BOUNDCHK( pInfo->GetNumber(), pDInfo->nMaxCnt);

	return pDInfo->pNumber[pInfo->GetNumber()];*/
	return -1;
}

INT32				QPI_GetWeapon_DerivedFrom( WEAPON_CLASS_TYPE type, INT32 num)
{
	/*I3_BOUNDCHK( type, WEAPON_CLASS_COUNT);

	WPN_DERIVATION_INFO * pDInfo = &s_WpnDrvInfo[type];
	I3_BOUNDCHK( num, pDInfo->nMaxCnt);

	return pDInfo->pNumber[num];*/
	return -1;
}

// 주인공이 다른 캐릭터를 맞힌 경우, 총알을 맞은 상대 캐릭터의 부위를 반환한다.
HIT_CHARA_PART		QPI_GetHitPart(void)
{
	CGameCharaBase * pPlayer = getPlayer();
	if( pPlayer != NULL)
	{
		WeaponBase * pWeapon = pPlayer->getWeaponContext()->getCurrent();
		return pWeapon->GetHitPart();
	}

	return HIT_CHARA_PART_NA;
}

// 다른 캐릭터에 의해 주인공이 공격을 당했을 경우, 주인공의 피격 부위를 반환한다.
HIT_CHARA_PART		QPI_GetHittedPart(void)
{
	CGameCharaBase * pPlayer = getPlayer();
	if( pPlayer != NULL)
	{
		CGameCharaBase * pAttacker = pPlayer->getLastAttacker();

		if( pAttacker != NULL)
		{
			WeaponBase * pWeapon = pAttacker->getWeaponContext()->getCurrent();
			return pWeapon->GetHitPart();
		}
	}

	return HIT_CHARA_PART_NA;
}

// 현재 Stage에서의 총 Kill Count를 반환한다.
INT32				QPI_GetKillCount(void)
{
	return ScoreContext::i()->IndividualScore[g_pGameContext->getMySlotIdx()].m_KillCount;
}

// 최후의 Respawn 이후부터의 Kill Count를 반환한다.
INT32				QPI_GetKillCountInALife(void)
{
	CGameCharaBase * pPlayer = getPlayer();
	return pPlayer->getKillCountInALife();
}

// 현재까지의 누계 Kill Count를 반환한다.
INT32				QPI_GetKillCountTotal(void)
{
	const USER_INFO_RECORD* pRecord = g_pGameContext->GetMyUserInfoRecord();
	SCORE_INFO*	pInfo = &ScoreContext::i()->IndividualScore[g_pGameContext->getMySlotIdx()];

	#ifdef USE_MODE_RECORD
	return pRecord->_total_record._killcount + pInfo->m_KillCount;
	#else
	return pRecord->_killcount + pInfo->m_KillCount;
	#endif
}

// 현재 Stage에서의 Death Count를 반환한다.
INT32				QPI_GetDeathCount(void)
{
	return ScoreContext::i()->IndividualScore[g_pGameContext->getMySlotIdx()].m_DeathCount;
}

// 현재까지의 누계 Death Count를 반환한다.
INT32				QPI_GetDeathCountTotal(void)
{
	const USER_INFO_RECORD* pRecord = g_pGameContext->GetMyUserInfoRecord();
	SCORE_INFO*	pInfo = &ScoreContext::i()->IndividualScore[g_pGameContext->getMySlotIdx()];

	#ifdef USE_MODE_RECORD
	return pRecord->_total_record._death + pInfo->m_DeathCount;
	#else
	return pRecord->_death + pInfo->m_DeathCount;
	#endif
}

// 현재 Stage의 Type
STAGE_TYPE			QPI_GetStageType(void)
{
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();
	return getStageType( StageID);
}

// 현재 Stage의 ID
STAGE_ID			QPI_GetStage(void)
{
	return (STAGE_ID) g_pGameContext->getStageID();
}

// 난입 여부를 반환한다.
BOOL				QPI_IsIntercept(void)
{
	return (BOOL)IntrudeContext::i()->IsInterEnter();
}

// Round 제의 Stage인 경우, 승리 Round 회수를 반환한다.
INT32				QPI_GetWinRoundCount(void)
{
	INT32 Rv = 0;
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();
	switch( getStageType( StageID) )
	{
	case STAGE_TYPE_ESCAPE : 
	case STAGE_TYPE_ANNIHILATION : 
	case STAGE_TYPE_BOMB :
	case STAGE_TYPE_DESTROY :
	case STAGE_TYPE_DEFENCE	:
	case STAGE_TYPE_SUDDEN_DEATH	:
	case STAGE_TYPE_HEAD_KILLER	:
		if( g_pGameContext->getMyTeam() & CHARACTER_TEAM_RED)
		 	Rv =  ScoreContext::i()->RedWinRound;
		else 
			Rv =  ScoreContext::i()->BlueWinRound;
		break; 
	default: break; 
	}
	return Rv ;
}

// Round 제의 Stage인 경우, 패배 Round 회수를 반환한다.
INT32				QPI_GetDefeatRoundCount(void)
{
	INT32 totalRound = QPI_GetTotalRoundCount();

	return totalRound - QPI_GetWinRoundCount();
}

// Round 제의 Stage인 경우, 전체 Round 회수를 반환한다.
INT32				QPI_GetTotalRoundCount(void)
{
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();
	switch( getStageType( StageID) )
	{
	case STAGE_TYPE_ESCAPE : 
	case STAGE_TYPE_ANNIHILATION : 
	case STAGE_TYPE_BOMB :
	case STAGE_TYPE_DESTROY :
	case STAGE_TYPE_DEFENCE	:
	case STAGE_TYPE_SUDDEN_DEATH :
	case STAGE_TYPE_HEAD_KILLER :
		{
			INT32 round = LONIBBLE( g_pGameContext->GetMyRoomSubType() );
			round = MINMAX( BATTLE_ROUND_TYPE_1, round, BATTLE_ROUND_TYPE_9);
			return g_MissionRoundCountTable[round];
		}
		break;
	}
	return 0;
}

// 만약 Round 제의 Stage가 아닌 경우, 전체 시간을 초 단위로 반환한다. 그렇지 않다면, 각 Round의 시간 제한을 초 단위로 반환한다.
INT32				QPI_GetLimitTime(void)
{
	INT32 idxTime = HINIBBLE( g_pGameContext->GetMyRoomSubType());
	return g_TeamTimeTable[ idxTime];
}

// 만약 Kill Count가 승리 조건에 있다면, 해당 조건을 반환한다. 그렇지 않다면 0을 반환한다.
INT32				QPI_GetLimitKillCount(void)
{
	INT32 idxKillCount = LONIBBLE( g_pGameContext->GetMyRoomSubType());

	UINT32 StageID = g_pGameContext->GetMyRoomStageID();
	switch( getStageType( StageID) )
	{
	case STAGE_TYPE_ESCAPE : 
	case STAGE_TYPE_ANNIHILATION	: 
	case STAGE_TYPE_BOMB	:
	case STAGE_TYPE_DESTROY :
	case STAGE_TYPE_DEFENCE	:
	case STAGE_TYPE_SUDDEN_DEATH :
	case STAGE_TYPE_HEAD_KILLER :
		return 0;
	}

	return g_TeamKillCountTable[ idxKillCount];
}

// 사용자의 계급을 반환한다. (0~51)
INT32				QPI_GetPlayerRank(void)
{
	return g_pGameContext->GetMyRank();
}

// 사용자의 스테이지 종료시 최종 랭킹을 반환한다.
INT32				QPI_GetPlayerStageRank(void)
{
	INT32 nTeam = 0;
	if( g_pGameContext->getMyTeam() & CHARACTER_TEAM_BLUE)
	{
		nTeam = 1;
	}

	SCORE_INFO*	pScoreInfo = &ScoreContext::i()->IndividualScore[g_pGameContext->getMySlotIdx()];
	
	INT32 nPlayerscore		= pScoreInfo->m_KillCount;
	INT32 nPlayerRank		= 0;

	for(INT32 i=0; i < SLOT_MAX_COUNT; i++)
	{
		if( g_pGameContext->getMySlotIdx() != i && i%2 == nTeam)
		{
			if(g_pGameContext->getBattleEndUserFlag() & (0x01 <<i))
			{
				pScoreInfo = &ScoreContext::i()->IndividualScore[i];
				if( pScoreInfo->m_KillCount > nPlayerscore)
				{
					nPlayerRank++;
				}
			}
		}
	}

	return nPlayerRank;
}

};	//extern "C"