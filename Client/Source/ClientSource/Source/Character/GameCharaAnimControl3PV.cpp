#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaAnimControl3PV.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "GameDinoAnimContext.h"
#include "i3Base/string/ext/generic_string_cat.h"

I3_CLASS_INSTANCE( CGameCharaAnimControl3PV);

void CGameCharaAnimControl3PV::OnChangeAIContext( const char * pszPVState)
{
	CGameCharaAnimControl::OnChangeAIContext( "3PV:");

	char szAILowPath[ 256] = "Lower:";
	char * pszStandState = "Stand:";	
	switch( getActionContext()->getBodyLower())
	{
	case CHARA_BODYLOWER_STAND:		pszStandState = "Stand:";		break;
	case CHARA_BODYLOWER_CROUCH:		pszStandState = "Sit:";			break;
	}

	i3::generic_string_cat( szAILowPath, pszStandState);	

	//	Lower AI Path
	szAILowPath[ strlen( szAILowPath) - 1] = '\0';
	
	//	3인칭 일때
	i3AIContext * pAILower = m_pOwner->FindAI( szAILowPath);

	if (pAILower)
	{
		getAnimContext()->setAILower(pAILower);
	}
	else	
	{
		TRACE("CGameCharaAnimControl3PV::OnChangeAIContext pAILower = %s\n", szAILowPath);
	}
		
	//	UpperRoot를 기준으로 m_pAILower만 활성하고 관련없는 AIState는 Disable 시킨다.
	i3AIContext * UpperRoot = m_pOwner->FindAI( "Lower");

	if (UpperRoot)
	{
		I3ASSERT_RETURN(pAILower);
		UpperRoot->setGroupEnable( pAILower);
	}
	else
		TRACE("CGameCharaAnimControl3PV::OnChangeAIContext UpperRoot = Lower\n");
	
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameCharaAnimControl3PV::OnMoveRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaAnimControl::OnMoveRun( pCtx, tm);
	// Add code here

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return;

	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaAnimContext * pAnimCtx = getAnimContext();

	REAL32 rElapseTime = pAnimCtx->getRunElapseTime();
	REAL32 rDuration = pAnimCtx->getRunDuration();

	//	서서 이동할때만 발자국 소리나오게...현재 앉아 이동하면 발자국 소리 안나오게 한다.
	if( pActionCtx->isLower( CHARA_LOWER_RUN) ||
		pActionCtx->isLower( CHARA_LOWER_WALK))
	{	
//		if( isNetwork()) I3TRACE("ON MOVE RUN : %f, Elapse : %f\n", tm, getAnimContext()->getRunElapseTime());
		//I3TRACE("ON MOVE RUN : %f, Elapse : %f\n", tm, getAnimContext()->getRunElapseTime());

		//	발자국 소리 플레이~
		REAL32 rMaxDuration = ANIMTIME_MOVE_DURATION;
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
					rDuration *= 1.8f;
				}

				pAnimCtx->setLeftStepTime( rDuration * 0.822f);
				pAnimCtx->setRightStepTime( rDuration * 0.322f);
			}
		}

		rElapseTime = _PlayFootStepSound( rDuration, rElapseTime, tm);
	}

	pAnimCtx->setRunElapseTime( rElapseTime);
	pAnimCtx->setRunDuration( rDuration);
}


void CGameCharaAnimControl3PV::OnJumpStartRun( i3AIContext * pCtx, REAL32 tm)
{
	//렙터스팅
	if( !m_pOwner->isLocal() && getAnimContext()->isEnableRunJump() && !m_pOwner->IsRaptorBase())
	{
		ID_LOWER_AI nId = getAnimContext()->getMoveIdByDir( getMoveContext()->getMoveDir());
		REAL32 rSyncTime = 0.f;
		if( getAnimContext()->getRunJumpStepTime() > 0.1f && getAnimContext()->getRunJumpStepTime() < 0.4f)
		{
			rSyncTime = 0.31f;
		}

		getAnimContext()->_Play_Anim_Lower( nId, rSyncTime);
	}

	if( (getAnimContext()->getOldUpperID() == ID_UPPER_JUMP_START) )
	{
		getAnimContext()->_Play_Anim_Upper( ID_UPPER_MOVE);
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