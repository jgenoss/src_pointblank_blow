#include "pch.h"
#include "WeaponFlash.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaBoneContext.h"
#include "BattleSlotContext.h"
#include "../MainWeapon_Flash.h"

I3_CLASS_INSTANCE( WeaponFlash);//, WeaponGrenade);

WeaponFlash::WeaponFlash( void)
{
}

WeaponFlash::~WeaponFlash(  void)
{

}


void WeaponFlash::_OnCheckCollisionByExplosion()
{
	//MainWeapon_Flash * mainWeapon = new MainWeapon_Flash(this);
	//mainWeapon->_OnCheckCollisionByExplosion();
	static_cast<MainWeapon_Flash*>(GetMainWeapon())->_OnCheckCollisionByExplosion();
}

bool WeaponFlash::_OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rExplosionRange)
{	
	//MainWeapon_Flash * mainWeapon = new MainWeapon_Flash(this);
	return static_cast<MainWeapon_Flash*>(GetMainWeapon())->_OnCheckCharaCollisionByExplosion(pChara, nColGroup, rExplosionRange);
}

MainWeapon* WeaponFlash::CreateMainWeapon()
{
	MainWeapon_Flash * p = new MainWeapon_Flash(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_Flash));
	return p;
}