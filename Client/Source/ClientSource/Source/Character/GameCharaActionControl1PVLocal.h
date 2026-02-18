#if !defined( __GAME_CHARA_ACTION_CONTROL_1PV_LOCAL_H__ )
#define __GAME_CHARA_ACTION_CONTROL_1PV_LOCAL_H__

#include "GameCharaActionControl1PV.h"

class CGameCharaActionControl1PVLocal : public CGameCharaActionControl1PV
{
	I3_CLASS_DEFINE( CGameCharaActionControl1PVLocal, CGameCharaActionControl1PV);
protected:
public:
	CGameCharaActionControl1PVLocal();
	virtual ~CGameCharaActionControl1PVLocal();

	//	param: 필요없음
	//virtual bool	OnActionStandup( void);

	//	param: _dir = pMoveDir
	//virtual bool	OnActionRunLower( VEC3D * pDir);

	//	param: 필요없음
	//virtual bool	OnActionJumpStart( void);

	
	//	param: _bool = bActive
	//virtual bool	OnActionExtensionWeapon( bool bActivate);
	

	// CROUCH 점프

	REAL32 m_rStartStandUpTime;
};


#endif
