#pragma once

#include "MainWeapon_Throw.h"

//
//  수류탄과 유사해보이지만, 군데군데 다른 곳도 적지 않아서 수류탄으로 상속받기 어렵다..
//

class MainWeapon_C5 : public MainWeapon_Throw
{
public:
	MainWeapon_C5(WeaponBase* p);

	void			OnUpdate_C5(REAL32 rDeltaSeconds);
	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed);

	void			NET_Bound_C5( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );		// 일단 비가상처리..WeaponC5의 동명함수로부터 내부 호출된다.
	void			Explosion_C5();					// 가상함수일 필요가 없어 비가상으로 돌리고, C5접미어 붙여 구별.
	
	virtual void	ChainExplosion(void) override;
	virtual VEC3D*	GetThrowInfo_Normal(void) override { return &m_vAttachNormal; }

	void			RemoveInvalidBomb(void);

private:
	virtual void	OnInitVariable() override;
	virtual void	OnCreate( bool bCreateInWeapon) override {}
	virtual void	Reset() override;

	REAL32			_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir);
	void			_ProcessBoundObjectForce();

	void			UpdateFlying( REAL32 rDeltaSeconds);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);				// 수류탄클래스들에서는 가상함수이지만, C5에서는 비가상


	void			_TakeObject( i3Object * pObject);
	void			_TakeObject( I3_PHYSIX_HIT_RESULT * pResult );

	void			_CollisionTestForAttach( void);
	void			_ProcessOnTakeObject( void);

	void			UpdateNetworkData( REAL32 rDeltatime);					// 가상일 필요가 없어 virtual을 떼었다..

	// 유저가 나간 경우 해당 무기를 돌려줘야 하는 경우 호출
	bool			_CheckProcessUserLeave( void);

	REAL32			m_rThrowLen;					// 거리에 따른 벽에 붙는 기능을 제어
	VEC3D			m_vAttachNormal;				// 붙은 벽의 노멀
	i3Transform *	m_pAttachObjectTransform;		// 오브젝트에 붙는 경우의 transform
	i3Object *		m_pAttachObject;
	MATRIX			m_AttachLocalMatrix;

	REAL32			m_rElapsedExplosionTime;		// 격발이되고난 후의 폭파 시간
	REAL32			m_rExplosionDelay;
	REAL32			m_rExplosionDelayTimer;
	REAL32			m_rRotateY;

	bool			m_bFirstBound;

	REAL32			m_rFloorCheckTime;

	bool			m_bCheckAttachFloor;
	bool			m_bAttchedObject;

	VEC3D			m_vObjectForce;
	VEC3D			m_vObjectForceNormal;
	REAL32			m_rBoundObjectLen;
	i3Object *		m_pBoundObject;
};

