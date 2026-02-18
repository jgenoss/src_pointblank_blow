#if !defined( __GAME_CHARA_ACTION_CONTROL_1PV_H__)
#define __GAME_CHARA_ACTION_CONTROL_1PV_H__

#include "GameCharaActionControl.h"

class CGameCharaActionControl1PV : public CGameCharaActionControl
{
	I3_CLASS_DEFINE( CGameCharaActionControl1PV, CGameCharaActionControl);

public:
	virtual bool	OnActionRunLower( VEC3D * pDir) override;
	virtual bool	OnActionDinoRunLower( VEC3D * pDir) override;
	virtual bool	OnActionWalkLower( VEC3D * pDir) override;
	virtual bool	OnActionJumpLowerFinish( void) override;
	virtual bool	OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir) override;
	bool			OnDinoActionDamage( CHARA_HIT_PART part, VEC3D * pDir);
	virtual bool	OnActionExtensionWeapon( bool bActivate) override;
	virtual bool	OnActionSwapWeapon( void) override;
};

#endif
