#if !defined( __GAME_CHARA_INPUT_CONTROL_NPC_H__)
#define __GAME_CHARA_INPUT_CONTROL_NPC_H__

#include "GameCharaInputControl.h"

class CGameCharaInputControlNPC : public CGameCharaInputControl
{
	I3_CLASS_DEFINE( CGameCharaInputControlNPC, CGameCharaInputControl);
protected:
public:
	CGameCharaInputControlNPC();
	virtual ~CGameCharaInputControlNPC();

	virtual void	OnFirstUpdate( REAL32 rDeltatime);
	virtual void	OnUpdate( REAL32 rDeltatime);
	virtual void	OnLastUpdate( REAL32 rDeltatime);


};

#endif
