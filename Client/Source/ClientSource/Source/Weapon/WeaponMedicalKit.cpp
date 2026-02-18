#include "pch.h"
#include "WeaponMedicalKit.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "WeaponFunction.h"
#include "MainWeapon_MedicalKit.h"

I3_CLASS_INSTANCE(WeaponMedicalKit);

WeaponMedicalKit::WeaponMedicalKit(void)
{
	
}

WeaponMedicalKit::~WeaponMedicalKit(void)
{
}

void WeaponMedicalKit::Reset()
{
	WeaponGrenade::Reset();

}

void WeaponMedicalKit::OnUpdate(REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade* mainWeapon = static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());
	mainWeapon->OnUpdate_Grenade(rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
}

void WeaponMedicalKit::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx )
{
	//WeaponGrenade::NET_Throw( pStartPos, pTargetPos, nNetSlotIdx );

	WeaponBase::NET_Throw( pStartPos, pTargetPos, nNetSlotIdx );

	VEC3D start, dir;
	
	if( pStartPos == nullptr )
	{	
		VEC3D	vAt;
		VEC3D vPos;
		VEC3D offset;
		
		m_pOwner->getDBInfo()->GetFPSCamPos( &offset);
		if( m_pOwner->isLeftHand())	offset.x = -offset.x;

		if( m_pOwner->is1PV())
		{
			i3Vector::Copy( &start, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_NECK)));
		}
		else
		{
			i3Vector::Copy( &start, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_IK)));
			i3Vector::SetY( &start, getY( m_pOwner->GetPos()) + m_pOwner->getCameraContext()->getIKHeightForCam());
		}

		//i3Vector::Add( &start, &vAt, &vPos);
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


MainWeapon*	WeaponMedicalKit::CreateMainWeapon()
{
	MainWeapon_MedicalKit * p = new MainWeapon_MedicalKit(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_MedicalKit));
	return p;
}
