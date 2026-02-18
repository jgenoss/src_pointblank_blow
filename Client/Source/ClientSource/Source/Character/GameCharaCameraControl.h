/**
 * \file	Character\GameCharaCameraControl.h
 *
 * Declares the game chara camera control class.
 */


#if !defined( __GAME_CHARA_CAMERA_CONTROL_H__ )
#define __GAME_CHARA_CAMERA_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaCameraControlDef.h"

class CGameCharaCameraControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaCameraControl, CGameCharaControlBase);

public:
	/** \brief FPS Camera로 변경합니다. */
	virtual bool	OnChangeFPSCamera( void);

	/** \brief Kill Camera로 변경합니다. */
	virtual bool	OnChangeKillCamera( INT32 idx)	{ return false; }

	/** \brief Observer Camera로 변경합니다. */
	virtual bool	OnChangeObserverCamera( void);

	/** \brief Observer Camera에서 Zoom을 설정합니다. */
	virtual bool	OnSetObserveZoom(void);

	/** \brief Zoom으로 설정합니다. */
	virtual bool	OnSetZoom( void);

	/** \brief Respawn시 처리하는 함수입니다. */
	virtual bool	OnProcessRespawn( void);
};

#endif
