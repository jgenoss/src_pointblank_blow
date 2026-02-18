#if !defined( __GAME_CHARA_UPDATER_NPC_H__)
#define __GAME_CHARA_UPDATER_NPC_H__

#include "GameCharaUpdater3PV.h"

class CGameCharaUpdater3PVNPC : public CGameCharaUpdater3PV
{
	I3_CLASS_DEFINE( CGameCharaUpdater3PVNPC, CGameCharaUpdater3PV );
protected:

public:
	CGameCharaUpdater3PVNPC();
	virtual ~CGameCharaUpdater3PVNPC();

	void	ProcessRespawn( REAL32 rDeltatime);
	void	SendDataToNetwork( void);

	virtual void	OnFirstUpdate( REAL32 rDeltatime);
	virtual void	OnUpdate( REAL32 rDeltatime);

	virtual void	OnProcessInvincible( REAL32 rDeltatime);
};

#endif
