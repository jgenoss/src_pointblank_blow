#include "pch.h"
#include "AIDriverCharaUpper_ShotGunMaleFemale.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverCharaUpper_ShotGunMaleFemale);

////////////////////////////////////////////////////////////////////////////////
//
// Move
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/Move_AttackIdle.i3a
//
static void AIPROC_Move_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Move_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/AttackIdle.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/UTS-15/Attack.i3a
//
static void AIPROC_Attack_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_Attack_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/UTS-15/ReloadD.i3a
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagazine
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/UTS-15/ReloadB.i3a
//
static void AIPROC_LoadMagazine_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagazine_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/UTS-15/Change.i3a
//
static void AIPROC_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_SWAP_WEAPON_RUN( pCtx, tm);
}

static void AIPROC_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageBackB
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/Damage_AttackIdle.i3a
//
static void AIPROC_DamageBackB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageBackB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontLeftB
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/Damage_AttackIdle.i3a
//
static void AIPROC_DamageFrontLeftB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontLeftB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontRightB
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/Damage_AttackIdle.i3a
//
static void AIPROC_DamageFrontRightB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontRightB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpEnd
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/JumpEnd_AttackIdle.i3a
//
static void AIPROC_JumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_JUMP_END_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpStart
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/JumpStart_AttackIdle.i3a
//
static void AIPROC_JumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageBackA
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/Damage_AttackIdle.i3a
//
static void AIPROC_DamageBackA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageBackA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontLeftA
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/Damage_AttackIdle.i3a
//
static void AIPROC_DamageFrontLeftA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontLeftA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontRightA
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/Common/Damage_AttackIdle.i3a
//
static void AIPROC_DamageFrontRightA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontRightA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA
//
static void AIPROC_UI_IdleA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEA_RUN( pCtx, tm);
}

static void AIPROC_UI_IdleA_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEA_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB
//
static void AIPROC_UI_IdleB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEB_RUN( pCtx,  tm);
}

static void AIPROC_UI_IdleB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEB_FIN( pCtx,  tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagazine2
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/UTS-15/ReloadC.i3a
//
static void AIPROC_LoadMagazine2_Run( i3AIContext * pCtx, REAL32 tm)
{
//	CGameCharaBase *pChara = (CGameCharaBase *) pCtx->getOwner();
//	pChara->SetLoadMagChg(false);
	AIPROC_CHARA_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagazine2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagazineReady
// ANIMATION : Chara/SWAT_Male/1PV/ShotGun8/UTS-15/ReloadA.i3a
//
static void AIPROC_LoadMagazineReady_Run( i3AIContext * pCtx, REAL32 tm)
{
//	CGameCharaBase *pChara = (CGameCharaBase *) pCtx->getOwner();
//	pChara->SetLoadMagChg(true);
	AIPROC_CHARA_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagazineReady_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_Change
//
static void AIPROC_UI_Idle_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_CHANGE_RUN(pCtx, tm);
}

static void AIPROC_UI_Idle_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_CHANGE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_Stand
//
static void AIPROC_UI_Idle_Stand_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_RUN(pCtx, tm);
}

static void AIPROC_UI_Idle_Stand_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_FIN(pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_CharaUpper_ShotGunMaleFemaleTable[] =
{
	{	"Attack",	AIPROC_Attack_Run,	AIPROC_Attack_Fin	},
	{	"Change",	AIPROC_Change_Run,	AIPROC_Change_Fin	},
	{	"DamageBackA",	AIPROC_DamageBackA_Run,	AIPROC_DamageBackA_Fin	},
	{	"DamageBackB",	AIPROC_DamageBackB_Run,	AIPROC_DamageBackB_Fin	},
	{	"DamageFrontLeftA",	AIPROC_DamageFrontLeftA_Run,	AIPROC_DamageFrontLeftA_Fin	},
	{	"DamageFrontLeftB",	AIPROC_DamageFrontLeftB_Run,	AIPROC_DamageFrontLeftB_Fin	},
	{	"DamageFrontRightA",	AIPROC_DamageFrontRightA_Run,	AIPROC_DamageFrontRightA_Fin	},
	{	"DamageFrontRightB",	AIPROC_DamageFrontRightB_Run,	AIPROC_DamageFrontRightB_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMagazine",	AIPROC_LoadMagazine_Run,	AIPROC_LoadMagazine_Fin	},
	{	"LoadMagazine2",	AIPROC_LoadMagazine2_Run,	AIPROC_LoadMagazine2_Fin	},
	{	"LoadMagazineReady",	AIPROC_LoadMagazineReady_Run,	AIPROC_LoadMagazineReady_Fin	},
	{	"Move",	AIPROC_Move_Run,	AIPROC_Move_Fin	},
	{	"UI_Idle_Change",	AIPROC_UI_Idle_Change_Run,	AIPROC_UI_Idle_Change_Fin	},
	{	"UI_Idle_Stand",	AIPROC_UI_Idle_Stand_Run,	AIPROC_UI_Idle_Stand_Fin	},
	{	"UI_IdleA",	AIPROC_UI_IdleA_Run,	AIPROC_UI_IdleA_Fin	},
	{	"UI_IdleB",	AIPROC_UI_IdleB_Run,	AIPROC_UI_IdleB_Fin	},
};

AIDriverCharaUpper_ShotGunMaleFemale::AIDriverCharaUpper_ShotGunMaleFemale(void)
{
	setAIProcTable( s_CharaUpper_ShotGunMaleFemaleTable, (sizeof(s_CharaUpper_ShotGunMaleFemaleTable)/sizeof(s_CharaUpper_ShotGunMaleFemaleTable[0])) );
}

AIDriverCharaUpper_ShotGunMaleFemale::~AIDriverCharaUpper_ShotGunMaleFemale(void)
{
}
