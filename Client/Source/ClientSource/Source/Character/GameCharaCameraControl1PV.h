#if !defined( __GAME_CHARA_CAMERA_CONTROL_1PV_H__)
#define __GAME_CHARA_CAMERA_CONTROL_1PV_H__

#include "GameCharaCameraControl.h"

class CGameCharaCameraControl1PV : public CGameCharaCameraControl
{
	I3_CLASS_DEFINE( CGameCharaCameraControl1PV, CGameCharaCameraControl);

public:	
	virtual bool	OnChangeKillCamera( INT32 idx) override;
};

#endif
