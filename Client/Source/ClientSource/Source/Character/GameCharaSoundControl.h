/**
 * \file	Character\GameCharaSoundControl.h
 *
 * Declares the game chara sound control class.
 */


#if !defined( __GAME_CHARA_SOUND_CONTROL_H__ )
#define __GAME_CHARA_SOUND_CONTROL_H__

#include "GameCharaControlBase.h"

class CGameCharaSoundControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaSoundControl, CGameCharaControlBase);

public:

	/** \brief 캐릭터 타격 사운드를 Update합니다. */
	virtual void	OnHit( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pPos, bool bListener);

	/** \brief 점프 사운드를 Update합니다. */
	virtual void	OnJumpStart( void);

	/** \brief 착지 사운드를 Update합니다. */
	virtual void	OnJumpEnd( void);
};

#endif
