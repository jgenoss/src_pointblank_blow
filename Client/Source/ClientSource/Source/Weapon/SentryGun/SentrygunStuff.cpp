#include "pch.h"

#if 0

#ifdef	DOMI_SENTRYGUN

#include "SentrygunStuff.h"


void SentrygunStuff::RemovePrevious(WeaponSentryGun* cur)
{
	CGameCharaBase* curInstaller = cur->GetInstaller();

	INT32 total = g_pWeaponManager->GetDropDominationCount();
	for (INT32 i=0; i<total; i++)
	{
		WeaponBase* obj = g_pWeaponManager->GetDropDomination(i);
		if (!obj) return;

		if (obj->getObjectType() == TYPE_OBJ_INSTALL_SENTRYGUN)
		{
			WeaponSentryGun* target = static_cast<WeaponSentryGun*>(obj);
			CGameCharaBase* targetInstaller = target->GetInstaller();
			if (!targetInstaller) continue;

			INT32 curInstallerIdx = curInstaller->getCharaNetIndex();
			INT32 targetInstallerIdx = targetInstaller->getCharaNetIndex();
			INT32 curSGIdx = cur->getNetworkIdx();
			INT32 targetSGIdx = target->getNetworkIdx();

			if ((curInstallerIdx == targetInstallerIdx) && (curSGIdx != targetSGIdx))
				g_pWeaponManager->PushDropWeaponToChara(target);
		}
	}
}

void SentrygunStuff::Remove(INT16 objIdx)
{
	if (objIdx < 0) return;

	INT32 total = g_pWeaponManager->GetDropDominationCount();
	for (INT32 i=0; i<total; i++)
	{
		WeaponBase* obj = g_pWeaponManager->GetDropDomination(i);
		if (!obj) return;

		if (obj->getObjectType() == TYPE_OBJ_INSTALL_SENTRYGUN)
		{
			WeaponSentryGun* sg = static_cast<WeaponSentryGun*>(obj);
			if (sg->getNetworkIdx() == objIdx)
				g_pWeaponManager->PushDropWeaponToChara(sg);
		}
	}
}

#endif

#endif