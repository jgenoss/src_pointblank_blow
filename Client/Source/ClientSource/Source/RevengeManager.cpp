#include "pch.h"
#include "RevengeManager.h"
#include "ChainKills.h"
#include "MyRoomInfoContext.h"


I3_CLASS_INSTANCE(CRevengeManager);//, i3ElementBase);

CRevengeManager::CRevengeManager(void)
{
	m_CurrentKilledUserIdx = -1;
	m_bLoveShot = false;
	for(INT32 i=0; i < SLOT_MAX_COUNT; i++)
	{
		m_RevengeUser[i].m_bRevengeUserState = false;
		m_RevengeUser[i].m_nKillDeathCount = 0;
		m_RevengeUser[i].m_nChainKill = 0;

		m_LoveShotUser[i].m_state = false;
		m_LoveShotUser[i].m_Time = 0.0f;

		m_LoveShotUserAfter[i].m_state = false;
		m_LoveShotUserAfter[i].m_Time = 0.0f;
	}
}

CRevengeManager::~CRevengeManager(void)
{

}

void CRevengeManager::InitData()
{
	m_CurrentKilledUserIdx = -1;
	m_nMyHotKillerCount = 0;
	m_DeathWeaponNum = 0;
	m_bLoveShot = false;
	m_bReservedLoveShot = false;
	for(INT32 i=0; i < SLOT_MAX_COUNT; i++)
	{
		m_RevengeUser[i].m_bRevengeUserState = false;
		m_RevengeUser[i].m_nKillDeathCount = 0;
		m_RevengeUser[i].m_nChainKill = 0;

		m_LoveShotUser[i].m_state = false;
		m_LoveShotUser[i].m_Time = 0.0f;

		m_LoveShotUserAfter[i].m_state = false;
		m_LoveShotUserAfter[i].m_Time = 0.0f;
	}
}

void CRevengeManager::AddKillDeath(INT32 idx)
{
	I3_BOUNDCHK(idx,SLOT_MAX_COUNT);
	m_CurrentKilledUserIdx = (UINT8)idx;
	m_RevengeUser[idx].m_nKillDeathCount++;
	m_RevengeUser[idx].m_nChainKill++;
	SetMyHotKillerCount();
}

void CRevengeManager::SubKillDeath(INT32 idx)
{
	I3_BOUNDCHK(idx,SLOT_MAX_COUNT);
	m_RevengeUser[idx].m_nKillDeathCount--;
	SetMyHotKillerCount();
	ResetChainKillUser();
	if(m_RevengeUser[idx].m_nKillDeathCount <= REVENGE_TYPE_PAYBACK)
		RegistRevengeUser(idx);
}

void CRevengeManager::SetDeathUserNick(const i3::rc_wstring& wstrNick)
{
	m_DeathNick = wstrNick;
}

void CRevengeManager::ResetRevengeUser(INT32 idx)
{
	m_RevengeUser[idx].m_nChainKill = 0;
	m_RevengeUser[idx].m_bRevengeUserState = false;
	m_RevengeUser[idx].m_nKillDeathCount = 0;
}

void CRevengeManager::ResetRevengeUserAll()
{
	for(INT32 i=0; i < SLOT_MAX_COUNT; i++)
	{
		m_RevengeUser[i].m_nChainKill = 0;
		m_RevengeUser[i].m_bRevengeUserState = false;
		m_RevengeUser[i].m_nKillDeathCount = 0;
	}
}

void CRevengeManager::ResetChainKillUser()
{
	for(INT32 i=0; i < SLOT_MAX_COUNT; i++)
		m_RevengeUser[i].m_nChainKill = 0;
}

bool CRevengeManager::isBefore(INT32 nKillMessage, INT32 nPreKillCount, INT32 nNowKillCount)
{
	if( nKillMessage & KILLING_MESSAGE_OBJECTDEFENCE)		return false;
	else if (nKillMessage & KILLING_MESSAGE_PIERCINGSHOT)	return false;
	else if (nKillMessage & KILLING_MESSAGE_MASSKILL)		return false;
	else if (nKillMessage & KILLING_MESSAGE_CHAINHEADSHOT)	return false;
	else if (nKillMessage & KILLING_MESSAGE_HEADSHOT)		return false;
	else if (nKillMessage & KILLING_MESSAGE_CHAINSLUGGER)	return false;
	else if (nKillMessage & KILLING_MESSAGE_CHAINSTOPPER)	return false;
	else																		// 특수 메시지가 아니라면...
	{
		if (3 <= nPreKillCount && nNowKillCount == 1)		return false;
		else if (2 == nPreKillCount && nNowKillCount == 1)	return true;		// 트리플킬
		else if (1 == nPreKillCount && nNowKillCount == 1)	return true;		// 더블킬
		else												return false;
	}

}

void CRevengeManager::SetMyHotKillerCount()
{
	UINT8 myHotKillerCount = 0;
	for(INT32 i=0; i < SLOT_MAX_COUNT; i++)
	{
		if(m_RevengeUser[i].m_nKillDeathCount >= REVENGE_TYPE_HOTKILLER)
			myHotKillerCount++;
	}
	m_nMyHotKillerCount = myHotKillerCount;
}

INT32 CRevengeManager::GetCurrentMessage()
{
	if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) == true || LocaleValue::Enable("EnableRevengeMode") == false) // 복수모드는 트레이닝에서 작동안한다.
		return -1;

	I3_BOUNDCHK(m_CurrentKilledUserIdx, SLOT_MAX_COUNT);
	I3TRACE("CurrentKilledUserIdx : %d\n", m_CurrentKilledUserIdx);
	INT32 CurrentType = REVENGE_TYPE_NONE;

	if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount < REVENGE_TYPE_HOTKILLER && m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount > 0)
		CurrentType = REVENGE_TYPE_NONE;

	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount == REVENGE_TYPE_HOTKILLER && GetMyHotKillerCount() == 1)
		CurrentType = REVENGE_TYPE_HOTKILLER;
	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount == REVENGE_TYPE_HOTKILLER && GetMyHotKillerCount() == 2)
		CurrentType = REVENGE_TYPE_NIGHTMARE;
	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount == REVENGE_TYPE_HOTKILLER && GetMyHotKillerCount() >= 3)
		CurrentType = REVENGE_TYPE_MEATCHOPPER;

	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount == REVENGE_TYPE_NIGHTMARE && GetMyHotKillerCount() == 1)
		CurrentType = REVENGE_TYPE_NIGHTMARE;
	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount == REVENGE_TYPE_NIGHTMARE && GetMyHotKillerCount() == 2)
		CurrentType = REVENGE_TYPE_NIGHTMARE;
	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount == REVENGE_TYPE_NIGHTMARE && GetMyHotKillerCount() >= 3)
		CurrentType = REVENGE_TYPE_MEATCHOPPER;

	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount >= REVENGE_TYPE_MEATCHOPPER)
		CurrentType = REVENGE_TYPE_MEATCHOPPER;

	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount < 0 && isRevengeUser(m_CurrentKilledUserIdx) && m_RevengeUser[m_CurrentKilledUserIdx].m_nChainKill > 1)
		CurrentType = REVENGE_TYPE_AVENGER;
	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount < 0 && isRevengeUser(m_CurrentKilledUserIdx))
		CurrentType = REVENGE_TYPE_PAYBACK;
	else if( m_RevengeUser[m_CurrentKilledUserIdx].m_nKillDeathCount == 0 && isRevengeUser(m_CurrentKilledUserIdx))
	{
		ResetRevengeUser(m_CurrentKilledUserIdx);
		CurrentType = REVENGE_TYPE_STALKER;
	}

	return CurrentType;
}

i3::rc_wstring CRevengeManager::getRevengeMessage(INT32 c_type)
{
	INT32 rand1 = i3Math::Rand();
	INT32 rand2 = i3Math::Rand();

	switch(c_type)
	{
	case CK_REVENGE_HOTKILLER: 
		if(rand1 > rand2)
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_HOTKILLER1");
		else
		{
			i3::rc_wstring wstrHotKillerMsg;
			i3::sprintf(wstrHotKillerMsg, GAME_RCSTRING("STR_INGAME_HUD_REVENGE_HOTKILLER2"), m_DeathNick);
			return wstrHotKillerMsg;
		}
	case CK_REVENGE_NIGHTMARE:
		if(GetMyHotKillerCount() == 2)
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_NIGHTMARE1");
		else
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_NIGHTMARE2");
	case CK_REVENGE_MEATCHOPPER:
		//if(rand1 > rand2)
		//	return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_MEATCHOPPER1");
		//else
		return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_MEATCHOPPER2");
	case CK_REVENGE_PAYBACK:
		if(rand1 > rand2)
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_PAYBACK1");
		else
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_PAYBACK2");
	case CK_REVENGE_AVENGER:
		if(rand1 > rand2)
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_AVENGER1");
		else
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_AVENGER2");
	case CK_REVENGE_STALKER:
		if(rand1 > rand2)
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_STALKER1");
		else
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_STALKER2");
	case CK_REVENGE_LOVESHOT:
		if(rand1 > rand2)
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_LOVESHOT1");
		else
			return GAME_RCSTRING("STR_INGAME_HUD_REVENGE_LOVESHOT2");
	case CK_REVENGE_AWESOME:
		return GAME_STRING("STR_INGAME_HUD_COMPLIMENT_AWESOME");

	}

	return i3::rc_wstring();
}

bool CRevengeManager::isLoveShot(INT32 idx)
{
	if( m_LoveShotUser[idx].m_state == true)
	{
		m_LoveShotUser[idx].m_state = false;
		m_LoveShotUser[idx].m_Time = 0.0f;
		return true;
	}
	else
	{
		m_LoveShotUser[idx].m_state = false;
		m_LoveShotUser[idx].m_Time = 0.0f;
		return false;
	}
}

bool CRevengeManager::isLoveShotAfter(INT32 idx)
{
	if( m_LoveShotUserAfter[idx].m_state == true)
	{
		m_LoveShotUserAfter[idx].m_state = false;
		m_LoveShotUserAfter[idx].m_Time = 0.0f;
		return true;
	}
	else
	{
		m_LoveShotUserAfter[idx].m_state = false;
		m_LoveShotUserAfter[idx].m_Time = 0.0f;
		return false;
	}
}

void CRevengeManager::OnUpdate(REAL32 rDeltaSeconds)
{
	if( LocaleValue::Enable("EnableRevengeMode") == false )
		return;
	for(INT32 i=0; i < SLOT_MAX_COUNT; i++)
	{
		if( m_LoveShotUser[i].m_state == true)
		{
			m_LoveShotUser[i].m_Time += rDeltaSeconds;
		}

		if( m_LoveShotUser[i].m_Time > 5.0f )
		{
			m_LoveShotUser[i].m_state = false;
			m_LoveShotUser[i].m_Time = 0.0f;
		}

		if( m_LoveShotUserAfter[i].m_state == true)
		{
			m_LoveShotUserAfter[i].m_Time += rDeltaSeconds;
		}

		if( m_LoveShotUserAfter[i].m_Time > 5.0f )
		{
			m_LoveShotUserAfter[i].m_state = false;
			m_LoveShotUserAfter[i].m_Time = 0.0f;
		}
	}
}