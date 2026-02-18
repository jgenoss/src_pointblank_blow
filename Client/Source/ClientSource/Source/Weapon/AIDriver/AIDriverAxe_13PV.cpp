#include "pch.h"
#include "AIDriverAxe_13PV.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverAxe_13PV);

////////////////////////////////////////////////////////////////////////////////
//
// Idle3PV
// ANIMATION : Weapon/Hammer_Halloween/Resource/idle_3pv.i3a
//
static void AIPROC_Idle3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnIdle(pCtx, tm);
}

static void AIPROC_Idle3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire_3PV
// ANIMATION : Weapon/Hammer_Halloween/Resource/AttackB_3pv.i3a
//
static void AIPROC_SecondaryFire_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	if (i3::same_of<WeaponKnife*>(pWeapon) == true)
	{
		pWeapon->OnSecondaryFire(pCtx, tm);
	}
	else
	{
		if (pWeapon->getLoadedBulletCount() > 0)
		{
			pWeapon->OnSecondaryFire(pCtx, tm);
		}
		else
			pWeapon->ReturnToReady();
	}
}

static void AIPROC_SecondaryFire_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag
//
static void AIPROC_LoadMag_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMag_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire3PV
// ANIMATION : Weapon/Hammer_Halloween/Resource/AttackA_3pv.i3a
//
static void AIPROC_Fire3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnFire(pCtx, tm);
}

static void AIPROC_Fire3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Left
//
static void AIPROC_AttackA_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_AttackA_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Right
//
static void AIPROC_AttackA_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_AttackA_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Left
// ANIMATION : Weapon/Hammer_Halloween/Resource/AttackB.i3a
//
static void AIPROC_AttackB_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnFire(pCtx, tm);
}

static void AIPROC_AttackB_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Right
// ANIMATION : Weapon/Hammer_Halloween/Resource/AttackB.i3a
//
static void AIPROC_AttackB_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnFire(pCtx, tm);
}

static void AIPROC_AttackB_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Left
//
static void AIPROC_Change_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Right
//
static void AIPROC_Change_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Left
//
static void AIPROC_UI_RebelMale_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelMale_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Right
//
static void AIPROC_UI_RebelMale_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelMale_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Left
//
static void AIPROC_UI_RebelFemale_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelFemale_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Right
//
static void AIPROC_UI_RebelFemale_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelFemale_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale
// ANIMATION : Weapon/Hammer_Halloween/Resource/UI_Idle.i3a
//
static void AIPROC_UI_RebelMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale
// ANIMATION : Weapon/Hammer_Halloween/Resource/UI_Idle.i3a
//
static void AIPROC_UI_RebelFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV
// ANIMATION : Weapon/Hammer_Halloween/Resource/Idle.i3a
//
static void AIPROC_Change1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change3PV
// ANIMATION : Weapon/Hammer_Halloween/Resource/idle_3pv.i3a
//
static void AIPROC_Change3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale2
// ANIMATION : Weapon/Hammer_Halloween/Resource/UI_Idle.i3a
//
static void AIPROC_UI_RebelMale2_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelMale2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale2
// ANIMATION : Weapon/Hammer_Halloween/Resource/UI_Idle.i3a
//
static void AIPROC_UI_RebelFemale2_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*)pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon(pCtx, tm);
}

static void AIPROC_UI_RebelFemale2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle1PV
// ANIMATION : Weapon/Hammer_Halloween/Resource/Idle.i3a
//
static void AIPROC_Idle1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnIdle(pCtx, tm);
}

static void AIPROC_Idle1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire1PV
// ANIMATION : Weapon/Hammer_Halloween/Resource/AttackA.i3a
//
static void AIPROC_Fire1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnFire(pCtx, tm);
}

static void AIPROC_Fire1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
// ANIMATION : Weapon/Hammer_Halloween/Resource/AttackB.i3a
//
static void AIPROC_SecondaryFire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	if (i3::same_of<WeaponKnife*>(pWeapon) == true)
	{
		pWeapon->OnSecondaryFire(pCtx, tm);
	}
	else
	{
		if (pWeapon->getLoadedBulletCount() > 0)
		{
			pWeapon->OnSecondaryFire(pCtx, tm);
		}
		else
			pWeapon->ReturnToReady();
	}
}

static void AIPROC_SecondaryFire_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_Axe_13PVTable[] =
{
	{	"AttackA_Left",	AIPROC_AttackA_Left_Run,	AIPROC_AttackA_Left_Fin	},
	{	"AttackA_Right",	AIPROC_AttackA_Right_Run,	AIPROC_AttackA_Right_Fin	},
	{	"AttackB_Left",	AIPROC_AttackB_Left_Run,	AIPROC_AttackB_Left_Fin	},
	{	"AttackB_Right",	AIPROC_AttackB_Right_Run,	AIPROC_AttackB_Right_Fin	},
	{	"Change1PV",	AIPROC_Change1PV_Run,	AIPROC_Change1PV_Fin	},
	{	"Change3PV",	AIPROC_Change3PV_Run,	AIPROC_Change3PV_Fin	},
	{	"Change_Left",	AIPROC_Change_Left_Run,	AIPROC_Change_Left_Fin	},
	{	"Change_Right",	AIPROC_Change_Right_Run,	AIPROC_Change_Right_Fin	},
	{	"Fire3PV",	AIPROC_Fire3PV_Run,	AIPROC_Fire3PV_Fin	},
	{	"Fire1PV",	AIPROC_Fire1PV_Run,	AIPROC_Fire1PV_Fin	},
	{	"Idle3PV",	AIPROC_Idle3PV_Run,	AIPROC_Idle3PV_Fin	},
	{	"Idle1PV",	AIPROC_Idle1PV_Run,	AIPROC_Idle1PV_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"SecondaryFire_3PV",	AIPROC_SecondaryFire_3PV_Run,	AIPROC_SecondaryFire_3PV_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
	{	"UI_RebelFemale",	AIPROC_UI_RebelFemale_Run,	AIPROC_UI_RebelFemale_Fin	},
	{	"UI_RebelFemale2",	AIPROC_UI_RebelFemale2_Run,	AIPROC_UI_RebelFemale2_Fin	},
	{	"UI_RebelFemale_Left",	AIPROC_UI_RebelFemale_Left_Run,	AIPROC_UI_RebelFemale_Left_Fin	},
	{	"UI_RebelFemale_Right",	AIPROC_UI_RebelFemale_Right_Run,	AIPROC_UI_RebelFemale_Right_Fin	},
	{	"UI_RebelMale",	AIPROC_UI_RebelMale_Run,	AIPROC_UI_RebelMale_Fin	},
	{	"UI_RebelMale2",	AIPROC_UI_RebelMale2_Run,	AIPROC_UI_RebelMale2_Fin	},
	{	"UI_RebelMale_Left",	AIPROC_UI_RebelMale_Left_Run,	AIPROC_UI_RebelMale_Left_Fin	},
	{	"UI_RebelMale_Right",	AIPROC_UI_RebelMale_Right_Run,	AIPROC_UI_RebelMale_Right_Fin	},
};

AIDriverAxe_13PV::AIDriverAxe_13PV(void)
{
	setAIProcTable( s_Axe_13PVTable, (sizeof(s_Axe_13PVTable)/sizeof(s_Axe_13PVTable[0])) );
}

AIDriverAxe_13PV::~AIDriverAxe_13PV(void)
{
}
