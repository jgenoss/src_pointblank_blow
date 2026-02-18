#pragma once

#include "MainWeapon_Throw.h"

#define NET_GRENADE_OFFTIME		15.f				// 네트워크로 15초간 못 받으면 삭제

class MainWeapon_Grenade : public MainWeapon_Throw
{
public:
	MainWeapon_Grenade(WeaponBase* p); 
	
	virtual void	OnInitVariable() override;
	virtual void	OnCreate( bool bCreateInWeapon) override;
	virtual void	Reset() override;

	virtual void	OnEndFire(i3AIContext * pCtx, REAL32 tm) override;

	virtual void	ChainExplosion(void);
	virtual void	OnUpdate_Grenade( REAL32 rDeltaSeconds);
	virtual void	SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo);

//// 임시로 비가상으로 작성..	

	void	NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );

////
	//  일단 임시로 작성...원래 함수가 상태가 좋지 않아보임..
	virtual void	CheckWorldCollision( REAL32 rDeltaSeconds);
	virtual void	UpdateNetworkData( REAL32 rDeltatime);
	virtual void	UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange);

	virtual void	Explosion();

	bool	SuccessiveStateFlagCheck(INT32 nState);

	void			_ProcessTakeObject();		// 일단 public으로..(옮겨야함)
	REAL32			_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir);
	void			_ProcessBoundObjectForce();

	void			CheckBullet();

/// 임시 함수..
	REAL32			GetElapsedExplosionTime() const { return m_rElapsedExplosionTime; }
	void			SetElapsedExplosionTime(REAL32 fTime) { m_rElapsedExplosionTime = fTime; }

	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx);
	
	virtual bool	isPlayRadioSound(void)	{ return true; }
	virtual bool	isUseTraceEffect(void)	{ return true; }

protected:

	// 유저가 나간 경우 해당 무기를 돌려줘야 하는 경우 호출
	bool			_CheckProcessUserLeave( void);
	virtual void	_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0);

	virtual WeaponGrenade *	_CreateThrowWepaon(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx);
	virtual void			_SetThrowData(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed);

	virtual void			_TakeObject(void);
	virtual void			_UpdateTakeObjectTransform(void);

private:
	virtual	bool	_CheckThrow(REAL32 LocalTime);		

protected:
	REAL32			m_rElapsedExplosionTime;		// 바닥에 부딛히고 터지기까지의 경과 시간

	UINT32			m_uCount;
	VEC3D			m_vObjectForce;	// 움직이는 오브젝트로 부터 힘을 받은 경우


	bool			m_bCheckFloor;	// 정지하고 바닥을 체크확인
	i3Transform *	m_pAttachObjectTransform;
	i3Object *		m_pAttachObject;
	
	REAL32			m_rBoundObjectLen;
	INT32			m_nFireEffectCount;

	INT32			m_nStateFlag;
	typedef i3::vector<INT32> vStateFlag;
	vStateFlag		m_vGrenadeStateFlag;

	bool			m_bResetBullet;
};

