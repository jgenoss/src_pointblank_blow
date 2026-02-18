/**
 * \file	Character\GameCharaHUDControl.h
 *
 * Declares the game chara hud control class.
 */


#if !defined( __GAME_CHARA_HUD_CONTROL_H__)
#define __GAME_CHARA_HUD_CONTROL_H__

#include "GameCharaControlBase.h"
#include "StageBattle.h"

class CHud;

class CGameCharaHUDControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaHUDControl, CGameCharaControlBase);

public:

	/** \brief 무기 교체를 처리합니다. */
	virtual void	OnChangeWeapon( void);

	/** \brief Damage 효과를 처리합니다. */
	virtual void	OnDamage( VEC3D * pDir = nullptr, REAL32 rHitTheta = 0.f, bool bDirectionView = false, INT32 filter_index = -1 );

	/** \brief Crosshair를 적용합니다. */
	virtual bool	OnApplyCrosshair( void);
};

#endif
