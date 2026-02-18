#include "pch.h"
#include "WeaponSentryGun.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"

I3_CLASS_INSTANCE(WeaponSentryGun);//, WeaponBase);

WeaponSentryGun::WeaponSentryGun() : m_isInstalled(FALSE)
{
}

WeaponSentryGun::~WeaponSentryGun()
{
}

void WeaponSentryGun::Create(CGameCharaBase* owner, INT32 Itemid)
{
	WeaponBase::Create(owner, Itemid);
}

void WeaponSentryGun::OnUpdate(REAL32 deltaSeconds)
{
	WeaponBase::OnUpdate(deltaSeconds);
}

void WeaponSentryGun::UpdateAfterAnim( REAL32 rDeltaSeconds)
{
	if (IsInstalled())
	{
		RotateAxisY(ROTATE_RIGHT, 1.0f, 1.5f);
		Fire();
	}
}

void WeaponSentryGun::OnIdle(i3AIContext* ctx, REAL32 tm)
{
}

void WeaponSentryGun::Explosion()
{
}

void WeaponSentryGun::Reset()
{
}

void WeaponSentryGun::Fire()
{
	_NetFire();
}

void WeaponSentryGun::_NetFire()
{
	if (getLoadedBulletCount() <= 0) return;

	m_FireOrder = WEAPON::PRIMARY_FIRE;

	VEC3D at, up;

	i3Vector::Copy(&m_StartPos, _GetMuzzlePos());
	i3Vector::Copy(&at, _GetMuzzleDir());
	i3Vector::Scale(&at, &at, GetRange());
	i3Vector::Add(&m_TargetPos, &at, &m_StartPos);
	i3Vector::Copy(&m_bulletTraceStartPos, _GetMuzzlePos());
	i3Vector::Copy(&at, _GetMuzzleDir());
	i3Vector::ScaleAdd(&m_bulletTraceStartPos, &at, -2.5f);
	i3Vector::Set(&up, 0.0f, 1.0f, 0.0f);
	i3Vector::ScaleAdd(&m_bulletTraceStartPos, &up, -1.0f);

	//SetDeviation();

	NET_Fire(&m_StartPos, &m_TargetPos, m_FireOrder);
}

VEC3D* WeaponSentryGun::_GetMuzzleDir()
{
	VEC3D* muzzleDir = i3Matrix::GetRight(getFXMatrix());
	muzzleDir->x *= -1.0f;
	muzzleDir->y *= -1.0f;
	muzzleDir->z *= -1.0f;
	i3Vector::Normalize(muzzleDir, muzzleDir);

	return muzzleDir;
}

VEC3D* WeaponSentryGun::_GetMuzzlePos()
{
	return i3Matrix::GetPos(getFXMatrix());
}

void WeaponSentryGun::ReturnToReady()
{
}

void WeaponSentryGun::Install(i3Node* sceneNode, i3GameNode* owner, VEC3D* installPosition)
{
	if(getOwner() == NULL) return;
	
	MATRIX matRot;

	i3Matrix::Identity(&matRot);
	i3Matrix::SetRotateAxisDeg(&matRot, &I3_XAXIS, -90.0f);

	SetMatrix(&matRot);

	VEC3D sentryGunPos = {installPosition->x, installPosition->y, installPosition->z};
	WeaponBase::Transmission(sceneNode, owner, &sentryGunPos, TRUE);

	m_isInstalled = TRUE;
}

void WeaponSentryGun::Uninstall()
{
	m_isInstalled = FALSE;
}

void WeaponSentryGun::Destroy()
{
}

void WeaponSentryGun::RotateAxisX(ROTATE_AXIS_X rotDir, REAL32 degreeX, REAL32 speed)
{
	MATRIX* mat = GetMatrix();
	VEC3D pos = { i3Matrix::GetPos(mat)->x, i3Matrix::GetPos(mat)->y, i3Matrix::GetPos(mat)->z };

	i3Matrix::SetPos(mat, 0.0f, 0.0f, 0.0f);

	if (rotDir == ROTATE_UP)
		degreeX *= -1.0f;

	i3Matrix::PostRotateAxisDeg(mat, i3Matrix::GetRight(mat), degreeX*speed*g_pViewer->getDeltaTime());
	i3Matrix::SetPos(mat, pos.x, pos.y, pos.z);
}

void WeaponSentryGun::RotateAxisY(ROTATE_AXIS_Y rotDir, REAL32 degreeY, REAL32 speed)
{
	MATRIX* mat = GetMatrix();
	VEC3D pos = { i3Matrix::GetPos(mat)->x, i3Matrix::GetPos(mat)->y, i3Matrix::GetPos(mat)->z };

	i3Matrix::SetPos(mat, 0.0f, 0.0f, 0.0f);

	if (rotDir == ROTATE_RIGHT)
		degreeY *= -1.0f;

	i3Matrix::PostRotateAxisDeg(mat, &I3_YAXIS, degreeY*speed*g_pViewer->getDeltaTime());
	i3Matrix::SetPos(mat, pos.x, pos.y, pos.z);
}

void WeaponSentryGun::OnLostDevice(bool isDeviceLost)
{
}

void WeaponSentryGun::OnRevive(i3RenderContext* ctx)
{
}