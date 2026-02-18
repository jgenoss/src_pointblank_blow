#include "pch.h"
#include "MainWeapon_FlashbangShell.h"
#include "MainWeapon_Flash.h"
#include "MyRoomInfoContext.h"
#include "GameMode.h"
#include "TutorialMode.h"
#include "BattleObjContext.h"

MainWeapon_FlashbangShell::MainWeapon_FlashbangShell(WeaponBase* p) : MainWeapon_GrenadeShell(p), m_bFlashExplosed(false)
{
	m_pFlash = new MainWeapon_Flash(p);
	MEMDUMP_NEW(m_pFlash, sizeof(MainWeapon_Flash));
}

MainWeapon_FlashbangShell::~MainWeapon_FlashbangShell() 
{
	I3_SAFE_DELETE(m_pFlash);
}

void MainWeapon_FlashbangShell::OnUpdate_GrenadeShell(REAL32 tm)
{
	MainWeapon_GrenadeShell::OnUpdate_GrenadeShell(tm);

	m_pFlash->OnUpdate_Grenade(tm);
}

void MainWeapon_FlashbangShell::Explosion()
{
	if (m_bFlashExplosed) return;

	m_pFlash->Explosion();

	m_pWeaponBase->SetVisible(false);
	//g_pWeaponManager->PushGrenadeShellWeapon(static_cast<WeaponFlashbangShell*>(m_pWeaponBase));

	m_bFlashExplosed = true;
}

void MainWeapon_FlashbangShell::Reset()
{
	MainWeapon_GrenadeShell::Reset();

	m_pFlash->Reset();

	m_bFlashExplosed = false;
}

void MainWeapon_FlashbangShell::_OnCheckCollisionByExplosion()
{
	m_pFlash->_OnCheckCollisionByExplosion();
}

bool MainWeapon_FlashbangShell::_OnCheckCharaCollisionByExplosion(CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange)
{
	return m_pFlash->_OnCheckCharaCollisionByExplosion(pChara, nColGroup, rRange);
}