#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

#define NPC_GIRAN_ATTACKTIME	1.3f
#define NPC_AI_TIME				1.0f

I3_CLASS_INSTANCE( CGameCharaAIContext);//, CGameCharaContextBase);

CGameCharaAIContext::CGameCharaAIContext() : m_rRespawnTimer(0.0f)
{
	m_rAICheckTimer		= 0.f;	
	m_nCurrentAIState	= NPC_AI_STATE_NONE;

	m_bUseAI			= FALSE;
	m_nCurrentTarget	= -1;
	m_nNextTarget		= -1;
	m_bArrowFire		= FALSE;
	i3Vector::Zero( &m_vTargetDir);
	m_nFireEffectAttachBoneIdx	= -1;
	m_nFireEffectIdx			= -1;
	m_rIdleSoundPlayTime		= 0.f;
	m_rIdleSoundPlayCheckTime	= 3.f;
	m_rDeathSendTime			= 0.f;
}

CGameCharaAIContext::~CGameCharaAIContext()
{
	OnDestroy();
}

void CGameCharaAIContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
	m_nFireEffectIdx = g_pEffectManager->FindEffect( "Fir_Arr_L2_01" );
	m_nFireEffectAttachBoneIdx = m_pOwner->getSceneObject()->GetBody()->FindBoneByName( "Bow_Dummy");	
}

void CGameCharaAIContext::OnReset( void)
{
	m_rAICheckTimer				= 0.f;
	m_nCurrentAIState			= NPC_AI_STATE_NONE;
	m_nCurrentTarget			= -1;
	m_nNextTarget				= -1;	
	m_bArrowFire				= FALSE;	
	m_rRespawnTimer				= 0.f;
	m_rDeathSendTime			= 0.f;	
	m_rIdleSoundPlayTime		= 0.f;
	m_rIdleSoundPlayCheckTime	= 3.f;
	i3Vector::Zero( &m_vTargetDir);	
}

void CGameCharaAIContext::OnUpdate( REAL32 rDeltatime)
{	
	if( !m_bUseAI )return; 

	switch( m_nCurrentAIState )
	{
	case NPC_AI_STATE_IDLE :
		ProcessTargetDir( rDeltatime);
		ProcessIdle( rDeltatime);
		break;
	case NPC_AI_STATE_ATTACK :
		ProcessTargetDir( rDeltatime);
		ProcessAttack( rDeltatime);
		break;
	case NPC_AI_STATE_DAMAGE :			
		ProcessTargetDir( rDeltatime);
		ProcessDamage( rDeltatime);
		break;
	case NPC_AI_STATE_DEATH :
		ProcessDeath( rDeltatime);
		break;
	case NPC_AI_STATE_NONE		:
	case NPC_AI_STATE_RESPAWN	:
	case NPC_AI_STATE_FIREARROW :
	default						: 
		SetCommand( NPC_AI_STATE_IDLE ); 
		break;
	}

}

void CGameCharaAIContext::ProcessIdle( REAL32 rDeltatime)
{		
	m_rAICheckTimer += rDeltatime;	

	BOOL bReserveArrow = FALSE;
	if( g_pGameContext->IsP2PHost() && (m_rAICheckTimer >= NPC_AI_TIME) )
	{	//방장이고 공격할 유저를 찾음
		INT32 nFindTarget = RoleFindTarget();
		if( nFindTarget > -1)
		{
			m_nCurrentTarget = nFindTarget;
			SetCommand( NPC_AI_STATE_ATTACK);
			bReserveArrow = TRUE;
		}		
		m_rAICheckTimer = 0.f;
	}
}

void CGameCharaAIContext::ProcessAttack( REAL32 rDeltatime)
{
	VEC3D vPos;

	//수정해야함
	if( m_nCurrentTarget == -1 || m_nCurrentTarget >= SLOT_MAX_COUNT)
	{
		SetCommand( NPC_AI_STATE_IDLE );
		return;
	}
	i3AIContext * pCtx = getAnimContext()->getAIDefault();

	//수정해야함
	if( pCtx == NULL)
	{ 
		SetCommand( NPC_AI_STATE_IDLE );
		return; 
	}
	if( pCtx->getCurrentAIState() == NULL)
	{
		SetCommand( NPC_AI_STATE_IDLE );
		return; 
	}

	VEC3D vDir, vRand;
	if( !m_bArrowFire && pCtx->getLocalTime() > NPC_GIRAN_ATTACKTIME )
	{
		m_bArrowFire = TRUE;

		if( g_pGameContext->IsP2PHost() )
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( (WEAPON_CLASS_TYPE)WEAPON_CLASS_NPC_ARROW, 1);

			i3Vector::GetRandVector( &vRand);

			i3Vector::Scale( &vRand, &vRand, i3Math::Randf() * pWeaponInfo->GetDeviation());
			i3Vector::Copy( &vDir, &m_vTargetDir);

			REAL32 rLen = i3Vector::Length( &vDir);
			i3Vector::AddY( &vDir, rLen * rLen * 0.05f);
			i3Vector::Add( &vDir, &vDir, &vRand);
			i3Vector::Normalize( &vDir, &vDir);

			i3Vector::Copy( &vPos, m_pOwner->GetPos());
			i3Vector::AddY( &vPos, 1.5f);

			g_pWeaponManager->ArrowFire( m_pOwner, &vPos, &vDir, pWeaponInfo->GetThrowSpeed());

			/////////////////////////////////////////////////////////////////////////////
			//	Network info send
			I3ASSERT( m_pOwner->getCurHP() < 255);
			GAMEEVENT::Write_NPC( m_pOwner->getCharaNetIndex(),
				(UINT8) ((NPC_AI_STATE_FIREARROW & 0x0F) | (m_nCurrentTarget << 4)),
				(UINT8) m_pOwner->getCurHP(), &vDir);
		}
	}

	//자기가 알아서 Animation Stop 
	if( pCtx->getLocalTime() + rDeltatime > pCtx->getCurrentAIState()->getDuration())
	{
		getAnimContext()->_Stop_Anim();

		SetCommand( NPC_AI_STATE_IDLE );
		m_rAICheckTimer = NPC_AI_TIME;

		m_bArrowFire = FALSE;

		if( !g_pGameContext->IsP2PHost())
		{//내가 알아서 
			GAMEINFO_NPC * pNPCInfo = g_pGameContext->getGameInfo_NPC( m_pOwner->getCharaNetIndex() );
			pNPCInfo->_nState = (UINT8) ((pNPCInfo->_nState & 0xF0) | NPC_AI_STATE_IDLE);
		}
	}
}

void CGameCharaAIContext::ProcessDamage( REAL32 rDeltatime)
{
	i3AIContext * pCtx = getAnimContext()->getAIDefault();

	if( pCtx == NULL)
		return;

	if( pCtx->getCurrentAIState() == NULL)
		return;

	if( pCtx->getLocalTime() + rDeltatime > pCtx->getCurrentAIState()->getDuration())
	{
		getAnimContext()->_Stop_Anim();
		if( g_pGameContext->IsP2PHost() )
			SetCommand( NPC_AI_STATE_IDLE);
	}
}

void CGameCharaAIContext::ProcessDeath( REAL32 rDeltatime)
{
	m_rRespawnTimer += rDeltatime;
	
	if( g_pGameContext->IsP2PHost() )
	{//방장이 패킷을 Setting 합니다. 
		m_rDeathSendTime += rDeltatime;
		if( m_rDeathSendTime > 1.f)
		{
			I3ASSERT( m_pOwner->getCurHP() < 255);
			GAMEEVENT::Write_NPC( m_pOwner->getCharaNetIndex(),
				(UINT8) ((NPC_AI_STATE_DEATH & 0x0F) | (m_nCurrentTarget << 4)),
				(UINT8) m_pOwner->getCurHP());

			m_rDeathSendTime = 0.f;
		}
	}

	if( m_pOwner->getRespawnElapseTime() == 0.f && m_rRespawnTimer > 3.f)
		m_pOwner->setRespawnElapseTime( m_pOwner->getDBInfo()->GetNPCRespawnTime() );

	if( m_rRespawnTimer >= m_pOwner->getDBInfo()->GetNPCRespawnTime() )
	{
		if( g_pGameContext->IsP2PHost() )
		{
			m_pOwner->setRespawnElapseTime( 0.f);
			m_rRespawnTimer = 0.f;
			
			SetCommand( NPC_AI_STATE_RESPAWN);
		}
	}
}

BOOL CGameCharaAIContext::FindTarget( void)
{
	return TRUE;
}

void CGameCharaAIContext::ProcessTargetDir( REAL32 rDeltatime)
{
	if( m_nCurrentTarget == -1)	return;
	CGameCharaBase * pTarget = g_pCharaManager->getCharaByNetIdx( m_nCurrentTarget);
	if( pTarget == NULL )return;

	VEC3D vVel;
	i3Vector::Sub( &m_vTargetDir, pTarget->GetPos(), m_pOwner->GetPos());
	REAL32 rDistance = i3Vector::Length( &m_vTargetDir);
	if( rDistance > 1.f)
	{
		i3Vector::Scale( &vVel, pTarget->getMoveContext()->getVelocity(), rDistance);
		setY( &vVel, 0.f);
		i3Vector::Add( &m_vTargetDir, &m_vTargetDir, &vVel);
	}

	i3Vector::Normalize( &m_vTargetDir, &m_vTargetDir);
	REAL32 rTheta = i3Math::atan2( getX( &m_vTargetDir), getZ( &m_vTargetDir));
	getBoneContext()->setTheta( rTheta);
	getBoneContext()->_CalcTheta( rDeltatime);
}

//가장 가까운것을 공격
//확률필요없음.(기획실 최지훈)
INT32 CGameCharaAIContext::RoleFindTarget( void)
{
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( (WEAPON_CLASS_TYPE)WEAPON_CLASS_NPC_ARROW, 1);
	I3ASSERT( pWeaponInfo != NULL);
	REAL32 rMinLength = pWeaponInfo->GetRange();

	INT32 nRvIndex = -1;

	//가장 가까운 곳에 있는 유저를 찾습니다. 
	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == NULL )	continue;
		if( pChara->isCharaStateMask( CHARA_STATE_DEATH))continue;

		// 팀 체크
		if( ((m_pOwner->getCharaTeam() & NPC_TEAM_RED) && (pChara->getCharaTeam() & CHARACTER_TEAM_RED)) ||
			((m_pOwner->getCharaTeam() & NPC_TEAM_BLUE) && (pChara->getCharaTeam() & CHARACTER_TEAM_BLUE)) )
			continue;

		VEC3D vDir;		
		i3Vector::Sub( &vDir, pChara->GetPos(), m_pOwner->GetPos());
		REAL32 rLen = i3Vector::Length( &vDir);
		
		if( rLen < rMinLength)
		{
			rMinLength	= rLen;
			nRvIndex	= pChara->getCharaNetIndex();
		}
	}
	return nRvIndex;
}

void CGameCharaAIContext::SetCommand( INT32 cmd)
{
	switch( cmd )
	{
	case NPC_AI_STATE_NONE :
		{	
			m_pOwner->ResetChara();
			g_pCharaManager->RespawnGiranNPC( m_pOwner->getCharaNetIndex());
			if( !g_pGameContext->IsP2PHost() )
			{
				m_pOwner->setRespawnElapseTime( 0.f);
				m_rRespawnTimer = 0.f;
			}
		}
		break;
	case NPC_AI_STATE_IDLE :
		{
			getAnimContext()->_Play_Anim_Default( ID_DEFAULT_ATTACKIDLE);
		}
		break;
	case NPC_AI_STATE_ATTACK :
		if( m_nCurrentAIState != NPC_AI_STATE_DAMAGE &&
			m_nCurrentAIState != NPC_AI_STATE_DEATH )
		{
			if( m_nFireEffectAttachBoneIdx != -1)
			{
				i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
				MATRIX * pMtx = pBody->getCacheMatrix( m_nFireEffectAttachBoneIdx);
				VEC3D norm;
				i3Vector::Set( &norm, 0.f, 1.f, 0.f);

				g_pEffectManager->AddEffect( m_nFireEffectIdx, i3Matrix::GetPos(pMtx), &norm);
			}

			getAnimContext()->_Play_Anim_Default( ID_DEFAULT_ATTACK);
		}
		break;
	case NPC_AI_STATE_DAMAGE :					
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		{
			getAnimContext()->_Play_Anim_Default( ID_DEFAULT_DAMAGE);
			m_nCurrentTarget = -1;
		}
		break;
	case NPC_AI_STATE_DEATH :
		{
			getAnimContext()->_Play_Anim_Default( ID_DEFAULT_DEATH);
			m_nCurrentTarget = -1;
			m_pOwner->addCharaStateMask( CHARA_STATE_DEATH);
			getMoveContext()->EnableController( FALSE);
		}
		break;
	case NPC_AI_STATE_RESPAWN :
		{
			m_pOwner->ResetChara();
			g_pCharaManager->RespawnGiranNPC( m_pOwner->getCharaNetIndex());

			if( g_pGameContext->IsP2PHost() )
			{
				getAnimContext()->_Play_Anim_Default( ID_DEFAULT_ATTACKIDLE);
			}
			else
			{
				m_pOwner->setRespawnElapseTime( 0.f);
				m_rRespawnTimer = 0.f;
			}
		}
		break;
	case NPC_AI_STATE_FIREARROW :// Network at client
		break;
	}
	m_nCurrentAIState = cmd ; 

	// Send packet data
	if( !g_pGameContext->IsP2PHost() )return;

	I3ASSERT( m_pOwner->getCurHP() < 255);
	GAMEEVENT::Write_NPC( m_pOwner->getCharaNetIndex(), (UINT8) ((cmd & 0x0F) | (m_nCurrentTarget << 4)),(UINT8) m_pOwner->getCurHP());
}


