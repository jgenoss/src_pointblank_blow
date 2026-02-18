#if !defined( __GAME_CHARA_CAMERA_CONTROL_3PV_NPC_H__)
#define __GAME_CHARA_CAMERA_CONTROL_3PV_NPC_H__

#include "GameCharaCameraControl3PV.h"

class CGameCharaCameraControl3PVNPC : public CGameCharaCameraControl3PV
{
	I3_CLASS_DEFINE( CGameCharaCameraControl3PVNPC, CGameCharaCameraControl3PV);
protected:
public:
	CGameCharaCameraControl3PVNPC();
	virtual ~CGameCharaCameraControl3PVNPC();

	virtual bool	OnChangeFPSCamera( void)				{ return false; }
	virtual bool	OnChangeKillCamera( INT32 idx)			{ return false; }
	virtual bool	OnChangeObserverCamera( void)			{ return false;	}
	virtual bool	OnSetObserveZoom(void)					{ return false; }

	virtual bool	OnSetZoom( void)						{ return false;	}

	virtual bool	OnProcessRespawn( void)					{ return false; }
};

#endif
