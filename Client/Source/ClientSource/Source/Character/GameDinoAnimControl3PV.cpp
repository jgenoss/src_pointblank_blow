#include "pch.h"
#include "GameDinoAnimControl3PV.h"

#include "GameDinoAnimContext.h"

I3_CLASS_INSTANCE( CGameDinoAnimControl3PV);

void CGameDinoAnimControl3PV::OnMoveRun( i3AIContext * pCtx, REAL32 tm)
{
	// Add code here
	if( m_pOwner->IsRex() || m_pOwner->IsTank())
	{
		REAL32 rElapseTime = getAnimContext()->getRunElapseTime();
		rElapseTime += tm;// * getAnimContext()->getLowerTimeScale();
		getAnimContext()->setRunElapseTime( rElapseTime);

		if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
			return;

		CGameCharaActionContext * pActionCtx = getActionContext();
		CGameCharaAnimContext * pAnimCtx = getAnimContext();

		REAL32 rDuration = pAnimCtx->getRunDuration();

		if( pActionCtx->isLower( CHARA_LOWER_WALK) &&
			pActionCtx->isUpper( CHARA_UPPER_MOVE))
		{
			i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
			if( pAIUpper != nullptr)
			{
				if( ((CGameDinoAnimContext*)getAnimContext())->getOldDinoUpperID() != ID_UPPER_DINO_WALK )
				{
					((CGameDinoAnimContext*)pAnimCtx)->_Play_Anim_DinoUpper( ID_UPPER_DINO_WALK, pAIUpper->getLocalTime());
				}
			}
		}

		//	서서 이동할때만 발자국 소리나오게...현재 앉아 이동하면 발자국 소리 안나오게 한다.
		if( pActionCtx->isLower( CHARA_LOWER_RUN))
		{	
			bool bReverse = false;

			ID_LOWER_AI nId = pAnimCtx->getMoveIdByDir( getMoveContext()->getMoveDir());

			if( nId == 	ID_LOWER_MOVE_BACK || nId == ID_LOWER_MOVE_BACK_LEFT || nId == ID_LOWER_MOVE_BACK_RIGHT)
			{
				bReverse = true;
			}
		
			//	발자국 소리 플레이~
			REAL32 rMaxDuration = 2.03f;
			if( rElapseTime <= tm)
			{
				i3AIContext * pAILower = pAnimCtx->getAILower();

				if( pAILower != nullptr)
				{
					i3AIState * pAIState = pAILower->getCurrentAIState();
					rDuration = (pAIState != nullptr) ? pAIState->getActualDuration() : 0.0f;

					if( rDuration > rMaxDuration) rDuration = rMaxDuration;
					
					if( pActionCtx->isLower( CHARA_LOWER_WALK))
					{
						rDuration *= 2.0f;
					}

					pAnimCtx->setLeftStepTime( rDuration * 0.822f);
					pAnimCtx->setRightStepTime( rDuration * 0.322f);
				}
			}

			if( rDuration > 0.0f)
			{
				if( rElapseTime >= rDuration || rElapseTime <= tm)
				{	
					//I3TRACE("STEP ELAPSE RESET: %f : DeltaTime: %f\n", rElapseTime, tm);
					//	Reset
					if( rElapseTime >= rDuration)
					{
						rElapseTime = 0.f;
					}
				}
				else if( rElapseTime >= getAnimContext()->getLeftStepTime())
				{	//	왼발 소리 플레이	
					if( m_pOwner->isCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT))
					{
						if( !pActionCtx->isBody( CHARA_BODYLOWER_CROUCH) && !pActionCtx->isLower( CHARA_LOWER_WALK))
						{
							getSoundContext()->PlayFootStep( getMoveContext()->getFloorTerrainType(), true, false);

							VEC3D vPos;
							CGameCharaBoneContext * pBoneCtx = getBoneContext();
							if( pBoneCtx != nullptr)
							{
								if (m_pOwner->IsRex())
								{

									MATRIX * pMtx = pBoneCtx->getBoneMatrix( TREX_BONE_FOOT_LEFT);
									if( bReverse)
									{
										pMtx = pBoneCtx->getBoneMatrix( TREX_BONE_FOOT_RIGHT);
									}

									i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));
									setY( &vPos, getY( m_pOwner->GetPos()));

								}
								else if (m_pOwner->IsTank())
								{

									MATRIX * pMtx = pBoneCtx->getBoneMatrix( RAPTOR_BONE_FOOT_LEFT);
									if( bReverse)
									{
										pMtx = pBoneCtx->getBoneMatrix( RAPTOR_BONE_FOOT_RIGHT);
									}

									i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));
									setY( &vPos, getY( m_pOwner->GetPos()));

								}
								g_pEffectManager->AddEnvironmentEffect( ENV_EFEECT_TREX_WALK, &vPos, &I3_YAXIS);
							}
						}
	 
						m_pOwner->removeCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
						//I3TRACE("LEFT STEP: %f, DeltaTime: %f\n", rElapseTime, tm);
					}				
				}
				else if( rElapseTime >= getAnimContext()->getRightStepTime())
				{	//	오른발 소리 플레이
					if( !m_pOwner->isCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT))
					{
						if( !pActionCtx->isBody( CHARA_BODYLOWER_CROUCH) && !pActionCtx->isLower( CHARA_LOWER_WALK))
						{
							getSoundContext()->PlayFootStep( getMoveContext()->getFloorTerrainType(), false, false);

							VEC3D vPos;
							CGameCharaBoneContext * pBoneCtx = getBoneContext();
							if( pBoneCtx != nullptr)
							{

								if (m_pOwner->IsRex())
								{

									MATRIX * pMtx = pBoneCtx->getBoneMatrix( TREX_BONE_FOOT_RIGHT);
									if( bReverse)
									{
										pMtx = pBoneCtx->getBoneMatrix( TREX_BONE_FOOT_LEFT);
									}

									i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));
									setY( &vPos, getY( m_pOwner->GetPos()));

								}
								else if (m_pOwner->IsTank())
								{
									MATRIX * pMtx = pBoneCtx->getBoneMatrix( RAPTOR_BONE_FOOT_RIGHT);
									if( bReverse)
									{
										pMtx = pBoneCtx->getBoneMatrix( RAPTOR_BONE_FOOT_LEFT);
									}

									i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));
									setY( &vPos, getY( m_pOwner->GetPos()));

								}
								
								g_pEffectManager->AddEnvironmentEffect( ENV_EFEECT_TREX_WALK, &vPos, &I3_YAXIS);
							}
						}

						m_pOwner->addCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
						//I3TRACE("RIGHT STEP: %f, DeltaTime: %f\n", rElapseTime, tm);
					}
				}
				else
				{
					//I3TRACE("no available: %f, DeltaTime: %f\n", rElapseTime, tm);
				}
			}
		}

		pAnimCtx->setRunElapseTime( rElapseTime);
		pAnimCtx->setRunDuration( rDuration);
	}
	else
		CGameCharaAnimControl3PV::OnMoveRun( pCtx, tm);
}

void CGameDinoAnimControl3PV::OnJumpStartRun( i3AIContext * pCtx, REAL32 tm)
{
	//렙터스팅
	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());

	if( !m_pOwner->isLocal() && getAnimContext()->isEnableRunJump() && !m_pOwner->IsRaptorBase())
	{
		ID_LOWER_DINO_AI nId = pAnimCtx->getMoveIdByDirForDino( getMoveContext()->getMoveDir());
		REAL32 rSyncTime = 0.f;
		if( pAnimCtx->getRunJumpStepTime() > 0.1f && pAnimCtx->getRunJumpStepTime() < 0.4f)
		{
			rSyncTime = 0.31f;
		}

		pAnimCtx->_Play_AnimDino_Lower( nId, rSyncTime);
	}

	
	if( (pAnimCtx->getOldDinoUpperID() == ID_UPPER_DINO_JUMP_START) )
	{
		pAnimCtx->_Play_Anim_DinoUpper( ID_UPPER_DINO_MOVE);
	}

	if(pCtx == nullptr) 
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))	//	땅에 닿으면
			_ProcessJumpEnd();
	}
	else
	{
		if(pCtx->getLocalTime() != 0.f) {
			if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))	//	땅에 닿으면
				_ProcessJumpEnd();
		}
	}
}