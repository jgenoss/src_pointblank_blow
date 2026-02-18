
#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaPlayer.h"
#include "StageBattle.h"

//#define ANIMTIME_MOVE_DURATION		0.666f			//	MOVE 총길이
//#define ANIMTIME_MOVE_RIGHT_STEP	0.322f			//	MOVE 오른발 타이밍
//#define ANIMTIME_MOVE_LEFT_STEP		(ANIMTIME_MOVE_RIGHT_STEP * 2.0f - 0.01f)			//	MOVE 왼발 타이밍
/*
void CGameCharaPlayer::ResetDefaultAnim( BOOL bIs3PV)
{
	//	캐릭터 Anim 정보 리셋
	m_pActionContext->setBody( CHARA_BODY_STAND);
	m_pActionContext->setBody2( CHARA_BODY_ATTACKIDLE);
		
	m_pActionContext->setLower( CHARA_LOWER_IDLE);
	m_pActionContext->setUpper( CHARA_UPPER_IDLE);

	if( !bIs3PV)
		m_AnimSet = CHARA_ANIMSET_1PV;
	else
		m_AnimSet = CHARA_ANIMSET_3PV;

	ACTION_apply( bIs3PV);
	//
	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	_ANIM_stop();				

	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_IDLE);
	ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE);

	//	강제로 적용
	m_pAnimContext->getAILower()->SetAIStateImmediately( g_pViewer->getDeltaTime());
	m_pAnimContext->getAIUpper()->SetAIStateImmediately( g_pViewer->getDeltaTime());
}

void CGameCharaPlayer::ResetLobbyAnim( void)
{
	//	캐릭터 Anim 정보 리셋
	m_pActionContext->setBody( CHARA_BODY_STAND);
	m_pActionContext->setBody2( CHARA_BODY_ATTACKIDLE);
		
	m_pActionContext->setLower( CHARA_LOWER_NONE);
	m_pActionContext->setUpper( CHARA_UPPER_NONE);

	m_AnimSet = CHARA_ANIMSET_3PV;

	//ACTION_apply( TRUE);
	getAnimContext()->getControl()->AnimSetApply();

	//
	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	_ANIM_stop();				

	//	로비 애니메이션으로 플레이
	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_IDLE);
	ANIM_playUpper( CHARA_UPPER_NONEBATTLEIDLE, ID_UPPER_NONEBATTLE_IDLE, 0.0f);

	//	강제로 적용
	m_pAnimContext->getAILower()->SetAIStateImmediately( g_pViewer->getDeltaTime());
	m_pAnimContext->getAIUpper()->SetAIStateImmediately( g_pViewer->getDeltaTime());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Anim Callback
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHSND_ACTRATE_FACTER 10	//발생빈도계수

#define CHSND_ACTRATE_ACCESSORY 4	//장비 달그락 거리는 소리
#define CHSND_ACTRATE_RUSTLE	8	//옷이 부시럭 거리는 소리

void CGameCharaPlayer::OnMoveRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnMoveRun( pCtx, tm );

	m_fRunElapseTime += tm;
	
	//	서서 이동할때만 발자국 소리나오게...현재 앉아 이동하면 발자국 소리 안나오게 한다.
	if( getActionContext()->isLower( CHARA_LOWER_MOVE)  || getActionContext()->isLower( CHARA_LOWER_WALK))
	{	
//		if( isNetwork()) I3TRACE("ON MOVE RUN : %f, Elapse : %f\n", tm, m_fRunElapseTime);
		//I3TRACE("ON MOVE RUN : %f, Elapse : %f\n", tm, m_fRunElapseTime);

		//	발자국 소리 플레이~
//		REAL32 fRunDuration = ANIMTIME_MOVE_DURATION;
		BOOL bTrue1PV = FALSE;
		if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
			CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == this ) bTrue1PV = TRUE;
		}	

		if( m_fRunElapseTime <= tm)
		{
			if( m_pAnimContext->isAILower())
			{
				i3AIState * pAIState = m_pAnimContext->getAILower()->getCurrentAIState();
				m_fRunDuration = (pAIState != NULL) ? pAIState->getDuration() * (1.0f / m_pMoveContext->getSpeed()) : 0.0f;
				if( m_fRunDuration > ANIMTIME_MOVE_DURATION) m_fRunDuration = ANIMTIME_MOVE_DURATION;
			}

			//if( is1PV())
			if( bTrue1PV || (m_fRunDuration == 0.0f) )	
			{
				if( getActionContext()->isUpper( CHARA_UPPER_MOVE ))
				{
					i3AIState * pAI = m_pAnimContext->getAIUpper()->getCurrentAIState();
					if( pAI != NULL)
					{
						m_fRunDuration = pAI->getActualDuration();//	1인칭 애니메이션은 하체를 플레이하지 않기 때문에 임의 시간 설정
					}
				}

				REAL32	rScaleRate = 0.0f;
				if( !getActionContext()->isUpper( CHARA_UPPER_MOVE ))
				{
					WeaponBase * pWeapon = getWeaponContext()->getCurrent();
					if( pWeapon != NULL )
					{	
						CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

						if( getActionContext()->isBody( CHARA_BODY_STAND))
							rScaleRate =  pWeaponInfo->m_rMoveNormalStand * 0.01f;
						else if( getActionContext()->isBody( CHARA_BODY_CROUCH))
							rScaleRate = pWeaponInfo->m_rMoveNormalSit * 0.01f;

						if( isStateMask( CHARA_STATE_ZOOM))
						{
							if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) &&
								!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))	//	줌인 중이지만 장전시에는 적용안되게...
								rScaleRate *= pWeaponInfo->m_rMoveNormalZoom * 0.01f;
						}

						if( getActionContext()->isUpper( CHARA_UPPER_FIRE) || getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))			//	발사 상태일때
						{
							rScaleRate *= pWeaponInfo->m_rMoveNormalFire * 0.01f;
						}

						m_fRunDuration = 0.544f * (1.0f / rScaleRate);
					}
				}

				if( m_fRunDuration < I3_EPS)
				{
					m_fRunDuration = 0.544f * (1.0f / rScaleRate);
				}
			}

			m_pAnimContext->setLeftStepTime( m_fRunDuration * 0.822f);
			m_pAnimContext->setRightStepTime( m_fRunDuration * 0.322f);
		}

		if( m_fRunDuration > 0.0f)
		{
			if( m_fRunElapseTime >= m_fRunDuration)
			{	
				//I3TRACE("STEP ELAPSE RESET: %f : DeltaTime: %f\n", m_fRunElapseTime, tm);
				//	Reset
				m_fRunElapseTime = 0.f;
				m_bRightStepSound = FALSE;

				if( isAlive())
				{
					m_nAccessory = ( m_nAccessory > 0)	? 0 : 1;
					{
						if( !getActionContext()->isBody( CHARA_BODY_CROUCH))
						{
							if( getActionContext()->isLower( CHARA_LOWER_WALK))
							{
								g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WALK_EQUIPMENT, m_nAccessory, GetPos(), is1PV());
							}
							else if( getActionContext()->isLower( CHARA_LOWER_MOVE))
							{
								//if( !isNetwork())
								{
									g_pSndMng->StartCharacterActionSnd( GTBCA_ON_RUN_EQUIPMENT, m_nAccessory, GetPos(), is1PV());
								}
							}
						}
					}
				}
			}
			else if( m_fRunElapseTime >= m_pAnimContext->getLeftStepTime())
			{	//	왼발 소리 플레이	
				if( m_bRightStepSound)
				{
					if( !getActionContext()->isBody( CHARA_BODY_CROUCH) && !getActionContext()->isLower( CHARA_LOWER_WALK))
					{	
						m_pSoundContext->PlayFootStep( m_pMoveContext->getFloorTerrainType(), true, is1PV());
					}

					m_bRightStepSound = FALSE;
					//I3TRACE("LEFT STEP: %f, DeltaTime: %f\n", m_fRunElapseTime, tm);
				}				
			}
			else if( m_fRunElapseTime >= m_pAnimContext->getRightStepTime())
			{	//	오른발 소리 플레이
				if( !m_bRightStepSound)
				{
					if( !getActionContext()->isBody( CHARA_BODY_CROUCH) && !getActionContext()->isLower( CHARA_LOWER_WALK))
					{
						m_pSoundContext->PlayFootStep( m_pMoveContext->getFloorTerrainType(), false, is1PV());
					}

					m_bRightStepSound = TRUE;
					//I3TRACE("RIGHT STEP: %f, DeltaTime: %f\n", m_fRunElapseTime, tm);
				}
			}
			else
			{
				//I3TRACE("no available: %f, DeltaTime: %f\n", m_fRunElapseTime, tm);
			}
		}
	}
}

void CGameCharaPlayer::OnAttackRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnAttackRun( pCtx, tm);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	WeaponStateIdle();
}

void CGameCharaPlayer::OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnAttackReadyFinish( pCtx, tm );

	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);		//	준비 끝난후 바로 발사 가능
}

void CGameCharaPlayer::OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnJumpEndFinish( pCtx, tm );

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	//	다른 시점으로 변환중 애니메이션이 다르더라고 같은 동작이면 이전 동작으로 돌아가지 않는다.		
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration())
	{
		ANIM_playUpperReturn( !is1PV());		//	이전 동작이었던 Upper의 Idle 또는 Move로 돌아온다.
	}
}

void CGameCharaPlayer::OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnLoadBulletRun( pCtx, tm );
}

void CGameCharaPlayer::OnDamageRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnDamageRun( pCtx, tm );

	if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE) || getActionContext()->isLower( CHARA_LOWER_DEATH))	return;

	//	다른 시점으로 변환중 애니메이션이 다르더라고 같은 동작이면 이전 동작으로 돌아가지 않는다.
	
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration())
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE);

		//	이전 행동으로 돌아옵니다.. (Idle 이나 Move)
		ANIM_playUpperReturn( !is1PV());
	}
}

void CGameCharaPlayer::OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnExtensionDeactivateRun( pCtx, tm );
 
	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration() - tm)
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

		ANIM_playUpperReturn( !is1PV());
		
		ApplyCrosshair();
	}
}

void CGameCharaPlayer::OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnExtensionActivateRun( pCtx, tm );

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;
	
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration() - tm)
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);

		ANIM_playUpperReturn( !is1PV());
		
		ApplyCrosshair();
	}
}

void CGameCharaPlayer::OnDeathFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnDeathFinish( pCtx, tm );

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	//	리스폰 타이머 설정
	if( !g_pGameContext->IsBombMissionMode() ||
		(pMissionMng->getTotalRespawnCount() > pMissionMng->getRespawnCount( getCharaNetIndex())) )
	{
#if defined( I3_DEBUG )
		if( !g_pConfig->m_bNetworking)
		{
			m_rRespawnElapseTime = g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime * 2.f;
		}
		else
#endif
		{
			m_rRespawnElapseTime = g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime;
		}
	}
}

void CGameCharaPlayer::OnUI_IdleAFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnUI_IdleAFinish( pCtx, tm);

	// 기본 Idle로 값니다
	if (FALSE == g_pFramework->IsBattleStage())	
	{
		//if( STATE_getUpper( ) == CHARA_UPPER_NONEBATTLEWEAPONSHOW )
		//	ACTION_NoneBattleIdle();
	}
	else
	{
		if (g_pFramework->GetCurrentStage())
		{
			char szTrace[ 256] = { 0,};
			sprintf( szTrace, "잘못된 애니메니이션. 유저:%s(%d), ", g_pGameContext->getNickForSlot( getCharaNetIndex()), getCharaNetIndex() );
			if( getWeaponContext()->getCurrent() != NULL)
			{
				sprintf( szTrace, "무기:%s", getWeaponContext()->getCurrent()->getWeaponInfo()->m_szName);
			}

			I3TRACE( szTrace);
		}
		else
		{
			// CurrentStage가 NULL이다. 스테이지 점프 중 호출되었다.
			I3TRACE("[WARNING] CGameCharaPlayer::OnUI_IdleAFinish() - currentStage is NULL\n");			
		}
	}
}

void CGameCharaPlayer::OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnUI_IdleBRun( pCtx, tm);

	if( m_pAnimContext->getAIUpper() == NULL )
		return;

	if( getUpdateContext()->getUpperAniTimer() + tm > m_pAnimContext->getAIUpper()->getCurrentAIState()->getDuration())
	{
		ACTION_NoneBattleIdle();
		getUpdateContext()->setUpperAniTimer( 0.f);
	}
}

void CGameCharaPlayer::OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaBase::OnUI_IdleBFinish( pCtx, tm);

	// 기본 Idle로 값니다
	if (FALSE == g_pFramework->IsBattleStage())	
	{
		ACTION_NoneBattleIdle();
		getUpdateContext()->setUpperAniTimer( 0.f);
	}
	else
	{
		if (g_pFramework->GetCurrentStage())
		{
			char szTrace[ 256] = { 0,};
			sprintf( szTrace, "잘못된 애니메니이션. 유저:%s(%d), ", g_pGameContext->getNickForSlot( getCharaNetIndex()), getCharaNetIndex() );
			if( getWeaponContext()->getCurrent() != NULL)
			{
				sprintf( szTrace, "무기:%s", getWeaponContext()->getCurrent()->getWeaponInfo()->m_szName);
			}

			I3TRACE( szTrace);
		}
		else
		{
			// CurrentStage가 NULL이다. 스테이지 점프 중 호출되었다.
			I3TRACE("[WARNING] CGameCharaPlayer::OnUI_IdleAFinish() - currentStage is NULL\n");			
		}
	}
}
*/

