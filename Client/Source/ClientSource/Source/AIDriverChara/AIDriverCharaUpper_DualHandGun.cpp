#include "pch.h"
#include "AIDriverCharaUpper_DualHandGun.h"
#include "GlobalVariables.h"
#include "GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverCharaUpper_DualHandGun);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// RightMove
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Move_AttackIdle_Right.i3a
//
static void AIPROC_RightMove_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Right_IdleRun( pCtx, tm);
}

static void AIPROC_RightMove_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// RightIdle
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/AttackIdle_Right.i3a
//
static void AIPROC_RightIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Right_IdleRun( pCtx, tm);
}

static void AIPROC_RightIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// RightAttack
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/DualHandGun/Attack_Right.i3a
//
static void AIPROC_RightAttack_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Right_AttackRun( pCtx, tm);
}

static void AIPROC_RightAttack_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/DualHandGun/Reload.i3a
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagazine
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/DualHandGun/Reload.i3a
//
static void AIPROC_LoadMagazine_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_LoadMagazineRun( pCtx, tm);
}

static void AIPROC_LoadMagazine_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageBackB
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Damage_AttackIdle.i3a
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
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Damage_AttackIdle.i3a
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
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Damage_AttackIdle.i3a
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
// RightJumpEnd
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/JumpEnd_AttackIdle_Right.i3a
//
static void AIPROC_RightJumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_RightJumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Right_JumpEndFinish( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// RightJumpStart
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/JumpStart_AttackIdle_Right.i3a
//
static void AIPROC_RightJumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_RightJumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageBackA
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Damage_AttackIdle.i3a
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
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Damage_AttackIdle.i3a
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
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Damage_AttackIdle.i3a
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
// ExtensionDeactivate
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/AttackIdle_Right.i3a
//
static void AIPROC_ExtensionDeactivate_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/AttackIdle_Left.i3a
//
static void AIPROC_ExtensionActivate_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_ExtensionActivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA
//
static void AIPROC_UI_IdleA_Run( i3AIContext * pCtx, REAL32 tm)
{
	// revision 28683
	// [HK45 Dual] ³×Æ®¿öÅ© ¾Ö´Ï¸ÞÀÌ¼Ç ½ÌÅ© ¼öÁ¤, ºü¸¥ ¾Ö´Ï¸ÞÀÌ¼Ç, µà¾ó¹«±â Á¡ÇÁÂøÁö ¾Ö´Ï¸ÞÀÌ¼Ç ¼öÁ¤
	AIPROC_CHARA_UI_IDLEA_RUN( pCtx,  tm);
}

static void AIPROC_UI_IdleA_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEA_FIN( pCtx,  tm);
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
	AIPROC_CHARA_UI_IDLEB_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LeftMove
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/Move_AttackIdle_Left.i3a
//
static void AIPROC_LeftMove_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Left_IdleRun( pCtx, tm);
}

static void AIPROC_LeftMove_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LeftIdle
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/AttackIdle_Left.i3a
//
static void AIPROC_LeftIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Left_IdleRun( pCtx, tm);
}

static void AIPROC_LeftIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LeftAttack
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/DualHandGun/Attack_Left.i3a
//
static void AIPROC_LeftAttack_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Left_AttackRun( pCtx, tm);
}

static void AIPROC_LeftAttack_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LeftJumpEnd
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/JumpEnd_AttackIdle_Left.i3a
//
static void AIPROC_LeftJumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LeftJumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_Left_JumpEndFinish( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LeftJumpStart
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/Common/JumpStart_AttackIdle_Left.i3a
//
static void AIPROC_LeftJumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LeftJumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change
// ANIMATION : Chara/SWAT_Female/1ÀÎÄª/DualHandGun/DualHandGun/Change.i3a
//
static void AIPROC_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase * pChara = (CGameCharaBase*) pCtx->getOwner();
	I3ASSERT( pChara != nullptr);
	pChara->getAnimContext()->CallbackAnim_Dual_SwapWeaponRun( pCtx, tm);
}

static void AIPROC_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_Change
// ANIMATION : Chara/SWAT_Male/3PV/SMG7/UI/Idle_Change.i3a
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
// ANIMATION : Chara/SWAT_Male/3PV/SMG7/UI/Idle_Stand.i3a
//
static void AIPROC_UI_Idle_Stand_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_RUN(pCtx, tm);
}

static void AIPROC_UI_Idle_Stand_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_FIN(pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_CharaUpper_DualHandGunTable[] =
{
	{	"Change",	AIPROC_Change_Run,	AIPROC_Change_Fin	},
	{	"DamageBackA",	AIPROC_DamageBackA_Run,	AIPROC_DamageBackA_Fin	},
	{	"DamageBackB",	AIPROC_DamageBackB_Run,	AIPROC_DamageBackB_Fin	},
	{	"DamageFrontLeftA",	AIPROC_DamageFrontLeftA_Run,	AIPROC_DamageFrontLeftA_Fin	},
	{	"DamageFrontLeftB",	AIPROC_DamageFrontLeftB_Run,	AIPROC_DamageFrontLeftB_Fin	},
	{	"DamageFrontRightA",	AIPROC_DamageFrontRightA_Run,	AIPROC_DamageFrontRightA_Fin	},
	{	"DamageFrontRightB",	AIPROC_DamageFrontRightB_Run,	AIPROC_DamageFrontRightB_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"LeftAttack",	AIPROC_LeftAttack_Run,	AIPROC_LeftAttack_Fin	},
	{	"LeftIdle",	AIPROC_LeftIdle_Run,	AIPROC_LeftIdle_Fin	},
	{	"LeftJumpEnd",	AIPROC_LeftJumpEnd_Run,	AIPROC_LeftJumpEnd_Fin	},
	{	"LeftJumpStart",	AIPROC_LeftJumpStart_Run,	AIPROC_LeftJumpStart_Fin	},
	{	"LeftMove",	AIPROC_LeftMove_Run,	AIPROC_LeftMove_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMagazine",	AIPROC_LoadMagazine_Run,	AIPROC_LoadMagazine_Fin	},
	{	"RightAttack",	AIPROC_RightAttack_Run,	AIPROC_RightAttack_Fin	},
	{	"RightIdle",	AIPROC_RightIdle_Run,	AIPROC_RightIdle_Fin	},
	{	"RightJumpEnd",	AIPROC_RightJumpEnd_Run,	AIPROC_RightJumpEnd_Fin	},
	{	"RightJumpStart",	AIPROC_RightJumpStart_Run,	AIPROC_RightJumpStart_Fin	},
	{	"RightMove",	AIPROC_RightMove_Run,	AIPROC_RightMove_Fin	},
	{	"UI_Idle_Change",	AIPROC_UI_Idle_Change_Run,	AIPROC_UI_Idle_Change_Fin	},
	{	"UI_Idle_Stand",	AIPROC_UI_Idle_Stand_Run,	AIPROC_UI_Idle_Stand_Fin	},
	{	"UI_IdleA",	AIPROC_UI_IdleA_Run,	AIPROC_UI_IdleA_Fin	},
	{	"UI_IdleB",	AIPROC_UI_IdleB_Run,	AIPROC_UI_IdleB_Fin	},
};

AIDriverCharaUpper_DualHandGun::AIDriverCharaUpper_DualHandGun(void)
{
	setAIProcTable( s_CharaUpper_DualHandGunTable, (sizeof(s_CharaUpper_DualHandGunTable)/sizeof(s_CharaUpper_DualHandGunTable[0])));
}

AIDriverCharaUpper_DualHandGun::~AIDriverCharaUpper_DualHandGun(void)
{
}
