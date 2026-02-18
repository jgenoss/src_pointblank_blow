#include "pch.h"
#include "WeaponMeatBomb.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "../Mode/Domination/DomiUserSkillMgr.h"

#include "../StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"

#include "WeaponFunction.h"

#include "MainWeapon_MeatBomb.h"


// 기획 문서 참조 : http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3341

I3_CLASS_INSTANCE(WeaponMeatBomb);// , WeaponGrenade) 



WeaponMeatBomb::WeaponMeatBomb(void)
{


}

WeaponMeatBomb::~WeaponMeatBomb(void)
{
	
}

void WeaponMeatBomb::Reset()
{
	WeaponGrenade::Reset();

}

MainWeapon*	WeaponMeatBomb::CreateMainWeapon()
{
	MainWeapon_MeatBomb * p = new MainWeapon_MeatBomb(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_MeatBomb));
	return p;
}

void WeaponMeatBomb::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);
}

void WeaponMeatBomb::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NormalWeapon(this);
	i3Scene::ModifyDynamicState(getSceneObject(), I3_PHYSIX_STATE_DISABLE, 0);
}

/*virtual*/ void WeaponMeatBomb::SetHitCollideeGroup(void)
{
	WF::SetCollideeGroup_HitWeapon(this);
}

void WeaponMeatBomb::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx )
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
		//i3Vector::Sub( &dir, pTargetPos, &start);
		//i3Vector::Normalize( &dir, &dir);
		dir.x = 0.0f; dir.y = -1.0f; dir.z = 0.0f;
		NET_FirstThrow( &start, &dir, nNetSlotIdx );
	}
}

