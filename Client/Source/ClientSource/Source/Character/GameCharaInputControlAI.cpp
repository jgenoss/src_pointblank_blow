#include "pch.h"
#include "GameCharaInputControlAI.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GameStateMgr.h"
#include "i3Base/itl/vector_set.h"
#include "MyRoomInfoContext.h"
#include "BattleObjContext.h"
#include "HackContext.h"
#include "GameMissionManager.h"
#include "GameMaster.h"

#if 0
#define		AIDUMP			I3TRACE
#else
#define		AIDUMP			__noop
#endif

#include "../ui/Batmanager.h"


I3_CLASS_INSTANCE( CGameCharaInputControlAI);


CGameCharaInputControlAI::CGameCharaInputControlAI()
{
	i3mem::FillZero(m_EvasionRate, sizeof(m_EvasionRate));
	i3mem::FillZero(m_WayPoint, sizeof(m_WayPoint));

	OnResetAI();
}

CGameCharaInputControlAI::~CGameCharaInputControlAI()
{
	I3_SAFE_RELEASE( m_pGamePath);

	for( size_t i = 0;i < m_RemoveObjectList.size(); ++i)
	{
		TMPOBJ_INFO * pInfo = m_RemoveObjectList[i];
		
		I3MEM_SAFE_FREE( pInfo);
	}
	m_RemoveObjectList.clear();
}

void CGameCharaInputControlAI::BindAI(AI_BIND_DATA* pData)
{
	// 출력 정보 (닉네임, 성별, 계급, 사용무기, 사용장비)
	m_nickname = pData->_wstr_nickname;	
	m_rank = pData->_rank;

	m_primaryWeaponName = pData->_primaryWeaponName;
	m_secondaryWeaponName = pData->_secondaryWeaponName;
	m_meleeWeaponName = pData->_meleeWeaponName;
	m_throwWeaponName = pData->_throwWeaponName;
	m_itemWeaponName= pData->_itemWeaponName;
	m_hp = pData->_hp;
	m_respawnTime = pData->_respawnTime;

	//Attack Time
	m_AIData._PreAttack = pData->_AIData._PreAttack;
	m_AIData._Attack = pData->_AIData._Attack;
	m_AIData._Attack += pData->_AIData._PreAttack;

	m_AIData._AfterAttack = pData->_AIData._AfterAttack;
	m_AIData._AfterAttack += pData->_AIData._Attack;

	m_AIData._TotalTime += pData->_AIData._TotalTime;

	//SeeSight Angle
	m_AIData._Angle = pData->_AIData._Angle;
	m_AIData._Range = pData->_AIData._Range;

	//Targeting Level
	m_AIData._Deviation = pData->_AIData._Deviation;

	m_AIData._KeepWatchTime = pData->_AIData._KeepWatchTime;
	m_AIData._Priority = pData->_AIData._Priority;
	m_AIData._SwapWeaponDistance = pData->_AIData._SwapWeaponDistance;

	m_AIData._SwapRatioHandgun = pData->_AIData._SwapRatioHandgun;
	m_AIData._SwapRatioKnife= pData->_AIData._SwapRatioKnife;

	// 수류탄 투척 패턴
	m_GrenadePattern = pData->_GrenadePattern;
	m_SmokeProbability = pData->_SmokeProbability;

	// 기본 조준 위치보다 아래로 조준하여 사격하는 정도
	m_AIData._TargetHeightUnerNeck = pData->_AIData._TargetHeightUnerNeck;

	m_EvasionRate[AIET_SIDECRAWL] = pData->_EvasionRate[AIET_SIDECRAWL];
	m_EvasionRate[AIET_MOVE] = pData->_EvasionRate[AIET_MOVE];
	m_EvasionRate[AIET_SIDECRAWLCROUCH] = pData->_EvasionRate[AIET_SIDECRAWLCROUCH];
	m_EvasionRate[AIET_MOVECROUCH]= pData->_EvasionRate[AIET_MOVECROUCH];
	m_EvasionRate[AIET_SIDECRAWLFOOTSTEP] = pData->_EvasionRate[AIET_SIDECRAWLFOOTSTEP];
	m_EvasionRate[AIET_MOVEFOOTSTEP] = pData->_EvasionRate[AIET_MOVEFOOTSTEP];
	m_EvasionRate[AIET_SIDECRAWLCROUCHFOOTSTEP] = pData->_EvasionRate[AIET_SIDECRAWLCROUCHFOOTSTEP];
	m_EvasionRate[AIET_MOVECROUCHFOOTSTEP] = pData->_EvasionRate[AIET_MOVECROUCHFOOTSTEP];
	m_EvasionRate[AIET_CROUCH] = pData->_EvasionRate[AIET_CROUCH];
	m_EvasionRate[AIET_STAND] = pData->_EvasionRate[AIET_STAND];
	
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		m_bAttack_Death_Player = pData->_AIFeature._Attack_Death_Player == 1 ? true:false;

	m_bAttacking_Death_Player = false;

}

void CGameCharaInputControlAI::OnResetAI()
{
	m_nickname.clear();	

	m_rank = 0;
	m_primaryWeaponName = 0;
	m_secondaryWeaponName = 0;
	m_meleeWeaponName = 0;
	m_redCharaName = 0;	
	m_blueCharaName = 0;
	m_itemWeaponName = 0;
	m_headCharaName = 0;
	m_beretCharaName = 0;
	m_hp = 0;
	m_respawnTime = 0;


	m_nActionState	= 0;
	m_nMoveState	= 0;

	m_idxCurTargetPoint	= -1;
	m_idxOldTargetPoint	= -1;

	m_CurrentTargetIdx = -1;

	m_WayPointCount		= 0;
	m_CurWayPointIndex	= 0;
	m_rElapsedKeepWatchTime = 0.0f;
	m_rElapsedTraceTime = 0.0f;
	m_rElapsedJumpTime	= 0.0f;
	m_rAIUpdateElapsedTime = 10.0f;
	m_rElapsedAutoFireTime	= 0.0f;
	m_rAfterThrowTimer		= 0.0f;

	m_rTargetTheta		= 0.0f;
	m_rOldTheta			= 0.0f;
	m_rThetaTimer		= 0.0f;
	m_InterpolateTime	= 0.2f;

	m_rEvasionStateTime	= 0.0;

	m_nAIState	=  m_nOldAIState = AI_STATE_NONE;
	if( m_pGamePath != nullptr)
	{
		m_nAIState	=  m_nOldAIState = AI_STATE_GOPATH;
	}

	m_bLeft				= false;
	m_timeGrenadeThrow	= 0.0f;

	if( m_GrenadePattern == AI_GRENADE_RANDOM)
	{
		m_GrenadePattern = (AI_GRENADE_PATTERN)(i3Math::Rand() % 3);
	}

	{
		// 연막탄을 던질 확률로 던질지를 계산.
		INT32 p = i3Math::Rand() % 100;

		m_bThrowSmoke = ( p <= m_SmokeProbability);
	}

	m_bForceEvadeSelfThrow = false;

	m_bMoveFlag[0] = m_bMoveFlag[1] = false;

	m_bAttacking_Death_Player = false;

}

void CGameCharaInputControlAI::OnFirstUpdate( REAL32 rDeltatime)
{
	if( m_pOwner == nullptr)		return;

	if( m_pGamePath == nullptr)
	{
		if( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE)
		{
			CStageBattle * pStage = g_pFramework->GetStageBattle();
			I3ASSERT( pStage != nullptr);

			i3GamePath * pGamePath = (i3GamePath*)i3Framework::FindGameNodeByType( pStage, i3GamePath::static_meta(), true);

			if (pGamePath == nullptr)
			{
				HackContext::i()->HackingDetected( CC_AI_HACK ,C_AI_HACK );
			}
			else
			I3_REF_CHANGE( m_pGamePath, pGamePath);
		}

		m_nAIState			= AI_STATE_GOPATH;
	}
}

AI_STATE CGameCharaInputControlAI::ProcessAI( REAL32 rDeltatime)
{
	if( m_nAIState == AI_STATE_NONE)	return AI_STATE_NONE;

	AI_STATE oldState	= m_nAIState;

	//주변에 적이 있는지 파악
	INT32 idx = FindTarget();
	if( idx != -1)
	{
		//있다면 교전 시작
		if( FightWidth( idx) )
		{
			AI_STATE nextState	= AI_STATE_FIGHT;
			
			//교전 시작시 교전패턴을 정하기위해
			if( oldState != AI_STATE_FIGHT)
			{
				m_rElapsedTraceTime = 5.0f;
			}
			return nextState;
		}
	}

	//아니면 이동상태로 
	AI_STATE nextState = AI_STATE_MOVE;

	//이전에 교전중이었다면
	if( oldState == AI_STATE_FIGHT)	
	{
		nextState = AfterFight();
	}
	else
	{
		m_nActionState &= ~AI_ACTION_STOPATTACK;
	}

	return nextState;
}

AI_STATE CGameCharaInputControlAI::AfterFight(void)
{
	bool bFixed = false;
	AI_STATE		nextState = AI_STATE_MOVE;

	//교전 중이었던 케릭터가 교전 이후에도 살았다면. 
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);

	if( pChara != nullptr )
	{
		if( !pChara->isCharaStateMask( CHARA_STATE_DEATH) && m_idxCurTargetPoint >= 0)
		{
			// 수류탄을 던질 수 있으면 던진다.
			if( ProcessGrenadeForLazyPattern( pChara->GetPos()))
			{
				nextState = AI_STATE_THROW;
			}
			else
			{
				INT32 idx = GetClosestPoint( pChara->GetPos());
				if( idx >= 0 && m_idxCurTargetPoint != idx)
				{
					FindTargetPosition( m_idxCurTargetPoint, idx, MAX_PATH_DEPTH);
				}
			}

			bFixed = true;
		}
	}

	if( bFixed == false)
	{
		FindStartPosition();
	}

	StopAttack();

	m_nActionState |= AI_ACTION_STOPATTACK;

	return nextState;
}

void CGameCharaInputControlAI::OnGoPath(REAL32 rDeltaSeconds, bool bFirst)
{
	m_idxCurTargetPoint = FindStartPosition();

	if( m_idxCurTargetPoint != -1)
	{
		m_nAIState = AI_STATE_MOVE;
	}
}

bool CGameCharaInputControlAI::IsThisNegativeByWatchTime()
{
	REAL32	MaxLazy	= 10.0f;
	INT32	nRand = i3Math::Rand()%100;
	REAL32	rRand, Lazy, Rate;

	Lazy		= MIN( m_AIData._KeepWatchTime, MaxLazy);

	Rate = Lazy / MaxLazy;
	Rate *= Rate;
	
    rRand = nRand * 0.01f;
	if( rRand < Rate)
	{
		return true;
	}
	
	return false;
}

//움직이지 않고 주시한다.
void CGameCharaInputControlAI::OnKeepWatch(REAL32 rDeltaSeconds, bool bFirst)
{
	m_rElapsedTraceTime += rDeltaSeconds;
	m_rElapsedKeepWatchTime += rDeltaSeconds;

	if( m_rAIUpdateElapsedTime > 0.5f)
	{
		INT32 idx = FindTarget();
		if( idx != -1)
		{
			//있다면 교전 시작
			if( FightWidth( idx) )
			{
				m_nAIState = AI_STATE_FIGHT;

				m_rElapsedTraceTime = 5.0f;

				return ;
			}
		}

		m_rAIUpdateElapsedTime = 0.0f;
	}

	if( m_rElapsedKeepWatchTime > 1.5f)
	{	
		I3_NEXTPOINTINFO Next[10];
		INT32 cnt = m_pGamePath->GetLinkedPoint( m_idxOldTargetPoint, Next, 10);
		if( cnt > 0)
		{
			INT32 nRand = i3Math::Rand() %cnt;
			if( m_idxOldTargetPoint != Next[nRand].m_idxNext)
			{
				i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( Next[nRand].m_idxNext);
				VEC3D target(pPointInfo->m_Pos.x, pPointInfo->m_Pos.y, pPointInfo->m_Pos.z);
				SeeTargetPosition( &target, 0.2f, true);
			}
		}

		m_rElapsedKeepWatchTime = 0.0f;
	}

	m_nActionState |= AI_ACTION_CROUCH;

	if( m_rElapsedTraceTime > m_AIData._KeepWatchTime)
	{
		m_nAIState = AI_STATE_MOVE;

		m_rElapsedTraceTime = 0.0f;

		m_nActionState &= ~AI_ACTION_CROUCH;
	}
}

//이동한다.
void CGameCharaInputControlAI::OnMove(REAL32 rDeltaSeconds, bool bFirst)
{
	if( m_idxCurTargetPoint == -1)
	{
		FindStartPosition();

		return;
	}

	i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);
	//I3TRACE("MOVE TO:%d\n", m_idxCurTargetPoint);

	VEC3D pos(pPointInfo->m_Pos.x, pPointInfo->m_Pos.y, pPointInfo->m_Pos.z);
	AI_MOVE_RETURN_VALUE ret = MoveTo( &pos, rDeltaSeconds);

	if( ret == AI_MOVE_RET_ARRIVED)
	{
		m_rElapsedTraceTime = 0.0f;

		if( m_CurWayPointIndex >= m_WayPointCount)
		{
			FindNextTargetPosition();
		}
		else
		{
			m_idxOldTargetPoint = m_idxCurTargetPoint;

			m_idxCurTargetPoint = m_WayPoint[m_CurWayPointIndex];

			m_CurWayPointIndex++;

			m_rElapsedTraceTime = 0.0f;
		}

		AI_STATE next = OnArrived_FindNext();
		if( next == AI_STATE_MOVE)
		{
			pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);

			SeeTargetPosition( &pPointInfo->m_Pos);

			getBoneContext()->setRho( 0.0f);

			SelectWeapon();
		}
		else if( next == AI_STATE_KEEPWATCH)
		{
			m_rElapsedTraceTime = 0.0f;
			m_rElapsedKeepWatchTime = 0.0f;
			
			m_nAIState = AI_STATE_KEEPWATCH;

			SelectWeapon();
		}
	}
	else if( ret == AI_MOVE_RET_BLOCKED)
	{
		if( DestroyIIObject( rDeltaSeconds) == false)
		{
			FindStartPosition();

			pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);

			SeeTargetPosition( &pPointInfo->m_Pos);

			m_rElapsedTraceTime = 0.0f;
		}
	}
	else
	{
		//이동중이던 라인에서 많이 벗어난 경우엔 새로 이동포지션 선택
		if( m_idxOldTargetPoint != -1 && m_idxCurTargetPoint != -1 &&
			!m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) && (m_rElapsedTraceTime > 1.0f))
		{
			VEC3D vDir, vDest;

			pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);
			i3::pack::PATH_POINT_INFO * pOldPointInfo = m_pGamePath->getPoint( m_idxOldTargetPoint);

			i3Vector::Sub( &vDir, &pPointInfo->m_Pos, &pOldPointInfo->m_Pos);
			i3Vector::Normalize( &vDir, &vDir);

			i3Vector::Sub( &vDest, m_pOwner->GetPos(), &pOldPointInfo->m_Pos);
			REAL32 t = i3Vector::Normalize( &vDest, &vDest);

			REAL32 cost = 1 - i3Math::abs( i3Vector::Dot( &vDest, &vDir));
			 
			REAL32 dist = t * cost;
			if( dist > 1.0f)
			{
				FindStartPosition();
			}
		}
	}
}

AI_STATE CGameCharaInputControlAI::OnArrived_FindNext()
{
	i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( m_idxOldTargetPoint);
	if( pPointInfo->m_Priority >= m_AIData._Priority)
	{
		if( IsThisNegativeByWatchTime())	//현 위치를 떠나는데 부정적이라면
		{
			for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)	// 현위치에 있는 다른아군이 있는지 확인해보고
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);
				if( pChara == nullptr )	continue;
				if( m_pOwner == pChara)
					continue;

				if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
					continue;

				// 팀 체크
				if( m_pOwner->getCharaTeam() != pChara->getCharaTeam())	
					continue;

				//
				VEC3D vDiff;
				i3Vector::Sub( &vDiff, m_pOwner->GetPos(), pChara->GetPos());
				if( i3Vector::Length( &vDiff) < 1.0f)
				{
					//현 위치에 다른 아군이 있다면 그냥 이동 시작
					return AI_STATE_MOVE;
				}
			}

			return AI_STATE_KEEPWATCH;
		}
		else
		{
			return AI_STATE_MOVE;
		}
	}
	
	return AI_STATE_MOVE;
}

void CGameCharaInputControlAI::ActivateExtension( bool bActivate)
{
	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr) return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	if( getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) || getActionContext()->isUpper(CHARA_UPPER_LOADBULLET) ||
		getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || getActionContext()->isUpper(CHARA_UPPER_SWAPWEAPON) || 
		getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
		return;

	WEAPON::EXT_TYPE type = pCurWeapon->GetCurrentExtensionType();
	if( type == WEAPON::EXT_DUAL_MAG || type == WEAPON::EXT_SILENCER)
	{
		return;
	}

	if(type == WEAPON::EXT_DOT_SIGHT || type == WEAPON::EXT_SCOPE)
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) == bActivate)
			return;
	}
	else
	{
		if( pCurWeapon->isExtensionActivate() == bActivate)	
			return;
	}

	switch( pCurWeapon->GetCurrentExtensionType())
	{
	case WEAPON::EXT_SCOPE :
	case WEAPON::EXT_ZOOM :					// 견착
	case WEAPON::EXT_STEADY_AIM :

	case WEAPON::EXT_DOT_SIGHT :
		m_nActionState |= AI_ACTION_SUBFUNC;
		break;
	case WEAPON::EXT_GLAUNCHER:
		{
			I3ASSERT( i3::kind_of<WeaponGrenadeLauncher*>(pCurWeapon));

			WeaponGrenadeLauncher * pLauncher = (WeaponGrenadeLauncher*)pCurWeapon;
			if( pLauncher->getLoadedShellCount() > 0)
			{
				m_nActionState |= AI_ACTION_SUBFUNC;
			}
		}
		break;
	case WEAPON::EXT_SHOTGUN_LAUNCHER:
		{
			I3ASSERT( i3::kind_of<WeaponShotGunLauncher*>(pCurWeapon));

			WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*)pCurWeapon;
			if( pLauncher->getLoadedShellCount() > 0)
			{
				m_nActionState |= AI_ACTION_SUBFUNC;
			}
		}
		break;
	case WEAPON::EXT_BAYONET:
		{
			m_nActionState |=AI_ACTION_EXT_ACTIVATE;// AI_ACTION_SUBFUNC;
		}


		break;
	case WEAPON::EXT_DUAL_MAG :		// AK47
	case WEAPON::EXT_SILENCER :		// 사용과 동시에 Activate
			break;
	default:
		m_nActionState |= AI_ACTION_SUBFUNC;
		break;
	}
}

void CGameCharaInputControlAI::OnHit( INT32 idx)
{
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);
	if( pChara != nullptr)
	{
		SeeTargetPosition( pChara->GetPos());
	}
}

//교전한다.
void CGameCharaInputControlAI::OnFight( REAL32 rDeltaSeconds, bool bFirst)
{
	WeaponBase * pCurWeapon = nullptr;

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);
	if( pChara == nullptr)
		return;

	if(m_pOwner->IsDino())
	{
		pCurWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	
	if( pCurWeapon == nullptr) return;

	CWeaponInfo * pWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	REAL32	rDistance;
	VEC3D vStart;

	REAL32 rate = 1.0f;

	if(m_pOwner->IsDino())
	{
		rate = 1.f;
	}
	else
	{
		if( getWeaponContext()->getCurrentSlotNum() == 1)
		{
			rate = 0.3f;
		}
		else if( getWeaponContext()->getCurrentSlotNum() == 2)
		{
			rate = 0.5f;
		}
	}
	

	REAL32	Pre = m_AIData._PreAttack * rate;
	REAL32	For = m_AIData._Attack * rate;
	REAL32	After = m_AIData._AfterAttack * rate;
	REAL32	Total = m_AIData._TotalTime * rate;
	bool	bForce = true;

	if( m_pOwner->IsDino() || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
	{
		bForce = false;
	}
	else
	{
		// 근접무기의 경우, 조준점을 아래로 내리지 않습니다.
		if( !(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS)) 
			&& getWeaponContext()->getCurrentSlotNum() == 2 )	
			bForce = false;
	}
	

	rDistance = GetAttackDirection( pChara, &vStart, &m_vTargetDir, bForce);
	if( rDistance < 0.0f)
		return;

	if(( m_nActionState & 0x000FF000) == 0)
	{	
		//점사 처리 구간
		/*GlobalFunc::PB_TRACE("Pre %f", Pre);
		GlobalFunc::PB_TRACE("m_rElapsedAutoFireTime %f", m_rElapsedAutoFireTime);
		GlobalFunc::PB_TRACE("For %f", For);
		GlobalFunc::PB_TRACE("After %f", After);
		*/
		if( m_rElapsedAutoFireTime >= 0.0f && m_rElapsedAutoFireTime < Pre)
		{
			ActivateExtension( true);

			SeeTarget( &m_vTargetDir, true);
		}
		else if( (m_rElapsedAutoFireTime >= Pre) && (m_rElapsedAutoFireTime < For))
		{
			//GlobalFunc::PB_TRACE("Attack");
			Attack( &m_vTargetDir, rDeltaSeconds);
		}
		else if( (m_rElapsedAutoFireTime >= For) && (m_rElapsedAutoFireTime < After))
		{
			ActivateExtension( false);

			SeeTarget( &m_vTargetDir, true);
		}
		else
		{
			SeeTarget( &m_vTargetDir, true);
		}

		m_rElapsedAutoFireTime += rDeltaSeconds;

		if( m_rElapsedAutoFireTime > Total)
		{
			m_rElapsedAutoFireTime = 0.0f;
		}
	}

	m_rElapsedTraceTime += rDeltaSeconds;
	if( m_rElapsedTraceTime > 4.0f)
	{
		//회피 동작을 결정한다.
		SelectFightPose();

		m_rElapsedTraceTime = 0.0f;
	}

	m_nMoveState = 0;
	if( pWeaponInfo->GetRange() < rDistance)
	{
		MoveToTarget( pChara, pWeaponInfo->GetRange(), rDeltaSeconds);
	}
	else
	{
		if( m_rAfterThrowTimer > 0.0f)
		{	
			MoveOnFight( rDistance, rDeltaSeconds);
		}
		else
		{
			if( m_nActionState & AI_ACTION_MOVE)
			{
				MoveOnFight( rDistance, rDeltaSeconds);
			}
			else if( m_nActionState & AI_ACTION_EVASION)
			{
				ProcessEvasion( rDeltaSeconds);
			}
		}
	}
}

void CGameCharaInputControlAI::ProcessEvasion( REAL32 rDeltaSeconds)
{
	m_rEvasionStateTime += rDeltaSeconds;

	if( m_rEvasionStateTime > 1.0f)
	{
		m_bLeft = !m_bLeft;

		m_rEvasionStateTime = 0.0f;
	}

    if( m_bLeft)
	{
		m_nMoveState &= ~AI_MOVE_RIGHT;
		m_nMoveState |= AI_MOVE_LEFT;
	}
	else
	{
		m_nMoveState &= ~AI_MOVE_LEFT;
		m_nMoveState |= AI_MOVE_RIGHT;
	}
}

void	CGameCharaInputControlAI::SelectFightPose()
{
	WeaponBase * pWeapon = nullptr;

	if(m_pOwner->IsDino())
	{
		pWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}

	m_nActionState &= ~0x00000000000000FF;

	
	if( pWeapon == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "pWeapon != nullptr");
		return ;
	}

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeapon == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "pWeaponInfo != nullptr");
		return ;
	}

	if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNIFE || pWeaponInfo->GetTypeClass() == WEAPON_CLASS_DUALKNIFE)
	{
		if(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) )
		{
			switch( m_AIData._EvasionType)
			{
			case 5:	m_nActionState	|= AI_ACTION_EVASION;		break;
			case 4:	m_nActionState	|= AI_ACTION_MOVE;			break;
			case 3:	m_nActionState	|= AI_ACTION_MOVE;			break;
			case 2: m_nActionState	|= AI_ACTION_JUMP;			break;
			case 1: m_nActionState	|= AI_ACTION_CROUCH;		break;
			case 0: m_nActionState	|= AI_ACTION_FOOTSTEP;		break;
			}
		}
		else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) )
		{
			INT32 nRate = i3Math::Rand()%10 * i3Math::Rand()%10;	

			INT32 move = g_pTempConfig->m_Evade[EVADE_TYPE_MOVE];

			if( move < nRate) m_nActionState |= AI_ACTION_MOVE;
			else m_nActionState |= AI_ACTION_JUMP;
		}
		else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		{
			INT32 nRate = i3Math::Rand()%10 * i3Math::Rand()%10;	

			REAL32 move = m_EvasionRate[AIET_MOVE];

			if( m_bAttacking_Death_Player )
			{
				m_nActionState |= AI_ACTION_CROUCH;
			}
			else
			{
				if( move < nRate) m_nActionState |= AI_ACTION_MOVE;
				else m_nActionState |= AI_ACTION_JUMP;
			}
		}
	}
	else
	{
		INT32 idx = 9;

		REAL32 Rand = (REAL32)(i3Math::Rand()%100);
		REAL32 Rate = m_EvasionRate[0];
		for( INT32 i = 0; i < AIET_COUNT - 1; ++i)
		{
			if( Rate >= Rand)
			{
				idx = i;
				break;
			}

			Rate += m_EvasionRate[i+1];
		}

		switch( idx)
		{
			case 0:	m_nActionState	|= AI_ACTION_EVASION;											break;
			case 1: m_nActionState	|= AI_ACTION_MOVE;												break;
			case 2: m_nActionState	|= AI_ACTION_EVASION	| AI_ACTION_CROUCH;						break;						
			case 3: m_nActionState  |= AI_ACTION_MOVE		| AI_ACTION_CROUCH;						break;
			case 4: m_nActionState	|= AI_ACTION_EVASION	| AI_ACTION_FOOTSTEP;					break;
			case 5: m_nActionState	|= AI_ACTION_MOVE		| AI_ACTION_FOOTSTEP;					break;
			case 6: m_nActionState	|= AI_ACTION_EVASION	| AI_ACTION_FOOTSTEP | AI_ACTION_CROUCH;break;
			case 7: m_nActionState	|= AI_ACTION_MOVE		| AI_ACTION_FOOTSTEP | AI_ACTION_CROUCH;break;
			case 8: m_nActionState	|= AI_ACTION_CROUCH;											break;
			case 9: m_nActionState	|= AI_ACTION_JUMP;												break;
				/*
			default:
				break;
				*/
		}
	}
}

void	CGameCharaInputControlAI::MoveOnFight( REAL32 rDistance, REAL32 rDeltaSeconds)
{
	WeaponBase * pWeapon = nullptr;

	if(m_pOwner->IsDino())
	{
		pWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	
	if( pWeapon == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "pWeapon != nullptr");
		return ;
	}

	if( m_idxCurTargetPoint != -1)
	{
		i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( m_idxCurTargetPoint);

		VEC3D pos(pPointInfo->m_Pos.x, pPointInfo->m_Pos.y, pPointInfo->m_Pos.z);
		if( MoveTo( &pos, rDeltaSeconds) != AI_MOVE_RET_OK)
		{
			FindNextTargetPosition();
		}
	}
}

//회피한다.
void CGameCharaInputControlAI::OnEscape( REAL32 rDeltaSeconds, bool bFirst)
{

}


void CGameCharaInputControlAI::OnUpdate( REAL32 rDeltatime)
{
	if( BaTMaN::use_batman )
	{
		if( BaTMaN::disable_ai_attack ) return;
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DISABLE_CONTROL) == true)	return;

	bool bFirst = false;

	CGameCharaInputControl::OnUpdate( rDeltatime);

	if( m_pGamePath == nullptr)
		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) == true)	return;

	m_rAIUpdateElapsedTime += rDeltatime;

	m_nActionState &= ~AI_ACTION_FIRE;

	EvadeSelfThrow( rDeltatime);

	//수류탄 투척후 자신의 수류탄에 죽어버릴 위험이 있는 상황이면 무조건 피하도록
	if( m_bForceEvadeSelfThrow == false)
	{
		if( (m_nAIState & AI_STATE_MASK_HOLDPOSITION) == 0)
		{
			if( m_rAIUpdateElapsedTime > 0.5f)
			{
				m_nAIState = ProcessAI( rDeltatime);
				// 이전과 AIState가 달라졌다면...
				m_rAIUpdateElapsedTime = 0.f;
			}
		}
		
		{
			bool bGrenade = true;

			if( m_bThrowSmoke)
			{
				if( isWeaponExist( WEAPON_SLOT_THROWING2))
				{
					// 연막탄을 던지는 패턴이면서 아직 연막을 던지지 않았다면...
					ProcessGrenadeForBusyPattern( rDeltatime, WEAPON_SLOT_THROWING2);
				}

				bGrenade = false;
			}

			if( bGrenade && (m_GrenadePattern == AI_GRENADE_BUSY))
			{
				// 가능한 한, 빨리 (리스폰 후) 수류탄을 던지는 패턴의 경우...

				if( m_nAIState & AI_STATE_MASK_THROW_CONSIDER)
				{
					// 수류탄을 던질 수 있는 상황.
					ProcessGrenadeForBusyPattern( rDeltatime, WEAPON_SLOT_THROWING1);
				}
			}
		}
	}
	else
	{
		if( (m_nAIState & AI_STATE_MASK_HOLDPOSITION) != 0)
		{
			m_nAIState = AI_STATE_MOVE;
		}
	}

	m_rAfterThrowTimer -= rDeltatime;

	if( m_nAIState != m_nOldAIState)
	{
		bFirst = true;

		m_rAIUpdateElapsedTime = 0.0f;

		m_nOldAIState = m_nAIState;
	}

	switch( m_nAIState)
	{
	case AI_STATE_GOPATH:
		{
			AIDUMP( "[%p] AI : GoPath\n", this);
			OnGoPath( rDeltatime, bFirst);
		}break;
	case AI_STATE_KEEPWATCH:
		{
			AIDUMP( "[%p] AI : KeepWatch\n", this);
			OnKeepWatch( rDeltatime, bFirst);
		}break;
	case AI_STATE_MOVE: 
		{
			AIDUMP( "[%p] AI : Move\n", this);
			OnMove( rDeltatime, bFirst);
		}break;
	case AI_STATE_FIGHT: 
		{
			AIDUMP( "[%p] AI : Fight\n", this);
			OnFight(rDeltatime, bFirst);

		}break;
	case AI_STATE_ESCAPE: 
		{
			AIDUMP( "[%p] AI : Escape\n", this);
			OnEscape(rDeltatime, bFirst);
		}break;

	case AI_STATE_THROW :
		AIDUMP( "[%p] AI : Throw\n", this);
		OnThrow( rDeltatime, bFirst);
		break;
	case AI_STATE_REMOVE_OBJ:
		AIDUMP( "[%p] AI : Remove Obj\n", this);
		OnRemoveObject( rDeltatime);
		break;
	default:
		{
			AIDUMP( "[%p] AI : Unknown\n", this);
		}break;
	}

	// Add Code here
	ProcessInput( rDeltatime);

	OnInterpolateTheta( rDeltatime);
}

void CGameCharaInputControlAI::OnLastUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnLastUpdate( rDeltatime);
}

void CGameCharaInputControlAI::EvadeSelfThrow( REAL32 rDeltaTime)
{
	//수류탄을 던진후 얼마간은 수류탄을 피해 움직이는것에 우선순위를 준다.
	if( m_rAfterThrowTimer > 0.0f)
	{
		if( (m_rAfterThrowTimer - rDeltaTime) <= 0.0f)
		{
			INT32 valididx = -1;
			INT32 cnt = IsThrowWasSafeZone(&valididx);
			if( valididx != -1)
			{
				const GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon( m_pOwner->getCharaNetIndex(), valididx);
				I3ASSERT( pInfo != nullptr );

				if( pInfo->getWeaponBase() != nullptr)
				{
					VEC3D vInvDir;
					WeaponBase * pWeapon = const_cast<WeaponBase*>(pInfo->getWeaponBase());
					i3Vector::Sub( &vInvDir, m_pOwner->GetPos(), pWeapon->GetPos());
					i3Vector::Normalize( &vInvDir, &vInvDir);

					INT32 cur = GetClosestDirectionPoint( m_pOwner->GetPos(), &vInvDir);
					if( cur != -1)
					{
						m_idxOldTargetPoint = m_idxCurTargetPoint;
						m_idxCurTargetPoint	= cur;

						m_WayPoint[0] = cur;
						m_WayPointCount = 1;

						m_rElapsedTraceTime = 0.0f;

						INT32 idx = GetClosestDirectionPoint( cur, &vInvDir);
						if( idx != -1)
						{
							m_WayPoint[1] = idx;

							m_CurWayPointIndex = 1;
							m_WayPointCount = 2;
						}

						m_bForceEvadeSelfThrow = true;
					}
				}
			}
			else
			{
				m_bForceEvadeSelfThrow = false;
			}

			if( cnt > 0)
			{
				m_rAfterThrowTimer = 1.0f + rDeltaTime;
			}
			else
			{
				m_rAfterThrowTimer		= -0.1f;
				m_bForceEvadeSelfThrow	= false;
			}
		}
	}
}

void CGameCharaInputControlAI::SetMoveDir( VEC3D * pVecOut)
{
	UINT64	moveState = getMoveState();

	if( moveState & AI_MOVE_FORWARD )			setZ( pVecOut, 1.f);
	else if( moveState & AI_MOVE_BACKWARD)		setZ( pVecOut, -1.f);
	else											setZ( pVecOut, 0.f);

	if( moveState & AI_MOVE_LEFT )				setX( pVecOut, 1.f);
	else if( moveState & AI_MOVE_RIGHT )		setX( pVecOut, -1.f);
	else											setX( pVecOut, 0.f);
}

struct DEFCOMP_DIR
{
	REAL32	rDot = 0.0f;
	REAL32  length = 0.0f;
	INT32	idx = 0;
	VEC3D	vDir;
};

template<> struct i3::less<DEFCOMP_DIR*> 
{
	bool operator()( const DEFCOMP_DIR * p1, const DEFCOMP_DIR *p2 ) const {  return p2->rDot < p1->rDot ; }		// 내림차순..
};


/*
static INT32 _compFunc( _defCompDir * p1, _defCompDir *p2)
{
	if( p1->rDot < p2->rDot)	return 1;
	else if( p2->rDot < p1->rDot)	return -1;
	else return 0;
}
*/

//지정해준 위치와 가장 가까운곳에 위치한 이동가능한 pathpoint중 지정방향과 가장 근접한 방향의 포인트를 구합니다.
INT32 CGameCharaInputControlAI::GetClosestDirectionPoint( VEC3D * pvTargetPos, VEC3D * pvDirection)
{
	if( m_pGamePath == nullptr)
	{
		return -1;
	}

	INT32 idxArray[20];
	VEC3D vDir, vStart;
	i3CollideeLine line;
	i3::pack::PATH_POINT_INFO * pPointInfo;

	i3::vector_multiset<DEFCOMP_DIR*> list;			// 쓰기야하겠지만...이런건 그냥 소트함수를 쓰는게 훨씬 낫다..(2012.05.30.수빈.잘못된 자료구조사용)
//	list.SetCompareProc( (COMPAREPROC)_compFunc);

	DEFCOMP_DIR dir[20];

	//가까이 있는것들중 방향과 가장 근접한 순으로 정렬
	INT32 idx = m_pGamePath->FindClosestPoints( pvTargetPos, idxArray, 20);
	if( idx > 0)
	{
		INT32 i;
		for( i = 0; i < idx; ++i)
		{
			pPointInfo = m_pGamePath->getPoint( idxArray[i]);

			i3Vector::Sub( &vDir, &pPointInfo->m_Pos, pvTargetPos);

			dir[i].length = i3Vector::Normalize( &dir[i].vDir, &vDir);
			dir[i].idx = idxArray[i];
			dir[i].rDot = i3Vector::Dot( pvDirection, &dir[i].vDir);

			list.insert( &dir[i]);
		}

		i3Vector::Copy( &vStart, pvTargetPos);
		i3Vector::AddY( &vStart, 0.25f);
		for( i = 0; i < idx; ++i)
		{
			DEFCOMP_DIR * pInfo = list.get_vector()[i];
			
			line.SetStart( &vStart);
			line.SetDir( &pInfo->vDir);

			I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
			if( pHitResult != nullptr)
			{
				if( pHitResult->m_T >= pInfo->length)
				{
					return pInfo->idx;
				}
			}
		}
	}

	return -1;
}

//지정해준 위치와 가장 가까운곳에 위치한 이동가능한 pathpoint를 구합니다.
INT32 CGameCharaInputControlAI::GetClosestPoint( VEC3D * pvTargetPos)
{
	if( m_pGamePath == nullptr)
	{
		return -1;
	}

	INT32 idxArray[20];
	INT32 idx = m_pGamePath->FindClosestPoints( pvTargetPos, idxArray, 20);
	if( idx > 0)
	{
		for( INT32 i = 0; i < idx; ++i)
		{
			if( idxArray[i] != m_idxCurTargetPoint)
			{
				VEC3D vDir, vStart;
				i3CollideeLine line;
				REAL32 rLength;
				i3::pack::PATH_POINT_INFO * pPointInfo;

				pPointInfo = m_pGamePath->getPoint( idxArray[i]);

				i3Vector::Copy( &vStart, pvTargetPos);
				i3Vector::Sub( &vDir, &pPointInfo->m_Pos, pvTargetPos);
				rLength = i3Vector::Normalize( &vDir, &vDir);

				i3Vector::AddY( &vStart, 0.25f);
				line.SetStart( &vStart);
				line.SetDir( &vDir);

				I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
				if( pHitResult != nullptr)
				{
					if( pHitResult->m_T > rLength)
					{
						return idxArray[i];
					}
				}
			}
		}
	}

	return -1;
}

INT32 CGameCharaInputControlAI::FindStartPosition(void)
{
	if( m_pGamePath == nullptr)
	{
		return -1;
	}

	//I3TRACE("FIND NEW START POSITION\n");

	m_idxOldTargetPoint = m_idxCurTargetPoint;

	INT32 next = GetClosestPoint( m_pOwner->GetPos());
	if( next != -1)
	{
		m_idxCurTargetPoint = next;

		m_rElapsedTraceTime = 0.0f;
		m_CurWayPointIndex	= 0;
		m_WayPointCount		= 1;

		m_WayPoint[0]		= m_idxCurTargetPoint;

		return next;
	}

//	I3PRINTLOG(I3LOG_WARN, "Path중 길이가 너무 긴곳이 존재합니다. 수정해주세요: 현재 CLOEST범위 20");

	return -1;
}

void CGameCharaInputControlAI::FindTargetPosition(I3_POINTIDX idxStart, I3_POINTIDX idxEnd, INT32 nDepth)
{
	if( m_pGamePath == nullptr)		return;

	//가급적이면 이전 패스로 되돌아 가지 않도록
	INT32 cnt = m_pGamePath->getPointCount();
	if( cnt > 0)
	{
		m_WayPointCount = m_pGamePath->FindShortestPath( idxStart, idxEnd, m_WayPoint, nDepth);
		if( m_WayPointCount > 1)
		{
			m_idxOldTargetPoint = m_idxCurTargetPoint;

			m_idxCurTargetPoint = m_WayPoint[1];

			m_CurWayPointIndex	= 1;
		}

		return;
	}
}

void CGameCharaInputControlAI::FindNextTargetPosition(void)
{
	if( m_pGamePath == nullptr)		return;
	if( m_idxCurTargetPoint == -1)
	{
		FindStartPosition();

		return;
	}

	m_WayPointCount = SelectNextPath( m_idxOldTargetPoint, m_idxCurTargetPoint);
	if( m_WayPointCount > 1)
	{
		m_idxOldTargetPoint = m_idxCurTargetPoint;

		m_idxCurTargetPoint = m_WayPoint[1];

		m_CurWayPointIndex	= 1;
	}
	else
	{
		m_WayPoint[0] = m_idxOldTargetPoint;

		m_idxOldTargetPoint = m_idxCurTargetPoint;

		m_idxCurTargetPoint = m_WayPoint[0];

		m_CurWayPointIndex = 0;

		m_WayPointCount = 1;
	}
}

INT32 CGameCharaInputControlAI::SelectNextPath( INT32 idxOld, INT32 idxCur)
{
	//가급적이면 이전 패스로 되돌아 가지 않도록
	INT32 cnt = m_pGamePath->getPointCount();
	if( cnt > 0)
	{
		INT32 next = i3Math::Rand()%cnt;
		while( next == idxCur)
		{
			next = i3Math::Rand()%cnt;
		}

		INT32 newPathCount = m_pGamePath->FindShortestPath( idxCur, next, m_WayPoint, MAX_PATH_DEPTH);
		if( newPathCount > 0)
		{
			//I3TRACE("SelectNextPath:%d \n", newPathCount);
			return newPathCount;
		}
	}

	//I3TRACE("SelectNextPath: FAILED \n");
	return -1;
}

void CGameCharaInputControlAI::SeeTargetPosition( VEC3D * pvPos, REAL32 rTime, bool bRho)
{
	VEC3D vDir;
	i3Vector::Sub( &vDir, pvPos, m_pOwner->GetPos());

	SeeTarget( &vDir, rTime, bRho);
}

void CGameCharaInputControlAI::SeeTargetPosition(i3::pack::VEC3D * pPos, REAL32 rTime, bool bRho)
{
	VEC3D vDir;
	i3Vector::Sub(&vDir, pPos, m_pOwner->GetPos());

	SeeTarget(&vDir, rTime, bRho);
}

void CGameCharaInputControlAI::OnInterpolateTheta( REAL32 rDeltaSeconds)
{
	if( m_rThetaTimer < m_InterpolateTime)
	{
		REAL32 cur		= getBoneContext()->getTheta();
		REAL32 ratio	= MINMAX( 0.0f, m_rThetaTimer/m_InterpolateTime, 1.0f);

		REAL32 old		= ( m_rOldTheta < 0.0f) ? m_rOldTheta + I3_2PI : m_rOldTheta;
		REAL32 target	= ( m_rTargetTheta < 0.0f) ? m_rTargetTheta + I3_2PI : m_rTargetTheta;

		REAL32 diff		= target - old;
		if( i3Math::abs( diff) > I3_PI)
		{
			cur	= old - i3Math::vsin( ratio) * i3Math::NormalizeAngle(I3_2PI - diff);
		}
		else
		{
			cur = old + i3Math::vsin( ratio) * diff;
		}

		cur = i3Math::NormalizeAngle( cur);

		if( m_rThetaTimer + rDeltaSeconds > m_InterpolateTime)
		{
			cur = m_rTargetTheta;
		}

		getBoneContext()->setTheta( cur);
	}

	m_rThetaTimer += rDeltaSeconds;
}

void CGameCharaInputControlAI::SeeTarget( VEC3D *pvDir, REAL32 Time, bool bRho)
{
	VEC3D vTarget;

	REAL32 fLen;
	i3Vector::Normalize( &vTarget, pvDir);

	m_rOldTheta		= i3Math::NormalizeAngle(getBoneContext()->getTheta());
	m_rTargetTheta  = i3Math::NormalizeAngle( i3Math::atan2( getX( &vTarget), getZ( &vTarget)));
	m_rThetaTimer	= 0.0f;
	m_InterpolateTime = Time;

	if(( m_nActionState & AI_ACTION_ATTACK) != 0)
	{
		m_rThetaTimer = m_InterpolateTime + 1.0f;

		getBoneContext()->setTheta( m_rTargetTheta);
	}

	if( bRho)
	{
		fLen			= i3Math::Sqrt( (getX(&vTarget) * getX(&vTarget)) + (getZ(&vTarget) * getZ(&vTarget)) );
		REAL32 rho		= atan2f( getY(&vTarget), fLen);

		getBoneContext()->setRho( rho);
	}
}

bool CGameCharaInputControlAI::MakeKeyInput( VEC3D * pvDir, MATRIX * pOwnerMtx)
{
	VEC3D vDir;
	MATRIX mtx, invMtx;
	i3Matrix::Copy( &mtx, pOwnerMtx);
	i3Matrix::SetPos( &mtx, 0.0f, 0.0f, 0.0f);
	i3Matrix::Inverse( &invMtx, nullptr, &mtx);

	i3Vector::TransformCoord( &vDir, pvDir, &invMtx);

	m_nMoveState &= ~0x000000000000000F;

	setY( &vDir, 0.0f);
	i3Vector::Normalize( &vDir, &vDir);

	REAL32	comp = 0.5f;
	if( m_bMoveFlag[0] == true)
	{
		comp = 0.293f;
	}

	if( i3Math::abs( vDir.z) > comp)
	{
		if( vDir.z > 0.0f)
		{
			m_nMoveState |= AI_MOVE_FORWARD;
		}
		else
		{
			m_nMoveState |= AI_MOVE_BACKWARD;
		}

		m_bMoveFlag[0] = true;
	}
	else
	{
		 m_bMoveFlag[0] = false;
	}

	comp = 0.5f;
	if( m_bMoveFlag[1] == true)
	{
		comp = 0.293f;
	}

	if( i3Math::abs( vDir.x) > comp)
	{
		if( vDir.x > 0.0f)
		{
			m_nMoveState |= AI_MOVE_LEFT;
		}
		else
		{
			m_nMoveState |= AI_MOVE_RIGHT;
		}

		m_bMoveFlag[1] = true;
	}
	else
	{
		m_bMoveFlag[1] = false;
	}

	return true;
}

bool CGameCharaInputControlAI::MoveToTarget( i3GameObj* pTarget, REAL32 minLength, REAL32 rDeltaSeconds)
{
	m_nMoveState = 0;

	VEC3D	vDir;
	i3Vector::Sub( &vDir, pTarget->GetPos(), m_pOwner->GetPos());
	if( i3Vector::Length( &vDir) < minLength)
	{
		return false;
	}

	MakeKeyInput( &vDir, m_pOwner->GetMatrix());

	return true;
}

AI_MOVE_RETURN_VALUE CGameCharaInputControlAI::MoveTo( VEC3D * vPos, REAL32 rDeltaTime)
{
	m_nMoveState = 0;
	m_nActionState &= ~AI_ACTION_JUMP;

	AI_MOVE_RETURN_VALUE ret = AI_MOVE_RET_OK;

	VEC3D	vDir;
	i3Vector::Sub( &vDir, vPos, m_pOwner->GetPos());

	m_rElapsedTraceTime += rDeltaTime;

	REAL32 rHeight		= getY( &vDir);
	REAL32 rDistance	= i3Vector::Normalize( &vDir, &vDir);
	if( rDistance <= 0.5f)
	{
		m_rElapsedJumpTime = 0.0f;
		ret = AI_MOVE_RET_ARRIVED;
	}

	if( m_rElapsedTraceTime > MINMAX( 2.0f, rDistance, 8.0f))
	{
//		I3TRACE("TIME ELAPSED: %f, distance : %f\n", m_rElapsedTraceTime, rDistance);
		m_rElapsedJumpTime = 0.0f;
		ret = AI_MOVE_RET_BLOCKED;
	}

	if( ret == AI_MOVE_RET_OK)
	{
		//걸리는 부분이 생기면 점프...;;
		m_rElapsedJumpTime += rDeltaTime;

		if( m_rElapsedJumpTime > 0.5f)
		{
			m_rElapsedJumpTime = 0.0f;

			REAL32 fLen;
			fLen		= i3Math::Sqrt( (getX(&vDir) * getX(&vDir)) + (getZ(&vDir) * getZ(&vDir)) );
			REAL32 rho	= atan2f( getY(&vDir), fLen);

			if( rho > 0.3f || rHeight > 0.3f)
			{
				m_nActionState |= AI_ACTION_JUMP;
			}
		}
	}

	MakeKeyInput( &vDir, m_pOwner->GetMatrix());

	return ret;
}

WEAPON_SLOT_TYPE CGameCharaInputControlAI::_GetAvailableWeapon(void)
{
	WeaponBase * pWeaponPrimary = getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY);
	WeaponBase * pWeaponSecond = getWeaponContext()->getWeapon( WEAPON_SLOT_SECONDARY);

	if( pWeaponPrimary == nullptr)
	{
		//I3PRINTLOG(I3LOG_WARN,  "no weapon AI %s", BattleSlotContext::i()->getNickForSlot( m_pOwner->getCharaNetIndex()));

		if( pWeaponSecond == nullptr)
		{
			return WEAPON_SLOT_MELEE;
		}
		else
		{
			if( pWeaponSecond->hasLoadedBullet() == false && pWeaponSecond->getTotalBulletCount() <= 0)
			{
				return WEAPON_SLOT_MELEE;
			}
			else
			{
				return WEAPON_SLOT_SECONDARY;
			}
		}		
	}
	else if(pWeaponSecond == nullptr)
	{
		return WEAPON_SLOT_MELEE;
	}
	else if( pWeaponPrimary->hasLoadedBullet() == false && pWeaponPrimary->getTotalBulletCount() <= 0)//장전된 총알도 없고..뭐..남은 탄창도 없고...
	{
		if( pWeaponSecond->hasLoadedBullet() == false && pWeaponSecond->getTotalBulletCount() <= 0)
		{
			return WEAPON_SLOT_MELEE;
		}
		else
		{
			return WEAPON_SLOT_SECONDARY;
		}
	}
	else
	{
		return WEAPON_SLOT_PRIMARY;
	}
}

bool CGameCharaInputControlAI::_SwapAvailableWeapon()
{
	if( getWeaponContext() == nullptr)	return false;

	INT32 curSlot = getWeaponContext()->getCurrentSlotNum();

	INT32 nSlot = _GetAvailableWeapon();
	if( nSlot != curSlot)
	{
		switch( nSlot)
		{
		case 0 : m_nActionState |= AI_ACTION_SWAPWEAPON0;
			break;
		case 1 : m_nActionState |= AI_ACTION_SWAPWEAPON1;
			break;
		case 2 : m_nActionState |= AI_ACTION_SWAPWEAPON2;
			break;
		}
	}

	return true;
}

bool CGameCharaInputControlAI::SelectWeapon()
{
	if( m_pOwner->IsDino())
		return false;

	I3ASSERT( getWeaponContext() != nullptr);
	
	REAL32 min = 3.0f;

	INT32 curSlot = getWeaponContext()->getCurrentSlotNum();

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);

	if(( m_nActionState & AI_ACTION_ATTACK) != 0 && (pChara != nullptr))
	{
		VEC3D vDiff;
		i3Vector::Sub(  &vDiff, pChara->GetPos(), m_pOwner->GetPos());
		REAL32 length = i3Vector::Length( &vDiff);

		if( length > min)
		{
			_SwapAvailableWeapon();	
		}
		else
		{
			INT32 nSlot = _GetAvailableWeapon();
			if( nSlot != curSlot)
			{
				m_nActionState |= AI_ACTION_SWAPWEAPON2;
			}
		}
	}
	else
	{
		if( m_nAIState == AI_STATE_KEEPWATCH || m_nAIState == AI_STATE_REMOVE_OBJ)
		{
			_SwapAvailableWeapon();	
		}
		else
		{
			if( m_idxOldTargetPoint != -1)
			{
				i3::pack::PATH_POINT_INFO * pPoint		= m_pGamePath->getPoint( m_idxCurTargetPoint);
				i3::pack::PATH_POINT_INFO * pOldPoint  = m_pGamePath->getPoint( m_idxOldTargetPoint);

				VEC3D vDiff;
				i3Vector::Sub( &vDiff, &pPoint->m_Pos, &pOldPoint->m_Pos);
				if( i3Vector::Length( &vDiff) > m_AIData._SwapWeaponDistance)
				{
					INT32 nRand = i3Math::Rand()%100;
					if( nRand < m_AIData._SwapRatioHandgun)
					{
						m_nActionState |= AI_ACTION_SWAPWEAPON1;
					}
					else if( nRand >= m_AIData._SwapRatioHandgun && nRand < m_AIData._SwapRatioKnife + m_AIData._SwapRatioHandgun)
					{
						m_nActionState |= AI_ACTION_SWAPWEAPON2;
					}
				}
			}
			else
			{
				_SwapAvailableWeapon();	
			}
		}
	}

	return true;
}

bool CGameCharaInputControlAI::FightWidth( INT32 idx)
{
	CGameCharaBase * pTarget = g_pCharaManager->getCharaByNetIdx( idx);
	if( pTarget == nullptr ) return false;

	if( m_CurrentTargetIdx != idx)
	{
		m_rElapsedAutoFireTime	= 0.0f;
		m_CurrentTargetIdx = idx;
	}

	m_nActionState |= AI_ACTION_ATTACK;

	SelectWeapon();
	
	return true;
}

REAL32 CGameCharaInputControlAI::GetAttackDirection( CGameCharaBase * pTargetChara, VEC3D * pvStart, VEC3D * pvDir, bool bForce)
{
	REAL32 rLen;
	CGameCharaBoneContext * pBoneCtx = pTargetChara->getBoneContext();
	if( pBoneCtx == nullptr)
	{
		//I3TRACE("Target Character's BoneContext is Null\n");
		return -1.0f;
	}

	MATRIX * pMatrixNeck = pBoneCtx->getBoneMatrix( BONE_HEAD);

	m_pOwner->getCameraContext()->getVirtualCamPosition( pvStart);
	VEC3D vEnd;
	i3Vector::Copy( &vEnd, i3Matrix::GetPos( pMatrixNeck));
	i3Vector::AddY( &vEnd, - 0.25f);

	if( bForce)
		i3Vector::AddY( &vEnd, - m_AIData._TargetHeightUnerNeck);
	
	i3Vector::Sub( pvDir, &vEnd, pvStart);

	rLen = i3Vector::Normalize( pvDir, pvDir);

	return rLen;
}

INT32 CGameCharaInputControlAI::FindTarget()
{
	WeaponBase* pWeapon = nullptr;

	if( getWeaponContext() == nullptr)	
	{
		if(m_pOwner->IsDino())
		{
			pWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
		}
		else
		{
			return -1;
		}
	}
	else
	{
		pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
		
	if( pWeapon == nullptr)	return -1;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	INT32	nTargetIndex = -1;
	REAL32	rMinLength = pWeaponInfo->GetRange();
	if( rMinLength < 30.0f)	rMinLength = 30.0f;

	UINT32 nColGroup = CGRP_TEAM_BLUE_ROUGH;
	if( (m_pOwner->getCharaInfo()->GetTeam() & CHARACTER_TEAM_RED))
	{
		nColGroup = CGRP_TEAM_RED_ROUGH;
	}

	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr )	continue;
		if( m_pOwner == pChara)
			continue;

		if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
		{
			if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && !m_bAttack_Death_Player)
				continue;
		}

		// 팀 체크
		if( m_pOwner->getCharaTeam() == pChara->getCharaTeam())	
			continue;

		REAL32 rLen;
		VEC3D vStart, vDir;
		rLen = GetAttackDirection( pChara, &vStart, &vDir, false);
		if( rLen < 0.0f)
			continue;
		
		REAL32 rDot = i3Vector::Dot( &vDir, i3Matrix::GetAt( m_pOwner->GetMatrix()));

		//시야내에 적이 있거나 감지 최소거리 내에 있으면
		if( rDot > m_AIData._Angle || (rLen < m_AIData._Range)) 
		{
			i3CollideeLine line;
			line.SetStart( &vStart);
			line.SetDir( &vDir);

			I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ONLYWORLDHIT);
			if( pHitResult != nullptr)
			{
				if( pHitResult->m_T > rLen)
				{
					if( rLen < rMinLength)
					{
						rMinLength	= rLen;
						nTargetIndex	= pChara->getCharaNetIndex();
					}
				}
			}
			else
			{
				if( rLen < rMinLength)
				{
					rMinLength	= rLen;
					nTargetIndex	= pChara->getCharaNetIndex();
				}
			}
		}
	}

	return nTargetIndex;
}

void CGameCharaInputControlAI::StopAttack( void)
{
	m_nActionState &= ~0x0000000000000FFF;
	m_nActionState &= ~AI_ACTION_FIRE;

	m_rElapsedAutoFireTime = 0.0f;

	ActivateExtension( false);

	m_bAttacking_Death_Player = false;
}

void CGameCharaInputControlAI::Attack( VEC3D *pvTarget, REAL32 rDeltatime)
{
	SeeTarget( pvTarget, 0.0f, true);

	m_nActionState |= AI_ACTION_FIRE;

	m_bAttacking_Death_Player = false;
	
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentTargetIdx);
	if( pChara != nullptr )
	{
		if( pChara->isCharaStateMask( CHARA_STATE_DEATH) )
		{
			m_bAttacking_Death_Player = true;
		}
	}
}

void CGameCharaInputControlAI::Attack( INT32 target, REAL32 rDeltatime)
{
	if( target == -1)	return;
	CGameCharaBase * pTarget = g_pCharaManager->getCharaByNetIdx( target);
	if( pTarget == nullptr )return;

	Attack( pTarget, rDeltatime);
}

void CGameCharaInputControlAI::Attack( i3GameObj * pTarget, REAL32 rDeltatime)
{
	if( pTarget == nullptr) return;

	VEC3D vDir;
	i3Vector::Sub( &vDir, pTarget->GetPos(), m_pOwner->GetPos());

	Attack( &vDir, rDeltatime);
}

REAL32 CGameCharaInputControlAI::GetTargetRate(void)
{
	return m_AIData._Deviation;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameCharaInputControlAI::ProcessDropWeapon( REAL32 rDeltatime)
{
	UINT64	actionState = getActionState();

	if( actionState & AI_ACTION_DROPWEAPON)
	{
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) && !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_DROPWEAPON))
		{
			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

			if( pWeapon != nullptr)
			{
				GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, getWeaponContext()->getCurrentSlotNum(), pWeapon->getLoadedBulletCount(),
					pWeapon->getLoadedBulletDualCount(), pWeapon->getTotalBulletCount());
			}

			// 무기를 버리고 0.5초간 발사를 못하도록 세팅합니다.
			m_pOwner->setDropWeaponTimer( 0.5f);
		}
	}
}

bool CGameCharaInputControlAI::ProcessCrouch( REAL32 rDeltatime)
{
	bool	bChangeBody = false;
	UINT64	actionState = getActionState();

	if(m_pOwner->IsDino()) {
		return false;
	}

	if( actionState & AI_ACTION_CROUCH )
	{	// 앉기
		bChangeBody = m_pOwner->Cmd_Crouch();
	}
	else
	{	// 일어서기
		bChangeBody = m_pOwner->Cmd_StandUp();
	}

	return bChangeBody;
}


// -------------------------------------  이동  ----------------------------------------------------
void CGameCharaInputControlAI::ProcessMovement( REAL32 rDeltatime, bool bChangeBody)
{	
	VEC3D vOldDir;
	WeaponBase * pWeapon = nullptr;

	pWeapon = m_pOwner->GetCurrentCharaWeapon();

	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaMoveContext * pMoveCtx = getMoveContext();

	VEC3D * pMoveDir = pMoveCtx->getMoveDir();

	bool bHaveUpperPlay = false;
	CHARA_ACTION_UPPER nActionUpper = pActionCtx->getUpper();
	if( (nActionUpper == CHARA_UPPER_IDLE) || (nActionUpper == CHARA_UPPER_MOVE) )
		bHaveUpperPlay = true;

	
	if( (pWeapon != nullptr) && pWeapon->isDualGun())
	{
		CHARA_ACTION_UPPER nActionUpper_Left = pActionCtx->getUpperLeft();
		if( (nActionUpper_Left == CHARA_UPPER_IDLE) || (nActionUpper_Left == CHARA_UPPER_MOVE) )
			bHaveUpperPlay = true;
	}

	UINT64	moveState = getMoveState();
	if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) &&		//	이동이 가능하면?
		!pActionCtx->isLower( CHARA_LOWER_JUMP))						//	점프중이 아니면?
	{
		if( (moveState & AI_MOVE_LEFT) || (moveState & AI_MOVE_RIGHT)
			|| (moveState & AI_MOVE_FORWARD) || (moveState & AI_MOVE_BACKWARD))
		{
			i3Vector::Copy( &vOldDir, pMoveDir);

			SetMoveDir( pMoveDir);	//	입력키에 따라 이동 방향 지정

			if( getActionState() & AI_ACTION_FOOTSTEP )
				m_pOwner->Cmd_Walk( pMoveDir);			//	걷기
			else
				m_pOwner->Cmd_Run( pMoveDir);	//	뛰기

			/*if( bHaveUpperPlay)						pActionCtx->_Play_Upper_Run();*/
		}	
	}

	///////////////////////////////////////////////////////////		정지
	if( !(moveState & AI_MOVE_LEFT) && !(moveState & AI_MOVE_RIGHT) && !(moveState & AI_MOVE_FORWARD)
		&& !(moveState & AI_MOVE_BACKWARD) || m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) )
	{	
		//	이동 중에 정지
		if( pActionCtx->isLower( CHARA_LOWER_RUN) || pActionCtx->isLower( CHARA_LOWER_WALK))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}			
	}

	// revision 53646 HK417 경직기능 추가
	if( (moveState & AI_MOVE_LEFT) || (moveState & AI_MOVE_RIGHT) || (moveState & AI_MOVE_FORWARD) || (moveState & AI_MOVE_BACKWARD))
	{	
		//	이동 중에 정지
		if( pActionCtx->isLower( CHARA_LOWER_RUN) && (m_pOwner->getRunParalysis() > 0))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}			
		else if( pActionCtx->isLower( CHARA_LOWER_WALK)&& (m_pOwner->getWalkParalysis() > 0))
		{
			i3Vector::Zero( pMoveDir);

			m_pOwner->Cmd_Idle_Lower();
			m_pOwner->Cmd_Idle_Upper();
		}
	}
}

void CGameCharaInputControlAI::ProcessWeapon( REAL32 rDeltatime)
{	
	WeaponBase * pCurWeapon = nullptr;

	if(m_pOwner->IsDino())
	{
		pCurWeapon = ((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->GetDinoWeapon();
	}
	else
	{
		pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	
	if( pCurWeapon == nullptr)	return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	WEAPON::eSTATE	weaponState = pCurWeapon->getWeaponState();

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);


	bool bCancelMission = false;	//	미션 행동 취소 여부
	bool bCancelDisarm = false;		//	폭탄 해체 취소 여부

	UINT64 actionState = getActionState();

	if( actionState & AI_ACTION_FIRE )
	{	
		bool bFire = false;

		if( i3::kind_of<WeaponGrenadeLauncher*>(pCurWeapon))
		{
			if( pCurWeapon->isExtensionActivate())
				bFire = true;
		}
		else if( i3::same_of<WeaponBombTrigger*>(pCurWeapon))
			bFire = true;
		else if( pCurWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MELEE )
			bFire = true;

		if( (!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && (!pCurWeapon->isWeaponStateMask( WSM_FIRING)))
			&& (pCurWeapon->hasLoadedBullet() || bFire) && (m_pOwner->getDropWeaponTimer() < 0.f) )	// 드랍 웨폰 타이머가 있다면 발사를 못하도록 합니다.
		{
			if( pCurWeapon->isFireEnabled() )
			{
				//	Mission 무기 예외처리
				if( pCurWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION)
				{//C4처리하는 코드 입니다. 구조상 이상합니다. 
					//	행동
					if( !m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) )
					{
						//설치를 하는 동작입니다. 
						if( m_pOwner->Cmd_Attack())
						{
							bCancelMission = false;
						}
					}
				}
				else
				{
					//	폭탄 해체 중일땐 발사 불가
					if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) == false)
					{
						//	무기 발사!!		(투척 무기만 발사준비)
						switch(pCurWeaponInfo->GetTypeClass())
						{
						case WEAPON_CLASS_THROWING_GRENADE:
						case WEAPON_CLASS_THROWING_CONTAIN:
						case WEAPON_CLASS_THROWING_ATTACK:
						case WEAPON_CLASS_THROWING_HEAL:
							{
								if( !pCurWeapon->isWeaponStateMask( WSM_FIRING))
								{
									bCancelDisarm = true;
									m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
								}
							}
							break;
						default:
							{
								SeeTarget( &m_vTargetDir, true);
								bCancelDisarm = true;
								m_pOwner->Cmd_Attack();
							}
							break;
						}
					}
				}
			}
			else if( pCurWeapon->getWeaponInfo()->GetTypeTrigger() == WEAPON::TRIGGER_RELOADING_SHOT )
			{	
				// 장전중 발사 가능하면 탄약 장전 동작을 취한다(샷건)
				bCancelMission = m_pOwner->Cmd_Reload();
			}
		}
		else
		{
			// 자동으로 장전을 하는 경우
			switch( weaponState)
			{
				case WEAPON::STATE_NEEDTO_LOADMAG :
					{
						ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

						if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
							getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
							nLoadType = ACTION_LOAD_READY_MAG;

						bCancelMission = m_pOwner->Cmd_Reload(nLoadType);
					}
					break;
				case WEAPON::STATE_NEEDTO_LOADBULLET :
					{
						if( pCurWeapon->IsFireBulletWeapon())
						{
							bCancelMission = m_pOwner->Cmd_Reload( ACTION_LOAD_FIRE_BULLET);	
						}
						else
						{
							bCancelMission = m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);	
						}
					}
					break;
				case WEAPON::STATE_NEEDTO_REMOVESHELL :		
					{
						bCancelMission = m_pOwner->Cmd_Reload();	
					}
					break;
				case WEAPON::STATE_NEEDTO_FIRE:
					{
						SeeTarget( &m_vTargetDir, true);
						m_pOwner->Cmd_Attack();
						bCancelDisarm = true;
					}break;
			}
		}
	}
	else if( pCurWeapon->isWeaponStateMask( WSM_FIRING))//	발사 클릭후 버튼을 떼면 (Key Stroke 시점)
	{
		bCancelMission = true;  

		//	투척 무기일 경우 버튼을 놓으면 던진다.
		if( pCurWeapon->isWeaponStateMask( WSM_FIRING) )
		{
			switch(pCurWeaponInfo->GetTypeClass())
			{
			case WEAPON_CLASS_THROWING_GRENADE:
			case WEAPON_CLASS_THROWING_CONTAIN:
			case WEAPON_CLASS_THROWING_ATTACK:
			case WEAPON_CLASS_THROWING_HEAL:
				{
					if( getActionContext()->isUpper( CHARA_UPPER_FIRE_READY) &&
						!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && pCurWeapon->getLoadedBulletCount() > 0)
					{
						SeeTarget( &m_vTargetDir, true);
						bCancelDisarm = m_pOwner->Cmd_Attack();
					}
				}
				break;
			}
		}

		pCurWeapon->FireEnd();		//	발사 종료 시점을 무기에 알려준다.
	}

	//총알이 모자라서 빈상태인경우 격발시도를 하게되면 나는 소리
	if( actionState & AI_ACTION_FIRE )
	{
		if( (weaponState == WEAPON::STATE_EMPTY_MAGAZINE) )
		{
			if( pCurWeaponInfo->isGun() )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, m_pOwner->GetPos(), m_pOwner->is1PV());

				SelectWeapon();
			}
		}
		else if( pCurWeapon->isExtensionActivate() &&
			((pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)) &&			
			(((WeaponGrenadeLauncher*)pCurWeapon)->getTotalShellCount() <= 0) && 
			(((WeaponGrenadeLauncher*)pCurWeapon)->getLoadedShellCount() <= 0) )
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, m_pOwner->GetPos(), m_pOwner->is1PV());

			m_pOwner->Cmd_Extension( false);

			// 기획팀 요청으로 커스텀 M4의 유탄/LSS 모드는 탄환 모두 소진시 익스텐션은 deavtivate되고 기본 모드로 돌아간다.
			if( pCurWeapon->isMultipleExtension())
				pCurWeapon->SwapExtensionType();
		}
		else if( pCurWeapon->isExtensionActivate() &&
			((pCurWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)) &&			
			(((WeaponShotGunLauncher*)pCurWeapon)->getTotalShellCount() <= 0) && 
			(((WeaponShotGunLauncher*)pCurWeapon)->getLoadedShellCount() <= 0) )
		{
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, m_pOwner->GetPos(), m_pOwner->is1PV());

			m_pOwner->Cmd_Extension( false);

			// 기획팀 요청으로 커스텀 M4의 유탄/LSS 모드는 탄환 모두 소진시 익스텐션은 deavtivate되고 기본 모드로 돌아간다.
			if( pCurWeapon->isMultipleExtension())
				pCurWeapon->SwapExtensionType();
		}
	}


	////////////////////////////////////////////////////////////	보조 공격	
	if( actionState & AI_ACTION_SUBFUNC )	//	<- Pressed 다
	{
		bool bEnableWeapon = false;

		switch( pCurWeaponInfo->GetTypeClass())
		{
		case WEAPON_CLASS_KNIFE:			bEnableWeapon = true;			break;
		case WEAPON_CLASS_KNUCKLE:			bEnableWeapon = true;			break;
		// KEYWORD : WEAPON_C5
		case WEAPON_CLASS_BOMBTRIGGER :
			if( pCurWeapon->getLoadedBulletCount() > 0 )	// 탄환이 있는 경우 가능
				bEnableWeapon = true;
			break;
		case WEAPON_CLASS_DUALKNIFE :
		case WEAPON_CLASS_CIC :				bEnableWeapon = true;			break;
		}

		if( bEnableWeapon )
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) &&					//	폭탄 해체 중이 아니고
				(!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) &&				//	발사 불가능 상태가 아니고
				pCurWeapon->isFireEnabled() && !pCurWeapon->isWeaponStateMask( WSM_FIRING)))	// 무기의 발사가 가능한 경우
			{	
				bCancelDisarm = m_pOwner->Cmd_Attack( WEAPON::SECONDARY_FIRE);
			}
		}
		
	}

	////////////////////////////////////////////////////////////	리로드
	if( actionState & AI_ACTION_RELOAD )
	{
		if( pCurWeapon->isLoadMagazineEnabled() && !(actionState & AI_ACTION_SPECIAL) )
		{
			ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

			if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
				getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
				nLoadType = ACTION_LOAD_READY_MAG;

			bCancelMission = m_pOwner->Cmd_Reload(nLoadType);
		}
	}

	////////////////////////////////////////////////////////////	무기 교체	
	if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE))
	{
		if( actionState & AI_ACTION_SWAPWEAPON0 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON1 )
		{	
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_SECONDARY);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON2 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_MELEE);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON3 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_THROWING1);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON4 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_THROWING2);
		}
		else if( actionState & AI_ACTION_SWAPWEAPON5 )
		{
			bCancelMission = m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_MISSION);
		}

		m_nActionState &= ~0x00000000000FF000;	//SWAP WEAPON FLAG
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	이 이후 특수 버튼이외에는 bCancelMission과 관련되어 다른 무기 버튼을 정의해선 안됩니다.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////	특수 버튼 (폭탄 해체등)
	if( actionState & AI_ACTION_SPECIAL )
	{	
		if( !bCancelDisarm)
		{
			//	미션전 일 경우
			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) ||
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
			{			
				//	미션 진행중
				if( (pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED ))
				{
					//	네트워크 폭탄 해체
					m_pOwner->Cmd_Uninstall();
				}//	미션 종료
				else if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
				{
					m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					m_pOwner->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
				}
			}
		}
	}
	else if( (actionState & GAME_KEY_ACTION_SPECIAL) == 0)
	{	//	미션전 일 경우
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
		{
			bCancelDisarm = true;
		}
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
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DISARMINGBOMB))		
		{
			m_pOwner->Cmd_Uninstall( false);	//	네트워크 폭탄 해체 취소	
		}
	}
}

void CGameCharaInputControlAI::ProcessExtension(void)
{
	WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pCurWeapon == nullptr)
		return;

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != nullptr);

	if( getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) || getActionContext()->isUpper(CHARA_UPPER_LOADBULLET) ||
		getActionContext()->isUpper(CHARA_UPPER_FIREBULLET) || getActionContext()->isUpper(CHARA_UPPER_SWAPWEAPON) || 
		getActionContext()->isUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || getActionContext()->isUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
		return;

	UINT64 actionState = getActionState();

	switch( pCurWeapon->GetCurrentExtensionType())
	{
	default :
		// Extension이 없는 무기 모두...
		if( actionState & AI_ACTION_SUBFUNC )
		{
			switch( pCurWeaponInfo->GetTypeZoom())
			{
				case WEAPON::ZOOM_SCOPE :	m_pOwner->Cmd_ToggleZoom();		break;		// 스코프 기능 사용
			}
		}
		break;
	case WEAPON::EXT_SCOPE :
	case WEAPON::EXT_ZOOM :					// 견착
		// K2, K1
		if( actionState & AI_ACTION_SUBFUNC )
		{
			// 스코프 기능 사용
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

	case WEAPON::EXT_STEADY_AIM :
		// Spectre
		if( actionState & AI_ACTION_SUBFUNC )
		{
			// 스코프 기능 사용
			m_pOwner->Cmd_ToggleZoom();
		}
		break;

		// Activate 단계 없이 사용하는 Extension 무기들
	case WEAPON::EXT_DUAL_MAG :		// AK47
	case WEAPON::EXT_SILENCER :		// 사용과 동시에 Activate
		// MP5K	// MK23
		if( actionState & AI_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	case WEAPON::EXT_DOT_SIGHT :
		if( actionState & AI_ACTION_SUBFUNC )
		{
			// 스코프 기능 사용
			m_pOwner->Cmd_ToggleZoom();
		}
		break;
	case WEAPON::EXT_GLAUNCHER:
	case WEAPON::EXT_SHOTGUN_LAUNCHER:
		if( actionState & AI_ACTION_SUBFUNC )
		{
			//	무기 Extension 활성화 토글
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	case WEAPON::EXT_BAYONET:
		if( actionState & AI_ACTION_SUBFUNC )
		{
			//	무기 Extension 활성화 토글
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;

	case	WEAPON::EXT_CROSSFIRE :
		if( actionState & AI_ACTION_SUBFUNC )
		{
			m_pOwner->Cmd_ToggleExtensionWeapon();
		}
		break;
	}


	m_nActionState &= ~AI_ACTION_EXT_ACTIVATE;
	m_nActionState &= ~AI_ACTION_SUBFUNC;
}

void CGameCharaInputControlAI::ProcessInput( REAL32 rDeltatime)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		return;
	}

	if ( CGameMaster::i()->IsPaused() )
	{
		return;
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) )
	{
		// 무기 드랍 상태 체크
		ProcessDropWeapon( rDeltatime);
	}

	//	이동 값을 처리합니다.
	bool bChangeBody = false;

	///////////////////////////////////////////////////////////		앉아/일어서
	bChangeBody = ProcessCrouch( rDeltatime);

	///////////////////////////////////////////////////////////		점프
	if( m_nActionState & AI_ACTION_JUMP )
	{
		if( (!m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_MOVE) &&
			(m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) || m_pOwner->isCharaStateMask( CHARA_STATE_CLIMB)))
			&& !m_pOwner->isCharaStateMask( CHARA_STATE_SLOPESLIDE) )
		{
			m_pOwner->Cmd_JumpStart();

			m_nActionState &= ~AI_ACTION_JUMP;
		}		
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

void CGameCharaInputControlAI::ProcessDinoAttack( REAL32 rDeltatime)
{
	//앉아있는경우 공격 처리를 하지 않습니다.
	if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
	{	
		getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	}


	//bool bCancelMission = false;	//	미션 행동 취소 여부

	UINT64 actionState = getActionState();

	((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->SetNowAttackFirstFrame(false);

	if( actionState & AI_ACTION_FIRE )
	{	
		if( (m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false) && (m_pOwner->isDeathblowState() == false))
		{
			if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
			{	
				getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
			}

			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::PRIMARY_FIRE);
		}
	}

	////////////////////////////////////////////////////////////	보조 공격	
	if( actionState & AI_ACTION_SUBFUNC )	//	<- Pressed 다
	{
		if((m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_SHOT) == false ) && (m_pOwner->isDeathblowState() == false))
		{
			((CGameCharaDino*)((CGameCharaBase*)m_pOwner))->DinoAttack(WEAPON::SECONDARY_FIRE);
		}
	}
}



struct TMPOBJ_INFO
{
	i3GameObj * pObj = nullptr;
	VEC3D		vPos;
};

bool CGameCharaInputControlAI::DestroyIIObject( REAL32 rDeltaSeconds)
{
	for( size_t i = 0;i < m_RemoveObjectList.size(); ++i)
	{
		TMPOBJ_INFO * pInfo = m_RemoveObjectList[i];
		
		I3MEM_SAFE_FREE( pInfo);
	}
	m_RemoveObjectList.clear();
	

	if( _CheckObjectCollision( m_RemoveObjectList, 1.0f, rDeltaSeconds) > 0)
	{
		m_nAIState = AI_STATE_REMOVE_OBJ;
		SelectWeapon();

		m_nActionState |= AI_ACTION_ATTACK;
		return true;
	}
	else
		return false;
}

void CGameCharaInputControlAI::OnRemoveObject( REAL32 rDeltaSeconds)
{
	size_t nCnt = m_RemoveObjectList.size();

	if( nCnt > 0)
	{
		TMPOBJ_INFO * pInfo = m_RemoveObjectList[0];

		i3Object * pObj = (i3Object*)pInfo->pObj;
		if( pObj->getCurrentHP() <= 0.0f)
		{
			i3::vu::remove(m_RemoveObjectList, pInfo);

			I3MEM_SAFE_FREE( pInfo);
		}
		else
		{
			VEC3D vStart;
			VEC3D vDir;

			m_pOwner->getCameraContext()->getVirtualCamPosition( &vStart);

			i3Vector::Sub( &vDir, &pInfo->vPos, &vStart);
			i3Vector::Normalize( &m_vTargetDir, &vDir);

 			SeeTarget( &m_vTargetDir, 0.0f, true);

			m_nActionState |= AI_ACTION_FIRE;
		}
	}
	else
	{
		m_nAIState = AI_STATE_GOPATH;
		m_nActionState |= AI_ACTION_STOPATTACK;
		m_nActionState &= ~AI_ACTION_ATTACK;
	}
}

INT32 CGameCharaInputControlAI::_CheckObjectCollision( i3::vector<TMPOBJ_INFO*>& List, REAL32 rRange, REAL32 rDeltaSeconds)
{
	INT32		i;
	i3Object *	pGameObj;
	
	CStageBattle * pStage = g_pFramework->GetStageBattle();
	if( pStage == nullptr )
		return 0;
	
	StageObjectListPtr pObjectList = pStage->GetStageObjectList();

	if( pObjectList == nullptr )
		return 0;

	I3ASSERT( rRange > I3_EPS);

	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[ 256];
	
	NxSphere nxSphere;
	nxSphere.center.x = getX( m_pOwner->GetPos());
	nxSphere.center.y = getY( m_pOwner->GetPos()) + rRange * 0.5f;
	nxSphere.center.z = getZ( m_pOwner->GetPos());

	nxSphere.radius = rRange;

	INT32 iActiveGrp = (1<<CGRP_OBJECT_HIT) | (1<<CGRP_OBJECT_ALL) | (1<<CGRP_OBJECT_WHOLE) | (1<<CGRP_OBJECT_WHOLE_DYNAMIC);
    
	INT32 nOverlapCount = pNxScene->overlapSphereShapes( nxSphere, NX_DYNAMIC_SHAPES, 256, pShapeList, nullptr, iActiveGrp);

	//I3TRACE("걸리는 오브젝트 %d개 발견!! 깨부수자\n", nOverlapCount);

	INT32 nCnt = 0;
	for( i = 0; i < nOverlapCount; i++ )
	{
		i3EventReceiver * pReceiver = (i3EventReceiver*) pShapeList[ i]->userData;
		if( pReceiver == nullptr)
			continue;

		if( !i3::same_of<i3PhysixShapeSet*>(pReceiver))
			continue;

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pReceiver;
		if( !i3::kind_of<i3Object*>(pShapeSet->getEventReceiver()))
			continue;

		pGameObj = (i3Object*)pShapeSet->getEventReceiver();
		I3ASSERT( pGameObj->getNetIndex() > -1);

		// HP가 있는 오브젝트는 0이면 스킵
		if( (pGameObj->getStateCount() > 1) && (pGameObj->getCurrentHP() == 0.f) )
			continue;

		//찾은 오브젝트가 웨폰 박스면 그냥 패스 이거 지우면 애들이 계속 돕니다.
		if( i3::same_of<CGameObjectWeaponBox*>(pGameObj) )
			continue;

		TMPOBJ_INFO * pInfo = (TMPOBJ_INFO*)i3MemAlloc( sizeof( TMPOBJ_INFO));

		pInfo->pObj = pGameObj;
		pShapeSet->getShapeMassCenterPos( &pInfo->vPos);

		List.push_back( pInfo);

		nCnt++;
	}

	return nCnt;
}
