#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaBase.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "PBLog.h"
#include "GameCharaDino.h"
#include "WeaponShotGun3.h"

I3_CLASS_INSTANCE( CGameCharaCommandContext, CGameCharaContextBase);

CGameCharaCommandContext::CGameCharaCommandContext()
{
}

CGameCharaCommandContext::~CGameCharaCommandContext()
{
	OnDestroy();
}

void CGameCharaCommandContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{	
	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaCommandContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();
}

void CGameCharaCommandContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();
}

void CGameCharaCommandContext::OnChange1PV()
{
	CGameCharaContextBase::OnChange1PV();

	
}

void CGameCharaCommandContext::OnChange3PV()
{
	CGameCharaContextBase::OnChange3PV();

	
}

void CGameCharaCommandContext::OnChangeLocal()
{
	CGameCharaContextBase::OnChangeLocal();

}

void CGameCharaCommandContext::OnChangeNetwork()
{
	CGameCharaContextBase::OnChangeNetwork();
	
}

void CGameCharaCommandContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);
}

BOOL CGameCharaCommandContext::Command( CHARA_COMMAND cmd)
{
	CGameCharaActionContext * pActionCtx = getActionContext();

	switch( cmd)
	{
		// default command
	case CHARA_CMD_RESET :
		{
			getCameraContext()->Command( CAMERA_CMD_IDLE);
			if(m_pOwner->IsDino() == false)
			{
				getWeaponContext()->Command( WEAPON_CMD_RETURNTOREADY);
			}
			return pActionCtx->Command( ACTION_CMD_RESET);
		}
		break;
	case CHARA_CMD_SETMARK :		break;
	case CHARA_CMD_VISIBLE :
		{
			m_pOwner->SetEnable( m_Param._bVal);
			getMoveContext()->EnableController( m_Param._bVal);
		}
		break;
	case CHARA_CMD_TEAMTAG :			getMaterialContext()->SetTeamMarkEnable( m_Param._bVal);	break;

	// action command
	case CHARA_CMD_CROUCH :				return pActionCtx->Command( ACTION_CMD_CROUCH);
	case CHARA_CMD_STANDUP :			return pActionCtx->Command( ACTION_CMD_STAND);
	case CHARA_CMD_RUN :
		{
			ActionParameter * param = pActionCtx->getParameter();
			i3Vector::Copy(&param->_dir, m_Param._pVec);
			return pActionCtx->Command( ACTION_CMD_RUNLOWER);
		}
		break;
	case CHARA_CMD_WALK :
		{
			ActionParameter * param = pActionCtx->getParameter();
			i3Vector::Copy(&param->_dir, m_Param._pVec);
			return pActionCtx->Command( ACTION_CMD_WALKLOWER);
		}
		break;
	case CHARA_CMD_JUMP :				return pActionCtx->Command( ACTION_CMD_JUMP_START);
	case CHARA_CMD_JUMPFINISH :
		{
			pActionCtx->Command( ACTION_CMD_JUMPLOWER_FINISH);
			getAnimContext()->setOldLowerID( ID_LOWER_IDLE);

			if( pActionCtx->isUpper( CHARA_UPPER_JUMP) || pActionCtx->isUpper( CHARA_UPPER_IDLE))
			{	
				pActionCtx->Command( ACTION_CMD_JUMPUPPER_FINISH);
				getAnimContext()->setOldUpperID( ID_UPPER_IDLE);
			}
		}
		break;
	case CHARA_CMD_PRIMATTACK :			return CmdPrimaryAttack();
	case CHARA_CMD_SECATTACK :
		{
			if(m_pOwner->IsDino()) {
				ActionParameter * param = pActionCtx->getParameter();
				param->_fireOrder = WEAPON_SECONDARY_FIRE;
				pActionCtx->Command( ACTION_CMD_SECATTACK);
				return TRUE;
			}
			else {
				getWeaponContext()->getParameter()->_pVec = m_Param._pVec;
				getWeaponContext()->getParameter()->_pTargetPos = m_Param._pVecTarget;
				if( getWeaponContext()->Command( WEAPON_CMD_SECFIRE))
				{
					ActionParameter * param = pActionCtx->getParameter();
					param->_fireOrder = WEAPON_SECONDARY_FIRE;
					pActionCtx->Command( ACTION_CMD_SECATTACK);
					return TRUE;
				}
			}

			
		}
		break;
	case CHARA_CMD_SPECIAL_ACTION:
		{
			if( m_pOwner->isLocal())	return CmdActiveDisarm();
		}
		break;
	case CHARA_CMD_ATTACKREADY :
		{
			if( getWeaponContext()->Command( WEAPON_CMD_FIREREADY))
				return pActionCtx->Command( ACTION_CMD_ATTACKREADY);
		}
		break;
	case CHARA_CMD_DAMAGE :									return CmdDamageChara();
	case CHARA_CMD_SWAPWEAPON :								return CmdWeaponChange();
	case CHARA_CMD_EXTACTIVATE :							return CmdExtensionActive();
	case CHARA_CMD_EXTDEACTIVATE :							return CmdExtensionDeactive();
	case CHARA_CMD_STOP :
		{
			pActionCtx->Command( ACTION_CMD_STEADYUPPER);
			pActionCtx->Command( ACTION_CMD_STEADYLOWER);
			return TRUE;
		}
		break;
	case CHARA_CMD_DEATH :									return CmdDeathChara();
	case CHARA_CMD_LOAD_MAGAZINE :
		{
			WeaponBase * pWeapon = getWeaponContext()->getCurrent();

			if( pWeapon != NULL)
			{
				if( pWeapon->IsExactTypeOf( WeaponShotGun3::GetClassMeta()) )
				{
					if( ((WeaponShotGun3*)pWeapon)->isReadyToLoadMagazine() == FALSE)
					{
						if( getWeaponContext()->Command( WEAPON_CMD_LOADMAG_READY))
							return pActionCtx->Command( ACTION_CMD_LOADMAG_READY);
						else
							return FALSE;
					}
				}
			}

			if( getWeaponContext()->Command( WEAPON_CMD_LOADMAG))
				return pActionCtx->Command( ACTION_CMD_LOADMAG);
		}
		break;
	case CHARA_CMD_LOAD_B_MAGAZINE :
		{
			if( getWeaponContext()->Command( WEAPON_CMD_LOADMAG))
			{
				return pActionCtx->Command( ACTION_CMD_B_LOADMAG);
			}
		}
		break;
	case CHARA_CMD_LOAD_MAGAZINE_DUAL:
		{
			if( getWeaponContext()->Command( WEAPON_CMD_LOADMAG_DUAL) )
				return pActionCtx->Command( ACTION_CMD_LOADMAG_DUAL);		//	무기 Extension 기능 활성화
		}
		break;
	case CHARA_CMD_LOAD_BULLET:
		{
			if( getWeaponContext()->Command( WEAPON_CMD_LOADBULLET))
				return pActionCtx->Command( ACTION_CMD_LOADBULLET);
		}
		break;
	case CHARA_CMD_REMOVE_SHELL :
		{
			if( getWeaponContext()->Command( WEAPON_CMD_REMOVESHELL))
				return pActionCtx->Command( ACTION_CMD_REMOVESHELL);
		}
		break;
	case CHARA_CMD_RESPAWN :								return CmdRespawnChara();
	case CHARA_CMD_FALLDAMAGE :								return CmdFallDamageChara();
	case CHARA_CMD_DROPWEAPON :
		{
			INT32 nSlot = m_Param._nSlotIdx;
			VEC3D * pDropPos = m_Param._pVec;

			getWeaponContext()->getParameter()->_nSlot = nSlot;
			getWeaponContext()->getParameter()->_pVec = pDropPos;
			getWeaponContext()->getParameter()->_bChangeNextWeapon = TRUE;

			BOOL bRv = getWeaponContext()->Command( WEAPON_CMD_DROP);
			return bRv;
		}
		break;
	case CHARA_CMD_DROPEQUIP :
		{
			I3ASSERT( m_Param._pSceneNode != NULL && m_Param._pOwner != NULL);

			getEquipContext()->getParameter()->_type = m_Param._EquipType;
			getEquipContext()->getParameter()->_pSceneNode = m_Param._pSceneNode;
			getEquipContext()->getParameter()->_pGameNode = m_Param._pOwner;
			getEquipContext()->getParameter()->_pVecPos = m_Param._pVec;

			return getEquipContext()->Command( EQUIP_CMD_DROP);
		}
		break;
	case CHARA_CMD_PROFILE_IDLE_A:
		{
			WeaponBase * pWeapon = getWeaponContext()->getCurrent();
			if( pWeapon != NULL)	
			{
				pWeapon->SetEnable( TRUE);
				pWeapon->PlayAnim_UI_IdleA();
			}
			return pActionCtx->Command( ACTION_CMD_PROFILE_IDLE_A);
		}
	case CHARA_CMD_PROFILE_IDLE_B:
		{
			WeaponBase * pWeapon = getWeaponContext()->getCurrent();
			if( pWeapon != NULL)
			{
				pWeapon->SetEnable( TRUE);
				pWeapon->PlayAnim_UI_IdleB();
			}
			return pActionCtx->Command( ACTION_CMD_PROFILE_IDLE_B);
		}
	case CHARA_CMD_PROFILE_CEREMONY :
		{
			WeaponBase * pWeapon = getWeaponContext()->getCurrent();
			if( pWeapon != NULL)	pWeapon->SetEnable( FALSE);
			if( pActionCtx->getCommon() == CHARA_ACTION_COMMON_CEREMONY) return FALSE;
			return pActionCtx->Command( ACTION_CMD_PROFILE_CEREMONY);
		}
	case CHARA_CMD_TOGGLEZOOM	:			return CmdToggleZoom();

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// only animation action command
	case CHARA_CMD_ONLYACTION_ATTACK :
		{
			ActionParameter * param = pActionCtx->getParameter();
			param->_fireOrder = m_Param._FireOrder;
			pActionCtx->Command( ACTION_CMD_ATTACK);
		}
		break;

	case CHARA_CMD_ONLYACTION_STEADYLOWER :
		{
			if( !pActionCtx->isLower( CHARA_LOWER_IDLE ))
				return pActionCtx->Command( ACTION_CMD_STEADYLOWER);
		}
		break;
	case CHARA_CMD_ONLYACTION_STEADYUPPER :
		{
			if(m_pOwner->IsDino())
			{
				if( pActionCtx->isUpper( CHARA_UPPER_IDLE) || pActionCtx->isUpper( CHARA_UPPER_MOVE))
						pActionCtx->Command( ACTION_CMD_STEADYUPPER);
			}
			else
			{
				// KEYWORD : WEAPON_DUALHANDGUN
				WeaponBase * pWeapon = getWeaponContext()->getCurrent();
				if( pWeapon != NULL && pWeapon->isDualGun() )
				{
					if( (pActionCtx->getUpper() == CHARA_UPPER_MOVE) || (pActionCtx->getUpper() == CHARA_UPPER_IDLE) )
						pActionCtx->Command( ACTION_CMD_RETURNUPPER_DUAL_RIGHT);

					if( (pActionCtx->getUpperLeft() == CHARA_UPPER_MOVE) || (pActionCtx->getUpperLeft() == CHARA_UPPER_IDLE) )
						pActionCtx->Command( ACTION_CMD_RETURNUPPER_DUAL_LEFT);
				}
				else
				{
					if( pActionCtx->isUpper( CHARA_UPPER_IDLE) || pActionCtx->isUpper( CHARA_UPPER_MOVE))
						pActionCtx->Command( ACTION_CMD_STEADYUPPER);
				}
			}
			
			
			//if( !pActionCtx->isUpper( CHARA_UPPER_IDLE))
			//	return pActionCtx->Command( ACTION_CMD_STEADYUPPER);
		}
		break;

	// animset change command
	case CHARA_CMD_ANIMSET :
		{
			if( m_Param._AnimSetType == CHARA_ANIMSET_1PV )		m_pOwner->ChangeFPSView();
			else												m_pOwner->ChangeObserverView();
			return TRUE;
		}
		break;
	case CHARA_CMD_ANIMSCALE_UPPER :	break;
	case CHARA_CMD_ANIMSCALE_LOWER :	break;

	// movement command
	case CHARA_CMD_MOVE :				break;
	case CHARA_CMD_SETPOS :				break;
	case CHARA_CMD_CLIMBING :			break;
	case CHARA_CMD_KEYMOVE :			break;

	// weapon command
	case CHARA_CMD_WEAPON_CREATE :
		{
			getWeaponContext()->getParameter()->SetCreateWeapon( m_Param._WeaponSlotNum, m_Param._WeaponType, m_Param._WeaponNumber);
			return getWeaponContext()->Command( WEAPON_CMD_CREATE);
		}
		break;
	case CHARA_CMD_WEAPON_DELETE :
		{
			getWeaponContext()->getParameter()->_nSlot = m_Param._nSlotIdx;
			return getWeaponContext()->Command( WEAPON_CMD_DELETE);
		}
		break;
	case CHARA_CMD_WEAPON_SELECT :
		{
			getWeaponContext()->getParameter()->_nSlot = m_Param._nSlotIdx;
			return getWeaponContext()->Command( WEAPON_CMD_SELECT);
		}
		break;
	case CHARA_CMD_WEAPON_DROP :
		{
			getWeaponContext()->getParameter()->_nSlot				= m_Param._nSlotIdx;
			getWeaponContext()->getParameter()->_pVec				= m_Param._pVec;
			getWeaponContext()->getParameter()->_bChangeNextWeapon	= m_Param._bVal;
			BOOL bRv = getWeaponContext()->Command( WEAPON_CMD_DROP);
			return bRv;
		}
		break;
	case CHARA_CMD_WEAPON_TAKE :
		{
			getWeaponContext()->getParameter()->_pWeapon = m_Param._pWeapon;
			return getWeaponContext()->Command( WEAPON_CMD_TAKE);
		}
		break;
	case CHARA_CMD_WEAPON_SWAP :
		{
			getWeaponContext()->getParameter()->_nSlot = m_Param._nSlotIdx;
			return getWeaponContext()->Command( WEAPON_CMD_SWAP);
		}
		break;
	case CHARA_CMD_WEAPON_FIRE :
		getWeaponContext()->getParameter()->_pVec		= m_Param._pVec;
		getWeaponContext()->getParameter()->_pTargetPos	= m_Param._pVecTarget;
		return getWeaponContext()->Command( WEAPON_CMD_FIRE);
	case CHARA_CMD_WEAPON_SECFIRE :
		getWeaponContext()->getParameter()->_pVec		= m_Param._pVec;
		getWeaponContext()->getParameter()->_pTargetPos	= m_Param._pVecTarget;
		return getWeaponContext()->Command( WEAPON_CMD_SECFIRE);
	case CHARA_CMD_WEAPON_EXTON :
		{	
			getWeaponContext()->getParameter()->_extensionOn = TRUE;
			return getWeaponContext()->Command( WEAPON_CMD_EXTENSION);
		}
		break;
	case CHARA_CMD_WEAPON_EXTOFF :
		{
			getWeaponContext()->getParameter()->_extensionOn = FALSE;
			return getWeaponContext()->Command( WEAPON_CMD_EXTENSION);
		}
		break;
	case CHARA_CMD_WEAPON_LOADMAG :			return getWeaponContext()->Command( WEAPON_CMD_LOADMAG);
	case CHARA_CMD_WEAPON_LOADBULLET :		return getWeaponContext()->Command( WEAPON_CMD_LOADBULLET);
	case CHARA_CMD_WEAPON_IDLE :			return getWeaponContext()->Command( WEAPON_CMD_RETURNTOREADY);

	// Equip command
	case CHARA_CMD_EQUIP_CREATE :
		{
			getEquipContext()->getParameter()->SetCreateEquip(m_Param._CharaResType, m_Param._EquipSubType, m_Param._EquipID, m_Param._EquipItemID);
			return getEquipContext()->Command( EQUIP_CMD_CREATE);
		}
		break;
	case CHARA_CMD_EQUIP_DELETE :	break;
	case CHARA_CMD_EQUIP_DROP :		break;
	case CHARA_CMD_EQUIP_TAKE :		break;

	case CHARA_CMD_DUMMY_ATTACK :
		{
			// KEYWORD : WEAPON_KNIFE	20080909. dfly79
			// 나이프의 공격 사운드를 직접 호출해줘야 합니다.
			// 일반 총기는 발사 애니메이션에서 호출이 되지만 나이프류는 컬리젼만 하기 때문에 사운드를 출력하지 않습니다.
			if(m_pOwner->IsDino() == false)
			{
				WeaponBase * pWeapon = getWeaponContext()->getCurrent();
				if( pWeapon != NULL)
				{
					pWeapon = pWeapon->getUsedWeapon();
					// KEYWORD : WEAPON_CIC
					if( pWeapon->IsExactTypeOf( WeaponDualCIC::GetClassMeta()))
						pWeapon = ((WeaponDualCIC*)pWeapon)->getLeftWeapon();
					
					// 예외 처리 무기
					// 무기 애니메이션이 있는 경우 처리합니다.
					if( pWeapon->IsExactTypeOf( WeaponThrowKnife::GetClassMeta()) ||
						pWeapon->IsExactTypeOf( WeaponShotGun3::GetClassMeta() ) )
						// 2009.11.10
						// Network M1887 근거리 공격시 무기 애니메이션을 세팅해야 함.
					{
						VEC3D vTargetPos;
						pWeapon->GetShotTargetPos( &vTargetPos);
						pWeapon->NET_Fire( m_Param._pVec, &vTargetPos, m_Param._FireOrder); 
					}
					else
					{
						// 나이프만 소리를 낸다.
						if( pWeapon->getWeaponInfo()->isMelee())
							pWeapon->PlayFireSoundByOrder( m_Param._FireOrder);
					}
				}
			}
			// 동작만 취하는 무기만 해당됩니다.
			ActionParameter * param = pActionCtx->getParameter();
			param->_fireOrder = m_Param._FireOrder;
			param->_WeaponSlot = m_Param._WeaponSlotNum;
			pActionCtx->Command( ACTION_CMD_ATTACK);
		}
		break;

	case CHARA_CMD_NET_FIRE :
		{

		}
		break;
	}

	return FALSE;
}

BOOL CGameCharaCommandContext::CmdCharaInvincible(BOOL bVal)
{
	if( g_pFramework->IsBattleStage() == FALSE)
		return FALSE;

	if( bVal )
	{
		//	캐릭터 무적 시간 계산
		REAL32 InvTime = g_pGameContext->getMissionManager()->GetInvincibleTime( g_pGameContext->getMyRoomInfo()->_StageID);

		////-----------------	무적시간 +1sec
		//	사용자가 사용중이면 무적시간 증가. (자신 사용)
		if( m_pOwner && g_pGameContext->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_INVINCIBLE_TIME))
		{
			//	무적시간 증가
			InvTime += (REAL32) VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME;
		}

		m_pOwner->setInvincibleTime( InvTime);
		m_pOwner->getMaterialContext()->SetBlendEnable( TRUE);
	}
	else
	{
		m_pOwner->setInvincibleTime( 0.f);
		m_pOwner->getMaterialContext()->SetBlendEnable( FALSE );
	}

	return TRUE;
}

BOOL CGameCharaCommandContext::CmdReturnToReady( BOOL bWithCurrentWeapon)
{
	getWeaponContext()->SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();

	if( pWeapon == NULL)										return FALSE;
	if( !pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()))		return FALSE;

	BOOL ret = FALSE;

	//	Idle 상태로 돌아간다.
	if( bWithCurrentWeapon )
		pWeapon->ReturnToReady();

	//	상체만 이전 상태로 돌아간다.
	ret = getActionContext()->Command( ACTION_CMD_RETURNUPPER);

	return ret;
}

BOOL CGameCharaCommandContext::CmdTurnBack( void)
{
	REAL32 rTheta = m_pOwner->getBoneContext()->getTheta();
	rTheta		+= I3_PI; 
	m_pOwner->getBoneContext()->setTheta( i3Math::NormalizeAngle( rTheta));

	m_pOwner->getMoveContext()->setVelocity( 0.0f, 0.0f, 0.0f);
	m_pOwner->getMoveContext()->setAccel( 0.0f, 0.0f, 0.0f);

	m_pOwner->getMoveContext()->setAccelElapsed( 0.f);

	MATRIX * pCharaMat = m_pOwner->getSceneObject()->GetMatrix();
	//MATRIX * pCharaMat = m_pOwner->getLocalMatrix();
	i3Matrix::PreRotateAxisDeg( pCharaMat, &I3_YAXIS, 180.0f);

	// Recoil 값에 대한 Restore 상태라면 그 상태를 해제해 주어야 한다.
	m_pOwner->getBoneContext()->cancelRecoilHorz();

	return TRUE;
}

BOOL CGameCharaCommandContext::CmdDeathChara( void)
{
	//	일반 카메라 상태 (캐릭터 Visible On, ScopeHUD off, Cam FOV default)
	getCameraContext()->Command( CAMERA_CMD_IDLE);

	if( m_pOwner->isLocal() )
	{
		getCameraContext()->getParameter()->_nEnemyIdx = m_Param._nUserIdx;
		getCameraContext()->Command( CAMERA_CMD_CHANGEKILL);
	}
	
	// 이동값을 0으로
	getMoveContext()->setVelocity( 0.0f, 0.0f, 0.0f);
	getMoveContext()->setAccel( 0.0f, 0.0f, 0.0f);

	if( m_pOwner->EFFECT_IsAvailable())
		m_pOwner->EFFECT_Clear();

	if(m_pOwner->IsDino() == false)
	{
		//	무기도 모든 동작을 종료하고 Ready 상태로
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		if( pWeapon != NULL)
		{
			pWeapon->ReturnToReady();
			pWeapon->SetEnable( TRUE); //SetVisible( TRUE);
		}
	}

	ActionParameter * param = getActionContext()->getParameter();
	param->SetDeathParam( m_Param._nUserIdx, m_Param._DeathType, m_Param._pVec);
	getActionContext()->Command( ACTION_CMD_DEATH);

	m_pOwner->addCharaStateMask( CHARA_STATE_DEATH);
	m_pOwner->getUpdateContext()->setToHideTime( 1.f);
	m_pOwner->getUpdateContext()->setHideTimer( 1.f);

	m_pOwner->ApplyCrosshair();

	return TRUE;
}

BOOL CGameCharaCommandContext::CmdExtensionActive( void)
{
	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeaponInfo == NULL )
		return FALSE;

	pWeapon->SetVisible( TRUE);

	switch( pWeapon->GetCurrentExtensionType())
	{
		case WEAPON_EXT_NONE:
		case WEAPON_EXT_SCOPE :
		case WEAPON_EXT_SCOPE2 :
		case WEAPON_EXT_DOT_SIGHT:
		case WEAPON_EXT_STEADY_AIM:
		case WEAPON_EXT_ZOOM :					// 견착
			{
				if(pWeaponInfo->isCameraZoomType())
				{
					if( getWeaponContext()->Command( WEAPON_CMD_ZOOM) )
					{
						return getActionContext()->Command( ACTION_CMD_ZOOM);
					}
				}
			}
			break;
		default:	//	무기 Extension 기능 활성화
			{
				//	무기 Extension 동작
				getWeaponContext()->getParameter()->_extensionOn = TRUE;
				if( getWeaponContext()->Command( WEAPON_CMD_EXTENSION) )
				{
					ActionParameter * param = getActionContext()->getParameter();
					param->_bool = TRUE;

					return getActionContext()->Command( ACTION_CMD_EXTACTIVATE);
				}
			}
			break;
	}

	return FALSE;
}

BOOL CGameCharaCommandContext::CmdExtensionDeactive( void)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeaponInfo == NULL )
		return FALSE;

	pWeapon->SetVisible( TRUE);

	switch( pWeapon->GetCurrentExtensionType())
	{
		case WEAPON_EXT_NONE:
		case WEAPON_EXT_SCOPE :
		case WEAPON_EXT_SCOPE2 :
		case WEAPON_EXT_DOT_SIGHT:
		case WEAPON_EXT_STEADY_AIM:
		case WEAPON_EXT_ZOOM :					// 견착
			{
				if(pWeaponInfo->isCameraZoomType())
				{
					return getActionContext()->Command( ACTION_CMD_ZOOMCANCLE);
				}
			}
			break;
		default:	//	무기 Extension 기능 비활성화
			{
				//	무기 Extension 동작
				getWeaponContext()->getParameter()->_extensionOn = FALSE;
				if( getWeaponContext()->Command( WEAPON_CMD_EXTENSION) )
				{
					ActionParameter * param = getActionContext()->getParameter();
					param->_bool = FALSE;
					return getActionContext()->Command( ACTION_CMD_EXTDEACTIVATE);
				}
			}
			break;
	}

	return FALSE;
}

BOOL CGameCharaCommandContext::CmdWeaponChange( void)
{
	if(m_pOwner->IsDino())
	{
		m_pOwner->SetDinoWeaponUsageType((WEAPON_USAGE_TYPE)m_Param._nSlotIdx);
		return TRUE;
	}
	else
	{
		//	Weapon Change
		getWeaponContext()->getParameter()->_nSlot = m_Param._nSlotIdx;
		if( getWeaponContext()->Command( WEAPON_CMD_SWAP))
		{
			if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{	//	견착 상태에서는 일반 카메라로 돌아갑니다
				getCameraContext()->Command( CAMERA_CMD_IDLE);
			}
			// Swap Action set
			ActionParameter * param = getActionContext()->getParameter();
			param->_idx = m_Param._nSlotIdx;
			getActionContext()->Command( ACTION_CMD_SWAPWEAPON);

			return TRUE;
		}

		return FALSE;
	}
	
}

BOOL CGameCharaCommandContext::CmdDamageChara( void)
{

	I3ASSERT( m_Param._pDamageInfo != NULL);

	if( m_pOwner->isInvincible() )
		return FALSE;

	//C4 설치중 취소하는 코드 입니다. 
	if( g_pGameContext->IsBombMissionMode() && (m_Param._pDamageInfo->_DeathType != CHARA_DEATH_POISON) )
	{
		g_pGameContext->getMissionManager()->Install( m_pOwner->getCharaNetIndex(), g_pGameContext->getMissionManager()->WhoInstallingArea( m_pOwner->getCharaNetIndex() ), FALSE );
		g_pGameContext->getMissionManager()->Uninstall( m_pOwner->getCharaNetIndex(), FALSE );

		if( m_pOwner->isLocal())
		{
			//	폭탄 설치 중이면 취소하도록
			if( m_pOwner->isCharaStateMask( CHARA_STATE_ARMINGBOMB) )				CmdCancelMission();
			//	폭탄 해체 중이면 취소하도록
			else if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )		CmdCancelDisarm();
		}
	}
	
	if( m_pOwner->isLocal() )
	{
		m_pOwner->ShowDamageHUD( m_Param._pDamageInfo->_pVecPos, m_pOwner->GetPos(), getBoneContext()->getTheta(),
			((m_Param._pDamageInfo->_DeathType == CHARA_DEATH_A) || (m_Param._pDamageInfo->_DeathType == CHARA_DEATH_H)) );
	}

	if(m_pOwner->IsDino() == false)
	{
		// Hit Effect
		if( ((CHARA_HIT_PART) m_Param._pDamageInfo->_HitPart == CHARA_HIT_HEAD) &&
			getEquipContext()->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) && (m_Param._pVec != NULL) )
		{	
			// 헬멧을 떨어뜨린다.
			if( m_Param._pDamageInfo->_WeaponType != WEAPON_CLASS_THROWING)
				getEquipContext()->DropToWorld( EQUIP_CLASS_TYPE_HELMET, m_Param._pVec );
		
			//	헬멧에 맞아 불꽃이 튄다.
			g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, m_Param._WeaponType,
				i3Matrix::GetPos( getBoneContext()->getHeadMatrix()), m_Param._pVec, m_pOwner->is1PV());
		}
		else
			m_pOwner->CharaHitEffect( m_Param._pDamageInfo );
	}
	else
	{
		m_pOwner->CharaHitEffect( m_Param._pDamageInfo );
	}

	bool bDamageAction = true;
	if( m_pOwner->IsRex())
	{
		if( m_Param._HitPart != CHARA_HIT_VITAL_EYE)
		{
			bDamageAction = false;
		}
	}

	// Damage Action
	if( m_Param._pDamageInfo->_DeathType != CHARA_DEATH_POISON)
	{	
		if( bDamageAction)
		{
			getActionContext()->getParameter()->_part = m_Param._HitPart;
			if( m_Param._pVec != NULL)		i3Vector::Copy( &getActionContext()->getParameter()->_dir, m_Param._pVec);
			else							i3Vector::Set( &getActionContext()->getParameter()->_dir, 0.f, 0.f, 1.f);
			getActionContext()->Command( ACTION_CMD_DAMAGE);
		}
	}
	else if( m_pOwner->is1PV() )
	{
		if( m_pOwner->isLocal() && (m_Param._pDamageInfo->_DeathType == CHARA_DEATH_POISON) &&
			m_pOwner->isCharaStateMask( CHARA_STATE_ARMINGBOMB | CHARA_STATE_DISARMINGBOMB) )
		{
			// 백린탄
			// 로컬이면서 폭파미션에서 설치하거나 해제중인경우는 액션을 취하지 않습니다.
		}
		else
		{
			if( bDamageAction)
			{
				getActionContext()->getParameter()->_part = m_Param._HitPart;
				if( m_Param._pVec != NULL)	i3Vector::Copy( &getActionContext()->getParameter()->_dir, m_Param._pVec);
				else						i3Vector::Set( &getActionContext()->getParameter()->_dir, 0.f, 0.f, 1.f);
				getActionContext()->Command( ACTION_CMD_DAMAGE);
			}
		}
	}

	// Head Watch system
	// 데미지를 주는 캐릭터를 주시하도록 세팅
	if( m_Param._pDamageInfo->_nKillerIdx > -1 && m_Param._pDamageInfo->_nKillerIdx < SLOT_MAX_COUNT)
		getBoneContext()->setHeadWatchUserIdx( m_Param._pDamageInfo->_nKillerIdx);

	return TRUE;
}

BOOL CGameCharaCommandContext::CmdRespawnChara( void)
{
	//g_pLog->Log(g_pGameContext->GetAccTime(), "CGameCharaCommandContext::CmdRespawnChara()\n");
	//	캐릭터 초기화
	m_pOwner->ResetChara();

	// 리스폰 카메라(Idle 상태의 FPS카메라로 세팅)
	getCameraContext()->Command( CAMERA_CMD_RESPAWN);

	m_pOwner->SetEnable( TRUE);

	// 나이트 비전을 꺼 준다.
	if(m_pOwner->isLocal())
	{
		g_pFramework->SetNightVisionEnable(FALSE);
	}
	
	if(m_pOwner->IsDino() == false)
	{
		//	처음 리스폰시 0번 무기로 교체하는 동작한다.
		getWeaponContext()->Command( WEAPON_CMD_RESPAWN);
	}

	// 앉아서 죽은 경우 앉은 상태의 컨트롤러가 세팅되어 있습니다.
	// 리스폰할시에 강제로 서 있는 상태로 세팅해줍니다.
	/*if( m_pOwner->isLocal() )
		getMoveContext()->ChangeControllerHeight( CAPSULE_FULL_HEIGHT );*/

	// 이팩트용 컨택트 박스를 Kinematic으로 변경
	getCollisionContext()->SetContactShapeSetEnable( FALSE);
	m_pOwner->forceSetPos( m_Param._pVec);		//	리스폰 위치 설정 (월드의 로컬 좌표)
	getMoveContext()->setTakeOffPos( m_Param._pVec);		// 캐릭터 리스폰시 월드가 높은 곳에 있으면 낙하처리가 됩니다.

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking )
	{
		MATRIX mat;
		VEC3D	vDir;
		i3Matrix::Identity( &mat);
		i3Matrix::SetRotateY( &mat, getBoneContext()->getTheta() );
		i3Vector::Copy( &m_pOwner->m_vDebugOriginalPos, m_Param._pVec );
		i3Vector::TransformCoord( &vDir, getMoveContext()->getMoveDir(), &mat );
		i3Vector::Add( &m_pOwner->m_vDebugTargetPos, m_Param._pVec, &vDir );
		m_pOwner->m_rDebugElp = 1.f;
	}
#endif

	// 리스폰 후 캐릭터 체크를 하여 걸리지 않도록 합니다.
	if( m_pOwner->isLocal() )
	{	
		UINT32 hitMask;
		VEC3D vLength;
		MATRIX mat;

		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++ )
		{
			CGameCharaBase * pOtherChara = g_pCharaManager->getChara( i );
			if( pOtherChara == m_pOwner )
				continue;
			
			i3Vector::Sub( &vLength, m_Param._pVec, pOtherChara->GetPos() );

			REAL32 rLength = i3Vector::Normalize( &vLength, &vLength );
			if( rLength > 1.f )
				continue;
			
			i3Matrix::Identity( &mat);
			i3Vector::Set( &vLength, 0.f, 0.f, -1.f );
			i3Matrix::SetRotateY( &mat, I3_PI * 0.12f * m_pOwner->getCharaNetIndex() );
			i3Vector::TransformCoord( &vLength, &vLength, &mat );

			getMoveContext()->getPhysixController()->Move( &vLength, 0.01f, &hitMask);
			getMoveContext()->GetPosition( &vLength);
			m_pOwner->forceSetPos( &vLength);
		}
	}
	else
	{
		// 캐릭터 마크 설정
		getMaterialContext()->SetTeamMarkEnable();
	}

	CmdCharaInvincible( TRUE);		//	캐릭터 무적 셋팅

	m_pOwner->removeCharaStateMask( CHARA_STATE_DEATH);
	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT );
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL | CHARA_STATE_DISABLE_MOVE );
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	m_pOwner->ApplyCrosshair();
	if( m_pOwner->isLocal() && m_pOwner->IsDino())
	{
		/*g_pFramework->SetVisionMaskingType( VE_DINO_EYE);
		g_pFramework->SetNightVisionEnable( TRUE);*/
	}

	if( g_pCamera != NULL )
	{	// 죽은 위치에서 한번 랜더링 되는 것을 막기 위해 업데이트를 한번 돌려서 애니메이션을 플레이 시킵니다.
		// 리스폰 위치로 옮기기
		getAnimContext()->OnUpdate( 0.016f);
		getUpdateContext()->OnUpdate( 0.016f);
	}

	return TRUE;
}

BOOL CGameCharaCommandContext::CmdFallDamageChara( void)
{
	//	Damage Direction
	if( m_pOwner->isLocal())
		m_pOwner->ShowDamageHUD( m_pOwner->GetPos(), m_pOwner->GetPos(), I3_PI, FALSE);

	getActionContext()->Command( ACTION_CMD_FALL_DAMAGE);

	return TRUE;
}

BOOL CGameCharaCommandContext::CmdPrimaryAttack( void)
{
	WeaponBase * pCurWeapon = m_pOwner->getWeaponContext()->getCurrent();
	if( pCurWeapon == NULL)	return FALSE;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != NULL);

	getWeaponContext()->getParameter()->_pVec		= m_Param._pVec;
	getWeaponContext()->getParameter()->_pTargetPos = m_Param._pVecTarget;

	if( getWeaponContext()->Command( WEAPON_CMD_FIRE) )
	{
		//	폭탄 설치일 경우
		if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION)
		{
			if( m_pOwner->isLocal())
				return CmdActiveMission(pCurWeapon);
			else
			{	// 네트워크이면 액션만 취해줍니다.
				Command( CHARA_CMD_ONLYACTION_ATTACK);
			}
		}
		//	일반 무기일 경우
		else
		{
			ActionParameter * param = getActionContext()->getParameter();
			param->_fireOrder = WEAPON_PRIMARY_FIRE;

			getActionContext()->Command( ACTION_CMD_ATTACK);
		}

		return TRUE;
	}
	else
	{
		if( pCurWeapon->GetCurrentExtensionType() == WEAPON_EXT_GLAUNCHER && pCurWeapon->isExtensionActivate() )
		{
			WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) getWeaponContext()->getCurrent();
			if( pGLauncher->getTotalShellCount() > 0)
				Command( CHARA_CMD_LOAD_MAGAZINE);
		}
		else if( pCurWeapon->GetCurrentExtensionType() == WEAPON_EXT_SHOTGUN_LAUNCHER && pCurWeapon->isExtensionActivate() )
		{
			WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) getWeaponContext()->getCurrent();
			if( pLauncher->getTotalShellCount() > 0)
				Command( CHARA_CMD_LOAD_MAGAZINE);
		}
		if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION)
		{
			if( getActionContext()->isUpper( CHARA_UPPER_FIRE) )
				g_pGameContext->getMissionManager()->SendMissionInstall( FALSE);
		}
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
	}

	return FALSE;
}

BOOL CGameCharaCommandContext::CmdToggleExtensionWeapon( void)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;
	
	if( !pWeapon->isExtensionActivate())
		return Command(CHARA_CMD_EXTACTIVATE);
	else
		return Command(CHARA_CMD_EXTDEACTIVATE);
}

BOOL CGameCharaCommandContext::CmdToggleZoom(void)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )		return FALSE;
	pWeapon = pWeapon->getUsedWeapon();

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL );

	WEAPON_ZOOM_TYPE type = pWeaponInfo->GetTypeZoom();

	//	견착 기능이 있는 무기
	switch( type)
	{
		case WEAPON_ZOOM_ZOOM :
		//case WEAPON_ZOOM_STEADY_AIM:
			if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				return Command(CHARA_CMD_EXTDEACTIVATE);		//	현재 견착일땐 취소
			else
				return Command(CHARA_CMD_EXTACTIVATE);			//	현재 보통일땐 견착
			break;

		case WEAPON_ZOOM_SCOPE :
			//	스코프 기능이 있는 무기
			if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				// if( getBoneContext()->getScopeWaveData()->m_nCurScopeLevel < pWeaponInfo->m_nMaxScopeLevel )
				if( getBoneContext()->getScopeWaveData()->m_nCurScopeLevel < pWeaponInfo->GetMaxScopeLevel() )
					return Command(CHARA_CMD_EXTACTIVATE);
				else
					return Command(CHARA_CMD_EXTDEACTIVATE);		//	현재 견착일땐 취소
			}
			else
				return Command(CHARA_CMD_EXTACTIVATE);				//	현재 보통일땐 견착
			break;

		case WEAPON_ZOOM_STEADY_AIM :
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				Command(CHARA_CMD_EXTACTIVATE);
			else
				Command(CHARA_CMD_EXTDEACTIVATE);
			break;

	}

	return FALSE;
}

BOOL CGameCharaCommandContext::CmdActiveMission( WeaponBase * pWeapon)
{
	I3ASSERT( pWeapon != NULL);
	
	CGameMissionManager * pMissionManager = g_pGameContext->getMissionManager();

	if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) || !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) )
		return FALSE;
	
	switch( pMissionManager->getMissionState() )
	{
	case GAME_MISSION_STATE_INBATTLE :					//	미션에 폭탄이 설치되어 있지 않다면
		{
			//	이동 불가. 
			i3Vector::Zero( getMoveContext()->getMoveDir());
			getMoveContext()->setAccel( 0.f, 0.f, 0.f);
			getMoveContext()->setVelocity( 0.f, 0.f, 0.f);

			getActionContext()->Command( ACTION_CMD_STEADYLOWER);
			
			getActionContext()->Command( ACTION_CMD_CROUCH);		// 설치시 강제로 앉는다.
			// 폭탄 설치!!
			{
				ActionParameter * param = getActionContext()->getParameter();
				param->_fireOrder = WEAPON_PRIMARY_FIRE;

				getActionContext()->Command( ACTION_CMD_ATTACK);	// 폭탄 설치!!
			}

			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
			m_pOwner->addCharaStateMask( CHARA_STATE_ARMINGBOMB);

			g_pGameContext->getMissionManager()->SendMissionInstall( TRUE );

			return TRUE;
		}
		break;

	case GAME_MISSION_STATE_INSTALLED :					//	이미 설치 됐다면
		{	
		}
		break;
		
	default :
		break;
	}

	g_pGameContext->getMissionManager()->SendMissionInstall( FALSE );

	return FALSE;
}

BOOL CGameCharaCommandContext::CmdActiveDisarm( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( (pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||		// 설치 후가 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )	// 방어팀이 아닌경우
	{
		return FALSE;
	}

	// 땅에 있지 않으면 취소한다.
	// if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) || !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))
	if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMP))
	{
		g_pGameContext->getMissionManager()->SendMissionUninstall( FALSE );
		return FALSE;
	}

	if( pMissionMng->IsEnableMy() == false)
		return FALSE;
	
	//	폭탄 해체	
	if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
		(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
	{	
		if( pMissionMng->IsEnableCrossHit() )
		{
			WeaponBase * pWeapon = NULL;

#if defined( USE_EVENT_SHUTTLEX)
			pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, GetItemIndex("MISSION_SHUTTLEX"));
#else
			pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, GetItemIndex("MISSION_C4"));
#endif

			if( pWeapon != NULL)
			{
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
				WeaponC4 * pBomb = (WeaponC4 *) pWeapon;

				m_pOwner->addCharaStateMask( CHARA_STATE_DISARMINGBOMB);

				pBomb->ActiveDisarm( TRUE);
			}
			else
			{
				m_pOwner->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
				m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
			}
		}
		else
		{
			m_pOwner->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
		}
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
	{	// 해체중
		//	이동 불가. 
		i3Vector::Zero( getMoveContext()->getMoveDir());

		getActionContext()->Command( ACTION_CMD_STEADYLOWER);

		// 해체시 강제로 앉는다.
		getActionContext()->Command( ACTION_CMD_CROUCH);

		if( getActionContext()->isUpper( CHARA_UPPER_MOVE))
			getActionContext()->Command( ACTION_CMD_STEADYUPPER);

		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->getMissionManager()->SendMissionUninstall( TRUE );		//	폭탄 해체

		return TRUE;
	}
	else
	{
//		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->getMissionManager()->SendMissionUninstall( FALSE );

		//I3TRACE( "-- 폭탄 해체취소...\n");

		return FALSE;
	}
}

BOOL CGameCharaCommandContext::CmdCancelMission( void)
{
#if defined( DEF_OBSERVER_MODE)
	// Observer는 처리하지 않는다.
	if( g_pGameContext->isObserverMe())
		return FALSE;
#endif

	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( g_pConfig->m_bNetworking )
	{
		if( (pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// 설치 전이 아닌경우
			(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// 공격팀이 아닌경우
			return FALSE;
	}
	
	if( pMissionMng->IsInstalling( mySlotIdx ) )
	{	//	자신이 폭탄 설치 중이면 취소
		g_pGameContext->getMissionManager()->SendMissionInstall( FALSE );	//	폭탄 설치 취소
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
		if( getCommandContext()->CmdReturnToReady( TRUE))
		{
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		}

		//I3TRACE("-- 폭탄 설치 취소!!\n");

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaCommandContext::CmdCancelDisarm( void)
{	//	폭탄 해체 중이면
#if defined( DEF_OBSERVER_MODE)
	// Observer는 처리하지 않는다.
	if( g_pGameContext->isObserverMe())
	{
		I3TRACE( "isObserverMe : call CmdCancelDisarm....\n");
		return FALSE;
	}
#endif

	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);
	
	if( (pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// 설치 후가 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// 방어팀이 아닌경우
		return FALSE;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
	{
		//	자신이 폭탄 해제 중이면 취소
		if( pMissionMng->IsUninstalling( mySlotIdx ))
		{			
			g_pGameContext->getMissionManager()->SendMissionUninstall( FALSE );		//	폭탄 해체 취소

			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

			m_pOwner->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);

			WeaponBase * pWeapon = NULL;

#if defined( USE_EVENT_SHUTTLEX)
			pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, GetItemIndex("MISSION_SHUTTLEX"));
#else
			pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, GetItemIndex("MISSION_C4"));
#endif

			if( pWeapon != NULL)
			{
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
				WeaponC4 * pBomb = ( WeaponC4*)pWeapon;

				pBomb->ActiveDisarm( FALSE);
			}

			//I3TRACE("-- 폭탄 해체 취소!!\n");

			return TRUE;
		}
	}

	return FALSE;
}

void CGameCharaCommandContext::CmdFireBulletForNetwork( void)
{
	REAL32 Theta, Rho;
	MATRIX mtx;
	VEC3D vPos, vDir, vFirePos;

	i3Vector::Normalize( &vDir, &vDir );

	// 무기가 맞는지 체크해봐야합니다.
	getWeaponContext()->WeaponValidCheck( m_Param._WeaponType, m_Param._WeaponNumber );
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon != NULL )
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		if( pWeaponInfo->GetTypeFire() == WEAPON_FIRE_MELEE )				// 칼.
		{
			// 애니메이션은 안 합니다. 이전의 액션을 취합니다.
			pWeapon->FireCollision( &vPos, m_Param._pVecTarget );	// 컬리젼을 직접한다.
			// 액션 상태 체크 액션을 취하고 있지 않으면 해줍니다.
			if( (!getActionContext()->isUpper( CHARA_UPPER_FIRE ) && !m_Param._FireOrder) ||
				(!getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) && m_Param._FireOrder))
			{	
				Command( CHARA_CMD_ONLYACTION_ATTACK);
			}
		}
		else if( (pWeaponInfo->GetTypeFire() == WEAPON_FIRE_THROWING) )		// 수류탄
		{
			// 애니메이션은 안 합니다. 이전의 액션을 취합니다.
			pWeapon->NET_Throw( &vPos, m_Param._pVecTarget, -1 );		// 직접 던지게 된다..
		}
		else if( pWeapon->IsExactTypeOf( WeaponBombTrigger::GetClassMeta() ))
		{	
			if( pWeapon->getWeaponState() == WEAPON_STATE_READY )
			{
				if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
				{// 캐릭터 액션을 취하는 경우
					// 무기 발사 액션은 내부에서 수행됩니다.
					if( m_Param._FireOrder == WEAPON_PRIMARY_FIRE)			Command( CHARA_CMD_PRIMATTACK);
					else if( m_Param._FireOrder == WEAPON_SECONDARY_FIRE)	Command( CHARA_CMD_SECATTACK);
				}
				else
				{// 캐릭터 액션 없이 무기 발사 액션만 취하는 경우
					if( m_Param._FireOrder == WEAPON_PRIMARY_FIRE)			Command( CHARA_CMD_WEAPON_FIRE);
					else if( m_Param._FireOrder == WEAPON_SECONDARY_FIRE)	Command( CHARA_CMD_WEAPON_SECFIRE);
				}
			}
		}
		else if( pWeapon->IsTypeOf( WeaponGrenadeLauncher::GetClassMeta()) &&
			(m_Param._FireOrder == WEAPON_SECONDARY_FIRE) )
		{
			// KEYWORD :  WEAPON_K201
			if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
			{	// 네트워크 3인칭의 경우 유탄 발사 애니메이션이 없습니다.
				// 기본 발사 애니메이션을 하도록 합니다.
				m_Param._FireOrder = WEAPON_PRIMARY_FIRE;
				Command( CHARA_CMD_ONLYACTION_ATTACK);
			}

			// 총구에서 발사해야 됩니다.
			i3Vector::Set( &vFirePos, -0.1f, -0.04f, -0.04f);
			i3Matrix::Identity( &mtx);
			Theta = i3Math::atan2( getX( &vDir), getZ( &vDir));
			Rho = i3Math::acos( getY( &vDir));
			i3Matrix::SetRotateY( &mtx, Theta );

			VEC3D vRight;
			i3Vector::Copy( &vRight, i3Matrix::GetRight( &mtx));
			//Rotate
			i3Matrix::PostRotateAxis( &mtx, &vRight, Rho);
			i3Vector::TransformCoord( &vFirePos, &vFirePos, &mtx);
			i3Vector::Add( &vFirePos, &vFirePos, &vPos);

			((WeaponGrenadeLauncher*)pWeapon)->ThrowShell( &vFirePos, &vDir, pWeapon->getExtensionWeaponInfo()->GetThrowSpeed(), -1);
		}
		else
		{// Fire !!
			if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
			{// 캐릭터 액션을 취하는 경우
				// 무기 발사 액션은 내부에서 수행됩니다.
				Command( CHARA_CMD_PRIMATTACK);
			}
			else
			{// 캐릭터 액션 없이 무기 발사 액션만 취하는 경우
				Command( CHARA_CMD_WEAPON_FIRE);
			}
			
			// 컬리젼
			pWeapon->FireCollision( &vPos, m_Param._pVecTarget);

			// KEYWORD : WEAPON_C4
			if( pWeapon->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION )
			{
				if( !getActionContext()->isLower( CHARA_LOWER_IDLE ))
					Command( CHARA_CMD_ONLYACTION_STEADYLOWER);
			}
		}
	}
}



