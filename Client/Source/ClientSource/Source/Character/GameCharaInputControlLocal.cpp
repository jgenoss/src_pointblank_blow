#include "pch.h"
#include "GameCharaInputControlLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GameCharaDino.h"
#include "GameEvent.H"
#include "GameStateMgr.h"
#include "GameMode.h"
#include "BattleHud.h"
#include "BattleHUD_CountDown.h"

#include "../Mode/Domination/GameMode_Domination.h"
#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "UserInfoContext.h"
#include "GameMissionManager.h"

#include "UI/UIHudManager.h"
#include "UI/UIHudMultiWeapon.h"

#include "MainWeapon_DualCIC.h"
#include "GameMaster.h"
#include "CQAWeaponCMgr.h"

#include "BattleServerContext.h"
#include "BattleObjContext.h"

#define THROW_DELAY_TIME	4.0f

I3_CLASS_INSTANCE( CGameCharaInputControlLocal);//, CGameCharaInputControl);

CGameCharaInputControlLocal::CGameCharaInputControlLocal() : m_fTimeLine_Throw(THROW_DELAY_TIME)
{
}

void CGameCharaInputControlLocal::OnFirstUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnFirstUpdate( rDeltatime);

	if( m_pOwner == nullptr)		return;

	ProcessInput( rDeltatime);
}

void CGameCharaInputControlLocal::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnUpdate( rDeltatime);
	// Add Code here
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
	{
		m_fTimeLine_Throw = THROW_DELAY_TIME;
		m_bThrowBomb = false;
	}
}

void CGameCharaInputControlLocal::OnLastUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnLastUpdate( rDeltatime);
}

void CGameCharaInputControlLocal::SetMoveDir( VEC3D * pVecOut)
{
	UINT64 keyPress = getInputContext()->getKeyPressInput();

	if( keyPress & GAME_KEY_MOVE_FORWARD )			setZ( pVecOut, 1.f);
	else if( keyPress & GAME_KEY_MOVE_BACKWARD)		setZ( pVecOut, -1.f);
	else											setZ( pVecOut, 0.f);

	if( keyPress & GAME_KEY_MOVE_LEFT )				setX( pVecOut, 1.f);
	else if( keyPress & GAME_KEY_MOVE_RIGHT )		setX( pVecOut, -1.f);
	else											setX( pVecOut, 0.f);
}

void CGameCharaInputControlLocal::ProcessDropWeapon( REAL32 rDeltatime)
{
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	if(m_pOwner->IsDino())
		return;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) ) 
		return;

	if( keyStroke & GAME_KEY_ACTION_DROP )
	{
		if ( m_pOwner->getMultiWeaponContext()->IsDropProhibit() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			return;

		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) && !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_DROPWEAPON))
		{
			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
			if( pWeapon != nullptr)
			{
				if ( HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == true && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
					pHud->SetEnable( false);
				}

				GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, getWeaponContext()->getCurrentSlotNum(), pWeapon->getLoadedBulletCount(),
					pWeapon->getLoadedBulletDualCount(), pWeapon->getTotalBulletCount());
			}
			
			// 무기를 버리고 0.5초간 발사를 못하도록 세팅합니다.
			m_pOwner->setDropWeaponTimer( 0.5f);
		}
	}
}

bool CGameCharaInputControlLocal::ProcessCrouch( REAL32 rDeltatime)
{
	bool bChangeBody = false;
	UINT64 keyPress = getInputContext()->getKeyPressInput();

	if( keyPress & GAME_KEY_ACTION_CROUCH  )
	{	
		// 앉기
		bChangeBody = m_pOwner->Cmd_Crouch();
	}
	else
	{	// 일어서기
		bChangeBody = m_pOwner->Cmd_StandUp();
	}


	return bChangeBody;
}

// -------------------------------------  이동  ----------------------------------------------------
void CGameCharaInputControlLocal::ProcessMovement( REAL32 rDeltatime, bool bChangeBody)
{	
	VEC3D vOldDir;

	UINT64 keyPress = getInputContext()->getKeyPressInput();

	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaMoveContext * pMoveCtx = getMoveContext();

	VEC3D * pMoveDir = pMoveCtx->getMoveDir();

	bool bHaveUpperPlay = false;
	CHARA_ACTION_UPPER nActionUpper = pActionCtx->getUpper();
	if( (nActionUpper == CHARA_UPPER_IDLE) || (nActionUpper == CHARA_UPPER_MOVE) )
		bHaveUpperPlay = true;

	if(m_pOwner->IsDino() == false)
	{
		WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
		if( pWeapon != nullptr)
		{
			if( pWeapon->isDualGun() ||
				( i3::same_of<WeaponKrissSuperV*>(pWeapon)
				&& ((WeaponKrissSuperV*)pWeapon)->isUsedDualWeapon()) )
			{
				CHARA_ACTION_UPPER nActionUpper_Left = pActionCtx->getUpperLeft();
				if( (nActionUpper_Left == CHARA_UPPER_IDLE) || (nActionUpper_Left == CHARA_UPPER_MOVE) )
					bHaveUpperPlay = true;
			}
		}
	}

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	bool jump_move_enable = false;
	jump_move_enable = MyRoomInfoContext::i()->getStage()->IsSpace();
	
	if( jump_move_enable ||												//  점프 중 이동이 가능하면?
		(!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) &&		//	이동이 가능하면?
		!pActionCtx->isLower( CHARA_LOWER_JUMP)  &&						//	점프중이 아니면?
		!bIsHackUser) )													//  치팅 유저 아니면
	{
		if( (keyPress & GAME_KEY_MOVE_LEFT) || (keyPress & GAME_KEY_MOVE_RIGHT)
			|| (keyPress & GAME_KEY_MOVE_FORWARD) || (keyPress & GAME_KEY_MOVE_BACKWARD))
		{
			i3Vector::Copy( &vOldDir, pMoveDir);

			SetMoveDir( pMoveDir);	//	입력키에 따라 이동 방향 지정

			REAL32 rSpeed = pMoveCtx->getSpeed();
			REAL32 rDist = i3Vector::Length( pMoveCtx->getVelocity());
			REAL32 rRet = rDist - rSpeed;

			if( m_pOwner->is1PV())
			{
				//	같은 키에서 다시 동작하는 것을 막는다 (입력 방향이 바뀌면)
				if( (vOldDir.x != getX( pMoveDir)) || (vOldDir.y != getY( pMoveDir)) ||	(vOldDir.z != getZ( pMoveDir)) ||
					(i3Math::abs( rRet) > 0.05f) )
				{
					if( keyPress & GAME_KEY_ACTION_FOOTSTEP )
					{	if (!m_pOwner->IsTank())
						m_pOwner->Cmd_Walk( pMoveDir);//	걷기
						else
						m_pOwner->Cmd_Run( pMoveDir);	//	뛰기

					}
					else
						m_pOwner->Cmd_Run( pMoveDir);	//	뛰기

					/*if( bHaveUpperPlay)
						pActionCtx->_Play_Upper_Run();*/
				}
				else
				{	//	뛰는 중에 걷기를 누르면
					if( pActionCtx->isLower( CHARA_LOWER_RUN))
					{
						if( keyPress & GAME_KEY_ACTION_FOOTSTEP )
						{	
							
							if (!m_pOwner->IsTank())
							m_pOwner->Cmd_Walk( pMoveDir);		//	걷기로 전환
							else
							m_pOwner->Cmd_Run( pMoveDir);

							/*if( !bChangeBody && bHaveUpperPlay )
								pActionCtx->_Play_Upper_Run();*/
						}
					}
					else if( pActionCtx->isLower( CHARA_LOWER_WALK))
					{
						if( (keyPress & GAME_KEY_ACTION_FOOTSTEP) == 0 )
						{	
							m_pOwner->Cmd_Run( pMoveDir);		//	뛰기로 전환

							/*if( !bChangeBody && bHaveUpperPlay )
								pActionCtx->_Play_Upper_Run();*/
						}
					}

					/*if( bChangeBody && bHaveUpperPlay)
						pActionCtx->_Play_Upper_Run();*/
				}
			}
			else
			{
				if( keyPress & GAME_KEY_ACTION_FOOTSTEP )
				{
					if (!m_pOwner->IsTank())
					{
						m_pOwner->Cmd_Walk( pMoveDir);	//	걷기
					}
					else
						m_pOwner->Cmd_Run( pMoveDir);	//	뛰기
				}
				else
					m_pOwner->Cmd_Run( pMoveDir);	//	뛰기

				/*if( bHaveUpperPlay)
					pActionCtx->_Play_Upper_Run();*/
			}
		}	
	}

	///////////////////////////////////////////////////////////		정지
	if( !(keyPress & GAME_KEY_MOVE_LEFT) && 
		!(keyPress & GAME_KEY_MOVE_RIGHT) && 
		!(keyPress & GAME_KEY_MOVE_FORWARD) && 
		!(keyPress & GAME_KEY_MOVE_BACKWARD) || 
		m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) )
	{	
		//	이동 중에 정지
		if( pActionCtx->isLower( CHARA_LOWER_RUN) || pActionCtx->isLower( CHARA_LOWER_WALK))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}
		else	// revision 29547 듀얼 핸드건 이동 사격중 정지시 애니메이션 문제 수정
		{
			m_pOwner->Cmd_Idle_Upper_Move();
		}
	}
}

void CGameCharaInputControlLocal::ProcessDinoAttack( REAL32 rDeltatime)
{

	//앉아있는경우 공격 처리를 하지 않습니다.
	if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
	{	
		getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	}


	bool bCancelMission = false;	//	미션 행동 취소 여부
	bool bCancelBooster = false;	//  가속 입력 취소 여부 // revision 60267 (Hansoft 5252) 랩터 공룡 가속 버튼 클릭 상태에서 킬하면 킬마크가 물기 이미지로 나오는 버그 수정

	UINT64 keyPress = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->SetNowAttackFirstFrame(false);

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	if( (keyPress & GAME_KEY_ACTION_ATTACK) && !bIsHackUser )
	{
		// revision 60267, 63436 (러시아 Hansoft 330) 티라노 하울링 버튼 누른채로 물기 공격 시 하울링 공격 범위까지 공격 가능한 버그 수정
		if (m_pOwner->IsCCRaptor() || m_pOwner->IsRaptor() || m_pOwner->IsRex())
		{
			bCancelBooster = true;
		}

		if( (m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false) && (m_pOwner->isDeathblowState() == false))
		{
			if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
			{	
				getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
			}

			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::PRIMARY_FIRE);
		}
	}
	// revision 60267
	// 발사 클릭후 버튼을 떼면 (Key Stroke 시점)
	if( (keyPress & GAME_KEY_ACTION_ATTACK) == 0)
	{
		bCancelBooster = false;
	}

	////////////////////////////////////////////////////////////	보조 공격	
	if( (keyPress & GAME_KEY_ACTION_SUBFUNC) && !bIsHackUser && !bCancelBooster)	//	<- Pressed 다
	{
		if((m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false ) && (m_pOwner->isDeathblowState() == false))
		{
			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::SECONDARY_FIRE);
		}
	}

	////////////////////////////////////////////////////////////	무기 교체	
	CHud * pHud = CHud::i();
	
	if( pHud != nullptr)
	{
		if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE) && !pHud->IsRadioChat() )
		{
			if(! g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
			{
				if( keyStroke & GAME_KEY_CHANGEWEAPON1 )
				{
					bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY);
				}
			}
		}
	}
}

#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
static bool s_bFiring = false;
static REAL32 s_rFireTime = 0.f;
#endif

void CGameCharaInputControlLocal::ProcessWeapon( REAL32 rDeltatime)
{	
	if(m_pOwner->IsDino())
	{
		return;
	}

	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr)	return;
	WeaponBase * pFireWeapon = pCurWeapon->getUsedWeapon();
	I3ASSERT( pFireWeapon != nullptr);
	CWeaponInfo * pCurWeaponInfo = pFireWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	WEAPON::eSTATE	weaponState = pFireWeapon->getWeaponState();

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	
	//스왑을 할 경우 bBowFireOrder를 False해주는 부분이 없어서 유탄발사 버그가 있어 예외처리합니다. 
	WEAPON_CLASS_TYPE weaponClass = pFireWeapon->getWeaponInfo()->GetTypeClass();
	if( weaponClass != WEAPON_CLASS_BOW && weaponClass != WEAPON_SUBCLASS_BOW )
	{
		UserContext::i()->bBowFireOrder = false;
	}

	bool bCancelMission = false;	//	미션 행동 취소 여부
	bool bCancelDisarm = false;		//	폭탄 해체 취소 여부

	UINT64 keyPress = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	// 수류탄 연속 던지지 못하도록 딜레이 계산
	if( m_bThrowBomb ) 
		m_fTimeLine_Throw -= rDeltatime;
	if( m_fTimeLine_Throw < 0.0f) 
	{
		m_fTimeLine_Throw = THROW_DELAY_TIME;
		m_bThrowBomb = false;		
	}

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	BOOL bMultiWeapon = TRUE;

	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

		I3ASSERT( pHud != nullptr);

		if ( pHud->IsVisible() && !pHud->GetAutoView())
		{
			bMultiWeapon = FALSE;
		}
	}

	if ( ( keyPress & GAME_KEY_ACTION_ATTACK) && !bIsHackUser)
	{
		UIHudMultiWeapon* pHud = nullptr;
		pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && HUD::instance()->IsEnableHud(HUD::MULTIWEAPON))
		{
			// 선택한 다중무기슬롯이 현재 장착한 다중무기의 슬롯과 다를 경우 무기 교체
			if ( m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo()->nSlotNumber != pHud->GetCurrentSlotIdx())
			{
				// HUD는 선택만 할 뿐 실제 교체는 나중에 진행함

				// 빠른무기교체에서 사용하기 위해 교체 전 Index를 저장 - 다른곳에서 일괄 처리 가능한지 확인 필요
				m_pOwner->getMultiWeaponContext()->SetQuickChangeIdx();
				m_pOwner->getMultiWeaponContext()->HasHUDSelectedChange();

				m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY);
			}

			pHud->SetEnableFire( FALSE);
			pHud->SetEnable( false);
		}
		else
		{
			PB_VM_S

				// 발사 가능(애니메이션만 할수 있는 무기류)
				bool bJustFire = false;

			/*if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && pHud->GetEnableFire() == FALSE)
			{
				REAL32 fTimer = m_pOwner->getMultiWeaponTimer() + 0.1f;

				if ( m_pOwner->getMultiWeaponTimer() > 1.0f)
				{
					pHud->SetEnableFire( TRUE);
					fTimer = 0.f;
				}
				m_pOwner->setMultiWeaponTimer(fTimer);
			}*/

			if ( CQAWeaponCMgr::i()->getOnWeaponCollision())
			{
				ProcessWeaponCollisionFire(rDeltatime);
				return;
			}

			if (i3::kind_of<WeaponGrenadeLauncher*>(pFireWeapon) ||
				i3::kind_of<WeaponBow*>(pFireWeapon))
			{
				if (pFireWeapon->isExtensionActivate())
					bJustFire = true;
			}
			else if (i3::same_of<WeaponBombTrigger*>(pFireWeapon))
				bJustFire = true;
			else if (pCurWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MELEE)
				bJustFire = true;

			const bool isDisableShot = m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_SHOT);
			const bool isFireState = pFireWeapon->isWeaponStateMask(WSM_FIRING);

			if ((!isDisableShot && !isFireState) &&
				(pFireWeapon->hasLoadedBullet() || bJustFire) &&
				(m_pOwner->getDropWeaponTimer() < 0.f))// &&	// 드랍 웨폰 타이머가 있다면 발사를 못하도록 합니다.
				//(m_pOwner->getMultiWeaponTimer() <= 0.f))	// 다중 무기 타이머가 있다면 발사를 못하도록 합니다.
			{
				if (pFireWeapon->isFireEnabled())
				{
#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
					s_bFiring = true;
#endif
					PB_VM_E

						//	Mission 무기 예외처리
						if (i3::same_of<WeaponC4*>(pFireWeapon))
						{//C4처리하는 코드 입니다. 구조상 이상합니다. 
							//	행동
							if (!m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR))
							{
								//설치를 하는 동작입니다. 
								if (m_pOwner->Cmd_Attack())
									bCancelMission = false;
							}
						}
						else
						{
							//	폭탄 해체 중일땐 발사 불가
							if (m_pOwner->isCharaStateMask(CHARA_STATE_DISARMINGBOMB) == false)
							{
								//	무기 발사!!		(투척 무기만 발사준비)
								switch (pCurWeaponInfo->GetTypeClass())
								{
								case WEAPON_CLASS_THROWING_GRENADE:
								case WEAPON_CLASS_THROWING_CONTAIN:
								case WEAPON_CLASS_THROWING_ATTACK:
								case WEAPON_CLASS_THROWING_HEAL:
									//case WEAPON_CLASS_BOW:
								{
									if ( !pFireWeapon->isWeaponStateMask( WSM_FIRING))
									{
										if ( pCurWeaponInfo->GetUseThrowDelay())
										{
											if ( m_fTimeLine_Throw >= THROW_DELAY_TIME )
											{
												bCancelDisarm = true;
												m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
											}
										}
										else
										{
											bCancelDisarm = true;
											m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
										}
									}
								}
								break;
								default:
								{
									bCancelDisarm = true;
									m_pOwner->Cmd_Attack();
								}
								break;
								}
							}
						}
				}
				// 장전중 발사 가능하면 장전 동작을 해야 하는데 어쩐 이유로 너무 오랬동안 스킵되어
				// 이미 유저들은 스킵으로 인식 주석처리 합니다.
				//else if( m_pOwner->isCharaStateMask(CHARA_STATE_RELOAD)
				//			&& pFireWeapon->getWeaponInfo()->GetTypeTrigger() == WEAPON::TRIGGER_RELOADING_SHOT 
				//			&& pFireWeapon->getWeaponInfo()->GetLoadBulletType() != 1)
				//{	// 장전중 발사 가능하면 탄약 장전 동작을 취한다(샷건)
				//	
				//	bCancelMission = m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);
				//}
			}
			else
			{
				// 자동으로 장전을 하는 경우
				switch ( weaponState)
				{
				case WEAPON::STATE_NEEDTO_LOADMAG:
				{
					ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

					if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
						getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
						nLoadType = ACTION_LOAD_READY_MAG;

					// revision 59544  P99 & HAK RMB 공격중 (자동)장전 안되도록 수정. hansoft.3655
					bool enable_loadmag = true;
					if (i3::same_of<WeaponDualCIC*>(pCurWeapon))
					{
						MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCurWeapon->GetMainWeapon());
						enable_loadmag = mainWeapon_DualCIC->getCurrentUseWeapon() != WEAPON_CIC_CURRENTUSE_LEFT;
					}
					m_pOwner->SetLoadMagChg(false);
					if (enable_loadmag)
						bCancelMission = m_pOwner->Cmd_Reload(nLoadType);

#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
					if (s_bFiring)
					{
						I3TRACE("Fire Time : %.4f\n", s_rFireTime);
						s_rFireTime = 0.f;
						s_bFiring = false;
					}
#endif
				}
				break;
				case WEAPON::STATE_NEEDTO_LOADBULLET:
				{
					if (pCurWeapon->IsFireBulletWeapon())
					{
						bCancelMission = m_pOwner->Cmd_Reload(ACTION_LOAD_FIRE_BULLET);
					}
					else
					{
						bCancelMission = m_pOwner->Cmd_Reload(ACTION_LOAD_MAG_BULLET);
					}
				}
				break;
				case WEAPON::STATE_NEEDTO_REMOVESHELL:
				{
					bCancelMission = m_pOwner->Cmd_Reload();
				}
				break;
				}
			}
		}
	}
	else if( (keyPress & GAME_KEY_ACTION_ATTACK) == 0 )	//	발사 클릭후 버튼을 떼면 (Key Stroke 시점)
	{
		if ( !pFireWeapon->isWeaponStateMask(WSM_POINTSHOTING) )
		{
			//WSM_POINTSHOTING 루틴 내에서 모든 발사가 완료된 후 WSM_POINTSHOTING 마스크를 지우고 해당 부분에서 초기화 되도록 하자. 
			//한번 클릭하면 무조건 설정된 발사수가 다 나가야되므로.
			//따라서, WSM_POINTSHOTING 중 강제로 루틴을 바꿀 경우 WSM_POINTSHOTING 마스크를 지워야 한다.
			pFireWeapon->FireEnd();		//	발사 종료 시점을 무기에 알려준다.
		}

		bCancelMission = true;

		//	투척 무기일 경우 버튼을 놓으면 던진다.
		if (pFireWeapon->isWeaponStateMask(WSM_FIRING))
		{
			switch (pCurWeaponInfo->GetTypeClass())
			{
			case WEAPON_CLASS_THROWING_GRENADE:
			case WEAPON_CLASS_THROWING_CONTAIN:
			case WEAPON_CLASS_THROWING_ATTACK:
			case WEAPON_CLASS_THROWING_HEAL:
				//case WEAPON_CLASS_BOW:
			{
				if (getActionContext()->isUpper(CHARA_UPPER_FIRE_READY) &&
					!m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_SHOT) && pFireWeapon->getLoadedBulletCount() > 0)
				{
					if (pCurWeaponInfo->GetUseThrowDelay())
					{
						m_bThrowBomb = true;
						bCancelDisarm = m_pOwner->Cmd_Attack();
					}
					else
					{
						if (UserContext::i()->bBowFireOrder)
						{
							bCancelDisarm = m_pOwner->Cmd_Attack(WEAPON::SECONDARY_FIRE);
							UserContext::i()->bBowFireOrder = false;
						}
						else
							bCancelDisarm = m_pOwner->Cmd_Attack();
					}
				}
			}
			break;
			}
		}
	}
	
#if defined( FIREDELAY_COST) && defined( I3_DEBUG)
	if( s_bFiring )
	{
		s_rFireTime += rDeltatime;
	}
#endif

	//총알이 모자라서 빈상태인경우 격발시도를 하게되면 나는 소리
	if (keyStroke & GAME_KEY_ACTION_ATTACK && bMultiWeapon)
	{
		if ( ( weaponState == WEAPON::STATE_EMPTY_MAGAZINE))
		{
			if ( pCurWeaponInfo->isGun())
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand() % 2, m_pOwner->GetPos(), true);
			}
		}
		else if ( pFireWeapon->isExtensionActivate() &&
			i3::same_of<WeaponBow*>(pFireWeapon) &&
			(((WeaponBow*)pFireWeapon)->getTotalShellCount() <= 0) &&
			(((WeaponBow*)pFireWeapon)->getLoadedShellCount() <= 0))
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand() % 2, m_pOwner->GetPos(), true);
			m_pOwner->Cmd_Extension( false);
		}
	}

	bool bOnSubAttack = true;
	WEAPON_CLASS_TYPE weapon_class_type = pCurWeaponInfo->GetTypeClass();
	if( bIsHackUser && 
		(weapon_class_type == WEAPON_CLASS_KNIFE || weapon_class_type == WEAPON_CLASS_KNUCKLE ||
		weapon_class_type == WEAPON_CLASS_DUALKNIFE) )
	{
		bOnSubAttack = false;
	}

	////////////////////////////////////////////////////////////	보조 공격	
	if( keyPress & GAME_KEY_ACTION_SUBFUNC && bOnSubAttack && bMultiWeapon)	//	<- Pressed 다
	{
		bool bEnableWeapon = false;
		bool bMustFire = false;

		switch( pCurWeaponInfo->GetTypeClass())
		{
		case WEAPON_CLASS_KNIFE:			bEnableWeapon = true;			break;
		case WEAPON_CLASS_KNUCKLE:			bEnableWeapon = true;			break;
		// KEYWORD : WEAPON_C5
		case WEAPON_CLASS_BOMBTRIGGER :
			if( pFireWeapon->getLoadedBulletCount() > 0 )	// 탄환이 있는 경우 가능
				bEnableWeapon = true;
			break;
		case WEAPON_CLASS_DUALKNIFE :
		case WEAPON_CLASS_CIC :
			bEnableWeapon = true;
			bMustFire = true;			// revision 32315 CIC 탄환이 0이어도 강제 공격
			break;
		case WEAPON_CLASS_THROWING_GRENADE:
			if( i3::same_of<WeaponM14*>(pFireWeapon) == true) bEnableWeapon = true;
			break;
		case WEAPON_CLASS_DUALSHOTGUN :		bEnableWeapon = true;			break;
		//case WEAPON_CLASS_BOW :
		//	{
		//		if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) == false)
		//		{
		//			if( !pFireWeapon->isWeaponStateMask( WSM_FIRING))
		//			{
		//				{										
		//					bCancelDisarm = true;
		//					UserContext::i()->bBowFireOrder = true;
		//					
		//					m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
		//				}
		//			}
		//		}

		//	}
		//	break;

		default :
			if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET ||
				pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_DIRECT_MELEE )
			{
				bEnableWeapon = true;
				bMustFire = true;		// 윈체스터는 강제로 공격하도록 합니다. 탄환이 0이어도..
			}
			break;
		}


		if( bEnableWeapon )
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) &&					//	폭탄 해체 중이 아니고
				(!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) &&				//	발사 불가능 상태가 아니고
				(pFireWeapon->isFireEnabled() || bMustFire) && !pFireWeapon->isWeaponStateMask( WSM_FIRING)))	// 무기의 발사가 가능한 경우
			{	
				if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_BOMBTRIGGER ) 
				{
					WeaponBase* bomb = static_cast<WeaponBombTrigger*>(pCurWeapon)->getBomb();
					if (bomb && i3::same_of<WeaponC5*>(bomb))
					{
						m_bThrowBomb = true;
						if( m_fTimeLine_Throw >= THROW_DELAY_TIME)
						{
							bCancelDisarm = m_pOwner->Cmd_Attack( WEAPON::SECONDARY_FIRE);
						}
					}
				}
				else
				{
					bCancelDisarm = m_pOwner->Cmd_Attack( WEAPON::SECONDARY_FIRE);
				}
			}
		}
		
	}

	//////////////////////////////////////////////////
	//	'B' 버튼으로 보조 익스텐션 활성화

	// issue : BUG_PORTABLE #5124
	// 타 무기에서 M4 무기로 변경함과 동시에 'B'버튼이 눌러지는 것을 막음으로써 임시로 버그를 고칩니다. 2011-09-26. ugeun.ji.  
	REAL32 elapsedTimeUpperAnim		= getAnimContext()->getUpperLocalTime();
	bool enableChangeWeapon			= !(m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_WEAPONCHANGE));
	bool enableChangeExtensionMode	= ((elapsedTimeUpperAnim > 0.05f) && enableChangeWeapon);

	// 내구도 0인 무기 익스텐션 막음
	if( UserInfoContext::i()->IsNewRepair() && pCurWeapon->IsEnduranceZero() )
	{
		enableChangeExtensionMode	= false;
	}

	if( (keyStroke & GAME_KEY_ACTION_WEAPON_EXTENSION) && enableChangeExtensionMode && bMultiWeapon)
	{
		// 하나의 무기에 두가지 Extension이 있다면 기능을 스왑한다.
		if( pFireWeapon->isMultipleExtension())
		{
			bool bPossible = true;

			// 기획팀 요청으로 커스텀 M4는 유탄/LSS 탄환 모두 소진시 유탄/LSS 모드로 익스텐션 전환이 안된다.
			if( i3::kind_of<WeaponGrenadeLauncher*>(pFireWeapon))
			{
				WeaponGrenadeLauncher * pWeaponGrenadeLauncher = (WeaponGrenadeLauncher*) pFireWeapon;

				if((pWeaponGrenadeLauncher->getTotalShellCount() <= 0) && (pWeaponGrenadeLauncher->getLoadedShellCount() <= 0))
					bPossible = false;
			}
			else if( i3::kind_of<WeaponShotGunLauncher*>(pFireWeapon))
			{
				WeaponShotGunLauncher * pWeaponShotGunLauncher = (WeaponShotGunLauncher*) pFireWeapon;

				if((pWeaponShotGunLauncher->getTotalShellCount() <= 0) && (pWeaponShotGunLauncher->getLoadedShellCount() <= 0))
					bPossible = false;
			}
			else if ( i3::same_of<WeaponCrossFire*>(pFireWeapon) )
			{
				// HK45 Dual의 경우, 'B' 버튼이 아닌 RMB로 Extension이 전환되므로, 처리하지 않는다.
				bPossible = false;
			}
			else if ( i3::same_of<WeaponKrissSuperV*>(pFireWeapon))
			{
				WeaponKrissSuperV* kriss = static_cast<WeaponKrissSuperV*>(pFireWeapon);
				if (kriss->m_pDualWeapon)
				{
					if (kriss->m_pDualWeapon->getLoadedBulletCount() <= 0)
						bPossible = false;
				}
			}

			// 무기 장전중에 변경되지 않습니다.
			if(weaponState == WEAPON::STATE_LOADMAG)
			{
				bPossible = false;
			}

			// 장전 중, 발사중, EXT 전환중일때는 EXT 전환 안되도록 막음. komet
			if( pFireWeapon->isWeaponStateMask( WSM_EXT_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE))
			{
			#if defined( I3_DEBUG)
				if(pFireWeapon->isWeaponStateMask( WSM_EXT_DISABLE))
				{
					I3TRACE("Ext");
				}

				if(pFireWeapon->isWeaponStateMask( WSM_LOADMAG_DISABLE))
				{
					I3TRACE("MAG");
				}

				if(pFireWeapon->isWeaponStateMask( WSM_LOADBULLET_DISABLE))
				{
					I3TRACE("Bullet");
				}
			#endif
				//if((pFireWeapon->isWeaponStateMask(WSM_LOADMAG_DISABLE ) && !(pFireWeapon->isWeaponStateMask(WSM_LOADED_BULLET ))))
					bPossible = false;
			}

			//2015-04-06 B키로 익스텐션이 켜지는 무기들은 익스텐션 상태가 2이상일 경우다 ( 실질적으론 2개가 맥스 )그러므로, 막는다. 하나일때는 처리안하도록...
			// ShotGun Launcher는 항상 익스텐션을 껏다 켰다 해야 합니다. Next가 없을 경우는 샷건으로 바뀌었을 경우...
			if( pFireWeapon->GetNextExtensionType() == WEAPON::EXT_NONE && !i3::kind_of<WeaponShotGunLauncher*>(pFireWeapon))
				bPossible = false;

			if( bPossible)
			{
				//	무기 Extension이 활성화되어 있으면 끈다.
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) || pFireWeapon->isExtensionActivate())						
					m_pOwner->Cmd_Extension( false);
				
				
				//	무기의 익스텐션을 스왑한다. 
				if( i3::same_of<WeaponKrissSuperV*, WeaponSniperDualMagazine* >(pFireWeapon) == false )
				{
					// 듀얼탄창 스나이프 무기의 경우 줌은 우클릭으로 , b로는 탄창교체만 함. 여기서 Extention Type 바꾸지 않고 고정시킵니다.
					pFireWeapon->SwapExtensionType();
				}
							
				if( pFireWeapon->getOwner() != nullptr && pFireWeapon->getOwner()->isLocal())
				{
				//	char szString[MAX_STRING_COUNT] = "";
					i3::stack_wstring wstrString;

					if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)
					{
						// 기획팀 요청으로 유탄 모드로 강제 전환
						m_pOwner->Cmd_Extension( true);

						//	"유탄 사용모드로 변경되었습니다."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_GRENADE_LAUNCHER_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}				
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_SCOPE)
					{ 
						//	"도트사이트 사용모드로 변경되었습니다."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_DOT_SIGHT_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)
					{
						// 기획팀 요청으로 LSS 모드로 강제 전환
						m_pOwner->Cmd_Extension( true);

						//	"LSS 사용모드로 변경되었습니다."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_LSS_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_ZOOM)
					{
						//	"줌 기능 사용모드로 변경되었습니다."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_ZOOM_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
					{	
						m_pOwner->Cmd_Extension( true);
						//	"착검 기능 사용모드로 변경되었습니다."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_BAYONET_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
					else if( pFireWeapon->GetCurrentExtensionType() == WEAPON::EXT_CROSSFIRE)
					{	
						m_pOwner->Cmd_Extension( true);
						//	"크로스파이어 기능 사용모드로 변경되었습니다."
						i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_CROSSFIRE_MODE"));

						g_pFramework->getChatBox()->PutSystemChatting( wstrString);
					}
				}

				//I3TRACE( "EXT %s\n", ((CGunInfo*)pCurWeaponInfo)->GetStringExtensionType());
			}
		}
		
		if( i3::same_of<WeaponKrissSuperV*>(pCurWeapon))
		{
			// CGameCharaWeaponControl::OnExtActivate() 보면 익스텐션 무기의 총알 개수가 사용 유무의 조건으로 돼있음.
			if (pCurWeapon->isExtensionActivate())
			{
				pCurWeapon->ExtensionActivate( !pCurWeapon->isExtensionActivate() );
				pCurWeapon->SwapExtensionType();
			}
			else
			{
				if (pCurWeapon->getLoadedBulletDualCount() > 0)
				{
					pCurWeapon->ExtensionActivate( !pCurWeapon->isExtensionActivate() );
					//듀얼로 바뀌기 전에 Extension(ZOOM) 상태일경우도 있으므로 Extension을 끈다.
					m_pOwner->Cmd_Extension(false);
					pCurWeapon->SwapExtensionType();
				}
			}
		}

		if( i3::same_of<WeaponShotGunGenderWithDual*>(pCurWeapon) )
		{
			bool bFlag = true;

			if( pCurWeapon->isExtensionActivate() ||			// 현재 활성화되있거나..
				pCurWeapon->getLoadedBulletDualCount() <= 0 )	// 탄 수가 없으면 Ext 기능 비활성
			{
				bFlag = false;
			}

			pCurWeapon->ExtensionActivate( bFlag );
		}

		if( i3::same_of<WeaponSniperDualMagazine*>(pCurWeapon))
		{
			bool bPossible = true;

			if( pFireWeapon->isWeaponStateMask( WSM_EXT_DISABLE  | WSM_LOADBULLET_DISABLE) ||
				getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || 
				getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
			{
				bPossible = false;
			}

			if( bPossible)
			{
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) || pFireWeapon->isExtensionActivate())						
					m_pOwner->Cmd_Extension( false);	
				else
					m_pOwner->Cmd_Extension( true);
			}
		}
	}

	////////////////////////////////////////////////////////////	리로드
	if( keyStroke & GAME_KEY_ACTION_RELOAD)// && HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsEnable() == false)
	{
		if( ( pCurWeapon->isLoadMagazineEnabled() && !(keyPress & GAME_KEY_ACTION_SPECIAL)) && bMultiWeapon)
		{
			if (i3::same_of<WeaponKrissSuperV*>(pCurWeapon))
			{
				if (!((WeaponKrissSuperV*)pCurWeapon)->isUsedDualWeapon())
					bCancelMission = m_pOwner->Cmd_Reload();
			}
			else
			{
				bool bAvailable = true;
				if (i3::same_of<WeaponDualCIC*>(pCurWeapon))
				{// revision 52253 CIC 웨폰이고, 칼을 휘두르고 있는 중이라면 리로드 불가능.
					MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCurWeapon->GetMainWeapon());

					if (mainWeapon_DualCIC->getCurrentUseWeapon() == WEAPON_CIC_CURRENTUSE_LEFT)
						bAvailable = false;
				}

				if (bAvailable)
				{
					ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

					if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
						getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
						nLoadType = ACTION_LOAD_READY_MAG;
					m_pOwner->SetLoadMagChg(false);
					bCancelMission = m_pOwner->Cmd_Reload(nLoadType);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////	무기 교체	
	bool bEnable = true;
	if( CHud::i() == nullptr || CHud::i()->IsRadioChat())
		bEnable = false;
	if( m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE))
		bEnable = false;
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
		bEnable = false;

	if( bEnable)
	{
		bool bEnableSwapWeapon = true;

		UIHudMultiWeapon* pHud = nullptr;

		if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			if ( m_pOwner->getMultiWeaponContext()->IsHUDSelectedChange() || m_pOwner->getMultiWeaponContext()->IsDropProhibit())
			{
				bEnableSwapWeapon = false;
			}

			pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

			I3ASSERT( pHud != nullptr);
		}

		if (!g_pFramework->getKeyboard()->GetPressState(I3I_KEY_LALT))
		{
			WEAPON_SLOT_TYPE weapontype = WEAPON_SLOT_UNKNOWN;
			WEAPON_SLOT_TYPE currentweapontype = (WEAPON_SLOT_TYPE)m_pOwner->getWeaponContext()->getCurrentSlotNum();
			WEAPON_SLOT_TYPE swapweapontype = (WEAPON_SLOT_TYPE)m_pOwner->getWeaponContext()->getSwapSlotNum();

			if (keyStroke & GAME_KEY_CHANGEWEAPON1)
				weapontype = WEAPON_SLOT_PRIMARY;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON2)
				weapontype = WEAPON_SLOT_SECONDARY;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON3)
				weapontype = WEAPON_SLOT_MELEE;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON4)
				weapontype = WEAPON_SLOT_THROWING1;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON5)
				weapontype = WEAPON_SLOT_THROWING2;
			else if (keyStroke & GAME_KEY_CHANGEWEAPON6)
				weapontype = WEAPON_SLOT_MISSION;

			if ( weapontype != WEAPON_SLOT_UNKNOWN && bEnableSwapWeapon)
			{
				bool bChange = true;

				if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				{
					bChange = false;

					// HUD 켜져 있을 경우
					if ( HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
					{
						if (weapontype == WEAPON_SLOT_PRIMARY && m_pOwner->GetCurrentCharaWeapon() != nullptr)
						{
							pHud->SetMultiWeaponSlotImage();
						}
						else if( weapontype != WEAPON_SLOT_PRIMARY)
						{
							pHud->ShowMultiWeaponSlot();
							bChange = true;
						}
					}
					else
					{
						if (weapontype == WEAPON_SLOT_PRIMARY && m_pOwner->GetCurrentCharaWeapon() != nullptr)
						{
							if ( m_pOwner->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
								&& !( m_pOwner->isLocal() && m_pOwner->getMultiWeaponContext()->IsHUDSelectedChange()))
							{
								pHud->UpdateMultiWeaponSlot();
								pHud->ShowMultiWeaponSlot();
								bCancelMission = true;
							}
							else
								bChange = true;
						}
						else if( weapontype != WEAPON_SLOT_PRIMARY)
						{
							bChange = true;
						}
					}
				}

				if ((currentweapontype != weapontype) && (swapweapontype != weapontype) && bChange
					&& !(m_pOwner->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY) == nullptr && weapontype == WEAPON_SLOT_PRIMARY) )
					bCancelMission = m_pOwner->Cmd_ChangeWeapon(weapontype);
			}

		}

		//	예전 무기로 교체
		if ( ( keyStroke & GAME_KEY_CHANGEWEAPON_QUICK) && bEnableSwapWeapon)
		{
			if ( getWeaponContext()->getOldSlotNum() != getWeaponContext()->getCurrentSlotNum() ||
				m_pOwner->getMultiWeaponContext()->GetQuickChangeIdx() != -1 && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			{
				if ( getWeaponContext()->getOldSlotNum() == WEAPON_SLOT_PRIMARY &&
					getWeaponContext()->getCurrentSlotNum() == WEAPON_SLOT_PRIMARY &&
					m_pOwner->getMultiWeaponContext()->GetMultiWeaponCount() > 1 &&
					MyRoomInfoContext::i()->getStage()->IsMultiWeapon() )
				{
					m_pOwner->getMultiWeaponContext()->HasQuickChange();
					getWeaponContext()->setForcedChange( true);
					I3TRACE("setForcedChange true\n");
				}
				else
				{
					I3TRACE("OldSlot %d, CurrentSlot %d\n", getWeaponContext()->getOldSlotNum(), getWeaponContext()->getCurrentSlotNum());
				}

				bCancelMission = m_pOwner->Cmd_ChangeWeapon(getWeaponContext()->getOldSlotNum());
			}
			else
			{
				I3TRACE("QUICK Change False - OldSlot %d, CurrentSlot %d, QuickIndex %d\n",
					getWeaponContext()->getOldSlotNum(), 
					getWeaponContext()->getCurrentSlotNum(),
					m_pOwner->getMultiWeaponContext()->GetQuickChangeIdx());
			}
		}
		//	Mouse Wheel Down (다음 무기 교체)
		else if( ( keyStroke & GAME_KEY_CHANGEWEAPON_NEXT) && bEnableSwapWeapon)
		{
			if (pHud != nullptr && m_pOwner->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
				&& HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
			{
				pHud->SetMultiWeaponSlotImage(FALSE);
			}
			else
			{
				bCancelMission = m_pOwner->Cmd_ChangeWeapon(getWeaponContext()->getNextUsableWeaponSlot());
			}
		}
		//	Mouse Wheel Up (이전 무기 교체)		
		else if( ( keyStroke & GAME_KEY_CHANGEWEAPON_PRE) && bEnableSwapWeapon)
		{
			if (pHud != nullptr && m_pOwner->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY
				&& HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
			{
				pHud->SetMultiWeaponSlotImage();
			}
			else
			{
				bCancelMission = m_pOwner->Cmd_ChangeWeapon(getWeaponContext()->getPrevUsableWeaponSlot());
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	이 이후 특수 버튼이외에는 bCancelMission과 관련되어 다른 무기 버튼을 정의해선 안됩니다.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	if( keyStroke & GAME_KEY_ACTION_SPECIAL )
	{
		I3TRACE("PRESS E KEY \n");
	}
	////////////////////////////////////////////////////////////	특수 버튼 (폭탄 해체등)
	if( keyPress & GAME_KEY_ACTION_SPECIAL && HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsVisible() == false)
	{
		if( !bCancelDisarm)
		{
			if( (pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED ))
			{
				m_pOwner->Cmd_Uninstall();
			}//	미션 종료
			else if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
			{
				m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				m_pOwner->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
			}
		}
	}
	else
	{	
		bCancelDisarm = true;
	} 

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY) )
	{
		if( keyPress & GAME_KEY_ACTION_SPECIAL )
		{
			if( !bCancelDisarm)
			{
				m_pOwner->Cmd_RunAwayInstall();
				ISTEST_S
				{
					BattleObjContext::i()->OnObjectMission(true);
				}
			}
		}
		else
		{	
			bCancelDisarm = true;
			m_pOwner->Cmd_RunAwayInstall(false);
			ISTEST_S
			{
				BattleObjContext::i()->OnObjectMission(false);
			}
		} 
		bCancelMission = false;
	}

	////////////////////////////////////////////////////////////////////////
	//	미션중 행동 취소	(입력 행동에 영향을 받는다.)
	if( bCancelMission)
	{
		//	미션 모드
		m_pOwner->Cmd_Install( false);		//	행동 취소
	}
	
	//	폭탄 해체 취소	(입력 행동에 영향을 받는다.)	
	if( bCancelDisarm)
	{
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY) )
		{
			if( m_pOwner->isCharaStateMask( CHARA_STATE_ARMINGBOMB))		
			{
				m_pOwner->Cmd_RunAwayInstall(false);
			}
		}
		else
		{
			if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB))		
			{
				m_pOwner->Cmd_Uninstall( false);	//	네트워크 폭탄 해체 취소
			}
		}
	}
}

void CGameCharaInputControlLocal::ProcessExtension(void)
{
	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr)
		return;

	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

		if ( pHud->IsVisible() && !pHud->GetAutoView())
			return;
	}

	// 내구도 없는 무기는 익스텐션 막음
	if( UserInfoContext::i()->IsNewRepair() && pCurWeapon->IsEnduranceZero() ) 
		return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	if( getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) || getActionContext()->isUpper(CHARA_UPPER_LOADBULLET) ||
		getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || 
		getActionContext()->isUpper(CHARA_UPPER_SWAPWEAPON) || getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || 
		getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
		return;

	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	switch( pCurWeapon->GetCurrentExtensionType())
	{
	default :
		// Extension이 없는 무기 모두...
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			switch( pCurWeaponInfo->GetTypeZoom())
			{
				case WEAPON::ZOOM_SCOPE :	m_pOwner->Cmd_ToggleZoom();	break;		// 스코프 기능 사용
			}
		}
		break;

	case WEAPON::EXT_SCOPE :
	case WEAPON::EXT_ZOOM :					// 견착
		// K2, K1
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			// 스코프 기능 사용
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

	case WEAPON::EXT_STEADY_AIM :
		// Spectre
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			// 스코프 기능 사용
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

		// Activate 단계 없이 사용하는 Extension 무기들
	case WEAPON::EXT_DUAL_MAG :		// AK47
	case WEAPON::EXT_SILENCER :		// 사용과 동시에 Activate
	// MP5K	// MK23
		
		if ( i3::same_of<WeaponSniperDualMagazine*>(pCurWeapon))
		{

			if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				m_pOwner->Cmd_ToggleZoomForDSR();
			}
			
		}
		else
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	case WEAPON::EXT_DOT_SIGHT :
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			// 스코프 기능 사용
			m_pOwner->Cmd_ToggleZoom();
		}
		break;
	case WEAPON::EXT_GLAUNCHER:
	case WEAPON::EXT_SHOTGUN_LAUNCHER:
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{			
			bool bPossibleSubfunc = true;

			// 기획팀 요청으로 커스텀 M4의 경우 유탄/LSS 익스텐션은 'B' 버튼으로만 가능하므로 유탄/LSS 모드일 경우 마우스 오른쪽 버튼을 막는다.
			if( pCurWeapon->isMultipleExtension() && pCurWeapon->isExtensionActivate() &&
				(pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER || pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER))
			{
				bPossibleSubfunc = false;
			}

			// 탄수에 대한 Extension 비활성화 코드가 없어 추가합니다.
			if (i3::kind_of<WeaponGrenadeLauncher*>(pCurWeapon))
			{
				WeaponGrenadeLauncher * pWeaponGrenadeLauncher = (WeaponGrenadeLauncher*)pCurWeapon;

				if ((pWeaponGrenadeLauncher->getTotalShellCount() <= 0) && (pWeaponGrenadeLauncher->getLoadedShellCount() <= 0))
					bPossibleSubfunc = false;
			}
			else if (i3::kind_of<WeaponShotGunLauncher*>(pCurWeapon))
			{
				WeaponShotGunLauncher * pWeaponShotGunLauncher = (WeaponShotGunLauncher*)pCurWeapon;

				if ((pWeaponShotGunLauncher->getTotalShellCount() <= 0) && (pWeaponShotGunLauncher->getLoadedShellCount() <= 0))
					bPossibleSubfunc = false;
			}
			else if (i3::kind_of<WeaponBow*>(pCurWeapon))
			{
				if (static_cast<WeaponBow*>(pCurWeapon)->getLoadedShellCount() <= 0)
					bPossibleSubfunc = false;
			}

			if( bPossibleSubfunc)
			{
				//	무기 Extension 활성화 토글
				m_pOwner->Cmd_ToggleExtensionWeapon();
			}
		}
		break;

	case  WEAPON::EXT_BAYONET :
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{			
			bool bPossibleSubfunc = true;

			if( pCurWeapon->isMultipleExtension() && pCurWeapon->isExtensionActivate() &&
				pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
			{
				bPossibleSubfunc = false;
			}

			if( bPossibleSubfunc)
			{
				//	무기 Extension 활성화 토글
				m_pOwner->Cmd_ToggleExtensionWeapon();
				
			//	m_pOwner->Cmd_Attack();
			}
		}

		break;

	case	WEAPON::EXT_CROSSFIRE :
		
		if( keyStroke & GAME_KEY_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;	
	}
}

void CGameCharaInputControlLocal::ProcessInput( REAL32 rDeltatime)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
		return;

	if(CGameMaster::i()->IsPaused())
	{
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);
			if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == false)
				pChara->Cmd_Idle();
		}
		return;
	}

	UINT64 keyPress;// = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	if( m_pOwner->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) )
	{
		// 무기 드랍 상태 체크
		ProcessDropWeapon( rDeltatime);

		if( keyStroke & GAME_KEY_ACTION_TURNBACK)
		{	// 뒤돌아 보기 기능
			m_pOwner->Cmd_Turn();
		}
	}
	//	키 입력( 본게임중에만 키입력을 받음)
	if(	!i3::kind_of<CGameCamera_FLY*>(g_pCamera) &&
		!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_CONTROL) &&
 		g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
	{	
		keyPress = g_pFramework->getKeyPress();
		keyStroke = g_pFramework->getKeyStroke();

		REAL32 theta = getBoneContext()->getTheta();
		REAL32 rho = getBoneContext()->getRhoValue();
		
		//REAL32 fl = 0.16666666f * tm;
		//	캐릭터 회전
		if( !g_pEnvSet->m_bEnableMouseInvert)
		{
			getInputContext()->setInputMoveRho( -g_pFramework->getMouseMoveDelta()->y * 0.005f);
		}
		else
		{
			getInputContext()->setInputMoveRho( g_pFramework->getMouseMoveDelta()->y * 0.005f);
		}

		rho		+= getInputContext()->getInputMoveRho();

		getInputContext()->setInputMoveTheta( -g_pFramework->getMouseMoveDelta()->x * 0.005f);
		theta		+= getInputContext()->getInputMoveTheta();

		if( g_pFramework->getMouseMoveDelta()->x != 0.0f)
		{
			// 수평 반동의 회복 중, 마우스가 돌아가면 회복 모드를 Cancel한다.
			//if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
				getBoneContext()->getRecoilData()->m_HorzState = RECOIL_HORZ_NA;
		}
		
		theta		= i3Math::NormalizeAngle( theta);
		rho			= i3Math::NormalizeAngle( rho);
		//	각도

		REAL32 fAimingAngleMax = g_pCharaInfoDataBase->getGlobal()->GetAimingAngleMax();	//	최대각
		REAL32 fAimingAngleMin = g_pCharaInfoDataBase->getGlobal()->GetAimingAngleMin();	//	최소각

		//	각도 제한 (시작시 바라보는 시선의 수평선을 0도로 기준)
		if( rho > I3_DEG2RAD( fAimingAngleMax))			rho = I3_DEG2RAD(fAimingAngleMax);	
		else if( rho < I3_DEG2RAD( fAimingAngleMin))		rho = I3_DEG2RAD(fAimingAngleMin);

		getBoneContext()->setTheta( theta);
		getBoneContext()->setRho( rho);
	}
	else
	{
		keyPress = keyStroke = 0;

		//if( g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
		//	m_pOwner->MoveStop();	//	키 입력이 없으면 이동 정지

		getInputContext()->setInputMoveRho( 0.f);
		getInputContext()->setInputMoveTheta( 0.f);
	}	

	if( CHUD_CountDown::instance() )
	{
		if( CHUD_CountDown::instance()->is_act_count_down() == true ) return; //카운트 중일 경우엔 키 행동을 제한한다.
	}

	// 임시 코드
	// Local에 잇는 무기 처리를 위해 저장해놓습니다. 차후 삭제 예정
	getInputContext()->setKeyPressInput( keyPress);
	getInputContext()->setKeyStrokeInput( keyStroke);

	//	이동 값을 처리합니다.
	bool bChangeBody = false;

	///////////////////////////////////////////////////////////		앉아/일어서
	bChangeBody = ProcessCrouch( rDeltatime);

	///////////////////////////////////////////////////////////		점프
	if( keyStroke & GAME_KEY_ACTION_JUMP )
	{
		if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) == false)
			m_pOwner->Cmd_JumpStart();
	}
	
	
	ProcessMovement( rDeltatime, bChangeBody);
	

	//	캐릭터 위치 이동
	getMoveContext()->ProcessAccel();

	if( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE && m_pOwner->IsDino() == false)
	{
		ProcessWeapon( rDeltatime);
		ProcessExtension();
	}
	else if(gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE && m_pOwner->IsDino() == true)
	{
		ProcessDinoAttack( rDeltatime);
	}
}

void CGameCharaInputControlLocal::ProcessWeaponCollisionFire( REAL32 rDeltatime )
{
	//UINT64 keyPress = getInputContext()->getKeyPressInput();
	UINT64 keyStroke = getInputContext()->getKeyStrokeInput();

	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	if( (keyStroke & GAME_KEY_ACTION_ATTACK) && !bIsHackUser)
	{	
		CQAWeaponCMgr::i()->ThrowBox(m_pOwner);
	}
}