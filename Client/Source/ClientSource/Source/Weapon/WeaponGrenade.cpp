#include "pch.h"
#include "WeaponGrenade.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaBoneContext.h"
#include "WeaponGlobal.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GlobalFunc.h"
#include "TutorialMode.h"
#include "GameMode.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "HackContext.h"
#include "GameContextUtil.h"
#include "WeaponFunction.h"
#include "MainWeapon_Grenade.h"



I3_CLASS_INSTANCE( WeaponGrenade);//, WeaponBase);




WeaponGrenade::WeaponGrenade(void) 
{

}

WeaponGrenade::~WeaponGrenade(void)
{	
}

void WeaponGrenade::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

void WeaponGrenade::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);
}

MainWeapon*	WeaponGrenade::CreateMainWeapon()
{
	MainWeapon_Grenade * p = new MainWeapon_Grenade(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_Grenade));
	return p;
}

void WeaponGrenade::Reset()
{
	WeaponBase::Reset();		//	먼저 해야함.


}


void WeaponGrenade::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<MainWeapon_Grenade*>(GetMainWeapon())->OnFire_Grenade(pCtx, tm);
}



void WeaponGrenade::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx)
{
	((MainWeapon_Grenade*)m_mainWeapon.get() )->Throw(vStart, vDir, rSpeed, bOrigin, nNetSlotIdx);
}

void WeaponGrenade::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire( pCtx, tm);
}


void WeaponGrenade::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_LOADBULLET_DISABLE);

	if( getLoadedBulletCount() <= 0)
	{
		setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

		if( getOwner() != nullptr && getOwner()->isLocal())
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}
		SetEnable( false);
	}
	else
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE);
		if( getOwner() != nullptr && getOwner()->isLocal())
		{
			addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}

		setWeaponState( WEAPON::STATE_READY);
	}
}

void WeaponGrenade::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade* pMainWeapon = static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());

	pMainWeapon->CheckWorldCollision(rDeltaSeconds);
	// 이 함수 호출은 임시이다..
}

void WeaponGrenade::Explosion()
{
	MainWeapon_Grenade* pMainWeapon = static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());
	
	pMainWeapon->Explosion();
// 이 함수 호출은 임시이다..
}



 
void WeaponGrenade::OnUpdate( REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade* mainWeapon = static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());
	
	mainWeapon->OnUpdate_Grenade(rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
}

bool WeaponGrenade::Fire( WEAPON::FIRE_ORDER type)
{
	if( getLoadedBulletCount() <= 0)
		return false;

	m_FireOrder = type;

	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

 	NET_Fire( nullptr, nullptr, type);

	addWeaponStateMask( WSM_NEED_SEND_FIREACTION);

	if(m_pOwner->getActionContext()->isLower(CHARA_LOWER_JUMP))
	{
		I3TRACE("Throw Grenade Once Check.\n");
 		setSkillOnce_Jump(true);
		setSkillOnce_Jump(true);
	}

	return true;
}

void WeaponGrenade::FireReady( void)
{	//	발사 준비 상태
	WF::PlayWeaponAnim( this, getAIIndex(WEAPON::AI_LOADBULLET));	// revision 46882 캔디수류탄 애니메이션 추가

	WeaponBase::FireReady();
}

void WeaponGrenade::FireEnd(void)
{	
}

void WeaponGrenade::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	setWeaponState( WEAPON::STATE_FIRE);

	WeaponBase::NET_Fire( nullptr, nullptr, type );
}

void WeaponGrenade::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx )
{
	WeaponBase::NET_Throw( pStartPos, pTargetPos, nNetSlotIdx );

	VEC3D start, dir;
	
	if( pStartPos == nullptr )
	{	
		m_pOwner->getCameraContext()->getVirtualCamPosition( &start);					// 현재는 Camera의 지점
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}

	// 수류탄이 던져집니다.
	NET_FireSync( pTargetPos );

	//다른캐릭터나 NoneServer일경우만 처리했으나
	//내 캐릭터도 바로 처리한다.
	if( getOwner() != nullptr )
	{
		i3Vector::Sub( &dir, pTargetPos, &start);
		i3Vector::Normalize( &dir, &dir);

		NET_FirstThrow( &start, &dir, nNetSlotIdx );
	}
}




void WeaponGrenade::ReturnInstance()
{
	WeaponBase::ReturnInstance();
}



void WeaponGrenade::SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo)
{
	MainWeapon_Grenade* mainWeapon = 
		static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());
	mainWeapon->SetReceiveStates(pGrenadeInfo, pNetInfo);
}


void WeaponGrenade::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	WeaponBase::NET_Bound( pStartPos, pDir, fSpeed, nBoundType );

	MainWeapon_Grenade* mainWeapon = static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());
	mainWeapon->NET_Bound(pStartPos, pDir, fSpeed, nBoundType);
}



void WeaponGrenade::NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx )
{
	I3ASSERT( m_pOwner != nullptr);

	REAL32 rThrowSpeed = 0.f;
	if (m_FireOrder == WEAPON::SECONDARY_FIRE)
	{
		rThrowSpeed = m_pOwner->GetExThrowSpeed(this->getWeaponInfo()->GetTypeUsage());
	}
	else
	{
		rThrowSpeed = m_pOwner->GetThrowSpeed(this->getWeaponInfo()->GetTypeUsage());
	}

	Throw( pStartPos, pDir, rThrowSpeed, true, nNetSlotIdx);

	PB_VM_S
#ifdef ENABLE_TEST_COMMAND
	if( !g_pWeaponManager->COMMAND_IsUnlimitGrenade())		//	Use Cheat
#endif
	{
		setLoadedBulletCount( i3Math::softAdd( getLoadedBulletCount(), -m_pWeaponInfo->GetFireDecBulletCount()));
	}
	PB_VM_E

	_PlayFireSound();

	WeaponBase::NET_FirstThrow( pStartPos, pDir, nNetSlotIdx );
}

/*virtual*/ void WeaponGrenade::ApplyBulletWithDropItem(REAL32 rRate)
{
	INT32 max_bullet = m_pWeaponInfo->GetMaxBullet();	//최대 수.
	INT32 increase = (INT32)(max_bullet * rRate);		//증가 수.
	INT32 current = getTotalBulletCount();				//현재 수.

	increase += current;
	if( increase > GetMaxBulletWithCashItem())
		increase = GetMaxBulletWithCashItem();

	setLoadedBulletCount( increase);					//수류탄은 바로 장전 시킨다.
}
