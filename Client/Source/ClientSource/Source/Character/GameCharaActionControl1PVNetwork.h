#if !defined( __GAME_CHARA_ACTION_CONTROL_1PV_NETWORK_H__)
#define __GAME_CHARA_ACTION_CONTROL_1PV_NETWORK_H__

#include "GameCharaActionControl1PV.h"

class CGameCharaActionControl1PVNetwork : public CGameCharaActionControl1PV
{
	I3_CLASS_DEFINE( CGameCharaActionControl1PVNetwork, CGameCharaActionControl1PV);
protected:
public:
	CGameCharaActionControl1PVNetwork();
	virtual ~CGameCharaActionControl1PVNetwork();

	//	param: _dir = pMoveDir
	//virtual bool	OnActionRunLower( VEC3D * pDir);
	
	//	param: 필요없음
	//virtual bool	OnActionJumpStart( void);
};

#endif
