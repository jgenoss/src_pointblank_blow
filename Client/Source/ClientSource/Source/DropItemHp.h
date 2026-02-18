#pragma once
#include "gamedropitem.h"

class CDropItemHp :
	public CGameDropItem
{
	I3_CLASS_DEFINE( CDropItemHp, CGameDropItem);
public:
	virtual bool		CheckForItemPickUp( REAL32 fDeltaSeconds);
	virtual void		process_authentication( CGameCharaBase* p);
};
