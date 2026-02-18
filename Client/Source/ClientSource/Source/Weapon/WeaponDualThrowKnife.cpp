#include "pch.h"
#include "WeaponDualThrowKnife.h"

#include "GameCharaAnimContext.h"
#include "MainWeapon_DualThrowKnife.h"

#include "MeleeWeaponInfo.h"

I3_CLASS_INSTANCE( WeaponDualThrowKnife);//, WeaponDualKnife);

MainWeapon* WeaponDualThrowKnife::CreateMainWeapon()
{
	MainWeapon_DualThrowKnife * p = new MainWeapon_DualThrowKnife(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualThrowKnife));
	return p;
}

WeaponDualThrowKnife::WeaponDualThrowKnife() : m_iAttackSoundPlayCount(0), m_DoThrowKnife(false)
{

}

WeaponDualThrowKnife::~WeaponDualThrowKnife()
{

}

void WeaponDualThrowKnife::OnFire( i3AIContext * pCtx, REAL32 tm )
{
	// 메인이 되는 오른쪽만 처리하고 더미로 사용되는 왼쪽은 처리할 필요 없다.
	MainWeapon_DualThrowKnife* mainWeapon = static_cast<MainWeapon_DualThrowKnife*>(GetMainWeapon());

	if( mainWeapon->GetHandType() == WEAPON_DUAL_HANDTYPE_LEFT)
		return;		

	if(m_FireOrder == WEAPON::PRIMARY_FIRE)
	{
		// 사운드 처리가 따로 필요함.
		INT32 nCollisionCount = 1;
		REAL32 fAttackTiming = 0.f;
		INT32 autoFireCount = getAutoFireCount();

		nCollisionCount = m_pWeaponInfo->GetAutoFireCount();
		fAttackTiming = m_pWeaponInfo->GetAttackTimeA(autoFireCount);

		if (m_iAttackSoundPlayCount < 2 && pCtx->getLocalTime() > fAttackTiming )
		{
			m_iAttackSoundPlayCount++;
			_OnPlayFireSound();
		}

		// 사운드 처리 우선
		WeaponDualKnife::OnFire(pCtx, tm);
	}
	else
	{
		if (m_iAttackSoundPlayCount < 1)
		{
			m_iAttackSoundPlayCount++;
			_PlayFireSound(WEAPON::FIRE_SOUND_NORMAL_2);
		}
		WeaponDualThrowKnife::OnSecondaryFire(pCtx, tm);
	}
}

// 여기서 시간을 계산해서 Net_Throw을 하면 됩니다.
void WeaponDualThrowKnife::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm )
{
	// 여기에 들어올 때는 이미 Left는 거르고 들어온 경우임
	INT32 autoFireCount = getAutoFireCount();
	INT32 nCollisionCount = 1;
	REAL32 fAttackTiming = 0.f;
	REAL32 fAttackDelay = m_pOwner->getAnimContext()->getUpperDuration();

	nCollisionCount = m_pWeaponInfo->GetAutoFireCount2();
	fAttackTiming = m_pWeaponInfo->GetAttackTimeB( autoFireCount);

	//	공격 시작부터 타격 시간이 되면 조준점까지 한번 컬리전 한다.
	if( (autoFireCount < nCollisionCount) && pCtx->getLocalTime() > fAttackTiming )
	{
		// 네트워크는 패킷으로 처리 합니다.
		if (m_pOwner->isLocal())
		{
			VEC3D target;
			GetShotTargetPos(&target);

			NET_Throw(nullptr, &target, -1);

			autoFireCount++;

			// 여기서 던졌으면 무조건 다음 타를 해야 합니다.
			m_DoThrowKnife = true;
		}
	}

	setAutoFireCount( autoFireCount);

	// 공격 지연시간이 충분히 지났다면...Ready 상태로
	if( pCtx->getLocalTime() + tm > fAttackDelay )
	{
		m_DoThrowKnife = false;
		ReturnToReady();
	}
}

void WeaponDualThrowKnife::NET_Fire(VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	m_iAttackSoundPlayCount = 0;

	if (type == WEAPON::PRIMARY_FIRE)
		setAutoFireCount(0);

	WeaponKnuckle::NET_Fire(pStartPos, pTargetPos, type);
}

// 실제 Throw
void WeaponDualThrowKnife::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx )
{
	MainWeapon_DualThrowKnife* mainWeapon = static_cast<MainWeapon_DualThrowKnife*>(GetMainWeapon());
	mainWeapon->NET_Throw(pStartPos, pTargetPos, nThrowNetIdx);
}

bool WeaponDualThrowKnife::Fire( WEAPON::FIRE_ORDER type /*= WEAPON::PRIMARY_FIRE*/ )
{
	SetFireOrder(type);

	// 1발 남은 상태에서 투척 불가능.
	if( (getLoadedBulletCount() <= 2) && 
		(getFireOrder() == WEAPON::SECONDARY_FIRE) && 
		((CMeleeWeaponInfo*)getBaseWeaponInfo())->DoNotThrowLast())
	{
		return false;
	}

	return WeaponDualKnife::Fire(type);
}

void WeaponDualThrowKnife::Reset(void)
{
	WeaponBase::Reset();

	m_DoThrowKnife = false;
}
