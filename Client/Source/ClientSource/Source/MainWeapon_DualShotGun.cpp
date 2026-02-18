#include "pch.h"
#include "MainWeapon_DualShotGun.h"
#include "WeaponFunction.h"

MainWeapon_DualShotGun::MainWeapon_DualShotGun(WeaponBase* p) : MainWeapon_Dual(p)
{
	m_ui8FireHand = DUAL_FIRE;	
	m_ui8FireState = STATE_READY;
	m_ui8NextFireHand = RIGHT_FIRE;
}

void MainWeapon_DualShotGun::_ChangeFireHand(WEAPON::FIRE_ORDER type)
{
	if( m_pLeftHandWeapon )
	{
		if( type == WEAPON::PRIMARY_FIRE )
		{
			m_ui8FireHand = DUAL_FIRE;
		}
		else if( type == WEAPON::SECONDARY_FIRE )
		{
			m_ui8FireHand = m_ui8NextFireHand;

			if( m_ui8NextFireHand & LEFT_FIRE )
				m_ui8NextFireHand = RIGHT_FIRE;
			else
				m_ui8NextFireHand = LEFT_FIRE;
		}

		setFireState(STATE_FIRING);
	}
}

/*virtual*/ void MainWeapon_DualShotGun::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Dual::OnCreate( bCreateInWeapon);

	m_ui8FireHand = DUAL_FIRE;
	m_ui8NextFireHand = RIGHT_FIRE;
	m_ui8FireState = STATE_READY;
}

/*virtual*/ void MainWeapon_DualShotGun::ReturnInstance() // MainWeapon_Dual::ReturnInstance()를 간접호출하지 않는다... (원래가 그렇게 구현되었던 상태이며 나중에 어떻게든 고칠것..)
{
	if( m_pLeftHandWeapon)	
	{
		m_pLeftHandWeapon->setPairWeapon( nullptr);
		g_pWeaponManager->ReleaseCharaWeapon( m_pLeftHandWeapon);
		m_pLeftHandWeapon = nullptr;
	}
}

/*virtual*/ void MainWeapon_DualShotGun::Reset()
{
	MainWeapon_Dual::Reset();
	m_ui8FireHand = DUAL_FIRE;
	m_ui8NextFireHand = RIGHT_FIRE;
	m_ui8FireState = STATE_READY;

	if( m_pLeftHandWeapon )
	{
		m_pLeftHandWeapon->Reset();
		m_pLeftHandWeapon->setLoadedBulletCount( m_pWeaponBase->getLoadedBulletCount() / 2);
	}
}

/*virtual*/ void MainWeapon_DualShotGun::ReturnToReady()
{
	MainWeapon_Dual::ReturnToReady();
	m_ui8FireHand = DUAL_FIRE;
	m_ui8NextFireHand = RIGHT_FIRE;
	m_ui8FireState = STATE_READY;
}

bool MainWeapon_DualShotGun::Fire_DualShotGun(WEAPON::FIRE_ORDER type /* = WEAPON::PRIMARY_FIRE */ )
{
	if( !isFireState(STATE_READY) )
		return false;

	INT32 bullet_count = m_pWeaponBase->getLoadedBulletCount();
	INT32 dec_bullet_count = m_pWeaponBase->getWeaponInfo()->GetFireDecBulletCount();

	if( bullet_count <= 0)	
		return false;

	if( bullet_count == dec_bullet_count )
		type = WEAPON::SECONDARY_FIRE;

	_ChangeFireHand(type);

	if( _isFireHand(RIGHT_FIRE) )
	{
		m_pWeaponBase->SetFireOrder(type);

		// 시점이 향하고 있는 목표 지점을 구한다.
		m_pWeaponBase->GetShotTargetPos( m_pWeaponBase->GetTargetPos() );
		m_pWeaponBase->NET_Fire( nullptr, m_pWeaponBase->GetTargetPos(), m_pWeaponBase->getFireOrder() );
	}

	if( m_pLeftHandWeapon && _isFireHand(LEFT_FIRE) )
	{
		// lefthand
		if( m_pWeaponBase->getOwner()->isLocal() )
		{
			m_pWeaponBase->addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
			m_pWeaponBase->removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);
		}

		m_pLeftHandWeapon->Fire(type);
	}

	return true;
}

void MainWeapon_DualShotGun::FireEnd_DualShotGun(void)
{
	if( isFireState(MainWeapon_DualShotGun::STATE_END) )
		setFireState(MainWeapon_DualShotGun::STATE_READY);

	if( m_pLeftHandWeapon && _isFireHand(LEFT_FIRE) )
		m_pLeftHandWeapon->FireEnd();
}

void MainWeapon_DualShotGun::NET_Fire_DualShotGun( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	// 네트워크의 경우에는 여기서 FireHand를 결정.
	if( m_pWeaponBase->getOwner()->isNetwork() )
		_ChangeFireHand(type);

	if( _isFireHand(RIGHT_FIRE) )
		m_pWeaponBase->WeaponBase::NET_Fire( pStartPos, pTargetPos, type);

	if( m_pLeftHandWeapon && _isFireHand(LEFT_FIRE) )
		m_pLeftHandWeapon->NET_Fire( pStartPos, pTargetPos, type);
}

void MainWeapon_DualShotGun::PlayAnim_Change_DualShotGun(REAL32 rTimeScale /* = 1.f */)
{
	WF::PlayWeaponAnim(m_pWeaponBase, m_pWeaponBase->getAIIndex(WEAPON::AI_CHANGEDUAL_RIGHT), rTimeScale);
	WF::PlayWeaponAnim(m_pLeftHandWeapon, m_pWeaponBase->getAIIndex(WEAPON::AI_CHANGEDUAL_LEFT), rTimeScale);
}

const REAL32 MainWeapon_DualShotGun::GetDualShotGunTimeScale()
{
	REAL32 rTimeScale = 1.f;
	INT8 AIIndex = m_pWeaponBase->getAIIndex( WEAPON::AI_FIRE);

	if( AIIndex != -1)
	{
		i3AIState * pAIState = m_pWeaponBase->GetAIContext()->getAIState( AIIndex );
		I3ASSERT( pAIState != nullptr);
		REAL32 rDelay = m_pWeaponBase->GetFireDelay();
		I3ASSERT( rDelay != 0.f);
		rTimeScale = pAIState->getDuration() / rDelay;
	}

	return rTimeScale;
}
