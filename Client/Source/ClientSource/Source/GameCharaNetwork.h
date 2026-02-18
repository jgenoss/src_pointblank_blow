#if !defined( CGAME_CHARA_NETWORK_H)
#define CGAME_CHARA_NETWORK_H

#include "GameCharaPlayer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Class : CGameCharaNetwork
//	Desc: 자신외의 네트워크 캐릭터에 필요한 처리를 담당합니다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGameCharaNetwork : public CGameCharaPlayer
{
	I3_CLASS_DEFINE(  CGameCharaNetwork);

/////////////////////////////////////////////////	Variables
protected:
	//i3Quad *		m_pTeamTag;

	//GAME_CHARA_NETWORK_MOVE_TYPE	m_CheckMoveType;

	//REAL32			m_rVelocity;
	//REAL32			m_rAttackIdleTime;
//	BOOL			m_bChangeDir;

/////////////////////////////////////////////////	Functions
protected:
	//void			_ProcessNetworkInput( REAL32 rDeltatime );	
	//GAME_CHARA_NETWORK_MOVE_TYPE	_ProcessNetworkMoveDir( GAMEINFO_ASSIST * pAssist, VEC3D * pRecvPos, UINT32 nLower, REAL32 rDeltatime );

public:
	CGameCharaNetwork( void);
	virtual ~CGameCharaNetwork( void);
	virtual void	Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo);
	virtual void	OnUpdate( REAL32 tm);

	virtual void	ResetChara( void);	
	virtual void	DumpChara( void);
	virtual void	ProcessFirst( REAL32 tm);

	virtual void	OnInitVariable( void);

	//	Anim (CallBack)
	//virtual void	OnAttackRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnAttackFinish( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm);

	// Mark
	//virtual void	SetCharaMark( CWeaponInfo * pWeaponInfo, BOOL bEnable = TRUE);
};

#endif

