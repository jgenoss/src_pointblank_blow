#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaActionControl3PV.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"

I3_CLASS_INSTANCE( CGameCharaActionControl3PV);


///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action
bool CGameCharaActionControl3PV::OnActionReset( void)
{
	CGameCharaActionControl::OnActionReset();

	if (m_pOwner->IsDino())
	{
		_Animation_LowerForDino( ID_LOWER_DINO_IDLE);
	}
	else
	{
		_Animation_Lower( ID_LOWER_IDLE);

	}

	//	강제로 적용
	if( getAnimContext()->isAILower())
		getAnimContext()->getAILower()->SetAIStateImmediately( g_pViewer->getDeltaTime());
	
	return true;
}

bool CGameCharaActionControl3PV::OnActionRexRunLower( VEC3D * pDir)
{
	//	ActionParameter가 필요한 함수입니다.
	EXT_ASSERT( pDir != nullptr);
	if( i3Vector::IsSame( pDir, &I3_ZEROVECTOR) )
		return false;

	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_DINO_AI nId = pAnimCtx->getMoveIdByDirForDino( pDir);
	ID_LOWER_DINO_AI oldAI = pAnimCtx->getOldDinoLowerID();
	CHARA_ACTION_LOWER state = CHARA_LOWER_RUN;

	if((m_pOwner->getActionPendingTime() > 0.f) && ( nId != oldAI))	return false;

	REAL32 fVel = getMoveContext()->getSpeed();

	INT32 nID = pAnimCtx->getDinoAILowerID( oldAI );
	I3ASSERT( nID >= 0);
	I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
	i3AIState * pOldAIState = pAnimCtx->getAILower()->getAIState( nID );

	nID = pAnimCtx->getDinoAILowerID( nId );
	I3ASSERT( nID != -1);
	I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
	i3AIState * pNewAIState = pAnimCtx->getAILower()->getAIState( nID );

	REAL32 fBlendTime = pNewAIState->getBlendTime();

	//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
	if( m_pOwner->getActionPendingTime() < 0.f)
	{
		if( isLower( CHARA_LOWER_RUN) ||
			isLower( CHARA_LOWER_WALK ))
		{
			fScaleTime = _CalcRunLowerDinoScaleTime( nId, fVel);
		}
	}

	if( pAnimCtx->isAILower() )
	{
		if( oldAI != nId &&	((oldAI >= ID_LOWER_DINO_IDLE) &&
			(oldAI <= ID_LOWER_DINO_WALK_BACK_RIGHT)) )
		{
			bool bNoResetSync = false;
			if(( oldAI >= ID_LOWER_DINO_WALK_FRONT && oldAI <= ID_LOWER_DINO_WALK_BACK_RIGHT) &&
				(oldAI - nId) == ( ID_LOWER_DINO_WALK_FRONT - ID_LOWER_DINO_MOVE_FRONT))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_MOVE_FRONT ||
				oldAI == ID_LOWER_DINO_MOVE_FRONT_LEFT ||
				oldAI == ID_LOWER_DINO_MOVE_FRONT_RIGHT ) && ( nId != ID_LOWER_DINO_MOVE_FRONT && 
				nId != ID_LOWER_DINO_MOVE_FRONT_LEFT && 
				nId != ID_LOWER_DINO_MOVE_FRONT_RIGHT ))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_MOVE_BACK ||
				oldAI == ID_LOWER_DINO_MOVE_BACK_LEFT ||
				oldAI == ID_LOWER_DINO_MOVE_BACK_RIGHT ) && ( nId != ID_LOWER_DINO_MOVE_BACK && 
				nId != ID_LOWER_DINO_MOVE_BACK_LEFT && 
				nId != ID_LOWER_DINO_MOVE_BACK_RIGHT ))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_MOVE_BACK ||
				oldAI == ID_LOWER_DINO_MOVE_FRONT ) && ( nId != ID_LOWER_DINO_MOVE_BACK && 
				nId != ID_LOWER_DINO_MOVE_FRONT))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_MOVE_LEFT ||
				oldAI == ID_LOWER_DINO_MOVE_RIGHT ) && ( nId != ID_LOWER_DINO_MOVE_LEFT && 
				nId != ID_LOWER_DINO_MOVE_RIGHT))
			{
				bNoResetSync = true;
			}

			if( bNoResetSync )
			{
				fSyncTime = getAnimContext()->GetLowerSyncTime();

				if( pOldAIState->getAnimDuration() > I3_EPS )
				{
					CGameFramework::LoadAIStateAnimation( pNewAIState);

					fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
				}
			}
		}
	}

	if( (oldAI != nId) )
	{
		setLower( state);
		_Animation_LowerForDino( nId, fSyncTime, fScaleTime);

		m_pOwner->setActionPendingTime( fBlendTime + 0.032f);
	}
	else if( pAnimCtx->isAILower() )
	{
		pAnimCtx->SetLowerTimeScale( fScaleTime );
	}

	return true;
}

bool CGameCharaActionControl3PV::OnActionDinoRunLower( VEC3D * pDir)
{

	//	ActionParameter가 필요한 함수입니다.
	EXT_ASSERT( pDir != nullptr);

	if( i3Vector::IsSame( pDir, &I3_ZEROVECTOR) )
		return false;

	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_DINO_AI nId = pAnimCtx->getMoveIdByDirForDino( pDir);
	ID_LOWER_DINO_AI oldAI = pAnimCtx->getOldDinoLowerID();
	CHARA_ACTION_LOWER state = CHARA_LOWER_RUN;

	REAL32 fVel = getMoveContext()->getSpeed();

	if( nId != ID_LOWER_DINO_IDLE )
	{
		fVel = getMoveContext()->getSpeed();
		fScaleTime = _CalcRunLowerDinoScaleTime( nId, fVel);

		//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
		if( isLower( CHARA_LOWER_RUN) ||
			isLower( CHARA_LOWER_WALK ))
		{
			if( pAnimCtx->isAILower() )
			{
				fSyncTime = pAnimCtx->getLowerLocalTime();

				if( oldAI != nId &&	((oldAI > ID_LOWER_DINO_IDLE) &&	(oldAI <= ID_LOWER_DINO_WALK_BACK_RIGHT)) )
				{	
					INT32 nID = pAnimCtx->getDinoAILowerID( oldAI );
					I3ASSERT( nID >= 0);
					I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
					i3AIState * pOldAIState = pAnimCtx->getAILower()->getAIState( nID );

					nID = pAnimCtx->getDinoAILowerID( nId );
					I3ASSERT( nID != -1);
					I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
					i3AIState * pNewAIState = pAnimCtx->getAILower()->getAIState( nID );

					if( pOldAIState->getAnimDuration() > I3_EPS )
					{
						CGameFramework::LoadAIStateAnimation( pNewAIState);

						fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
					}
				}
			}

			//I3TRACE( "fVel %f SyncTime:%.4f,ScaleTime:%.4f\n", fVel, fSyncTime, fScaleTime );
		}
	}

	if( (oldAI != nId) )
	{
		setLower( state);
		_Animation_LowerForDino( nId, fSyncTime, fScaleTime);
	}
	else if( pAnimCtx->isAILower() )
		pAnimCtx->SetLowerTimeScale( fScaleTime );

	return true;	
}

bool CGameCharaActionControl3PV::OnActionRunLower( VEC3D * pDir)
{
	if( m_pOwner->IsRex())
	{
		return OnActionRexRunLower( pDir);
	}

	if (m_pOwner->IsDino())
	{
		return OnActionDinoRunLower( pDir);
	}

	//	ActionParameter가 필요한 함수입니다.
	EXT_ASSERT( pDir != nullptr);

	if( i3Vector::IsSame( pDir, &I3_ZEROVECTOR) )
		return false;

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_AI nId = getAnimContext()->getMoveIdByDir( pDir);
	ID_LOWER_AI oldAI = getAnimContext()->getOldLowerID();
	CHARA_ACTION_LOWER state = CHARA_LOWER_RUN;

	REAL32 fVel = getMoveContext()->getSpeed();

	if( nId != ID_LOWER_IDLE )
	{
		fVel = getMoveContext()->getSpeed();
		fScaleTime = _CalcRunLowerScaleTime( nId, fVel);
	
		//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
		if( isLower( CHARA_LOWER_RUN) ||
			isLower( CHARA_LOWER_WALK ))
		{
			if( getAnimContext()->isAILower() )
			{
				fSyncTime = getAnimContext()->getLowerLocalTime();

				if( oldAI != nId &&	((oldAI > ID_LOWER_IDLE) &&	(oldAI <= ID_LOWER_WALK_BACK_RIGHT)) )
				{	
					INT32 nID = getAnimContext()->getAILowerID( getActionContext()->getBodyLower(), oldAI );
					I3ASSERT( nID >= 0);
					I3ASSERT( nID < getAnimContext()->getAILower()->getAIStateCount());
					i3AIState * pOldAIState = getAnimContext()->getAILower()->getAIState( nID );
					
					nID = getAnimContext()->getAILowerID( getActionContext()->getBodyLower(), nId );
					I3ASSERT( nID != -1);
					I3ASSERT( nID < getAnimContext()->getAILower()->getAIStateCount());
					i3AIState * pNewAIState = getAnimContext()->getAILower()->getAIState( nID );

					if( pOldAIState->getAnimDuration() > I3_EPS )
					{
						CGameFramework::LoadAIStateAnimation( pNewAIState);

						fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
					}
				}
			}

			//I3TRACE( "fVel %f SyncTime:%.4f,ScaleTime:%.4f\n", fVel, fSyncTime, fScaleTime );
		}
	}

	if( (oldAI != nId) )
	{
		setLower( state);
		_Animation_Lower( nId, fSyncTime, fScaleTime);
	}
	else if( getAnimContext()->isAILower() )
		getAnimContext()->SetLowerTimeScale( fScaleTime );

	return true;	
}
bool CGameCharaActionControl3PV::OnActionWalkDinoUpper( void)
{	
	//	ActionParameter가 필요한 함수입니다.
	if( m_pOwner->getAttackIdleTime() > 0.f)	return false;
	if( m_pOwner->getActionPendingTime() > 0.f)	return false;

	return CGameCharaActionControl::OnActionWalkDinoUpper();
}

bool CGameCharaActionControl3PV::OnActionRunDinoUpper( void)
{	
	//	ActionParameter가 필요한 함수입니다.
	if( m_pOwner->getAttackIdleTime() > 0.f)	return false;
	if( m_pOwner->getActionPendingTime() > 0.f)	return false;
	
	return CGameCharaActionControl::OnActionRunDinoUpper();
}

bool CGameCharaActionControl3PV::OnActionRunUpper( void)
{	//	ActionParameter가 필요한 함수입니다.
	if( m_pOwner->getAttackIdleTime() > 0.f)	return false;
	
	return CGameCharaActionControl::OnActionRunUpper();
}


bool CGameCharaActionControl3PV::OnActionWalkLower( VEC3D * pDir)
{
	
	if( m_pOwner->IsRex())
	{
		return OnActionRexWalkLower( pDir);
	}
	if (m_pOwner->IsDino())
	{
		return OnActionDinoWalkLower( pDir);
	}

	EXT_ASSERT( pDir != nullptr);
	EXT_ASSERT( !i3Vector::IsSame( pDir, &I3_ZEROVECTOR));

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_AI nId = getAnimContext()->getWalkIdByDir( pDir);
	ID_LOWER_AI oldAI = getAnimContext()->getOldLowerID();

	i3AIContext * pAILower = getAnimContext()->getAILower();
		
	if( nId != ID_LOWER_IDLE )
	{
		REAL32 fVel = getMoveContext()->getSpeed();

		fScaleTime = GetScaleTimeByWalkType(fVel, pDir);
				
		//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
		if( pAILower != nullptr )
		{
			if( isLower( CHARA_LOWER_RUN) ||
				isLower( CHARA_LOWER_WALK ))
			{
				fSyncTime = getAnimContext()->getLowerLocalTime();
				
				if( oldAI != nId && (oldAI > ID_LOWER_IDLE && oldAI <= ID_LOWER_WALK_BACK_RIGHT) )
				{	
					INT32 nID = getAnimContext()->getAILowerID( getActionContext()->getBodyLower(), nId );
					I3ASSERT( nID >= 0);
					I3ASSERT( nID < pAILower->getAIStateCount());
					i3AIState * pNewAIState = pAILower->getAIState( nID );

					nID = getAnimContext()->getAILowerID( getActionContext()->getBodyLower(), oldAI);
					I3ASSERT( nID != -1);
					I3ASSERT( nID < pAILower->getAIStateCount());
					i3AIState * pOldAIState = pAILower->getAIState( nID );
					if( pOldAIState->getAnimDuration() > I3_EPS )
					{
						CGameFramework::LoadAIStateAnimation( pNewAIState);

						fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
					}
				}

				//I3TRACE( "SyncTime:%.4f,ScaleTime:%.4f\n", fSyncTime, fScaleTime );
			}
		}
	}

	if( oldAI != nId )
	{
		setLower( CHARA_LOWER_WALK);
		_Animation_Lower( nId, fSyncTime, fScaleTime);
	}
	else if( pAILower != nullptr)
	{
		getAnimContext()->SetLowerTimeScale( fScaleTime );
	}

	return true;
}

bool CGameCharaActionControl3PV::OnActionDinoWalkLower( VEC3D * pDir)
{
	EXT_ASSERT( pDir != nullptr);
	EXT_ASSERT( !i3Vector::IsSame( pDir, &I3_ZEROVECTOR));

	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_DINO_AI nId = pAnimCtx->getWalkIdByDirForDino( pDir);
	ID_LOWER_DINO_AI oldAI = pAnimCtx->getOldDinoLowerID();

	if((m_pOwner->getActionPendingTime() > 0.f) && ( nId != oldAI))	return false;

	REAL32 fVel = getMoveContext()->getSpeed();
	i3AIContext * pAILower = pAnimCtx->getAILower();

	INT32 nID = pAnimCtx->getDinoAILowerID( oldAI );
	I3ASSERT( nID >= 0);
	I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
	i3AIState * pOldAIState = pAnimCtx->getAILower()->getAIState( nID );

	nID = pAnimCtx->getDinoAILowerID( nId );
	I3ASSERT( nID != -1);
	I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
	i3AIState * pNewAIState = pAnimCtx->getAILower()->getAIState( nID );

	REAL32 fBlendTime = pNewAIState->getBlendTime();

	//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
	if( m_pOwner->getActionPendingTime() < 0.f)
	{
		if( isLower( CHARA_LOWER_RUN) || isLower( CHARA_LOWER_WALK ))
		{
			fScaleTime = _CalcRunLowerDinoScaleTime( nId, fVel);
		}
	}

	//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
	if( pAILower != nullptr )
	{
		if( oldAI != nId && (oldAI >= ID_LOWER_DINO_IDLE && oldAI <= ID_LOWER_DINO_WALK_BACK_RIGHT) )
		{
			bool bNoResetSync = false;
			if(( oldAI >= ID_LOWER_DINO_MOVE_FRONT && oldAI <= ID_LOWER_DINO_MOVE_BACK_RIGHT) &&
				(nId - oldAI) == ( ID_LOWER_DINO_WALK_FRONT - ID_LOWER_DINO_MOVE_FRONT))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_FRONT ||
				oldAI == ID_LOWER_DINO_WALK_FRONT_LEFT ||
				oldAI == ID_LOWER_DINO_WALK_FRONT_RIGHT ) && ( nId != ID_LOWER_DINO_WALK_FRONT && 
				nId != ID_LOWER_DINO_WALK_FRONT_LEFT && 
				nId != ID_LOWER_DINO_WALK_FRONT_RIGHT ))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_BACK ||
				oldAI == ID_LOWER_DINO_WALK_BACK_LEFT ||
				oldAI == ID_LOWER_DINO_WALK_BACK_RIGHT ) && ( nId != ID_LOWER_DINO_WALK_BACK && 
				nId != ID_LOWER_DINO_WALK_BACK_LEFT && 
				nId != ID_LOWER_DINO_WALK_BACK_RIGHT ))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_BACK ||
				oldAI == ID_LOWER_DINO_WALK_FRONT ) && ( nId != ID_LOWER_DINO_WALK_BACK && 
				nId != ID_LOWER_DINO_WALK_FRONT))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_LEFT ||
				oldAI == ID_LOWER_DINO_WALK_RIGHT ) && ( nId != ID_LOWER_DINO_WALK_LEFT && 
				nId != ID_LOWER_DINO_WALK_RIGHT))
			{
				bNoResetSync = true;
			}

			if( bNoResetSync )
			{
				fSyncTime = getAnimContext()->GetLowerSyncTime();
				if( pOldAIState->getAnimDuration() > I3_EPS )
				{
					CGameFramework::LoadAIStateAnimation( pNewAIState);

					fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
				}
			}
		}
	}

	if( oldAI != nId )
	{
		setLower( CHARA_LOWER_WALK);
		_Animation_LowerForDino( nId, fSyncTime, fScaleTime);

		m_pOwner->setActionPendingTime( fBlendTime + 0.032f);
	}
	else if( pAILower != nullptr)
	{
		pAnimCtx->SetLowerTimeScale( fScaleTime );
	}

	return true;
}


bool CGameCharaActionControl3PV::OnActionRexWalkLower( VEC3D * pDir)
{
	EXT_ASSERT( pDir != nullptr);
	EXT_ASSERT( !i3Vector::IsSame( pDir, &I3_ZEROVECTOR));

	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_DINO_AI nId = pAnimCtx->getWalkIdByDirForDino( pDir);
	ID_LOWER_DINO_AI oldAI = pAnimCtx->getOldDinoLowerID();

	if((m_pOwner->getActionPendingTime() > 0.f) && ( nId != oldAI))	return false;

	REAL32 fVel = getMoveContext()->getSpeed();
	i3AIContext * pAILower = pAnimCtx->getAILower();

	INT32 nID = pAnimCtx->getDinoAILowerID( oldAI );
	I3ASSERT( nID >= 0);
	I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
	i3AIState * pOldAIState = pAnimCtx->getAILower()->getAIState( nID );

	nID = pAnimCtx->getDinoAILowerID( nId );
	I3ASSERT( nID != -1);
	I3ASSERT( nID < pAnimCtx->getAILower()->getAIStateCount());
	i3AIState * pNewAIState = pAnimCtx->getAILower()->getAIState( nID );

	REAL32 fBlendTime = pNewAIState->getBlendTime();

	//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
	if( m_pOwner->getActionPendingTime() < 0.f)
	{
		if( isLower( CHARA_LOWER_RUN) || isLower( CHARA_LOWER_WALK ))
		{
			fScaleTime = _CalcRunLowerDinoScaleTime( nId, fVel);
		}
	}

	//	현재 이동 중이다. ( 이동 애니메이션 하체 동작시간과 맞춰 플레이)
	if( pAILower != nullptr )
	{
		if( oldAI != nId && (oldAI >= ID_LOWER_DINO_IDLE && oldAI <= ID_LOWER_DINO_WALK_BACK_RIGHT) )
		{
			bool bNoResetSync = false;
			if(( oldAI >= ID_LOWER_DINO_MOVE_FRONT && oldAI <= ID_LOWER_DINO_MOVE_BACK_RIGHT) &&
						(nId - oldAI) == ( ID_LOWER_DINO_WALK_FRONT - ID_LOWER_DINO_MOVE_FRONT))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_FRONT ||
				oldAI == ID_LOWER_DINO_WALK_FRONT_LEFT ||
				oldAI == ID_LOWER_DINO_WALK_FRONT_RIGHT ) && ( nId != ID_LOWER_DINO_WALK_FRONT && 
														nId != ID_LOWER_DINO_WALK_FRONT_LEFT && 
														nId != ID_LOWER_DINO_WALK_FRONT_RIGHT ))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_BACK ||
				oldAI == ID_LOWER_DINO_WALK_BACK_LEFT ||
				oldAI == ID_LOWER_DINO_WALK_BACK_RIGHT ) && ( nId != ID_LOWER_DINO_WALK_BACK && 
														nId != ID_LOWER_DINO_WALK_BACK_LEFT && 
														nId != ID_LOWER_DINO_WALK_BACK_RIGHT ))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_BACK ||
				oldAI == ID_LOWER_DINO_WALK_FRONT ) && ( nId != ID_LOWER_DINO_WALK_BACK && 
														nId != ID_LOWER_DINO_WALK_FRONT))
			{
				bNoResetSync = true;
			}
			else if(( oldAI == ID_LOWER_DINO_WALK_LEFT ||
				oldAI == ID_LOWER_DINO_WALK_RIGHT ) && ( nId != ID_LOWER_DINO_WALK_LEFT && 
														nId != ID_LOWER_DINO_WALK_RIGHT))
			{
				bNoResetSync = true;
			}

			if( bNoResetSync )
			{
				fSyncTime = pAnimCtx->GetLowerSyncTime();

				if( pOldAIState->getAnimDuration() > I3_EPS )
				{
					CGameFramework::LoadAIStateAnimation( pNewAIState);

					fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
				}
			}
		}
	}

	if( oldAI != nId )
	{
		setLower( CHARA_LOWER_WALK);
		_Animation_LowerForDino( nId, fSyncTime, fScaleTime);

		m_pOwner->setActionPendingTime( fBlendTime + 0.032f);
	}
	else if( pAILower != nullptr)
	{
		pAnimCtx->SetLowerTimeScale( fScaleTime );
	}

	return true;
}

bool CGameCharaActionControl3PV::OnActionSteadyLower( void)
{
	if( m_pOwner->getActionPendingTime() > 0.f)				return false;

	return CGameCharaActionControl::OnActionSteadyLower();
}

bool CGameCharaActionControl3PV::OnActionJumpLowerFinish()
{
	CGameCharaActionControl::OnActionJumpLowerFinish();

	if (m_pOwner->IsDino())
	{
		if( getAnimContext()->isEnableRunJump() == false)
			_Animation_LowerForDino( ID_LOWER_DINO_JUMP_END);
		else
			_Animation_LowerForDino( ID_LOWER_DINO_JUMP_END_MOVE);

	}
	else
	{
		if( getAnimContext()->isEnableRunJump() == false)
			_Animation_Lower( ID_LOWER_JUMP_END);

	}
	return true;
}

bool CGameCharaActionControl3PV::OnActionDamageHuman( CHARA_HIT_PART part, VEC3D * pDir)
{
	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr)	return false;

	INT32 nType = i3Math::Rand() % 2;
	VEC3D * pAt = i3Matrix::GetAt( m_pOwner->GetMatrix());

	//	앞방향 구분
	bool bFront = (i3Vector::Dot( pAt, pDir) < 0.0f);

	ID_UPPER_AI id;
		
	switch( part)
	{	
	case CHARA_HIT_HEAD		:
	case CHARA_HIT_NECK		:
	case CHARA_HIT_NECK1:
	case CHARA_HIT_NECK2:				// B_Neck
	case CHARA_HIT_NECK_LOWER:
	case CHARA_HIT_BODY1	:
	case CHARA_HIT_BODY2	:
	case CHARA_HIT_BODY3	:		//	몸통을 맞았다.
		{
			if( bFront)
			{	
				if( nType == 0)
				{
					if( i3Math::Rand() % 2 == 0)	id = ID_UPPER_DAMAGE_FRONT_LEFT_A;
					else							id = ID_UPPER_DAMAGE_FRONT_LEFT_B;
				}
				else
				{
					if( i3Math::Rand() % 2 == 0)	id = ID_UPPER_DAMAGE_FRONT_RIGHT_A;
					else							id = ID_UPPER_DAMAGE_FRONT_RIGHT_B;
				}
			}
			else
			{
				if( (i3Math::Rand() % 2) == 0)		id = ID_UPPER_DAMAGE_BACK_A;
				else								id = ID_UPPER_DAMAGE_BACK_B;
			}
		}
		break;	

	//	왼쪽 데미지
	case CHARA_HIT_LUARM	:		//	왼쪽 팔을 맞았다.
	case CHARA_HIT_LDARM	:
	case CHARA_HIT_LHAND	:
	case CHARA_HIT_LCLAVICLE	:
		{
			if( bFront)
			{
				if( nType == 0)						id = ID_UPPER_DAMAGE_FRONT_LEFT_A;
				else								id = ID_UPPER_DAMAGE_FRONT_LEFT_B;
			}
			else
			{
				if( nType == 0)						id = ID_UPPER_DAMAGE_BACK_A;
				else								id = ID_UPPER_DAMAGE_BACK_B;
			}
		}
		break;

	//	오른쪽 데미지
	case CHARA_HIT_RUARM	:
	case CHARA_HIT_RDARM	:
	case CHARA_HIT_RHAND	:
	case CHARA_HIT_RCLAVICLE	:
		{
			if( bFront)
			{
				if( nType == 0)						id = ID_UPPER_DAMAGE_FRONT_RIGHT_A;
				else								id = ID_UPPER_DAMAGE_FRONT_RIGHT_B;
			}
			else
			{
				if( nType == 0)						id = ID_UPPER_DAMAGE_BACK_A;
				else								id = ID_UPPER_DAMAGE_BACK_B;
			}
		}
		break;
	//	다리
	case CHARA_HIT_ROOT		:
	case CHARA_HIT_LULEG	:
	case CHARA_HIT_LDLEG	:
	case CHARA_HIT_RULEG	:
	case CHARA_HIT_RDLEG	:
	case CHARA_HIT_LFOOT	:
	case CHARA_HIT_RFOOT	:
		{
			if( nType == 0)							id = ID_UPPER_DAMAGE_BACK_B;
			else									id = ID_UPPER_DAMAGE_BACK_A;
		}
		break;

	default :	
		{
			I3TRACE( "[%s] : WARNING! Hit part(%d) is Invalid!\n", __FUNCTION__, part ); 
			
			setUpper( CHARA_UPPER_DAMAGE);
			_Animation_Upper( ID_UPPER_DAMAGE_BACK_B);

			return false;
		}
		break;
	}
	
	bool	bCheckAction = false;
	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();

	if( getAnimContext()->getOldUpperID() != id )
	{
		if( isUpper( CHARA_UPPER_DAMAGE ) )
		{
			if( (pAIUpper != nullptr) &&
				(pAIUpper->getElapsedTime() > getAnimContext()->getUpperDuration() * 0.2f) )
				bCheckAction = true;
		}
		else
			bCheckAction = true;
	}
	else if( (pAIUpper != nullptr) &&
		(pAIUpper->getElapsedTime() > getAnimContext()->getUpperDuration() * 0.5f) )
		bCheckAction = true;

	if( bCheckAction )
	{
		setUpper( CHARA_UPPER_DAMAGE);
		_Animation_Upper( id);
	}

	return true;
}

bool CGameCharaActionControl3PV::OnActionDamageRex( CHARA_HIT_PART part, VEC3D * pDir)
{
	ID_UPPER_DINO_AI id;// = ID_UPPER_DINO_UNKNOWN;

	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());
		
	switch( part)
	{	
		case CHARA_HIT_VITAL_EYE:
		{
			if( isLower(CHARA_LOWER_RUN))
				id = ID_UPPER_DINO_DAMAGE_MOVE_RIGHT;
			else
				id = ID_UPPER_DINO_DAMAGE_RIGHT;
			break;
		}break;

		case CHARA_HIT_ROOT:					// B_Root
		case CHARA_HIT_BODY3:				// B_Spine1
		case CHARA_HIT_BODY2:				// B_Spine2
		case CHARA_HIT_BODY1:				// B_Spine3
		case CHARA_HIT_NECK:					// B_Neck
		case CHARA_HIT_NECK2:				// B_Neck
		case CHARA_HIT_HEAD:					// H_Head
		case CHARA_HIT_JAW:					// H_Head
		case CHARA_HIT_TAIL1:				// T_Tail1
		case CHARA_HIT_TAIL2:				// T_Tail2
		case CHARA_HIT_TAIL3:				// T_Tail3
		case CHARA_HIT_TAIL4:				// T_Tail4
		case CHARA_HIT_LCLAVICLE:			// B_L Clavicle
		case CHARA_HIT_LUARM:				// AU_L UpperArm
		case CHARA_HIT_LDARM:				// AD_L ForeArm
		case CHARA_HIT_LHAND:				// HN_L Hand
		case CHARA_HIT_LULEG:				// LU_L Thigh
		case CHARA_HIT_LDLEG:				// LD_L Calf
		case CHARA_HIT_LHHORSE_LINK:			// LH_L HorseLink
		case CHARA_HIT_LFOOT:				// F_L Foot
		case CHARA_HIT_RCLAVICLE:			// B_R Clavicle
		case CHARA_HIT_RUARM:				// AU_R UpperArm
		case CHARA_HIT_RDARM:				// AD_R ForeArm
		case CHARA_HIT_RHAND:				// HN_R Hand
		case CHARA_HIT_RULEG:				// LU_R Thigh
		case CHARA_HIT_RDLEG:				// LD_R Calf
		case CHARA_HIT_RHHORSE_LINK:			// LH_R HorseLink
		case CHARA_HIT_RFOOT:				// F_R Foot
		case CHARA_HIT_NECK_LOWER:
		default:
		return false;
	}
	
	bool	bCheckAction = false;

	if( pAnimCtx->getOldDinoUpperID() != id )
	{
		if( isUpper( CHARA_UPPER_DAMAGE ) == false)
		{
			bCheckAction = true;
		}
	}

	if( bCheckAction )
	{
		setUpper( CHARA_UPPER_DAMAGE);
		_Animation_DinoUpper( id);
		if(part == CHARA_HIT_VITAL_EYE)
		{
			g_pSndMng->StartDinoActionSound( 0, GTBDA_ON_DAMAGE_SERIOUSLY, m_pOwner->GetPos(), m_pOwner->is1PV());
		}
	}

	return true;
}

bool CGameCharaActionControl3PV::OnActionDamageRaptor( CHARA_HIT_PART part, VEC3D * pDir)
{
	ID_UPPER_DINO_AI id; // = ID_UPPER_DINO_UNKNOWN;

	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());
	
	switch( part)
	{	
	case CHARA_HIT_HEAD		:
	case CHARA_HIT_NECK		:
	case CHARA_HIT_NECK1:
	case CHARA_HIT_NECK2:				// B_Neck
	case CHARA_HIT_NECK_LOWER:
	case CHARA_HIT_BODY1	:
	case CHARA_HIT_BODY2	:
	case CHARA_HIT_BODY3	:		//	몸통을 맞았다.
		{

			id = ID_UPPER_DINO_DAMAGE_RIGHT;

		}
		break;	

		//	왼쪽 데미지
	case CHARA_HIT_LUARM	:		//	왼쪽 팔을 맞았다.
	case CHARA_HIT_LDARM	:
	case CHARA_HIT_LHAND	:
	case CHARA_HIT_LCLAVICLE	:
		{
			if( isLower(CHARA_LOWER_RUN))
				id = ID_UPPER_DINO_DAMAGE_MOVE_LEFT;
			else
				id = ID_UPPER_DINO_DAMAGE_LEFT;
		}
		break;

		//	오른쪽 데미지
	case CHARA_HIT_RUARM	:
	case CHARA_HIT_RDARM	:
	case CHARA_HIT_RHAND	:
	case CHARA_HIT_RCLAVICLE	:
		{
			if( isLower(CHARA_LOWER_RUN))
				id = ID_UPPER_DINO_DAMAGE_MOVE_RIGHT;
			else
				id = ID_UPPER_DINO_DAMAGE_LEFT;
		}
		break;
		//	다리
	case CHARA_HIT_ROOT		:
	case CHARA_HIT_LULEG	:
	case CHARA_HIT_LDLEG	:
	case CHARA_HIT_RULEG	:
	case CHARA_HIT_RDLEG	:
	case CHARA_HIT_LFOOT	:
	case CHARA_HIT_RFOOT	:
		{
			id = ID_UPPER_DINO_DAMAGE_LEFT;
		}
		break;

	default :	
		{
			return false;
		}
		break;
	}
	
	
	bool	bCheckAction = false;
	i3AIContext * pAIUpper = pAnimCtx->getAIUpper();

	if( pAnimCtx->getOldDinoUpperID() != id )
	{
		if( isUpper( CHARA_UPPER_DAMAGE ) )
		{
			if( (pAIUpper != nullptr) &&
				(pAIUpper->getElapsedTime() > pAnimCtx->getUpperDuration() * 0.2f) )
				bCheckAction = true;
		}
		else
			bCheckAction = true;
	}
	else if( (pAIUpper != nullptr) &&
		(pAIUpper->getElapsedTime() > pAnimCtx->getUpperDuration() * 0.5f) )
		bCheckAction = true;

	if( bCheckAction )
	{
		setUpper( CHARA_UPPER_DAMAGE);
		_Animation_DinoUpper( id);
	}

	return true;
}

bool CGameCharaActionControl3PV::OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir)
{	//	ActionParameter가 필요한 함수입니다.
	
	if(m_pOwner->IsRaptorBase())
	{
		return OnActionDamageRaptor( part, pDir);
	}
	else if(m_pOwner->IsRex())
	{
		return OnActionDamageRex( part, pDir);
	}
	else
	{
		return OnActionDamageHuman( part, pDir);
	}

	
}

bool CGameCharaActionControl3PV::OnActionProfileIdleA( void)
{
	REAL32 fSyncTime = 0.f;

	if( getAnimContext()->isAIUpper() &&
		isUpper( CHARA_UPPER_NONEBATTLE_IDLE))
	{	
		fSyncTime = getAnimContext()->getUpperAniTimer();
	}
	
	if( fSyncTime < 0.1f)
		fSyncTime = 0.1f;

	setUpper( CHARA_UPPER_NONEBATTLE_IDLE);
	_Animation_Upper( ID_UPPER_NONEBATTLE_IDLE, fSyncTime);

	return true;
}

bool CGameCharaActionControl3PV::OnActionProfileIdleB( void)
{
	REAL32 fSyncTime = 0.f;

	if( getAnimContext()->isAIUpper() &&
		isUpper( CHARA_UPPER_NONEBATTLE_WEAPONSHOW))
		fSyncTime = getAnimContext()->getUpperAniTimer();
	else
		getAnimContext()->setUpperAniTimer( 0.f);

	if( fSyncTime < 0.1f)
		fSyncTime = 0.1f;

	setUpper( CHARA_UPPER_NONEBATTLE_WEAPONSHOW);
	_Animation_Upper( ID_UPPER_NONEBATTLE_SHOWWEAPON, fSyncTime);

	return true;
}

bool CGameCharaActionControl3PV::OnActionProfileIdleChange( bool bIsSameWeapon)
{
	REAL32 fSyncTime = 0.0f;

	// Change애니메이션중에, 이전과 같은 무기인경우에는, 싱크만 맞춥니다.
	if( bIsSameWeapon == true)
		fSyncTime = getAnimContext()->getUpperAniTimer();
	else
		getAnimContext()->setUpperAniTimer( 0.0f);

	setUpper( CHARA_UPPER_NONEBATTLE_IDLE_CHANGE);
	_Animation_Upper( ID_UPPER_NONEBATTLE_IDLE_CHANGE, fSyncTime);

	return true;
}

bool CGameCharaActionControl3PV::OnActionProfileIdleStand( void)
{
	REAL32 fSyncTime = 0.f;

	if( getAnimContext()->isAIUpper() &&
		isUpper( CHARA_UPPER_NONEBATTLE_IDLE_STAND))
		fSyncTime = getAnimContext()->getUpperAniTimer();
	else
		getAnimContext()->setUpperAniTimer( 0.f);

	if( fSyncTime < 0.1f)
		fSyncTime = 0.1f;

	setUpper( CHARA_UPPER_NONEBATTLE_IDLE_STAND);
	_Animation_Upper( ID_UPPER_NONEBATTLE_IDLE_STAND, fSyncTime);

	return true;
}
