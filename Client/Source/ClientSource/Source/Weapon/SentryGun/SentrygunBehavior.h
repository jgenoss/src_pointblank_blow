#ifndef _SENTRYGUN_BEHAVIOR_H_
#define _SENTRYGUN_BEHAVIOR_H_

class WeaponSentryGun;
struct Behavior
{
	virtual void operator()(WeaponSentryGun& sg) = 0;
};

Behavior* GetSentrygunBehavior(INT32 idx);

#endif