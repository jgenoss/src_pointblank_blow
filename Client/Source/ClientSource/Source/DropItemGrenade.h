#pragma once
#include "GameDropItem.h"

class CDropItemGrenade : public CGameDropItem
{
	I3_CLASS_DEFINE( CDropItemGrenade, CGameDropItem);
public:
	virtual bool		CheckForItemPickUp( REAL32 fDeltaSeconds) override;
	virtual void		process_authentication( CGameCharaBase* p) override;
};