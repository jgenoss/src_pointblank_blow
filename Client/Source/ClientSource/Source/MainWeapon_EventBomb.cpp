#include "pch.h"
#include "MainWeapon_EventBomb.h"
#include "i3Base/string/ext/utf16_to_utf8.h"


MainWeapon_EventBomb::MainWeapon_EventBomb(WeaponBase* p) : MainWeapon_DecoyBomb(p)
{

}

MainWeapon_EventBomb::~MainWeapon_EventBomb(void)
{
	//ResetDecoyStatus();
}

/*virtual*/const WeaponDecoySndInterval * MainWeapon_EventBomb::_GetSndInterval(void)
{
	return g_pWeaponManager->GetEventDecoySndInverval();
}

/*virtual*/ i3SoundPlayInfo * MainWeapon_EventBomb::_GetSoundPlayInfo(void)
{
	return GetDefaultDecoySound();
}
