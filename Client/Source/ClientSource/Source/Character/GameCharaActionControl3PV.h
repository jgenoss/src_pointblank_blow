#if !defined( __GAME_CHARA_ACTION_CONTROL_3PV_H__)
#define __GAME_CHARA_ACTION_CONTROL_3PV_H__

#include "GameCharaActionControl.h"

class CGameCharaActionControl3PV : public CGameCharaActionControl
{
	I3_CLASS_DEFINE( CGameCharaActionControl3PV, CGameCharaActionControl);

public:	
	virtual bool	OnActionReset( void) override;
	virtual bool	OnActionRunLower( VEC3D * pDir) override;
	virtual bool	OnActionRunUpper( void) override;
	virtual bool	OnActionRunDinoUpper( void) override;
	virtual bool	OnActionWalkDinoUpper( void) override;
	virtual bool	OnActionWalkLower( VEC3D * pDir) override;
	virtual bool	OnActionSteadyLower( void) override;
	virtual bool	OnActionJumpLowerFinish( void) override;
	virtual bool	OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir) override;
	bool			OnActionDamageHuman( CHARA_HIT_PART part, VEC3D * pDir);
	bool			OnActionDamageRaptor( CHARA_HIT_PART part, VEC3D * pDir);
	bool			OnActionDamageRex( CHARA_HIT_PART part, VEC3D * pDir);
	virtual bool	OnActionProfileIdleA( void) override;
	virtual bool	OnActionProfileIdleB( void) override;

	virtual bool	OnActionProfileIdleChange(bool bIsSameWeapon) override;
	virtual bool	OnActionProfileIdleStand(void) override;

protected:
	bool	OnActionDinoRunLower( VEC3D * pDir);
	bool	OnActionDinoWalkLower( VEC3D * pDir);
	bool	OnActionRexRunLower( VEC3D * pDir);
	bool	OnActionRexWalkLower( VEC3D * pDir);
};

#endif

