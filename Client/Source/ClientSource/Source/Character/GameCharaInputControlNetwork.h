#if !defined( __GAME_CHARA_INPUT_CONTROL_NETWORK_H__)
#define __GAME_CHARA_INPUT_CONTROL_NETWORK_H__

#include "GameCharaInputControl.h"

class CGameCharaInputControlNetwork : public CGameCharaInputControl
{
	I3_CLASS_DEFINE( CGameCharaInputControlNetwork, CGameCharaInputControl);

	friend class CGameCharaInputControlFullbody;

protected:
	REAL32			m_rReloadTime = 0.0f;
	REAL32			m_rReloadElapesedTime = 2.0f;
	REAL32			m_rReloadReadyTime = 0.0f;
	REAL32			m_rReloadReadyElapesedTime = 2.0f;
	REAL32			m_rLoadBulletTime = 0.0f;
	REAL32			m_rLoadBulletElapesedTime = 2.0f;

protected:
	GAME_CHARA_NETWORK_MOVE_TYPE	_ProcessRecv_MoveDir( GAMEINFO_CHARA_NETDATA * pRecvInfo);
	void			_ProcessRecv_Position( GAMEINFO_CHARA_NETDATA * pInfo);
	void			_ProcessRecv_PositionOnLoadObject( GAMEINFO_CHARA_NETDATA * pInfo);

	void			_ProcessMoveByKey( UINT8 key, VEC3D * pPos);
	void			_InterpolateThetaRho( void);
	void			_InterpolatePosition( VEC3D * pPos);
	void			_ProcessAutoActionUpper( void);

public:
	virtual void	OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	void			ProcessNetworkActionBody( CHARA_ACTION_BODYLOWER body);
	void			ProcessNetworkActionBody2( CHARA_ACTION_BODYUPPER body2);
	void			ProcessNetworkActionUpper( CHARA_ACTION_UPPER upper);
	void			ProcessNetworkActionUpperDino( CHARA_ACTION_UPPER upper);
	void			ProcessNetworkActionUpperHuman( CHARA_ACTION_UPPER upper);
	void			ProcessNetworkActionLower( CHARA_ACTION_LOWER lower, REAL32 rLength);

	void			ProcessNetworkPosition( GAMEINFO_CHARA_NETDATA * pRecvInfo);
	void			ProcessNetworkRotationTheta( GAMEINFO_CHARA_NETDATA * pRecvInfo );
	void			ProcessNetworkRotationRho( GAMEINFO_CHARA_NETDATA * pRecvInfo );
};

#endif
