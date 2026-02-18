#include "pch.h"
#include "GlobalVariables.h"
#include "WeaponGrenadeLauncher.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "StageBattle.h"
#include "GameEvent.H"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "WeaponFunction.h"
#include "MainWeapon_GrenadeShell.h"
#include "WeaponItemID.h"

#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( WeaponGrenadeLauncher);//, WeaponBase);

WeaponGrenadeLauncher::WeaponGrenadeLauncher()
{
	m_aiiFireShell	= -1;
	m_aiiRemoveShell = -1;
	m_aiiReloadShell = -1;

	m_idxShellFireEffect	= -1;
	m_idxGrenadeFXBone		= -1;

	m_nLoadedShell	= 0;
	m_nTotalShell	= 0;
	m_bLoadedShell = true;

	m_uCount = (i3Math::Rand() & 0x00000FFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) << 16);
}

WeaponGrenadeLauncher::~WeaponGrenadeLauncher()
{
}

void WeaponGrenadeLauncher::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);

	// 익스텐션 정보가 반듯이 필요합니다.
	I3ASSERT( m_pExtWeaponInfo != nullptr);

	m_idxGrenadeFXBone = getSceneObject()->GetBody()->FindBoneByName( "GrenadeFXDummy");

	if( m_idxGrenadeFXBone == -1)
	{
		I3TRACE("[WeaponGrenadeLauncher] Cannot find GrenadeFXDummy (Itemid:%d )\n", Itemid);
	}

	// 발사 이펙트
	if( m_pWeaponInfo->hasFireEffectExt())
	{
		i3::rc_wstring wstrFireEffectExt;
		m_pWeaponInfo->GetFireEffectExt(wstrFireEffectExt);
		i3::string strFireEffectExt;	i3::utf16_to_mb(wstrFireEffectExt, strFireEffectExt);
		m_idxShellFireEffect = g_pEffectManager->FindEffect( strFireEffectExt.c_str());
	}
}

void WeaponGrenadeLauncher::Reset( void)
{
	WeaponBase::Reset();

	setLoadedShellCount( m_pExtWeaponInfo->GetLoadBullet());
	
	// Total Shell Count = 최대 탄수 - 장전된 탄수
	INT32 i32TotalShell = m_pExtWeaponInfo->GetMaxBullet() - m_pExtWeaponInfo->GetLoadBullet();
	
	if(i32TotalShell < 0)
		i32TotalShell = 0;

	setTotalShellCount( i32TotalShell );

	m_bLoadedShell = true;
	
	m_uCount = (i3Math::Rand() & 0x00000FFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) << 16);
}

void WeaponGrenadeLauncher::SetExtWeaponInfo()
{
	// 익스텐션 기능 무기의 익스텐션 정보
	if( m_pWeaponInfo->isExtensible() && m_pWeaponInfo->hasExtensionInfo() ) 
	{		
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) &&
			m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_ASSAULT &&
			m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::K201GL))
		{
			//	공룡 탈출 미션에서 제공되는 K201 유탄
			m_pExtWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_GRENADESHELL,  WEAPON::getItemIndex(WEAPON::GSHELL_K201_BOSS), true);
		}
		else
		{
			T_ItemID itemID = m_pWeaponInfo->GetSubExtensionItemID();

			if( itemID > 0)
			{
				// Weapon.pef 에서 설정된 발사체 (유탄) 정보
				m_pExtWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemID);
			}
			else	// default
			{
				m_pExtWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_GRENADESHELL, WEAPON::getItemIndex(WEAPON::GSHELL_K201), true);
			}			
		}
	}
}

void WeaponGrenadeLauncher::OnBindResource(void)
{
	WeaponBase::OnBindResource();

	if( m_pAICtx != nullptr)
	{
		m_aiiFireShell = (INT8) m_pAICtx->FindAIStateByName( "FireShell");
		m_aiiRemoveShell = (INT8) m_pAICtx->FindAIStateByName( "RemoveShell");
		m_aiiReloadShell = (INT8) m_pAICtx->FindAIStateByName( "LoadShell");
	}
}

bool WeaponGrenadeLauncher::Fire( WEAPON::FIRE_ORDER type )
{
	//2015-04-06 익스텐션 구조 변경으로 해당 조건으로 바꿉니다. (기존 !isExtension() ) ENG_김대영
	//isMultipleExtension 즉 2중으로 되어 있는애들은 잘 처리가 되있는데 이게 아니면!! 옛날방식으로 해야된다! 하하하하하하하하......... 진식님 파이팅 난 여기까지요...

	if( isMultipleExtension() ? GetCurrentExtensionType() != WEAPON::EXT_GLAUNCHER : isExtensionActivate() == false )
	{
		if( getLoadedBulletCount() <= 0)
			return false;

		m_FireOrder = type;

		// 시점이 향하고 있는 목표 지점을 구한다.
		GetShotTargetPos( m_TargetPos);

		NET_Fire( nullptr, m_TargetPos, m_FireOrder );
	}
	else
	{
		if( getLoadedShellCount() <= 0)
			return false;

		m_FireOrder = type;

		// 시점이 향하고 있는 목표 지점을 구한다.
		GetShotTargetPos( m_TargetPos);

//		I3ASSERT( g_pCameraManager != nullptr);
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

		WF::PlayWeaponAnim(this, m_aiiFireShell);

		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

		setWeaponState(WEAPON::STATE_FIRE);

		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
		removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);
	}

	return true;
}

void WeaponGrenadeLauncher::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{
	if( bAnimPlay )
	{
		if( bVal)
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));
	}
	else
		_OnPlayWeaponIdleAnim();
}

REAL32 WeaponGrenadeLauncher::GetThrowSpeed()
{
	I3ASSERT( getExtensionWeaponInfo() != nullptr);
	return getExtensionWeaponInfo()->GetThrowSpeed();
}

namespace
{
	void GrenadeHackDetection(UINT32 curCnt)
	{
		UINT32 ra = i3Math::Rand();
		curCnt = ((((curCnt & 0x0000F000)>>12) + 1) <<12) | (ra & 0x00000FFF) | (g_pRand->GetRand(ra & 0x00000FFF)<<16);

		if( ((curCnt & 0x0000F000) >> 12) > (UINT32) LocaleValue::GetInt32("FireWall/GrenadeCount") ||
			((curCnt & 0xFFFF0000) >> 16) != (UINT32) (g_pRand->GetRand( curCnt & 0x00000FFF) & 0x0000FFFF))
		{
			HackContext::i()->HackingDetected( CC_MANY_GRENADE_COUNT * 1.f ,C_MANY_GRENADE_COUNT);
		}
	}
}

void WeaponGrenadeLauncher::ThrowShell( VEC3D * pStart, VEC3D * pDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	if( m_pOwner == nullptr)
		return;

	WeaponGrenadeShell * pNewWeapon = (WeaponGrenadeShell*)g_pWeaponManager->GetFreeThrowWeapon( WEAPON_CLASS_GRENADESHELL, getExtensionWeaponInfo()->GetNumber());
	if( pNewWeapon == nullptr )
		return;

	// 유탄 발사기 종류에 따라 유탄 정보도 바뀌어야 한다.
	pNewWeapon->SetGrenadeOwner( getOwner(), m_pExtWeaponInfo);		// 먼저 세팅해야됩니다.	
	pNewWeapon->setPairWeapon(this);
	pNewWeapon->Reset();
	
	MainWeapon_GrenadeShell* mainWeapon_gs = static_cast<MainWeapon_GrenadeShell*>(pNewWeapon->GetMainWeapon());

	mainWeapon_gs->Throw( pStart, pDir, rSpeed, false);

	m_FireOrder = WEAPON::PRIMARY_FIRE;
	NET_FireSync( m_TargetPos, WEAPON::PRIMARY_FIRE );

	// Network ID
	// 2는 고유 Shell에 대한 고유 아이디입니다.
	pNewWeapon->setNetworkIdx( (INT16)nNetSlotIdx);
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pOwner->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}

	// 해킹 방지 count
	if( getOwner() != nullptr && getOwner()->isLocal() && !getOwner()->IsSting())
	{
		GrenadeHackDetection(m_uCount);
	}

#ifdef ENABLE_TEST_COMMAND
	if( !g_pWeaponManager->COMMAND_IsUnlimitGrenade())		//	Use Cheat
#endif
	{
		PB_VM_S

			INT32 loadedShell = getLoadedShellCount();

		loadedShell -= 1;
		if( loadedShell < 0)
			loadedShell = 0;

		setLoadedShellCount( loadedShell);

		PB_VM_E
	}

	//	발사 Effect
	_FireEffect( getGrenadeFXMatrix(), m_idxShellFireEffect, true);
	_PlayFireSound( WEAPON::FIRE_SOUND_EXT);		//	SECONDARY 사운드

	// 네트워크 정보를 세팅
	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, pStart);

	if( IsDedicatedHost() )
	{
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(pNewWeapon->getWeaponInfo()->GetItemID(), pNewWeapon->GetPos(), NET_GRENADE_STATE_FLY);

		throwInfo._bExtension = true;

		GAMEEVENT::Write_ThrowingWeapon( pNewWeapon->getNetworkIdx(), &throwInfo);
	}
	else
	{
		pNewWeapon->SetVisible( false );
		i3Matrix::MakeWithAt( pNewWeapon->GetMatrix(), pDir, &I3_YAXIS);
		i3Matrix::PreRotateX( pNewWeapon->GetMatrix(), I3_PI2);
	}
}

void WeaponGrenadeLauncher::LoadShell( void)
{
	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()) &&
		(getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT) )
	{
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE | WSM_LOADMAG_DISABLE);
	}

	_PlayWeaponAnimWithApplyTimeScale( m_aiiReloadShell );

	setWeaponState( WEAPON::STATE_LOADMAG);
}

void WeaponGrenadeLauncher::RemoveShell( void)
{
	if( getOwner() != nullptr && ( getOwner()->isLocal() || getOwner()->isAI()) &&
		( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT))
	{
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
	}

	//if( m_nLoadedShell <= 0)
		//m_bLoadedShell = false;

	_PlayWeaponAnimWithApplyTimeScale( m_aiiRemoveShell );
	
	setWeaponState( WEAPON::STATE_LOADBULLET);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//		Animation Callback
void WeaponGrenadeLauncher::OnIdle( i3AIContext * pCtx, REAL32 tm)
{	
	bool bLoaded = true;

	if( getOwner() == nullptr )
		return;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	// 로컬 캐릭터의 무기가 아니라면 들어오면 안됩니다.
	if( !getOwner()->isLocal() && !getOwner()->isAI() )
		return;

	//2015-04-06 익스텐션 구조 변경으로 해당 조건으로 바꿉니다. (기존 isExtension() ) ENG_김대영
	if( isMultipleExtension() ? GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER : isExtensionActivate() == true )
	{
		if( getLoadedShellCount() <= 0)
		{
			if( getTotalShellCount() > 0)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
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
					setWeaponState( WEAPON::STATE_READY);
					removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_EXT_DISABLE);

					// 캐릭터의 익스텐션을 직접 호출하기 때문에 캐릭터의 상태를 초기화 해주어야 한다.
					// 캐릭터는 쉘 리무브 상태에서 무기 스왑을 막아놨다. 무기가 레디 상태로 바뀌면 초기화해줘야 한다.
					m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

					m_pOwner->Cmd_Extension( false);

					//	기획팀 요청으로 커스텀 M4의경우 유탄이 없으면 기본 무기상태로 자동으로 돌아간다.
					if( isMultipleExtension())		SwapExtensionType();
				}
			}

			addWeaponStateMask( WSM_FIRE_DISABLE);
		}
		else
		{
			if( m_bLoadedShell )
			{
				setWeaponState( WEAPON::STATE_READY);
				removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);
			}
		}
	}
	else
	{
		INT32 iBullet = getLoadedBulletCount();

		if( iBullet <= 0)
		{
			if( getTotalBulletCount() > 0)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
				removeWeaponStateMask( WSM_LOADMAG_DISABLE );
			}
			else
				setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

			addWeaponStateMask( WSM_FIRE_DISABLE);
			bLoaded = false;
		}
		else
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);

			if( iBullet < GetLoadBulletWithCashItem() || getLoadedBulletDualCount() < GetLoadBulletWithCashItem() )
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			else
				addWeaponStateMask( WSM_LOADMAG_DISABLE);

			if( isWeaponStateMask( WSM_FIRING) == false)
			{	// 발사중이 아니면
				if(getWeaponInfo()->GetPumpAction() == false)	//	펌프액션 방식이면
				{
					setWeaponState( WEAPON::STATE_READY);
					removeWeaponStateMask( WSM_FIRE_DISABLE);
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

				switch( autoCnt)
				{
					case 0 :		// 연발
						setWeaponState( WEAPON::STATE_NEEDTO_FIRE);

						removeWeaponStateMask( WSM_FIRE_DISABLE);

#if defined( FIREDELAY_COST)
						addWeaponStateMask( WSM_MORE_FIRING);

						// Idle 상태에서 발사 체크시 타이머를 계산한다.
						// 60fps에 맞춰진 오차를 계산하여 Idle 타임을 계산
						m_rFireTime = m_rFireTime - (0.016f - tm);
#endif
						break;

					case 1 :		// 단발
						setWeaponState( WEAPON::STATE_READY);

						addWeaponStateMask( WSM_FIRE_DISABLE);
						break;

					default :		// 점사
						// 연사 중...
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

void WeaponGrenadeLauncher::OnShellFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnShellFire( pCtx, tm);

	// extension Info가 필요합니다
	CWeaponInfo * pWeaponInfo = getExtensionWeaponInfo();

	if( pCtx->getLocalTime() == 0.0f)
	{
		I3ASSERT( m_pOwner != nullptr);
		I3ASSERT( getWeaponInfo() != nullptr);

		SetCurrentAttachHand();

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
				ThrowShell( nullptr, m_TargetPos, GetThrowSpeed(), -1 );
		}
	}
	else
	{
		REAL32 localTime = pCtx->getLocalTime();

		REAL32 rDelay = localTime + tm;

		if( pWeaponInfo->isOverFireDelay(rDelay))
		{// 발사 지연시간이 충분히 지났다면...

			if( getLoadedShellCount() > 0 )
			{	
				removeWeaponStateMask( WSM_FIRE_DISABLE);
			}
			else
			{
				if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
					addWeaponStateMask( WSM_FIRE_DISABLE);
			}

			_OnPlayWeaponIdleAnim();
		}
	}
}

void WeaponGrenadeLauncher::OnShellReload( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnShellReload( pCtx, tm);

	REAL32 localTime = pCtx->getLocalTime();

	if( pCtx->getCurrentAIState() == nullptr)
		return;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();
	
	if( localTime + tm >= pCtx->getCurrentAIState()->getDuration())
	{
		PB_VM_S

			setLoadedShellCount( 1);

			INT32 totalShell = getTotalShellCount();

			totalShell--;

			setTotalShellCount( totalShell);

		PB_VM_E

		m_bLoadedShell = true;
		_OnPlayWeaponIdleAnim();
	}
}

void WeaponGrenadeLauncher::OnShellRemove( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnShellRemove( pCtx, tm);

	REAL32 localTime = pCtx->getLocalTime();

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	if( pCtx->getCurrentAIState() == nullptr)
		return;
	
	if( localTime + tm >= pCtx->getCurrentAIState()->getDuration())
	{	
		if( getLoadedShellCount() <= 0)
			m_bLoadedShell = false;
		_OnPlayWeaponIdleAnim();
	}
}

void WeaponGrenadeLauncher::GetTargetDir( VEC3D * pDir)
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

void WeaponGrenadeLauncher::OnExtensionActivate( i3AIContext * pCtx, REAL32 tm)
{
	REAL32 rElaps = pCtx->getLocalTime() + tm;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	//	동작이 종료되면
	if( rElaps > 0.8f )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		addWeaponStateMask( WSM_ACTIVATE_EXTENSION);
	}
	else
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}
}

void WeaponGrenadeLauncher::OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm)
{
	REAL32 rElaps = pCtx->getLocalTime() + tm;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	//	동작이 종료되면
	if( rElaps > 0.8f )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_ACTIVATE_EXTENSION);

		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}
}

INT32 WeaponGrenadeLauncher::getTotalShellCount( void)
{
	return GlobalFunc::DecryptValue( &m_nTotalShell, m_u8EncryptKey);
}

void WeaponGrenadeLauncher::setTotalShellCount( INT32 nCount)
{
	m_nTotalShell = nCount;

	GlobalFunc::EncryptValue( &m_nTotalShell, m_u8EncryptKey);
}

INT32 WeaponGrenadeLauncher::getLoadedShellCount( void)
{
	return m_nLoadedShell;
	//return GlobalFunc::DecryptValue( &m_nLoadedShell, m_u8EncryptKey);
}

void WeaponGrenadeLauncher::setLoadedShellCount( INT32 nCount)
{
	m_nLoadedShell = nCount;

	//GlobalFunc::EncryptValue( &m_nLoadedShell, m_u8EncryptKey);
}

void WeaponGrenadeLauncher::GetNetBullet( GAMEINFO_BULLET* pBullet )
{
	pBullet->Reset();
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT,	(UINT8)getLoadedBulletCount() );
	pBullet->SetStandbyBulletCount( WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT,	(UINT16)getTotalBulletCount( ) );
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_ON, WEAPON::HAND_TYPE_LEFT, (UINT8)getLoadedShellCount() );
}

void WeaponGrenadeLauncher::SetNetBullet( GAMEINFO_BULLET* pBullet )
{	
	setLoadedBulletCount( pBullet->GetLoadBulletCount(		WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT ) );
	setTotalBulletCount( pBullet->GetStandbyBulletCount(	WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT ) );
	m_nLoadedShell = pBullet->GetLoadBulletCount( WEAPON::EXTENSION_ON, WEAPON::HAND_TYPE_LEFT );
}