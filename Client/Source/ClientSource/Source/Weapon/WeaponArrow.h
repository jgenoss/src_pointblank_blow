#if !defined( __WEAPON_ARROW_H__)
#define __WEAPON_ARROW_H__

#include "WeaponBase.h"

#define		WEAPON_ARROW_STATE_NONE		0x00
#define		WEAPON_ARROW_STATE_FLY		0x01
#define		WEAPON_ARROW_STATE_HIT		0x02
#define		WEAPON_ARROW_STATE_HITCHARA	0x04


class WeaponArrow : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponArrow, WeaponBase);
protected:
	BOOL			m_bCheckThrow;	// 던져졌는지 체크
	REAL32			m_rElapsedThrowTime;
	REAL32			m_rTracerUpdateTime;

	VEC3D			m_vTracerOldPos;

	VEC3D			m_vOldPos;		//이전 위치
	VEC3D			m_vStartPos;	//시작 위치
	VEC3D			m_vDirection;	//방향
	REAL32			m_rSpeed;		//속도

	void *			m_pInfo;

protected:
	virtual	void	_OnInitVairable( void);

public:
	void			setInfo( void * pInfo)		{ m_pInfo = pInfo; }

public:
	WeaponArrow();
	virtual ~WeaponArrow();

	virtual void	Create( CGameCharaBase * pOwner, INT32 Itemid);
	virtual void	OnBindResource( void);
	
	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, BOOL bOrigin);
	void			UpdateFlying( REAL32 rDeltaSeconds);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);

	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	void			OnHit( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine);
};

#endif
