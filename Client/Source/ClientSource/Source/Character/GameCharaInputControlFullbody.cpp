#include "pch.h"

#include "GameCharaInputControlFullbody.h"
#include "GameCharaInputControlNetwork.h"
#include "GameStateMgr.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE(CGameCharaInputControlFullbody);

void CGameCharaInputControlFullbody::OnFirstUpdate(REAL32 tm)
{
	ISTEST_S
	{
		return;
	}
	ISTEST_E

	if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH)) return;
	if (!BattleServerContext::i()->IsP2PActivate())	return;
	if (m_pOwner->getCharaNetIndex() == -1) return;
	if (!g_pFramework->IsBattleStage())	return;
	if ((gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE)) return;

	GAMEINFO_CHARACTER* pGameInfo = BattleSlotContext::i()->setGameInfo_Chara( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
	pGameInfo->_bRecvFlag = false;
	
	m_controlNetwork->_InterpolateThetaRho();

	VEC3D lerp;
	m_controlNetwork->_InterpolatePosition(&lerp);
	
	m_pOwner->forceSetPos(&lerp);
}

void CGameCharaInputControlFullbody::OnUpdate(REAL32 tm)
{
	CGameCharaInputControl::OnUpdate(tm);
}

void CGameCharaInputControlFullbody::ProcessNetworkPosition(GAMEINFO_CHARA_NETDATA* info)
{
	m_controlNetwork->ProcessNetworkPosition(info);
}

void CGameCharaInputControlFullbody::ProcessNetworkRotationTheta(GAMEINFO_CHARA_NETDATA* info)
{
	m_controlNetwork->ProcessNetworkRotationTheta(info);
}

void CGameCharaInputControlFullbody::ProcessNetworkRotationRho(GAMEINFO_CHARA_NETDATA* info)
{
	m_controlNetwork->ProcessNetworkRotationRho(info);
}

void CGameCharaInputControlFullbody::SetNetworkInputControl(CGameCharaInputControlNetwork* adaptee)
{
	I3ASSERT(adaptee);
	m_controlNetwork = adaptee;
}
