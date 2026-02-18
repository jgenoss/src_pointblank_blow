#include "pch.h"
#include "../SentryGun/WeaponSentryGun.h"
#include "AIDriverSentrygun.h"

I3_CLASS_INSTANCE( AIDriverSentrygun)//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// NONE
//
static void AIPROC_NONE_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponSentryGun*>(pCtx->getOwner())->OnNone(pCtx, tm);
}

static void AIPROC_NONE_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UNDER_SURVEILLANCE
// ANIMATION : Weapon/SentryGunLV1/Resource/Idle.i3a
//
static void AIPROC_UNDER_SURVEILLANCE_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponSentryGun*>(pCtx->getOwner())->OnUnderSurveillance(pCtx, tm);
}

static void AIPROC_UNDER_SURVEILLANCE_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// SETUP
// ANIMATION : Weapon/SentryGunLV1/Resource/Install.i3a
//
static void AIPROC_SETUP_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponSentryGun*>(pCtx->getOwner())->OnSetup(pCtx, tm);
}

static void AIPROC_SETUP_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ATTACK
// ANIMATION : Weapon/SentryGunLV1/Resource/Attack.i3a
//
static void AIPROC_ATTACK_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponSentryGun*>(pCtx->getOwner())->OnAttack(pCtx, tm);
}

static void AIPROC_ATTACK_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DESTROY
// ANIMATION : Weapon/SentryGunLV1/Resource/Break.i3a
//
static void AIPROC_DESTROY_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponSentryGun*>(pCtx->getOwner())->OnDestroy(pCtx, tm);
}

static void AIPROC_DESTROY_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_SentrygunTable[] =
{
	{	"ATTACK",	AIPROC_ATTACK_Run,	AIPROC_ATTACK_Fin	},
	{	"DESTROY",	AIPROC_DESTROY_Run,	AIPROC_DESTROY_Fin	},
	{	"NONE",	AIPROC_NONE_Run,	AIPROC_NONE_Fin	},
	{	"SETUP",	AIPROC_SETUP_Run,	AIPROC_SETUP_Fin	},
	{	"UNDER_SURVEILLANCE",	AIPROC_UNDER_SURVEILLANCE_Run,	AIPROC_UNDER_SURVEILLANCE_Fin	},
};

AIDriverSentrygun::AIDriverSentrygun(void)
{
	setAIProcTable( s_SentrygunTable, (sizeof(s_SentrygunTable)/sizeof(s_SentrygunTable[0])) );
}

AIDriverSentrygun::~AIDriverSentrygun(void)
{
}
