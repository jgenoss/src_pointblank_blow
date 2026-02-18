#ifndef _WEAPON_CENTRYGUN_H_
#define _WEAPON_CENTRYGUN_H_


#include "weaponbase.h"
#include "SentrygunBehavior.h"


class i3Quad;

enum SG_BEHAVIOR
{
	SG_ATTACK,				// 공격
	SG_DESTROY,				// 파괴
	SG_NONE,				// NONE
	SG_SETUP,				// 월드에 설치
	SG_UNDER_SURVEILLANCE,	// 감시

	SG_BEHAVIOR_MAX,
};

struct FireArmAngle
{
	static REAL32 LERP_TIME;

	REAL32	theta;				//radian
	REAL32	thetaIncr;
	REAL32  rho;
	REAL32  rhoIncr;

	REAL32  src_theta;
	REAL32  recv_theta;
	REAL32  interpolate_time_theta;
	REAL32 interpolate_theta();


	FireArmAngle()
	{ 
		Reset();
	}
	void Reset()
	{
		theta = rho = 0.0f; thetaIncr = 0.005f; rhoIncr = 0.001f;

		src_theta = 0.f;
		recv_theta = 0.f;
		interpolate_time_theta = 0.f;
	}
	void ReceiveTheta( REAL32 recv);
};

class WeaponSentryGun : public WeaponBase
{
	I3_CLASS_DEFINE(WeaponSentryGun, WeaponBase);

public:
	WeaponSentryGun();
	virtual ~WeaponSentryGun();

	virtual void			Create(CGameCharaBase* owner, T_ItemID Itemid, bool bCreateInWeapon ) override;
	virtual void			OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void			InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta = 0.0f) override;
	virtual void			Reset() override;
	virtual	void			ReturnInstance() override;
	virtual void			OnBindResource() override;
	virtual void			_InitCollideeGroup( void) override;
	void					ActiveDisarm(bool enable) {}

	void					OnNone(i3AIContext* ctx, REAL32 tm);
	void					OnUnderSurveillance(i3AIContext* ctx, REAL32 tm);
	void					OnAttack(i3AIContext* ctx, REAL32 tm);
	void					OnSetup(i3AIContext* ctx, REAL32 tm);
	void					OnDestroy(i3AIContext* ctx, REAL32 tm);

	void					ProcessPacket(VEC3D* target);
	void					ProcessFirePacket(VEC3D* target);
	void					Uninstall();
	i3CollideeLine*			CollideeLine()			{ return &m_CollideeLine;	}
	const i3CollideeLine*	CollideeLine() const	{ return &m_CollideeLine;	}
	const VEC3D&			GetInstallPos() const	{ return m_installPos;		}
	CGameCharaBase*			GetInstaller() const	{ return m_installer;		}
	const FireArmAngle*		GetFireArmAngle() const	{ return &m_fireArmAngle;	}
	FireArmAngle*			GetFireArmAngle()		{ return &m_fireArmAngle;	}
	void					SetHP(INT16 hp);
	INT16					GetHP() const			{ return m_hp;}
	REAL32					GetHPRatio() const;

private:
	VEC3D					m_installPos;
	CGameCharaBase*			m_installer;

	i3Quad*					m_tagHud;

	FireArmAngle			m_fireArmAngle;
	INT32					m_team;
	UINT16					m_hp;
	UINT16					m_max_hp;
};
#endif

