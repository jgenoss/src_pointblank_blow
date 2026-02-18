#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaActionControl1PV.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CGameCharaActionControl1PV);


///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action
bool CGameCharaActionControl1PV::OnActionRunLower( VEC3D * pDir)
{
	if (m_pOwner->IsDino())
	{
		return OnActionDinoRunLower( pDir);
	}

	EXT_ASSERT( pDir != nullptr);

	if( i3Vector::IsSame( pDir, &I3_ZEROVECTOR) )
		return false;

	////////////////////////////////////////////////////////////////////
	REAL32 fScaleTime = 1.f;
	ID_LOWER_AI nId = getAnimContext()->getMoveIdByDir( pDir);
	
	if( nId != ID_LOWER_IDLE )
	{
		REAL32 fVel = i3Vector::Length( getMoveContext()->getVelocity());
		fScaleTime = _CalcRunLowerScaleTime( nId, fVel);
	}

	if( (getAnimContext()->getOldLowerID() != nId) )
	{
		setLower( CHARA_LOWER_RUN);
		_Animation_Lower( nId, 0.f, fScaleTime);
	}

	return true;
}

bool CGameCharaActionControl1PV::OnActionDinoRunLower( VEC3D * pDir)
{
	EXT_ASSERT( pDir != nullptr);

	if( i3Vector::IsSame( pDir, &I3_ZEROVECTOR) )
		return false;

	CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());

	////////////////////////////////////////////////////////////////////
	REAL32 fScaleTime = 1.f;
	ID_LOWER_DINO_AI nId = pAnimCtx->getMoveIdByDirForDino( pDir);
	REAL32 fVel = getMoveContext()->getSpeed();

	if( nId != ID_LOWER_DINO_IDLE )
	{
		fVel = i3Vector::Length( getMoveContext()->getVelocity());
		fScaleTime = _CalcRunLowerDinoScaleTime( nId, fVel);
	}

	if( (pAnimCtx->getOldDinoLowerID() != nId) )
	{
		setLower( CHARA_LOWER_RUN);
		_Animation_LowerForDino( nId, 0.f, fScaleTime);
	}

	return true;
}

bool CGameCharaActionControl1PV::OnActionWalkLower( VEC3D * pDir)
{
	EXT_ASSERT( pDir != nullptr);

	////////////////////////////////////////////////////////////////////
	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;

	if( m_pOwner->IsDino())
	{
		CGameDinoAnimContext * pAnimCtx = static_cast<CGameDinoAnimContext*>(getAnimContext());
		ID_LOWER_DINO_AI nId = pAnimCtx->getWalkIdByDirForDino( pDir);
			
		if( nId != ID_LOWER_DINO_IDLE )
		{
			REAL32 fVel = i3Vector::Length( getMoveContext()->getVelocity());
			fScaleTime = GetScaleTimeByWalkType(fVel, pDir);
		}

		if( pAnimCtx->getOldDinoLowerID() != nId )
		{
			setLower( CHARA_LOWER_WALK);
			_Animation_LowerForDino( nId, fSyncTime, fScaleTime);
		}
		else
		{
			REAL32 rRet = getMoveContext()->getSpeed() - fScaleTime;
			if( rRet < -0.01f || rRet > 0.01f )
			{
				pAnimCtx->SetLowerTimeScale( fScaleTime );
				getMoveContext()->setSpeed( fScaleTime);
			}
		}
	}
	else
	{
		ID_LOWER_AI nId = getAnimContext()->getWalkIdByDir( pDir);
			
		if( nId != ID_LOWER_IDLE )
		{
			REAL32 fVel = i3Vector::Length( getMoveContext()->getVelocity());

			fScaleTime = GetScaleTimeByWalkType(fVel, pDir);
		}

		if( getAnimContext()->getOldLowerID() != nId )
		{
			setLower( CHARA_LOWER_WALK);
			if( m_pOwner->IsDino() )
				_Animation_LowerForDino( (ID_LOWER_DINO_AI) nId, fSyncTime, fScaleTime);
			else
				_Animation_Lower( nId, fSyncTime, fScaleTime);
		}
		else
		{
			REAL32 rRet = getMoveContext()->getSpeed() - fScaleTime;
			if( rRet < -0.01f || rRet > 0.01f )
			{
				getAnimContext()->SetLowerTimeScale( fScaleTime );
				getMoveContext()->setSpeed( fScaleTime);
			}
		}
	}

	return true;
}

bool CGameCharaActionControl1PV::OnActionJumpLowerFinish( void)
{
	CGameCharaActionControl::OnActionJumpLowerFinish();
	
	setLower( CHARA_LOWER_IDLE);

	if( m_pOwner->IsDino() )
		_Animation_LowerForDino( ID_LOWER_DINO_JUMP_END);
	else
		_Animation_Lower( ID_LOWER_JUMP_END);

	return true;
}

bool CGameCharaActionControl1PV::OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir)
{
	if( m_pOwner->IsDino() )
	{
		return OnDinoActionDamage( part, pDir);
	}

	//	ActionParameter가 필요한 함수입니다.
	bool	bCheckAction = false;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();

	////////////////////////////////////////////////////////////////////
	bool bCheck = false;
	CHARA_ACTION_UPPER state = getActionContext()->getUpper();

	if( getAnimContext()->getOldUpperID() != ID_UPPER_DAMAGE_FRONT_LEFT_A )
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
	else
	{
		if( (pAIUpper != nullptr) && (pAIUpper->getCurrentAIState() != nullptr) )
		{
			if( pAIUpper->getElapsedTime() > getAnimContext()->getUpperDuration() * 0.5f)
				bCheckAction = true;
		}
	}

	m_pOwner->Cmd_SyncWeapon();

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr) return false;
	    
	if( (pWeapon->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION) && (state == CHARA_UPPER_FIRE) )
		bCheck = true;

	if( !bCheck && (state != CHARA_UPPER_IDLE) && (state != CHARA_UPPER_MOVE) && (state != CHARA_UPPER_JUMP ) )
		return false;

	if( bCheckAction )
	{
		// MachineGun animation play
		pWeapon->PlayAnimation_Damage();
	}

	if( bCheckAction )
	{
		setUpper( CHARA_UPPER_DAMAGE);
		_Animation_Upper( ID_UPPER_DAMAGE_FRONT_LEFT_A);
	}

	return true;
}

bool CGameCharaActionControl1PV::OnDinoActionDamage( CHARA_HIT_PART part, VEC3D * pDir)
{
	//	ActionParameter가 필요한 함수입니다.
	bool	bCheckAction = false;

	////////////////////////////////////////////////////////////////////
	//bool bCheck = false;
	CHARA_ACTION_UPPER state = getActionContext()->getUpper();

	if( ((CGameDinoAnimContext*)getAnimContext())->getOldDinoUpperID() != ID_UPPER_DINO_DAMAGE_RIGHT )
	{
		if( isUpper( CHARA_UPPER_DAMAGE ) == false)
		{
			bCheckAction = true;
		}
	}

	if( !bCheckAction && (state != CHARA_UPPER_IDLE) &&
		(state != CHARA_UPPER_MOVE) && (state != CHARA_UPPER_JUMP ) )
		return false;

	if( bCheckAction )
	{
		if( m_pOwner->IsRex() && (m_pOwner->isDeathblowState() ==false )&&part == CHARA_HIT_VITAL_EYE)
		{
			setUpper( CHARA_UPPER_DAMAGE);
			_Animation_DinoUpper( ID_UPPER_DINO_DAMAGE_RIGHT);

			if( g_pFramework->IsBattleStage())
			{
				CHud		 * pHud		= CHud::i();

				if( pHud != nullptr)
				{
					pHud->AddDeathBlow();
					
					m_pOwner->SetDeathblowState( true);
					CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
					if( pMyChara != nullptr)
						pMyChara->getHUDContext()->ScreenEffect_Blow(10.0f);
				
					g_pSndMng->StartDinoActionSound( 0, GTBDA_ON_DAMAGE_SERIOUSLY, m_pOwner->GetPos(), m_pOwner->is1PV());
				}
			}
		}
	}

	return true;
}

bool CGameCharaActionControl1PV::OnActionExtensionWeapon( bool bActivate)
{
	if( CGameCharaActionControl::OnActionExtensionWeapon( bActivate))
	{
		m_pOwner->Cmd_ApplyCrosshair();
		return true;
	}
		

	return false;
}

bool CGameCharaActionControl1PV::OnActionSwapWeapon( void)
{
	if( CGameCharaActionControl::OnActionSwapWeapon() )
	{
		m_pOwner->Cmd_ApplyCrosshair();
		return true;
	}

	return false;
}


