#include "pch.h"
#include "GameCharaActionControl3PVNPC.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaActionControl3PVNPC);//, CGameCharaActionControl3PV);

CGameCharaActionControl3PVNPC::CGameCharaActionControl3PVNPC()
{
}

CGameCharaActionControl3PVNPC::~CGameCharaActionControl3PVNPC()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action
bool CGameCharaActionControl3PVNPC::OnActionReset( void)
{
	//	캐릭터 State 정보 리셋
	// 애니메이션을 중지한다.
	getActionContext()->_Stop_Anim();

	return true;
}

bool CGameCharaActionControl3PVNPC::OnActionStandup( void)
{	
	getActionContext()->setBodyLower( CHARA_BODYLOWER_STAND);
	getActionContext()->_Play_Body_Sync();

	return true;
}


bool CGameCharaActionControl3PVNPC::OnActionSteadyLower( void)
{	
	return false;
}

bool CGameCharaActionControl3PVNPC::OnActionSteadyUpper( void)
{
	getActionContext()->_Play_Anim_Default( ID_DEFAULT_IDLE);
	return true;
}

bool CGameCharaActionControl3PVNPC::OnActionJumpStart( void)
{
	m_pOwner->addCharaStateMask( CHARA_STATE_JUMP);

	getMoveContext()->setElapsedTakeOffTime( 0.f);

	if( isBody( CHARA_BODYLOWER_CROUCH))	//	앉아 있으면
	{
		_ApplyActionBody( CHARA_BODYLOWER_STAND);
	}

	setLower( CHARA_LOWER_JUMP);
	_Animation_Lower( ID_LOWER_JUMP_START);

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE) )
	{
		m_pOwner->Cmd_SyncWeapon();
		_Animation_Upper( ID_UPPER_JUMP_START);
	}

	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_START, m_pOwner->GetPos(), m_pOwner->is1PV());

	return true;
}

bool CGameCharaActionControl3PVNPC::OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir)
{
	if( g_pGameContext->IsP2PHost() )
		getAIContext()->SetCommand( NPC_AI_STATE_DAMAGE);

	return true;
}

bool CGameCharaActionControl3PVNPC::OnActionDeath( INT32 iKillIdx, CHARA_DEATH_TYPE type, VEC3D * pDir)
{
	getAIContext()->SetCommand( NPC_AI_STATE_DEATH);
	return true;
}



