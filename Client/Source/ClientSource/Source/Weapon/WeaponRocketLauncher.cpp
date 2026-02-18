#include "pch.h"
#include "WeaponRocketLauncher.h"
#include "GlobalVariables.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "StageBattle.h"

#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"

#include "MainWeapon_RLShell.h"

#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( WeaponRocketLauncher);//, WeaponBase);

WeaponRocketLauncher::WeaponRocketLauncher(void)
{

	m_idxShellFireEffect = -1;
	m_uCount = 0x0000FF0F & i3Math::Rand();
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x0000FFFF) << 16;
	m_IsDropped = false;
}

WeaponRocketLauncher::~WeaponRocketLauncher(void)
{

}

void WeaponRocketLauncher::PostCreate( void)
{
	m_ShellDummyList.clear();
	

	i3Scene::FindNodeByName( getSceneObject(), "RPG7_7VL_Animation_Dummy", m_ShellDummyList);	

	// 발사 이펙트
	if( m_pWeaponInfo->hasFireEffect())
	{	
		i3::rc_wstring	wstrFireEffect;
		m_pWeaponInfo->GetFireEffect(wstrFireEffect);
		i3::string strFireEffect;	i3::utf16_to_mb(wstrFireEffect, strFireEffect);
		m_idxShellFireEffect = g_pEffectManager->FindEffect(strFireEffect.c_str());
	}
}

void WeaponRocketLauncher::ReturnToReady( void)
{
	
	WeaponBase::ReturnToReady();

	if( getLoadedBulletCount() <= 0)
	{

		for(size_t i = 0; i < m_ShellDummyList.size(); i++)
		{
			i3Node * pNode = m_ShellDummyList[i];
			pNode->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}
}

void WeaponRocketLauncher::Reset( void)
{
	

	WeaponBase::Reset();



	for(size_t i = 0; i < m_ShellDummyList.size(); i++)
	{
		i3Node * pNode = m_ShellDummyList[i];
		pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
	}

	m_uCount = 0x0000FF0F & i3Math::Rand();
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x0000FFFF) << 16;

	m_IsDropped = false;
}

void WeaponRocketLauncher::OnBindResource( void)
{
	WeaponBase::OnBindResource();
}

void WeaponRocketLauncher::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnIdle( pCtx, tm);

	//setWeaponState( WEAPON::STATE_READY);

	if( !m_IsDropped && !MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		if( m_pOwner != nullptr && m_pOwner->isLocal() && (getLoadedBulletCount() <= 0) && (getTotalBulletCount() <= 0 ) )
			setWeaponState( WEAPON::STATE_NEEDTO_DROPWEAPON);
	}			
}

void WeaponRocketLauncher::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	if( pCtx->getLocalTime() == 0.0f)
	{
		I3ASSERT( m_pOwner != nullptr);
		I3ASSERT( getWeaponInfo() != nullptr);

		// 연이어 발사하지 못하도록...
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

			if( m_bSetStart )
			{
				VEC3D vDir;
				GetTargetDir( &vDir);
				ThrowShell( m_StartPos, &vDir, GetThrowSpeed(), -1 );				
			}
			else
			{
				I3PRINTLOG(I3LOG_FATAL,  "RPG7 doesn't have Start position!!");
			}
		}
	}
	else
	{
		REAL32 localTime = pCtx->getLocalTime();

		REAL32 rDelay = localTime + tm;

		if( m_pWeaponInfo->isOverFireDelay(rDelay))
		{// 발사 지연시간이 충분히 지났다면...

			if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			{
				addWeaponStateMask( WSM_FIRE_DISABLE);
			}

			_OnPlayWeaponIdleAnim();
		}
	}

}

bool WeaponRocketLauncher::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{


	bool bRet = WeaponBase::OnLoadMag( pCtx, tm);

	if(pCtx->getLocalTime() > 0.32f)
	{
	
		for( size_t i = 0; i < m_ShellDummyList.size(); i++)
		{
			i3Node * pNode = m_ShellDummyList[i];
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
		}
	}

	//GlobalFunc::PB_TRACE("WeaponRocketLauncher::OnLoadMag pCtx->getLocalTime() %f \n", pCtx->getLocalTime());

	return bRet;
}

void WeaponRocketLauncher::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndLoadMag( pCtx, tm);
}

void WeaponRocketLauncher::OnUpdate( REAL32 rDeltaSeconds)
{	
	WeaponBase::OnUpdate( rDeltaSeconds);	
}

bool WeaponRocketLauncher::Fire( WEAPON::FIRE_ORDER type)
{	
	if( getLoadedBulletCount() == 0)
		return false;

	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

//	I3ASSERT( g_pCameraManager != nullptr);
	I3ASSERT( CGameCameraManager::i() != nullptr);

	REAL32	Theta;
	VEC3D	vAt, vPos;
	MATRIX	mtx;
	VEC3D	vFirePos;
	VEC3D	offset;

	m_pOwner->getDBInfo()->GetFPSCamPos( &offset);
	if( m_pOwner->isLeftHand())	offset.x = -offset.x;

	i3Vector::Copy( &vFirePos, (VEC3D*)&offset);
	//i3Vector::Add( &vFirePos, 0.1f, -0.04f, -0.4f);

	i3Vector::Copy( &vAt, i3Matrix::GetAt( m_pOwner->GetMatrix()));

	Theta = atan2f( getX( &vAt), getZ( &vAt));
	i3Matrix::SetRotateY( &mtx, Theta + I3_DEG2RAD( 180.0f));

	//Rotate
	i3Matrix::PostRotateAxis( &mtx, i3Matrix::GetRight( m_pOwner->GetMatrix()), - m_pOwner->getBoneContext()->getRho());
	i3Vector::TransformCoord( &vAt, &vFirePos, &mtx);
	
	if( m_pOwner->is1PV())
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_NECK)));
	}
	else
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_IK)));
		i3Vector::SetY( &vPos, getY( m_pOwner->GetPos()) + m_pOwner->getCameraContext()->getIKHeightForCam());
	}

	i3Vector::Add( m_StartPos, &vAt,&vPos);

	m_bSetStart	= true;

	setWeaponState( WEAPON::STATE_FIRE);
	_OnPlayWeaponFireAnim();
	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

	if( getOwner() != nullptr && getOwner()->isLocal())
		addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
	removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);

	return true;
}

void WeaponRocketLauncher::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponBase::AttachNode( pSceneNode, pOwner);
}

void WeaponRocketLauncher::DetachNode( void )
{
	WeaponBase::DetachNode();
}

void WeaponRocketLauncher::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
{
	WeaponBase::GetShotTargetPos( pDest, pStart);			
}

void WeaponRocketLauncher::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	WeaponBase::FireCollision( pStartPos, pTargetPos);
}

namespace
{
	void RPGHackDetection(UINT32 curCnt)
	{
		UINT32 ra = i3Math::Rand();
		curCnt = ((((curCnt & 0x000000F0)>>4) + 1) <<4) | (ra & 0x0000FF0F) | ((g_pRand->GetRand( ra&0x0000FF0F) & 0x0000FFFF) << 16);
		if( ((curCnt & 0x000000F0) >> 4) > (UINT32) LocaleValue::GetInt32("FireWall/RpgCount") ||
			((curCnt & 0xFFFF0000) >> 16) != (UINT32)(g_pRand->GetRand( curCnt&0x0000FF0F) & 0x0000FFFF) )
		{
			HackContext::i()->HackingDetected( CC_MANY_GRENADE_COUNT * 1.f, C_MANY_GRENADE_COUNT);
		}
	}
}

void WeaponRocketLauncher::ThrowShell( VEC3D * pStart, VEC3D * pDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	WeaponBase * pNewWeapon = g_pWeaponManager->GetFreeThrowWeapon( WEAPON_CLASS_ROCKET_LAUNCHER, 0);
	if( pNewWeapon == nullptr )
		return;

	pNewWeapon->SetOwner( getOwner());	// 먼저 세팅해야됩니다.	
	pNewWeapon->setPairWeapon(this);
	pNewWeapon->Reset();

	MainWeapon_RLShell* mainWeapon_Shell = static_cast<MainWeapon_RLShell*>(pNewWeapon->GetMainWeapon());
	mainWeapon_Shell->Throw( pStart, pDir, rSpeed, false);

	NET_FireSync( m_TargetPos );

	// Network ID
	pNewWeapon->setNetworkIdx( (INT16)nNetSlotIdx);
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pOwner->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}

	INT32 bullet = getLoadedBulletCount();

	if( bullet <= _OnProcessFireBulletCount())
	{
		if( (bullet > 0) && (m_pOwner != nullptr) && (m_pOwner->isLocal()))
		{
			REAL32 rError = 51.f;
			UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;

			GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rError);
		}
	}
	
	//	발사 Effect
	_FireEffect( getFXMatrix(), m_idxShellFireEffect, true);
	_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL);		//	사운드

	// 네트워크 정보를 세팅
	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, pStart);

	if( getOwner() != nullptr && getOwner()->isLocal())
	{
		RPGHackDetection(m_uCount);
	}

	if( IsDedicatedHost() )
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

	// Rocket이 발사되었으므로, 더미로켓을 비활성화 합니다.
	if( getLoadedBulletCount() <= 0)	
	{

		for( size_t i = 0; i < m_ShellDummyList.size(); i++)
		{
			i3Node * pNode = m_ShellDummyList[i];
			pNode->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}
}

void WeaponRocketLauncher::LoadShell( void)
{
	if( getOwner() != nullptr && getOwner()->isLocal() &&
		(getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT) )
	{
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
	}

//	_PlayWeaponAnim( m_aiiReloadShell );

	setWeaponState( WEAPON::STATE_LOADMAG);
}

void WeaponRocketLauncher::RemoveShell( void)
{
	if( getOwner() != nullptr && getOwner()->isLocal() &&
		( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT))
	{
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
	}

//	_PlayWeaponAnim( m_aiiRemoveShell );
	
	setWeaponState( WEAPON::STATE_LOADBULLET);
}

void WeaponRocketLauncher::GetTargetDir( VEC3D * pDir)
{
	VEC3D vDir;
				
	REAL32 r, t;
	VEC3D vXAxis, vYAxis;
	
	i3Vector::Sub( &vDir, m_TargetPos, m_StartPos);
	i3Vector::Normalize( &vDir, &vDir);

	i3Vector::Cross( &vXAxis, &vDir, &I3_YAXIS);
	i3Vector::Normalize( &vXAxis, &vXAxis);

	i3Vector::Cross( &vYAxis, &vDir, &vXAxis);
	i3Vector::Normalize( &vYAxis, &vYAxis);

	REAL32 deviation = m_pOwner->getBoneContext()->getFireDeviation();

	r = i3Math::Randf2();
	
	r *= deviation * 0.5f * 0.5f;
	t = i3Math::Randf2() * deviation * 0.5f * 0.5f;

	MATRIX tmp;
	i3Matrix::SetRotateAxisDeg( &tmp, &vXAxis, r);
	i3Matrix::PostRotateAxisDeg( &tmp, &vYAxis, t);

	i3Vector::TransformNormal( pDir, &vDir, &tmp);
}