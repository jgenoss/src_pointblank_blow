#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaLocal.h"

/*
void CGameCharaLocal::_WeaponStateIdle()
{
	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pCurWeapon != NULL);

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != NULL);

	WEAPON_STATE		nState = pCurWeapon->getState();

	//	발사중 끝나면 Idle 상태로 돌아온다.
	if( nState != WEAPON_STATE_FIRE )
	{
		WEAPON_CLASS_TYPE	nType = pCurWeaponInfo->m_TypeClass;

		// 무기에 대한 사용을 가능하도록 한다.
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE);

		if( getActionContext()->isBody2( CHARA_BODY_IDLE) )
		{	// 3인칭 뷰만 플레이
			_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());
		}
		
		//	Mission 예외 처리 (여기서 처리 해야 함)
		if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
		{
			if( pCurWeaponInfo->m_nNumber == WEAPON_NUM_MISSION_C4)
			{
#if defined( I3_DEBUG )
				if( g_pConfig->m_bNetworking)		//	Network
				{
				}
				else								//	가입하기
				{
					//	폭탄 장착이 끝났으면
					if( getWeaponContext()->getControl()->OnWeaponDropToWorld( getWeaponContext()->getCurrentSlotNum(), GetPos() ) )
					{
						//	Drop 됐다.
						pCurWeapon = NULL;
						pCurWeaponInfo = NULL;
					}
				}
#endif

				removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
			}
		}

		//	일반 무기
		switch( nState)
		{
			case  WEAPON_STATE_NEEDTO_LOADMAG:
				// 발사 후, 총알이 없어 탄창을 갈아 끼워야 하는 상태
				//	일반 총은 Reload 동작( LoadMagazine) 하나로 끝납니다.
				ACTION_LoadMagazine();
				return;

			case WEAPON_STATE_NEEDTO_LOADBULLET :
				// 발사 후, 장전을 해주어야 하는 상태
				ACTION_LoadBullet();
				return;
		
			case WEAPON_STATE_NEEDTO_FIRE:
				//	연발 발사합니다.
				//ACTION_Fire();
				getCommandContext()->Command(CHARA_CMD_PRIMATTACK);
				return;

			case WEAPON_STATE_EMPTY_MAGAZINE:
				{
					//	수류탄이나 아이템일 경우 EMPTY면 0번 무기로 교체
					if( nType == WEAPON_CLASS_THROWING || nType == WEAPON_CLASS_ITEM || nType == WEAPON_CLASS_MISSION)
					{
						if( !ACTION_SwapWeapon(0) )
						{
							if( !ACTION_SwapWeapon(1) )
							{
								ACTION_SwapWeapon(2);
							}
						}
					}
					else
						ACTION_SwapNextWeapon();
				}
				return;

			case WEAPON_STATE_NEEDTO_QUICKLOAD :
				ACTION_LoadMagazineDual();
				return;
		}

		//	이전 행동으로 돌아옵니다.. (Idle 이나 Move)
		if( !getActionContext()->isLower( CHARA_LOWER_DEATH) )
		{
			if( nState != WEAPON_STATE_FIRING )
			{
				if( pCurWeaponInfo != NULL )
				{
					if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING)
					{
						if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK))
						{
							STATE_SET_UPPER( CHARA_UPPER_MOVE);
							ANIM_PLAY_UPPER(ID_UPPER_SWAP_WEAPON);
						}
						else
						{
							STATE_SET_UPPER( CHARA_UPPER_IDLE);
							ANIM_PLAY_UPPER(ID_UPPER_SWAP_WEAPON);
						}
					}
					else
					{
						if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
						{
							ActionParameter * param = GET_PREPARE_PARAM;
							i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

							CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
						}
						else
						{
							ACTION_UPPER_RETURN;
						}
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Anim Callback
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameCharaLocal::OnIdleRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnIdleRun( pCtx, tm);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_WEAPONCHANGE);
}

void CGameCharaLocal::OnAttackRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnAttackRun( pCtx, tm);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	WeaponStateIdle();
}

void CGameCharaLocal::OnJumpStartRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnJumpStartRun( pCtx, tm );

	if( !getActionContext()->isLower( CHARA_LOWER_JUMP) || getActionContext()->isLower( CHARA_LOWER_DEATH))	return;

	if( isStateMask( CHARA_STATE_FLOOR) || isStateMask( CHARA_STATE_CLIMB))		//	땅에 닿으면
	{	
		CHARA_ACTION_LOWER lower_state = CHARA_LOWER_MOVE;
		
		//	착지후 다시 한번 방향키 입력을 체크합니다. (그냥 착지 할지 바로 이동할지)
		UINT64 key = getKeyPressInput();

		if( key & GAME_KEY_MOVE_FORWARD )
		{
			setZ( getMoveContext()->getMoveDir(), 1.f);
			//	m_MoveDir.z = 1.0f;		
		}
		else if( key & GAME_KEY_MOVE_BACKWARD )
		{
			setZ( getMoveContext()->getMoveDir(), -1.f);
		//	m_MoveDir.z = -1.0f;
		}
		else
		{
			setZ( getMoveContext()->getMoveDir(), 0.f);
			//	m_MoveDir.z = 0.0f;
		}

		if( key & GAME_KEY_MOVE_LEFT )
		{
			setX( getMoveContext()->getMoveDir(), 1.f);
		//	m_MoveDir.x = 1.0f;		
		}
		else if( key & GAME_KEY_MOVE_RIGHT)
		{
			setX( getMoveContext()->getMoveDir(), -1.f);
		//	m_MoveDir.x = -1.0f;
		}
		else
		{
			setX( getMoveContext()->getMoveDir(), 0.f);
			//m_MoveDir.x = 0.0f;
		}

		if( (getX( getMoveContext()->getMoveDir()) == 0.0f) &&
			(getY( getMoveContext()->getMoveDir()) == 0.0f) &&
			(getZ( getMoveContext()->getMoveDir()) == 0.0f))
			lower_state = CHARA_LOWER_IDLE;

		if( getActionContext()->isBody2( CHARA_BODY_IDLE))
		{
			_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV() );
		}
		
		//	제자리 점프다. -> 점프 끝 애니메이션으로
		if( lower_state == CHARA_LOWER_IDLE)
		{
			//ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_JUMP_END);
			CALL_ACTION_COMMAND( ACTION_CMD_JUMPLOWER_FINISH, NULL);

			//	(이전 동작이 무엇인지 저장한다.)	
			m_pAnimContext->setOldLowerID( ID_LOWER_IDLE);

			if( getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
				getActionContext()->isUpper( CHARA_UPPER_IDLE))
			{
				//ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_JUMP_END);
				CALL_ACTION_COMMAND( ACTION_CMD_JUMPUPPER_FINISH, NULL);
					
				//	(이전 동작이 무엇인지 저장한다.)
				m_pAnimContext->setOldUpperID( ID_UPPER_IDLE);
			}
		}
		//	이동중 점프다. -> 이동 애니메이션으로
		else
		{
			//	걷기
			if( key & GAME_KEY_ACTION_FOOTSTEP)
			{
				ID_LOWER_AI nId = _ANIM_getWalkId( getMoveContext()->getMoveDir());

				ANIM_playLower( CHARA_LOWER_WALK, nId);	
			}
			//	뛰기
			else	
			{
				ID_LOWER_AI nId = _ANIM_getMoveId( getMoveContext()->getMoveDir());

				ANIM_playLower( CHARA_LOWER_MOVE, nId);	
			}

			if( getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
				getActionContext()->isUpper( CHARA_UPPER_IDLE) )
			{
				//ACTION_MoveUpper();
				ActionParameter * param = GET_PREPARE_PARAM;
				i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

				CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
			}
		}

		removeEnableStateMask( CHARA_STATE_DISABLE_MOVE	| CHARA_STATE_DISABLE_CROUCH );

		//	점프 딜레이 시간 설정
		//m_fJumpDelayElapsedTime = getDBInfo()->m_rJumpDelay;
		setJumpDelayElapsedTime( 0.f);
	}
}

void CGameCharaLocal::OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnLoadMagazineRun( pCtx, tm);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL );

	switch( pWeapon->getState())
	{
		case  WEAPON_STATE_NEEDTO_LOADMAG:
			ACTION_LoadMagazine();
			break;

		case WEAPON_STATE_NEEDTO_LOADBULLET :
			ACTION_LoadBullet();
			break;
		case WEAPON_STATE_READY:
			{	
				//	샷건 외에는 일반 무기는 Idle 상태로 돌아가야 하기 때문에
				//	OnLoadBullet을 호출하지 않기 때문에 (AIState가 없으므로) 호출해야 합니다.
				OnLoadBulletRun( pCtx, tm);	
			}
			break;
		default :
			break;
	}
}

void CGameCharaLocal::OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnLoadBulletRun( pCtx, tm);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL );

	//	장전이 끝나면
	if( pWeapon->getState() == WEAPON_STATE_READY)
	{
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());		//	Idle 상태는 AttackIdle 상태로 한다.
		if( isStateMask( CHARA_STATE_ZOOM ) )
			ACTION_CancelZoom();
	
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE 
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_ZOOM | CHARA_STATE_DISABLE_WEAPONCHANGE);

		//	이전 행동으로 돌아옵니다.. (Idle 이나 Move)
		if( !getActionContext()->isLower( CHARA_LOWER_DEATH))
		{
			if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
			{
				//ACTION_MoveUpper();
				ActionParameter * param = GET_PREPARE_PARAM;
				i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

				CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
			}
			else
			{
				//ANIM_playUpperReturn( !is1PV());
				ACTION_UPPER_RETURN;
			}
		}

		//I3TRACE("WEAPON_STATE_READY\n");
	}
}

void CGameCharaLocal::OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnWeaponSwapRun( pCtx, tm);

	if( !getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON) ||
		getActionContext()->isLower( CHARA_LOWER_DEATH))	return;

	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL);

	//	다른 시점으로 변환중 애니메이션이 다르더라고 같은 동작이면 이전 동작으로 돌아가지 않는다.		
	//	동작이 끝났다.
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration())
	{
		//	이전 동작이었던 Upper의 Idle 또는 Move로 돌아온다.
		if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
		{
			//ACTION_MoveUpper();
			ActionParameter * param = GET_PREPARE_PARAM;
			i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

			CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
		}
		else
		{
			//ANIM_playUpperReturn( !is1PV());
			ACTION_UPPER_RETURN;
		}

		if( pWeapon != NULL )
		{
			pWeapon->ReturnToReady();
		}
		
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		//removeEnableStateMask( CHARA_STATE_DISABLE_BODY2);
		//removeStateMask( CHARA_STATE_DISABLE_DODGE);
	}
	else	//	진행중
	{
		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		//addStateMask( CHARA_STATE_DISABLE_BODY2);
		//addStateMask( CHARA_STATE_DISABLE_DODGE);
	}

	removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE);
}

*/
