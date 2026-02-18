/**
 * \file	Character\GameCharaMoveControl.h
 *
 * Declares the game chara move control class.
 */


#if !defined( __GAME_CHARA_MOVE_CONTROL_H__ )
#define __GAME_CHARA_MOVE_CONTROL_H__

#include "GameCharaControlBase.h"

class CGameCharaMoveControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaMoveControl, CGameCharaControlBase);

public:
	virtual void	OnUpdate( REAL32 rDeltatime);

	/** \brief 앉기 서기에 대한 처리를 합니다. */
	virtual void	OnChangeHeight( REAL32 rHeight);

	/** \brief 바닥 재질을 검사합니다. */
	virtual void	OnProcessFloorCheck( REAL32 rDeltatime);

	/** \brief 바닥의 재질을 검사합니다. */
	void			CheckFloorTerrain( void);
};

#endif
