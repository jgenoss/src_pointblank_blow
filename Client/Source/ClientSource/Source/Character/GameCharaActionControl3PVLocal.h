#if !defined( __GAME_CHARA_ACTION_CONTROL_3PV_LOCAL_H__)
#define __GAME_CHARA_ACTION_CONTROL_3PV_LOCAL_H__

#include "GameCharaActionControl3PV.h"

class CGameCharaActionControl3PVLocal : public CGameCharaActionControl3PV
{
	I3_CLASS_DEFINE( CGameCharaActionControl3PVLocal, CGameCharaActionControl3PV);
protected:
	
public:
	CGameCharaActionControl3PVLocal();
	virtual ~CGameCharaActionControl3PVLocal();


public:
	//	param: 필요없음
	//virtual bool	OnActionStandup( void);

	//	param: 필요없음
	//virtual bool	OnActionJumpStart( void);

	//	param: _bool = bActive
	//virtual bool	OnActionExtensionWeapon( bool bActivate);
};

#endif
