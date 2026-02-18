#include "pch.h"
#include "MainWeapon_DualThrowKnife.h"

#include "GameCharaCameraContext.h"
#include "MainWeapon_ThrowKnife.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"

MainWeapon_DualThrowKnife::MainWeapon_DualThrowKnife( WeaponBase* p ) : MainWeapon_Knuckle(p)
{

}

void MainWeapon_DualThrowKnife::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx )
{
	m_pWeaponBase->WeaponBase::NET_Throw( pStartPos, pTargetPos, nThrowNetIdx );  // 한정자 떼면 안된다..

	VEC3D start, dir;

	if( pStartPos == nullptr )
	{	
		m_pWeaponBase->getOwner()->getCameraContext()->getVirtualCamPosition( &start);					// 현재는 Camera의 지점
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}

	// 던져집니다.
	m_pWeaponBase->NET_FireSync( pTargetPos );

	i3Vector::Sub( &dir, pTargetPos, &start);
	i3Vector::Normalize( &dir, &dir);

	_Throw(&start, &dir, m_pWeaponBase->getWeaponInfo()->GetThrowSpeed(), nThrowNetIdx);

#ifdef ENABLE_TEST_COMMAND
	if( !g_pWeaponManager->COMMAND_IsUnlimitGrenade())		//	Use Cheat
#endif
	{
		INT32 iBullet = m_pWeaponBase->getLoadedBulletCount();

		iBullet--;
		if( iBullet < 0)
			iBullet = 0;

		//GlobalFunc::PB_TRACE("NET_FirstThrow iBullet %d", iBullet);
		m_pWeaponBase->setLoadedBulletCount( iBullet);
	}

	//m_pWeaponBase->_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL_2);
}

void MainWeapon_DualThrowKnife::_Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx )
{
	WEAPON::RES_ID weaponNum = m_pWeaponBase->getWeaponInfo()->GetRealNumber();

	for( UINT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( m_pWeaponBase->getWeaponInfo()->getExtensionType(i) == WEAPON::EXT_THROW)
		{
			weaponNum = WEAPON::ItemID2ResID( m_pWeaponBase->getWeaponInfo()->GetExtensionItemID(i) );
			break;
		}
	}

	/*if( m_ThrowItemID > 0)
		weaponNum = WEAPON::ItemID2ResID( m_ThrowItemID);*/

	WeaponThrowKnife * pNewWeapon = static_cast<WeaponThrowKnife*>(g_pWeaponManager->GetFreeThrowWeapon( WEAPON_CLASS_KNIFE, weaponNum));

	if( pNewWeapon == nullptr )
	{
		// 인스턴스가 없습니다..?
		//I3PRINTLOG(I3LOG_FATAL,  "no instance resource." );
		I3ASSERT( pNewWeapon != nullptr );
		return;
	}

	MainWeapon_ThrowKnife* mainWeapon_TK = static_cast<MainWeapon_ThrowKnife*>(pNewWeapon->GetMainWeapon());

	pNewWeapon->SetOwner( m_pWeaponBase->getOwner());	// 먼저 세팅해야됩니다.	
	pNewWeapon->Reset();
	pNewWeapon->setPairWeapon(m_pWeaponBase);
	mainWeapon_TK->Throw( vStart, vDir, rSpeed, nNetSlotIdx);
	pNewWeapon->SetFireOrder( WEAPON::SECONDARY_FIRE);

	VEC3D vDirection;
	i3Vector::Set( &vDirection, getX( mainWeapon_TK->getDirection()), 0.f, getZ( mainWeapon_TK->getDirection()));
	i3Vector::Normalize( &vDirection, &vDirection);

	// Kunai 회전
	{
		VEC3D vNormal;
		REAL32 rDotX = i3Vector::Dot(&vDirection, &I3_YAXIS);

		i3Matrix::SetRotateZ(pNewWeapon->GetMatrix(), I3_PI);
		i3Matrix::PostRotateX(pNewWeapon->GetMatrix(), -(1.f - rDotX) * I3_PI2);

		i3Vector::Set(&vNormal, getX(&vDirection), 0.f, getZ(&vDirection));
		i3Vector::Normalize(&vNormal, &vNormal);
		REAL32 fTheta = i3Math::atan2(-getX(&vNormal), -getZ(&vNormal));
		i3Matrix::PostRotateY(pNewWeapon->GetMatrix(), fTheta);
	}

	// Network ID
	pNewWeapon->setNetworkIdx( static_cast<INT16>(nNetSlotIdx) );
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pWeaponBase->getOwner()->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}
	else if( nNetSlotIdx < MAX_THROWWEAPON_COUNT)
	{
		GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( nNetSlotIdx );
		pGrenadeInfo->_tNetData.SetDir( *(mainWeapon_TK->getDirection()) );
	}

	VEC3D	vTemp;
	m_pWeaponBase->getOwner()->getCameraContext()->getVirtualCamPosition( &vTemp );

	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, &vTemp );

	if( m_pWeaponBase->IsDedicatedHost() == false )
	{	
		pNewWeapon->SetPos( &vTemp);
	}
}
