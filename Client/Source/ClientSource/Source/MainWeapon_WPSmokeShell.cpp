#include "pch.h"
#include "MainWeapon_WPSmokeShell.h"
#include "MyRoomInfoContext.h"
#include "GameMode.h"
#include "TutorialMode.h"
#include "BattleObjContext.h"
#include "MainWeapon_WPSmoke.h"

MainWeapon_WPSmokeShell::MainWeapon_WPSmokeShell(WeaponBase* p) : MainWeapon_GrenadeShell(p), m_bSmokeExplosed(false)
{
	m_pWPSmoke = new MainWeapon_WPSmoke(p);
	MEMDUMP_NEW(p, sizeof(MainWeapon_WPSmoke));
}

MainWeapon_WPSmokeShell::~MainWeapon_WPSmokeShell() 
{
	I3_SAFE_DELETE(m_pWPSmoke);
}

void MainWeapon_WPSmokeShell::OnUpdate_GrenadeShell(REAL32 rDeltaSeconds) 
{
	MainWeapon_GrenadeShell::OnUpdate_GrenadeShell(rDeltaSeconds);

	m_pWPSmoke->OnUpdate_Grenade(rDeltaSeconds);
}

void MainWeapon_WPSmokeShell::Explosion()
{
	if (m_bSmokeExplosed) return;

	m_pWPSmoke->Explosion();

	// 백린탄은 Grenade Shell이 Update를 돌면 움직이기 때문에 업데이트를 끕니다.
	m_pWeaponBase->SetVisible(false);
	removeBombState(GRENADESHELL_STATE_LAUNCH);

	m_bSmokeExplosed = true;
}

void MainWeapon_WPSmokeShell::Reset()
{
	MainWeapon_GrenadeShell::Reset();

	m_pWPSmoke->Reset();
	m_bSmokeExplosed = false;
}