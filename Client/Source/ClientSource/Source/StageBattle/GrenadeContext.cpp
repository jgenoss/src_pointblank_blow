#include "pch.h"
#include "GrenadeContext.h"

#include "../StageBattle.h"

void GrenadeContext::ResetGrenadeCheck()
{
	for( INT32 i = 0; i < MAX_THROWWEAPON_SLOP; ++i)
		m_GrenadeCheck[i].Reset();
}
