#pragma once
#include "gamedropitem.h"

class CDropItemAmmo : public CGameDropItem
{
	I3_CLASS_DEFINE( CDropItemAmmo, CGameDropItem);
public:
	virtual bool		CheckForItemPickUp( REAL32 fDeltaSeconds) override;
	virtual void		process_authentication( CGameCharaBase* p) override;
};
