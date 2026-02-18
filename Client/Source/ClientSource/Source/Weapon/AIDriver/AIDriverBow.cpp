#include "pch.h"
#include "AIDriverBow.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverBow);


static void AIPROC_GrenadeAttackReady1PV_Run( i3AIContext * pCtx, REAL32 tm)
{

}

static void AIPROC_GrenadeAttackReady1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}


static void AIPROC_GrenadeAttackReady3PV_Run( i3AIContext * pCtx, REAL32 tm)
{

}

static void AIPROC_GrenadeAttackReady3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire3PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Attack_3PV.i3a
//
static void AIPROC_Fire3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
//
static void AIPROC_ExtensionActivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_ACTIVATE_RUN( pCtx, tm);

}

static void AIPROC_ExtensionActivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
	//AIPROC_WEAPON_IDLE_RUN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate
//
static void AIPROC_ExtensionDeactivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_DEACTIVATE_RUN( pCtx, tm);

}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
	//AIPROC_WEAPON_IDLE_RUN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle.i3a
//
static void AIPROC_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Change1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Change3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}


////////////////////////////////////////////////////////////////////////////////
//
// Fire1PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Attack_1PV.i3a
//
static void AIPROC_Fire1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	//AIPROC_WEAPON_IDLE_RUN(pCtx,tm);
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Attack_G_G_3PV.i3a
//
static void AIPROC_SecondaryFire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	if (pWeapon)
		static_cast<WeaponBow*>(pWeapon)->OnSecondaryFire(pCtx, tm);
}

static void AIPROC_SecondaryFire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionIdle3PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_G_3PV.i3a
//
static void AIPROC_ExtensionIdle3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionIdle3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Rebel_Bella_B
//
static void AIPROC_UI_Rebel_Bella_B_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_Rebel_Bella_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle3PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_N_3PV.i3a
//
static void AIPROC_Idle3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle1PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_N_1PV.i3a
//
static void AIPROC_Idle1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate3PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Ext_G_N_3PV.i3a
//
static void AIPROC_ExtensionDeactivate3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_DEACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionDeactivate3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Chou_B
//
static void AIPROC_UI_SWAT_Chou_B_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_SWAT_Chou_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Hide_B
//
static void AIPROC_UI_SWAT_Hide_B_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_SWAT_Hide_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Leopard_B
//
static void AIPROC_UI_SWAT_Leopard_B_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_SWAT_Leopard_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Female_B
//
static void AIPROC_UI_SWAT_Female_B_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_SWAT_Female_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_A
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_Stand.i3a
//
static void AIPROC_UI_Idle_A_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_Idle_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_B
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_Change.i3a
//
static void AIPROC_UI_Idle_B_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_Idle_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
}


////////////////////////////////////////////////////////////////////////////////
//
// ExtensionIdle1PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_G_1PV.i3a
//
static void AIPROC_ExtensionIdle1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionIdle1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate3PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Ext_N_G_3PV.i3a
//
static void AIPROC_ExtensionActivate3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_ACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionActivate3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Leopard_A
//
static void AIPROC_UI_SWAT_Leopard_A_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_SWAT_Leopard_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Hide_A
//
static void AIPROC_UI_SWAT_Hide_A_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_SWAT_Hide_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Chou_A
//
static void AIPROC_UI_SWAT_Chou_A_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_SWAT_Chou_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Rebel_Bella_A
//
static void AIPROC_UI_Rebel_Bella_A_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_Rebel_Bella_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackReady3PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_N_3PV.i3a
//
static void AIPROC_AttackReady3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_AttackReady3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackReady1PV
// ANIMATION : Weapon/Compound_Bow_Blue/Resource/Idle_N_1PV.i3a
//
static void AIPROC_AttackReady1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_AttackReady1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_BowTable[] =
{
	{	"AttackReady1PV",	AIPROC_AttackReady1PV_Run,	AIPROC_AttackReady1PV_Fin	},
	{	"AttackReady3PV",	AIPROC_AttackReady3PV_Run,	AIPROC_AttackReady3PV_Fin	},
	{	"Change",	AIPROC_Change_Run,	AIPROC_Change_Fin	},
	{	"Change1PV",	AIPROC_Change1PV_Run,	AIPROC_Change1PV_Fin	},
	{	"Change3PV",	AIPROC_Change3PV_Run,	AIPROC_Change3PV_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionActivate3PV",	AIPROC_ExtensionActivate3PV_Run,	AIPROC_ExtensionActivate3PV_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"ExtensionDeactivate3PV",	AIPROC_ExtensionDeactivate3PV_Run,	AIPROC_ExtensionDeactivate3PV_Fin	},
	{	"ExtensionIdle1PV",	AIPROC_ExtensionIdle1PV_Run,	AIPROC_ExtensionIdle1PV_Fin	},
	{	"ExtensionIdle3PV",	AIPROC_ExtensionIdle3PV_Run,	AIPROC_ExtensionIdle3PV_Fin	},
	{	"Fire",	AIPROC_Fire1PV_Run,	AIPROC_Fire1PV_Fin	},
	{	"Fire1PV",	AIPROC_Fire1PV_Run,	AIPROC_Fire1PV_Fin	},
	{	"Fire3PV",	AIPROC_Fire3PV_Run,	AIPROC_Fire3PV_Fin	},
	{	"GrenadeAttackReady1PV",	AIPROC_GrenadeAttackReady1PV_Run,	AIPROC_GrenadeAttackReady1PV_Fin	},
	{	"GrenadeAttackReady3PV",	AIPROC_GrenadeAttackReady3PV_Run,	AIPROC_GrenadeAttackReady3PV_Fin	},
	{	"Idle",	AIPROC_Idle1PV_Run,	AIPROC_Idle1PV_Fin	},
	{	"Idle1PV",	AIPROC_Idle1PV_Run,	AIPROC_Idle1PV_Fin	},
	{	"Idle3PV",	AIPROC_Idle3PV_Run,	AIPROC_Idle3PV_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
	{	"UI_Idle_A",	AIPROC_UI_Idle_A_Run,	AIPROC_UI_Idle_A_Fin	},
	{	"UI_Idle_B",	AIPROC_UI_Idle_B_Run,	AIPROC_UI_Idle_B_Fin	},
	{	"UI_Rebel_Bella_A",	AIPROC_UI_Rebel_Bella_A_Run,	AIPROC_UI_Rebel_Bella_A_Fin	},
	{	"UI_Rebel_Bella_B",	AIPROC_UI_Rebel_Bella_B_Run,	AIPROC_UI_Rebel_Bella_B_Fin	},
	{	"UI_SWAT_Chou_A",	AIPROC_UI_SWAT_Chou_A_Run,	AIPROC_UI_SWAT_Chou_A_Fin	},
	{	"UI_SWAT_Chou_B",	AIPROC_UI_SWAT_Chou_B_Run,	AIPROC_UI_SWAT_Chou_B_Fin	},
	{	"UI_SWAT_Female_B",	AIPROC_UI_SWAT_Female_B_Run,	AIPROC_UI_SWAT_Female_B_Fin	},
	{	"UI_SWAT_Hide_A",	AIPROC_UI_SWAT_Hide_A_Run,	AIPROC_UI_SWAT_Hide_A_Fin	},
	{	"UI_SWAT_Hide_B",	AIPROC_UI_SWAT_Hide_B_Run,	AIPROC_UI_SWAT_Hide_B_Fin	},
	{	"UI_SWAT_Leopard_A",	AIPROC_UI_SWAT_Leopard_A_Run,	AIPROC_UI_SWAT_Leopard_A_Fin	},
	{	"UI_SWAT_Leopard_B",	AIPROC_UI_SWAT_Leopard_B_Run,	AIPROC_UI_SWAT_Leopard_B_Fin	},

};

AIDriverBow::AIDriverBow(void)
{
	setAIProcTable( s_BowTable, (sizeof(s_BowTable)/sizeof(s_BowTable[0])) );
}

AIDriverBow::~AIDriverBow(void)
{
}
