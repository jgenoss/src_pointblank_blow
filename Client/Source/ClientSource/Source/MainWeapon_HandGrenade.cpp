#include "pch.h"

#include "MainWeapon_HandGrenade.h"

#include "HackContext.h"
#include "MainWeapon_GrenadeShell.h"

#include "BattleSlotContext.h"
#include "MyRoomInfoContext.h"

#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "i3Base/string/ext/utf16_to_mb.h"


MainWeapon_HandGrenade::MainWeapon_HandGrenade(WeaponBase* p) : MainWeapon_Throw(p) 
{
	m_pAttachObjectTransform	= nullptr;
	m_pAttachObject				= nullptr;
	m_rBoundObjectLen			= 0.f;

	m_uCount = (i3Math::Rand() & 0x0000FFFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x00000FFF) << 20;

	m_bExplosion = 0;
	m_iAttackSoundPlayCount = 0;
	m_idxFxdummy = -1;
	m_idxShellFireEffect = -1;
}

void	MainWeapon_HandGrenade::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Throw::OnCreate( bCreateInWeapon);

	// 발사 이펙트
	if( m_pWeaponBase->getWeaponInfo()->hasFireEffectExt())
	{
		i3::rc_wstring wstrFireEffectExt;
		m_pWeaponBase->getWeaponInfo()->GetFireEffectExt(wstrFireEffectExt);
		i3::string strFireEffectExt;	i3::utf16_to_mb(wstrFireEffectExt, strFireEffectExt);
		m_idxShellFireEffect = g_pEffectManager->FindEffect( strFireEffectExt.c_str());
	}

}

void	MainWeapon_HandGrenade::Reset()
{
	MainWeapon_Throw::Reset();

	m_uCount = (i3Math::Rand() & 0x0000FFFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x00000FFF) << 20;
}

namespace
{
	void GrenadeHackDetection(UINT32 curCnt)
	{
		UINT32 ra = i3Math::Rand();
		curCnt = ((((curCnt & 0x000F0000)>>16) + 1) <<16) | (ra & 0x0000FFFF) | (g_pRand->GetRand(ra&0x0000FFFF)<<20);
		if( ((curCnt & 0x000F0000) >> 16) > (UINT32) LocaleValue::GetInt32("FireWall/GrenadeCount") ||
			((curCnt & 0xFFF00000)>>20) != (UINT32) (g_pRand->GetRand(curCnt&0x0000FFFF) & 0x00000FFF) )
		{
			HackContext::i()->HackingDetected( CC_MANY_GRENADE_COUNT * 1.f ,C_MANY_GRENADE_COUNT);
		}
	}
}

void MainWeapon_HandGrenade::ThrowShell( VEC3D * pStart, VEC3D * pDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	if( m_pWeaponBase->getOwner() == nullptr)
		return;

	WeaponBase * pNewWeapon = g_pWeaponManager->GetFreeThrowWeapon( WEAPON_CLASS_GRENADESHELL, m_pWeaponBase->getExtensionWeaponInfo()->GetNumber());

	if( pNewWeapon == nullptr )
		return;

	pNewWeapon->SetOwner( m_pWeaponBase->getOwner());	
	pNewWeapon->setPairWeapon(m_pWeaponBase);
	pNewWeapon->Reset();
	pNewWeapon->SetEndurance(m_pWeaponBase->GetEndurance());	// revision 61316 핸드 그레네이드 발사 시, 생성되는 GrenadeShell에 총기 내구도 정보를 붙여줌(태국 hansoft. 114)

	MainWeapon_GrenadeShell* mainWeapon_gs = static_cast<MainWeapon_GrenadeShell*>(pNewWeapon->GetMainWeapon());
	mainWeapon_gs->Throw( pStart, pDir, rSpeed, false);

	m_pWeaponBase->SetFireOrder(WEAPON::PRIMARY_FIRE);
	m_pWeaponBase->NET_FireSync( pDir, WEAPON::PRIMARY_FIRE );

	pNewWeapon->setNetworkIdx( (INT16)nNetSlotIdx);
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pWeaponBase->getOwner()->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}


	// 해킹 방지 count
	// KEYWORD : Hack Protect
	GrenadeHackDetection(m_uCount);

	//	발사 Effect
	m_pWeaponBase->_PlayFireSound( WEAPON::FIRE_SOUND_EXT);		//	SECONDARY 사운드
	m_pWeaponBase->_FireEffect( m_pWeaponBase->getFXMatrix(), m_idxShellFireEffect, true);

	// 네트워크 정보를 세팅
	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, pStart);

	if( m_pWeaponBase->IsDedicatedHost() )
	{
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(pNewWeapon->getWeaponInfo()->GetItemID(), pNewWeapon->GetPos(), NET_GRENADE_STATE_FLY);

		GAMEEVENT::Write_ThrowingWeapon( pNewWeapon->getNetworkIdx(), &throwInfo);
	}
	else
	{
		pNewWeapon->SetVisible( false );
		i3Matrix::MakeWithAt( pNewWeapon->GetMatrix(), pDir, &I3_YAXIS);
		i3Matrix::PreRotateX( pNewWeapon->GetMatrix(), I3_PI2);
	}

	CalcBullet();


}

void	MainWeapon_HandGrenade::OnFire_HandGrenade( i3AIContext * pCtx, REAL32 tm)
{
	VEC3D target;

	VEC3D start, vDir;

	if( m_pWeaponBase->getOwner() == nullptr )
		return;

	CalcGrenadeDirection();

	if( pCtx->getLocalTime() == 0.0f)
	{
		I3ASSERT( m_pWeaponBase->getWeaponInfo() != nullptr);

		// 연이어 발사하지 못하도록...
		m_pWeaponBase->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		
		GetTargetDir(&target);
		i3Vector::Sub( &vDir, &target, &start);
		i3Vector::Normalize( &vDir, &vDir);
		// 현재는 Camera의 지점
		ThrowShell( m_pWeaponBase->GetStartPos(), &target, m_pWeaponBase->getOwner()->GetThrowSpeed(m_pWeaponBase->getWeaponInfo()->GetTypeUsage()), -1 );
	}
	else
	{
		REAL32 localTime = pCtx->getLocalTime();

		REAL32 rDelay = localTime + tm;

		if(rDelay > m_pWeaponBase->GetFireDelay() )
		{// 발사 지연시간이 충분히 지났다면...

			if( m_pWeaponBase->getOwner()->isLocal() || m_pWeaponBase->getOwner()->isAI() )
				 m_pWeaponBase->addWeaponStateMask( WSM_FIRE_DISABLE);

			 m_pWeaponBase->_OnPlayWeaponIdleAnim();
		}

	}
}


void MainWeapon_HandGrenade::CalcBullet()
{
	PB_VM_S

	INT32 iBullet = m_pWeaponBase->getLoadedBulletCount();

	iBullet--;
	if( iBullet < 0)
		iBullet = 0;

	m_pWeaponBase->setLoadedBulletCount( iBullet);

	PB_VM_E


}


void MainWeapon_HandGrenade::CalcGrenadeDirection()
{
	
	// 시점이 향하고 있는 목표 지점을 구한다.
	m_pWeaponBase->GetShotTargetPos( m_pWeaponBase->GetTargetPos());

	I3ASSERT( CGameCameraManager::i() );
//			(g_pCameraManager != nullptr);

	REAL32	Theta;
	VEC3D	vAt, vPos;
	MATRIX	mtx;
	VEC3D	vFirePos;
	VEC3D	offset;

	m_pWeaponBase->getOwner()->getDBInfo()->GetFPSCamPos( &offset);
	if( m_pWeaponBase->getOwner()->isLeftHand())	offset.x = -offset.x;

	i3Vector::Copy( &vFirePos, (VEC3D*)&offset);
	
	i3Vector::Copy( &vAt, i3Matrix::GetAt( m_pWeaponBase->getOwner()->GetMatrix()));

	Theta = atan2f( getX( &vAt), getZ( &vAt));
	i3Matrix::SetRotateY( &mtx, Theta + I3_DEG2RAD( 180.0f));

	//Rotate
	i3Matrix::PostRotateAxis( &mtx, i3Matrix::GetRight( m_pWeaponBase->getOwner()->GetMatrix()), - m_pWeaponBase->getOwner()->getBoneContext()->getRho());
	i3Vector::TransformCoord( &vAt, &vFirePos, &mtx);

	if( m_pWeaponBase->getOwner()->is1PV())
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pWeaponBase->getOwner()->getBoneContext()->getBoneMatrix( BONE_NECK)));
	}
	else
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pWeaponBase->getOwner()->getBoneContext()->getBoneMatrix( BONE_IK)));
		i3Vector::SetY( &vPos, getY( m_pWeaponBase->getOwner()->GetPos()) + m_pWeaponBase->getOwner()->getCameraContext()->getIKHeightForCam());
	}

	i3Vector::Add( m_pWeaponBase->GetStartPos(), &vAt,&vPos);

	m_pWeaponBase->m_bSetStart	= true;


}


void MainWeapon_HandGrenade::GetTargetDir( VEC3D * pDir)
{
	VEC3D vDir;
	
	VEC3D vXAxis, vYAxis;
	MATRIX tmp;

	i3Vector::Sub( &vDir, m_pWeaponBase->GetTargetPos(), m_pWeaponBase->GetStartPos() );
	i3Vector::Normalize( &vDir, &vDir);

	i3Vector::Cross( &vXAxis, &vDir, &I3_YAXIS);
	i3Vector::Normalize( &vXAxis, &vXAxis);

	i3Vector::Cross( &vYAxis, &vDir, &vXAxis);
	i3Vector::Normalize( &vYAxis, &vYAxis);
		
	i3Vector::TransformNormal( pDir, &vDir, &tmp);
}