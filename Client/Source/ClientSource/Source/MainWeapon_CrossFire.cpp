#include "pch.h"

#include "MainWeapon_CrossFire.h"

#include "GameCharaEquipContext.h"
#include "BattleSlotContext.h"
#include "Designation/DesigApplyInfo.h"

MainWeapon_CrossFire::MainWeapon_CrossFire(WeaponBase* p) : MainWeapon_DualHandGun(p)
{
	m_bDualFired	= false;
}

bool	MainWeapon_CrossFire::Fire_CrossFire( WEAPON::FIRE_ORDER type)
{
	if( !m_pWeaponBase->isExtensionActivate() )
	{
		return Fire_DualHandGun( type );
	}
	
	// righthand
	if( m_pWeaponBase->getLoadedBulletCount() <= 0)	return false;

	m_pWeaponBase->SetFireOrder(type);

	m_pWeaponBase->GetShotTargetPos( m_pWeaponBase->GetTargetPos() );	// 시점이 향하고 있는 목표 지점을 구한다.

	NET_Fire_CrossFire( nullptr, m_pWeaponBase->GetTargetPos(), m_pWeaponBase->getFireOrder() );
	
	// lefthand
	if( m_pWeaponBase->getOwner()->isLocal() )
	{
		m_pWeaponBase->addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
		m_pWeaponBase->removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);
	}

	if( m_pLeftHandWeapon )	m_pLeftHandWeapon->Fire(WEAPON::PRIMARY_FIRE);
	
	return true;

}

void	MainWeapon_CrossFire::NET_Fire_CrossFire( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	if( !m_pWeaponBase->isExtensionActivate() )
	{
		MainWeapon_DualHandGun::NET_Fire_DualHandGun( pStartPos, pTargetPos, type);
		return;
	}

	// revision 29646
	m_bDualFired	= false;

	INT32 iBullet = m_pWeaponBase->getLoadedBulletCount();

	if( iBullet > 1 )		// 총알 한발남았을때 왼쪽만 발사!!
	{
		m_pWeaponBase->WeaponBase::NET_Fire( pStartPos, pTargetPos, type);		// 한정자를 떼면 안된다..(비가상동작이어야함)
		m_bDualFired	= true;
	}
	
	if( m_pLeftHandWeapon)
		m_pLeftHandWeapon->NET_Fire( pStartPos, pTargetPos, type);

	iBullet--;
	m_pWeaponBase->setLoadedBulletCount( iBullet);
	
}


REAL32	MainWeapon_CrossFire::GetRange_CrossFire(void)
{
	if( !m_pWeaponBase->isExtensionActivate() )
		return m_pWeaponBase->WeaponBase::GetRange();		// 한정자를 떼면 안된다..(비가상동작이어야함)

	REAL32		rVal = m_pWeaponBase->getWeaponInfo()->GetExtRange();

	// 아예 장비속성/스킬/호칭 적용이 전부 빠져있길래 삽입한다...
	// 아래는 WeaponBase::GetRange에서 어느 정도 복붙된 코드이다...( 시간이 되면 GetRange()/GetExtRange()관련되어 코드정리가 필요함)
	// 이것이 의도치 않은 동작일수도 있으므로 문제가 된다면 제거해야한다..그러나, 12월은 호칭정보가 반영된 상태임..
	// 2015.05.22.수빈...
	if (m_pWeaponBase->getOwner() )
	{
		CGameCharaBase* pOwner = m_pWeaponBase->getOwner();

		if ( pOwner->getEquipContext() )
			rVal += (rVal * pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_RANGE));

		// 스킬 속성.
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( pOwner->getCharaNetIndex(), pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(m_pWeaponBase->getWeaponInfo()->GetTypeClass(), SKILLLIST_RANGE);
		rVal += ratio;

		// 호칭 적용
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info)
			rVal += desig_apply_info->desigWeapon.weaponAttackRange;
	}

	return rVal;
}

REAL32	MainWeapon_CrossFire::GetFireDelay_CrossFire(void)
{
	if( !m_pWeaponBase->isExtensionActivate() )
		return m_pWeaponBase->WeaponBase::GetFireDelay();	// 한정자를 떼면 안된다..(비가상동작이어야함)

	REAL32 rVal = m_pWeaponBase->getWeaponInfo()->GetExtFireDelay();
	
	if ( m_pWeaponBase->getOwner() )
	{
		m_pWeaponBase->_CalcFireDelayToVersion(rVal);
	}

	return rVal;
}

