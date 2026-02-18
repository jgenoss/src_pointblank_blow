#include "pch.h"
#include "WeaponBow.h"
#include "GameCharaBase.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"

#include "../StageBattle/UserContext.h"
#include "BattleSlotContext.h"
#include "WeaponFunction.h"
#include "MainWeapon_Bow.h"
#include "MainWeapon_ThrowKnife.h"
#include "MainWeapon_GrenadeShell.h"
#include "WeaponItemID.h"


#define	DETACHARROW_TIME			0.01f
I3_CLASS_INSTANCE( WeaponBow);//, WeaponBase);

WeaponBow::WeaponBow() : m_b1PV(false), m_nLoadedShell(0), m_nTotalShell(0)
	, m_ExtensionArrowItemID(0)
{
}

WeaponBow::~WeaponBow()
{
}

void WeaponBow::Create(CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);

	for( UINT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		WEAPON::EXT_TYPE type = m_pWeaponInfo->getExtensionType( i);
		if( type == WEAPON::EXT_GLAUNCHER)
		{
			m_ExtensionArrowItemID = m_pWeaponInfo->GetExtensionItemID( i);
		}
	}
}

MainWeapon*	WeaponBow::CreateMainWeapon()
{
	MainWeapon_Bow * p = new MainWeapon_Bow(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_Bow));
	return p;
}

void WeaponBow::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	bool bLoaded = true;

	if (getOwner() == nullptr)
		return;

	if (pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachLeftHand();

	// 로컬 캐릭터의 무기가 아니라면 들어오면 안됩니다.
	if (!getOwner()->isLocal() && !getOwner()->isAI())
		return;

	//2015-04-06 익스텐션 구조 변경으로 해당 조건으로 바꿉니다. (기존 isExtension() ) ENG_김대영
	if (isMultipleExtension() ? GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER : isExtensionActivate() == true)
	{
		if (getLoadedShellCount() <= 0)
		{
			if (getTotalShellCount() > 0)
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
				removeWeaponStateMask(WSM_LOADMAG_DISABLE);
			}
			else
			{
				//if( m_bLoadedShell )
				{
					// 탄피를 비웁니다..
					//m_State = WEAPON::STATE_NEEDTO_REMOVESHELL;
					//removeWeaponStateMask( WSM_LOADBULLET_DISABLE );
				}
				//else
				{
					setWeaponState(WEAPON::STATE_READY);
					removeWeaponStateMask(WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_EXT_DISABLE);

					// 캐릭터의 익스텐션을 직접 호출하기 때문에 캐릭터의 상태를 초기화 해주어야 한다.
					// 캐릭터는 쉘 리무브 상태에서 무기 스왑을 막아놨다. 무기가 레디 상태로 바뀌면 초기화해줘야 한다.
					m_pOwner->removeEnableStateMask(CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

					m_pOwner->Cmd_Extension(false);

					//	기획팀 요청으로 커스텀 M4의경우 유탄이 없으면 기본 무기상태로 자동으로 돌아간다.
					if (isMultipleExtension())		SwapExtensionType();
				}
			}

			addWeaponStateMask(WSM_FIRE_DISABLE);
		}
		else
		{
			if (m_nLoadedShell > 0)
			{
				setWeaponState(WEAPON::STATE_READY);
				removeWeaponStateMask(WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
			}
		}
	}
	else
	{
		INT32 iBullet = getLoadedBulletCount();

		if (iBullet <= 0)
		{
			if (getTotalBulletCount() > 0)
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
				removeWeaponStateMask(WSM_LOADMAG_DISABLE);
			}
			else
				setWeaponState(WEAPON::STATE_EMPTY_MAGAZINE);

			addWeaponStateMask(WSM_FIRE_DISABLE);
			bLoaded = false;
		}
		else
		{
			removeWeaponStateMask(WSM_FIRE_DISABLE | WSM_EXT_DISABLE);

			if (iBullet < GetLoadBulletWithCashItem() || getLoadedBulletDualCount() < GetLoadBulletWithCashItem())
				removeWeaponStateMask(WSM_LOADMAG_DISABLE);
			else
				addWeaponStateMask(WSM_LOADMAG_DISABLE);

			if (isWeaponStateMask(WSM_FIRING) == false)
			{	// 발사중이 아니면
				if (getWeaponInfo()->GetPumpAction() == false)	//	펌프액션 방식이면
				{
					setWeaponState(WEAPON::STATE_READY);
					removeWeaponStateMask(WSM_FIRE_DISABLE);
				}
			}
			else
			{
				INT32 autoCnt = 0;
				INT32 autopointshot = 0;

				if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
					autoCnt = m_pWeaponInfo->GetAutoFireCount();
				else
					autoCnt = m_pWeaponInfo->GetExtAutoFireCount();

				if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
					autopointshot = m_pWeaponInfo->GetAutoPointShotEnable();
				else
					autopointshot = m_pWeaponInfo->GetExtAutoPointShotEnable();

				switch (autoCnt)
				{
				case 0:		// 연발
					setWeaponState(WEAPON::STATE_NEEDTO_FIRE);

					removeWeaponStateMask(WSM_FIRE_DISABLE);

#if defined( FIREDELAY_COST)
					addWeaponStateMask(WSM_MORE_FIRING);

					// Idle 상태에서 발사 체크시 타이머를 계산한다.
					// 60fps에 맞춰진 오차를 계산하여 Idle 타임을 계산
					m_rFireTime = m_rFireTime - (0.016f - tm);
#endif
					break;

				case 1:		// 단발
					setWeaponState(WEAPON::STATE_READY);

					addWeaponStateMask(WSM_FIRE_DISABLE);
					break;

				default:		// 점사
					if (autopointshot != 0)
					{
						ProcessAutoPoint(tm);
					}
					else
					{
						// 연사 중...
						if (isWeaponStateMask(WSM_WAIT_AUTOFIREDELAY))
						{
							// 연사 사이의 간격을 기다리고 있음.
							m_timeAutoFire += tm;
							addWeaponStateMask(WSM_FIRE_DISABLE);

							if (m_timeAutoFire >= (m_pWeaponInfo->GetAutoFireDelay() - tm))
							{
								m_timeAutoFire = 0.0f;
								setAutoFireCount(0);
								setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
								removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
							}
						}
						else
						{
							// 발사한다.
							if (getAutoFireCount() > autoCnt)
							{
								// 발사 개수가 점사 수만큼...
								addWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);

								m_timeAutoFire = 0.0f;
								setWeaponState(WEAPON::STATE_WAIT);
							}
							else
							{
								setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
							}
						}
						removeWeaponStateMask(WSM_FIRE_DISABLE);
					}
					break;
				}
			}
		}
	}
}

bool WeaponBow::Fire(WEAPON::FIRE_ORDER type)
{
	const INT32 currBulletCnt = isExtensionActivate() ? getLoadedShellCount() : getLoadedBulletCount();
	if( currBulletCnt <= 0)
		return false;

	m_FireOrder = type;

	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

	NET_Fire( nullptr, m_TargetPos, m_FireOrder );

	return true;
}

/*virtual*/ void WeaponBow::OnSwaping( i3AIContext * pCtx, REAL32 tm)
{
	I3ASSERT( pCtx != nullptr);

	SetCurrentAttachLeftHand();

	removeWeaponStateMask( WSM_ACTIVATE_EXTENSION);

	MainWeapon_Bow* mainWeapon = static_cast<MainWeapon_Bow*>(GetMainWeapon());
	mainWeapon->PlayAnim_ChangeRun(m_pOwner->is3PV());
	
	REAL32 localTime = pCtx->getLocalTime();

	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( localTime + tm > pAIState->getDuration())
	{
		mainWeapon->PlayAnim_ChangeEnd();
		OnIdle(pCtx, tm);
		setWeaponState( WEAPON::STATE_READY);
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_FIRING);

		_OnPlayWeaponIdleAnim();
	}
	
}

/*virtual*/ void WeaponBow::Reset( void)
{
	WeaponBase::Reset();

	if(m_pExtWeaponInfo) // Compound Bow의경우는 확장익스텐션이 없다,  WPSmoke, Grenade, Smoke, Flash Bang만 있다
	{
		setLoadedShellCount( m_pExtWeaponInfo->GetLoadBullet());

		// Total Shell Count = 최대 탄수 - 장전된 탄수
		INT32 i32TotalShell = m_pExtWeaponInfo->GetMaxBullet() - m_pExtWeaponInfo->GetLoadBullet();

		if(i32TotalShell < 0)
			i32TotalShell = 0;

		setTotalShellCount( i32TotalShell );
	}
	else
	{
		setLoadedShellCount(0);
		setTotalShellCount(0);
	}
}


INT32 WeaponBow::getTotalShellCount( void)
{
	return GlobalFunc::DecryptValue( &m_nTotalShell, m_u8EncryptKey);
}

void WeaponBow::setTotalShellCount( INT32 nCount)
{
	m_nTotalShell = nCount;

	GlobalFunc::EncryptValue( &m_nTotalShell, m_u8EncryptKey);
}

INT32 WeaponBow::getLoadedShellCount( void)
{
	return m_nLoadedShell;
	//return GlobalFunc::DecryptValue( &m_nLoadedShell, m_u8EncryptKey);
}

void WeaponBow::setLoadedShellCount( INT32 nCount)
{
	m_nLoadedShell = nCount;

	//GlobalFunc::EncryptValue( &m_nLoadedShell, m_u8EncryptKey);
}

void WeaponBow::_OnPlayWeaponIdleAnim(void)
{
	if (getOwner() != nullptr)
	{
		if (isWeaponStateMask(WSM_ACTIVATE_EXTENSION))
		{
			if (m_pOwner->is1PV())
			{
				WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_EXT_IDLE_1PV));
			}
			else
			{
				WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_EXT_IDLE_3PV));
			}
		}
		else
		{
			if (m_pOwner->is1PV())
			{
				WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_IDLE1PV));
			}
			else
			{
				WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_IDLE3PV));
			}
		}
	}
}


/*virtual*/ void WeaponBow::Transmission(i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos /* = nullptr */, bool bToWorld /* = false */)
{
	WeaponBase::Transmission(pNewSceneNode,pNewOwner, pPos, bToWorld);

	MainWeapon_Bow* mainWeapon = static_cast<MainWeapon_Bow*>(GetMainWeapon());

	mainWeapon->Transmission_Bow(bToWorld);

}

void WeaponBow::ThrowArrow( VEC3D * pStart, VEC3D * pDir, REAL32 rSpeed, INT32 nNetSlotIdx, bool bExtension)
{
	if( m_pOwner == nullptr)
		return;

	T_ItemID arrowItemID = m_ExtensionArrowItemID;

	if( bExtension)
		arrowItemID = m_ExtensionArrowItemID;
	
	WeaponBase * pNewWeapon = g_pWeaponManager->GetFreeThrowWeapon( WEAPON::ItemID2Class( arrowItemID), WEAPON::ItemID2ResID( arrowItemID));

	if( pNewWeapon == nullptr )
		return;
	
	pNewWeapon->SetOwner( m_pOwner);
	pNewWeapon->setPairWeapon( this);
	pNewWeapon->Reset();
	
	if( i3::same_of<WeaponThrowKnife*>( pNewWeapon))
	{
		MainWeapon_ThrowKnife* mainWeapon_New = static_cast<MainWeapon_ThrowKnife*>(pNewWeapon->GetMainWeapon());
		mainWeapon_New->ThrowArrow( pStart, pDir, rSpeed, true, m_pOwner->getCharaNetIndex(), (WeaponThrowKnife*) pNewWeapon);
	}
	else if( i3::kind_of<WeaponGrenadeShell*>( pNewWeapon))
	{
		MainWeapon_GrenadeShell* mainWeapon_gs = static_cast<MainWeapon_GrenadeShell*>(pNewWeapon->GetMainWeapon());
		mainWeapon_gs->Throw( pStart, pDir, rSpeed, false);
	}
	else
	{
		I3ASSERT_0;
	}
	
	m_FireOrder = WEAPON::PRIMARY_FIRE;
	NET_FireSync( pDir, WEAPON::PRIMARY_FIRE );
	
	pNewWeapon->setNetworkIdx( (INT16)nNetSlotIdx);
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pOwner->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}
	
	//	발사 Effect
	_PlayFireSound( WEAPON::FIRE_SOUND_EXT);		//	SECONDARY 사운드

	// 네트워크 정보를 세팅
	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, pStart);

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
	CalcBullet();
}



void WeaponBow::PlayAnimation_AttackReady(void)
{
	if (m_pOwner->is1PV())
	{
		if( !UserContext::i()->bBowFireOrder)
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_ATTACKREADY_1PV));
		}
		else
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_GRENADEATTACKREADY_1PV));
		}
	}
	else
	{
		if( !UserContext::i()->bBowFireOrder)
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_ATTACKREADY_3PV));
		}
		else
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_GRENADEATTACKREADY_3PV));
		}
	}
}

void WeaponBow::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire( pCtx, tm);

	SetEnable( true);
	_OnPlayWeaponIdleAnim();
	
	if (isExtensionActivate() && getLoadedShellCount() <= 0)
	{
		if (m_pOwner)
			m_pOwner->Cmd_Extension(false);
	}
}

/*virtual*/void WeaponBow::OnBindResource( void)
{
	WeaponBase::OnBindResource();

	//UI IDLE 상태
	{
		setAIIndex( WEAPON::AI_UI_IDLE_A, (INT8) m_pAICtx->FindAIStateByName( "UI_Idle_A"));
		setAIIndex( WEAPON::AI_UI_IDLE_B, (INT8) m_pAICtx->FindAIStateByName( "UI_Idle_B"));
		setAIIndex( WEAPON::AI_EXT_IDLE_1PV, (INT8) m_pAICtx->FindAIStateByName( "ExtensionIdle1PV"));
		setAIIndex( WEAPON::AI_EXT_IDLE_3PV, (INT8) m_pAICtx->FindAIStateByName( "ExtensionIdle3PV"));
	}

	

	//Change
	{
	}

}

void WeaponBow::PlayAnim_UI_IdleA()
{	
	MainWeapon_Bow* mainWeapon = static_cast<MainWeapon_Bow*>(GetMainWeapon());

	mainWeapon->PlayAnim_UI_IdleA_Bow();

}
void WeaponBow::PlayAnim_UI_IdleB()
{	
	MainWeapon_Bow* mainWeapon = static_cast<MainWeapon_Bow*>(GetMainWeapon());

	mainWeapon->PlayAnim_UI_IdleB_Bow();
}

/**********************************************************************************************//**
 * @fn	void WeaponBow::CalcBullet()
 *
 * @brief	Calculates the bullet.
 *
 * @author	Ran
 * @date	2011-09-21
 **************************************************************************************************/

void WeaponBow::CalcBullet()
{
	if (isExtensionActivate())
	{
		INT32 shellCnt = getLoadedShellCount() - 1;
		if (shellCnt < 0)
			shellCnt = 0;

		setLoadedShellCount(shellCnt);
	}
	else
	{
		INT32 iBullet = getLoadedBulletCount();

		iBullet--;
		if( iBullet < 0)
			iBullet = 0;

		setLoadedBulletCount( iBullet);
	}
}

/**********************************************************************************************//**
 * @fn	void WeaponBow::CalcArrowDirection()
 *
 * @brief	Calculates the arrow direction.
 *
 * @author	Ran
 * @date	2011-09-21
 **************************************************************************************************/

void WeaponBow::CalcArrowDirection()
{
	
	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

//	I3ASSERT( g_pCameraManager != nullptr);
	I3ASSERT( CGameCameraManager::i() );

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


}
void WeaponBow::OnExtensionActivate( i3AIContext * pCtx, REAL32 tm)
{
	SetCurrentAttachLeftHand();

	MainWeapon_Bow* mainWeapon = static_cast<MainWeapon_Bow*>(GetMainWeapon());

	if( m_pOwner->is3PV() && pCtx->getLocalTime() == 0.f)
	{
		mainWeapon->PlayAnim_ExtActivate3PV_Run();
	}
	REAL32 rElaps = pCtx->getLocalTime() + tm;

	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( rElaps > pAIState->getDuration())
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		addWeaponStateMask(WSM_ACTIVATE_EXTENSION);

		OnIdle(pCtx, tm);
		setWeaponState( WEAPON::STATE_READY);
		mainWeapon->PlayAnim_ExtActivate3PV_End();
	}
	else
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}

}

void WeaponBow::OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm)
{
	SetCurrentAttachLeftHand();

	MainWeapon_Bow* mainWeapon = static_cast<MainWeapon_Bow*>(GetMainWeapon());

	if( m_pOwner->is3PV() && pCtx->getLocalTime() == 0.f)
	{
		mainWeapon->PlayAnim_ExtDeActivate3PV_Run();
	}
	REAL32 rElaps = pCtx->getLocalTime() + tm;

	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);



	if( rElaps > pAIState->getDuration())
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_ACTIVATE_EXTENSION | WSM_FIRING);

		OnIdle(pCtx, tm);
		setWeaponState( WEAPON::STATE_READY);
		mainWeapon->PlayAnim_ExtDeActivate3PV_End();
	}
	else
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}
}
void WeaponBow::GetNetBullet(GAMEINFO_BULLET * pBullet)
{
	pBullet->Reset();
	pBullet->SetLoadBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT, (UINT8)getLoadedBulletCount());
	pBullet->SetStandbyBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT, (UINT16)getTotalBulletCount());
	pBullet->SetLoadBulletCount(WEAPON::EXTENSION_ON, WEAPON::HAND_TYPE_LEFT, (UINT8)getLoadedShellCount());
}

void WeaponBow::SetNetBullet(GAMEINFO_BULLET * pBullet)
{
	setLoadedBulletCount(pBullet->GetLoadBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT));
	setTotalBulletCount(pBullet->GetStandbyBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT));
	m_nLoadedShell = pBullet->GetLoadBulletCount(WEAPON::EXTENSION_ON, WEAPON::HAND_TYPE_LEFT);
}

/*virtual*/ void WeaponBow::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	//VEC3D target;
	//VEC3D start, vDir;

	if( getOwner() == nullptr )
		return;

	//CalcArrowDirection();

	if( pCtx->getLocalTime() == 0.0f)
	{
		I3ASSERT( getWeaponInfo() != nullptr);

		// 연이어 발사하지 못하도록...
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		/*GetTargetDir(&target);
		i3Vector::Sub( &vDir, &target, &start);
		i3Vector::Normalize( &vDir, &vDir);*/

		//ThrowArrow(&m_StartPos, &target, GetThrowSpeed(), -1, isWeaponStateMask(WSM_ACTIVATE_EXTENSION));
		FireCollision(nullptr, m_TargetPos);
		
		_PlayFireSound(WEAPON::FIRE_SOUND_NORMAL);		//	SECONDARY 사운드
		
		// Bullet 체크
		CalcBullet();
	}
	else
	{
		REAL32 localTime = pCtx->getLocalTime();
		REAL32 rDelay = localTime + tm;

		if(rDelay > GetFireDelay() )
		{// 발사 지연시간이 충분히 지났다면...
			if( getOwner()->isLocal() || getOwner()->isAI() )
				addWeaponStateMask( WSM_FIRE_DISABLE);

			_OnPlayWeaponIdleAnim();
		}
	}
}
void WeaponBow::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{


	VEC3D target;
	
	VEC3D start, vDir;


	if( getOwner() == nullptr )
		return;

	CalcArrowDirection();

	if( pCtx->getLocalTime() == 0.0f)
	{
		I3ASSERT( getWeaponInfo() != nullptr);
		// 연이어 발사하지 못하도록...
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		GetTargetDir(&target);
		i3Vector::Sub( &vDir, &target, &start);
		i3Vector::Normalize( &vDir, &vDir);

		// 현재는 Camera의 지점
		ThrowArrow( m_StartPos, &target, GetThrowSpeed(), -1, true );
	}
	else
	{
		REAL32 localTime = pCtx->getLocalTime();
		REAL32 rDelay = localTime + tm;

		if(rDelay > GetFireDelay() )
		{// 발사 지연시간이 충분히 지났다면...

			if( (getOwner()->isLocal() || getOwner()->isAI()))
				addWeaponStateMask( WSM_FIRE_DISABLE);

			_OnPlayWeaponIdleAnim();
		}
	}
}


/**********************************************************************************************//**
 * @fn	void WeaponBow::GetTargetDir( VEC3D * pDir)
 *
 * @brief	Gets a target dir.
 *
 * @author	Ran
 * @date	2011-09-21
 *
 * @param [in,out]	pDir	If non-null, the dir.
 **************************************************************************************************/

void WeaponBow::GetTargetDir( VEC3D * pDir)
{
	VEC3D vDir;

	VEC3D vXAxis, vYAxis;
	MATRIX tmp;

	i3Vector::Sub( &vDir, m_TargetPos, m_StartPos);
	i3Vector::Normalize( &vDir, &vDir);

	i3Vector::Cross( &vXAxis, &vDir, &I3_YAXIS);
	i3Vector::Normalize( &vXAxis, &vXAxis);

	i3Vector::Cross( &vYAxis, &vDir, &vXAxis);
	i3Vector::Normalize( &vYAxis, &vYAxis);
	
	i3Vector::TransformNormal( pDir, &vDir, &tmp);
}