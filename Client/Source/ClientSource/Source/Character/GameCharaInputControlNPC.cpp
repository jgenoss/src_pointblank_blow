#include "pch.h"
#include "GameCharaInputControlNPC.h"
#include "GlobalVariables.h"
#include "GameCharaDef.h"
#include "GameCharaArtificialIntelligenceContext.h"

I3_CLASS_INSTANCE( CGameCharaInputControlNPC);//, CGameCharaInputControl);

CGameCharaInputControlNPC::CGameCharaInputControlNPC()
{

}

CGameCharaInputControlNPC::~CGameCharaInputControlNPC()
{

}

void CGameCharaInputControlNPC::OnFirstUpdate( REAL32 rDeltatime)
{
	if( !g_pFramework->IsBattleStage())		return;
	if( g_pGameContext->IsP2PHost() )		return;		//방장이면 리턴 
	
	GAMEINFO_NPC * pInfo = g_pGameContext->getGameInfo_NPC( m_pOwner->getCharaNetIndex());
	INT32 nCmd		= (INT32)(pInfo->_nState & 0x0F);
	INT32 nTarget	= (INT32)((pInfo->_nState >> 4) & 0x0F);	

	if( nCmd == getAIContext()->getCurrentAIState() )return; 
	switch( nCmd )
	{
	case NPC_AI_STATE_IDLE :		
		getAIContext()->SetCommand( NPC_AI_STATE_IDLE);
		break;
	case NPC_AI_STATE_ATTACK :
		
		getAIContext()->SetCommand( NPC_AI_STATE_ATTACK);		
		getAIContext()->setCurrentTarget( nTarget );
		break;
	case NPC_AI_STATE_DAMAGE :		
		getAIContext()->SetCommand( NPC_AI_STATE_DAMAGE);
		break;
	case NPC_AI_STATE_DEATH :		
		getAIContext()->SetCommand( NPC_AI_STATE_DEATH);
		break;
	case NPC_AI_STATE_FIREARROW	:
		{	
			VEC3D vPos;
			i3Vector::Copy( &vPos, m_pOwner->GetPos());
			i3Vector::AddY( &vPos, 1.5f);
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( (WEAPON_CLASS_TYPE)WEAPON_CLASS_NPC_ARROW, 1);
			g_pWeaponManager->ArrowFire( m_pOwner, &vPos, &pInfo->_vTargetPos, pWeaponInfo->GetThrowSpeed());
			pInfo->_nState = (UINT8)(getAIContext()->getCurrentAIState() | (nTarget << 4));
		}
		break;
	case NPC_AI_STATE_HITARROW:
		pInfo->_nState = (UINT8)(getAIContext()->getCurrentAIState() | (nTarget << 4));
	case NPC_AI_STATE_RESPAWN		:		break;	//하는일 없음 //각자 알아서 리스폰
		break;
	}
}

void CGameCharaInputControlNPC::OnUpdate( REAL32 rDeltatime)
{
}

void CGameCharaInputControlNPC::OnLastUpdate( REAL32 rDeltatime)
{
	if( !g_pGameContext->IsP2PHost())
		return;


}

