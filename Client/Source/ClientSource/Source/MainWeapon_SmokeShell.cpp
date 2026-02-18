#include "pch.h"
#include "MainWeapon_SmokeShell.h"
#include "MainWeapon_Smoke.h"
#include "MyRoomInfoContext.h"
#include "GameMode.h"
#include "TutorialMode.h"
#include "BattleObjContext.h"

MainWeapon_SmokeShell::MainWeapon_SmokeShell(WeaponBase* p) : MainWeapon_GrenadeShell(p), m_bSmokeExplosed(false)
{
	m_pSmoke = new MainWeapon_Smoke(p);
	MEMDUMP_NEW(m_pSmoke, sizeof(MainWeapon_Smoke));
}

MainWeapon_SmokeShell::~MainWeapon_SmokeShell() 
{
	I3_SAFE_DELETE(m_pSmoke);
}

void MainWeapon_SmokeShell::Reset()
{
	MainWeapon_GrenadeShell::Reset();

	m_pSmoke->Reset();

	m_bSmokeExplosed = false;
}

void MainWeapon_SmokeShell::OnUpdate_GrenadeShell(REAL32 rDeltaSeconds) 
{
	MainWeapon_GrenadeShell::OnUpdate_GrenadeShell(rDeltaSeconds);

	m_pSmoke->OnUpdate_Grenade(rDeltaSeconds);
}

void MainWeapon_SmokeShell::Explosion()
{
	if (m_bSmokeExplosed) return;

	m_pSmoke->Explosion();

	g_pWeaponManager->PushGrenadeShellWeapon(static_cast<WeaponSmokeShell*>(m_pWeaponBase));

	m_bSmokeExplosed = true;
}
