#if !defined( __GAME_CHARA_ACTION_CONTROL_3PV_NETWORK_H__)
#define __GAME_CHARA_ACTION_CONTROL_3PV_NETWORK_H__

#include "GameCharaActionControl3PV.h"

class CGameCharaActionControl3PVNetwork : public CGameCharaActionControl3PV
{
	I3_CLASS_DEFINE( CGameCharaActionControl3PVNetwork, CGameCharaActionControl3PV);
protected:
public:
	CGameCharaActionControl3PVNetwork();
	virtual ~CGameCharaActionControl3PVNetwork();


public:
	//virtual bool OnActionStandup( void);
	//virtual bool OnActionJumpStart( void);
};


#endif
