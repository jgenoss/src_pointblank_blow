#include "pch.h"
#include "WeaponKnuckle.h"
#include "GameCharaBase.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "WeaponKnife.h"
#include "WeaponFunction.h"

#include "MainWeapon_Knuckle.h"


I3_CLASS_INSTANCE( WeaponKnuckle);//, WeaponDualKnife);

MainWeapon*	WeaponKnuckle::CreateMainWeapon()
{
	MainWeapon_Knuckle * p = new MainWeapon_Knuckle(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_Knuckle));
	return p;
}

WeaponKnuckle::WeaponKnuckle()
{

}

WeaponKnuckle::~WeaponKnuckle()
{
}


/*virtual*/ void WeaponKnuckle::ReturnInstance()
{
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());

	if( mainWeapon->getLeftWeapon() )	
	{
		mainWeapon->getLeftWeapon()->getAttachNode()->RemoveAllChild();

		mainWeapon->getLeftWeapon()->RemoveAllChild();	

		mainWeapon->getLeftWeapon()->DetachNode();		//	GameNode , SceneNode 제거

		I3_MUST_RELEASE(mainWeapon->getLeftWeapon());
		
		mainWeapon->setLeftWeapon_Null();
//		I3_SAFE_RELEASE(m_pLeftWeapon);
	}

	WeaponBase::ReturnInstance();			// WeaponDualKnife의 함수를 호출하거나, 아예 없으면 좋은데.....어설프게 중간 클래스 함수를 호출함...
}

/*virtual*/void WeaponKnuckle::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

/*virtual*/void WeaponKnuckle::OnBindResource( void)
{
	WeaponDualKnife::OnBindResource();
}

//virtual
/* 
void WeaponKnuckle::OnPlayChangeAniImpl( REAL32 rTimeScale )  //= 1.f)
{
	// 하위에서 호출해야 합니다.
	WeaponDualKnife::OnPlayChangeAniImpl( rTimeScale);
}
*/

/*virtual*/void WeaponKnuckle::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());
	mainWeapon->NET_Fire_Knuckle(pStartPos, pTargetPos, type);
}

void WeaponKnuckle::OnUpdate(REAL32 rDeltaSeconds)
{
	WeaponBase::OnUpdate(rDeltaSeconds);

	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());
	mainWeapon->OnUpdate(rDeltaSeconds);
}

/*virtual*/ void WeaponKnuckle::_OnPlayWeaponIdleAnim(void)
{
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());
	mainWeapon->PlayIdleAnim();
}

/*virtual*/ void WeaponKnuckle::_OnPlayWeaponFireAnim( void )
{
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());
	mainWeapon->PlayFireAnim();
}

void WeaponKnuckle::PlayAnim_UI_IdleA( void)
{
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());
	mainWeapon->PlayUIAnim();
}

void WeaponKnuckle::PlayAnim_UI_IdleB( void)
{
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());
	mainWeapon->PlayUIAnim();
}

void WeaponKnuckle::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	// 메인이 되는 오른쪽만 처리하고 더미로 사용되는 왼쪽은 처리할 필요 없다.
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());

	if( mainWeapon->GetHandType() == WEAPON_DUAL_HANDTYPE_LEFT)
		return;		

	WeaponDualKnife::OnFire( pCtx, tm);
}

void WeaponKnuckle::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	// 메인이 되는 오른쪽만 처리하고 더미로 사용되는 왼쪽은 처리할 필요 없다.
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());

	if( mainWeapon->GetHandType() == WEAPON_DUAL_HANDTYPE_LEFT)
		return;		

	WeaponDualKnife::OnSecondaryFire( pCtx, tm);
}

void WeaponKnuckle::_OnProcessFireEndAnim( REAL32 rDelay)
{	
	// 메인이 되는 오른쪽만 처리하고 더미로 사용되는 왼쪽은 처리할 필요 없다.
	MainWeapon_Knuckle* mainWeapon = static_cast<MainWeapon_Knuckle*>(GetMainWeapon());

	if( mainWeapon->GetHandType() == WEAPON_DUAL_HANDTYPE_LEFT)
		return;

	if( (m_pOwner != nullptr) && (m_pOwner->getAnimContext()->getAIUpper() != nullptr) )
	{ 
		i3AIState * pAIState = m_pOwner->getAnimContext()->getAIUpper()->getCurrentAIState();
		if( pAIState != nullptr )
		{
			if( rDelay > pAIState->getActualDuration() )
				ReturnToReady();
			else
				setWeaponState( WEAPON::STATE_FIRING);

			return;
		}
	}
	
	ReturnToReady();
}


