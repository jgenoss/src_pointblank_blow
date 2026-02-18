#include "pch.h"
#include "UserContext.h"

#include "../GameStateMgr.h"
#include "BattleSyncTimer.h"
#include "BattleSlotContext.h"
#include "ScoreContext.h"
#include "AssistContext.h"

namespace
{
	INT32 GetKillCount(INT32 slotIdx)
	{
		INT32 killCount = -1;

		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);

		if (chara != nullptr)
			killCount = ScoreContext::i()->getIndividualScore(slotIdx)->m_KillCount; 

		return killCount;
	}

	INT32 GetDeathCount(INT32 slotIdx)
	{
		INT32 deathCount = -1;

		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);

		if (chara != nullptr)
			deathCount = ScoreContext::i()->getIndividualScore(slotIdx)->m_DeathCount; 

		return deathCount;
	}

	struct RankingSortFunc
	{
		bool operator()(const INT32& l_slotIdx, const INT32& r_slotIdx)
		{
			if (GetKillCount(l_slotIdx) > GetKillCount(r_slotIdx)) return true;
			else if (GetKillCount(l_slotIdx) == GetKillCount(r_slotIdx)) 
			{
				if (GetDeathCount(l_slotIdx) < GetDeathCount(r_slotIdx))
					return true;
			}

			return false;
		}
	};
}


UserContext::UserContext() : PlayingSlotFlag(0), bBowFireOrder(false), ReceivedEvent(false)
{
	InitRoomData();
}

bool UserContext::IsPlayingSlot(INT32 idxSlot) const
{
	UINT16 nFinder = 0x0001;
	nFinder = (UINT16)(nFinder << idxSlot);

	return ((PlayingSlotFlag & nFinder) > 0)?true:false;
}

bool UserContext::IsBattleSlot(INT32 idxSlot) const
{
	UINT16 nFinder = 0x0001;
	nFinder = (UINT16)(nFinder << idxSlot);

	return ((BattleUserFlag & nFinder) > 0)?true:false;
}

void UserContext::QueDeathChara( INT32 idx, UINT8 ui8Weapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx)
{
	I3ASSERT( idx < SLOT_MAX_COUNT);
	KillInfo[idx].QueKillingInfo( ui8Weapon, nDeathPacket, nType, WeaponInfo, pDeathPos, nObjectIdx);
}

void UserContext::SendDeathChara()
{
	INT32 i, j, idx;
	CGameCharaBase * pChara; 
	bool	bMyDeath;

	CStageBattle * pStage = g_pFramework->GetStageBattle();

	if( pStage == nullptr || gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE )
		return;

	KILLDATA data;
	memset( &data, 0, sizeof( KILLDATA ) );

	for( i = 0; i < g_pCharaManager->getCharaCount(); i++ )
	{
		pChara = g_pCharaManager->getChara( i );
		if( pChara == nullptr )	continue;
		I3ASSERT( pChara != nullptr );

		idx = pChara->getCharaNetIndex();
		if( idx == -1 )			continue;

		bMyDeath = false;

		if( KillInfo[ idx].m_KillerAgainCount > 0 )
		{
			data.head._nKillType	= (UINT8) SET_KILLTYPE_LOW( data.head._nKillType, KillInfo[ idx].m_KillType );
			if(pChara->IsAcid())
				data.head._nKillType = (UINT8)SET_KILLTYPE_HIGH(data.head._nKillType, CHARA_KILL_HIGH_BOMBING);
			else if (pChara->IsCCAcid() || pChara->IsAcidVulcan())
				data.head._nKillType = (UINT8)SET_KILLTYPE_HIGH(data.head._nKillType, CHARA_KILL_HIGH_BOMBING);
			data.head._nCount		= (UINT8) KillInfo[ idx].m_KillerAgainCount;
			// KillMark5
			data.head._WeaponInfo	= KillInfo[ idx].m_KillWeaponInfo;
			data.head._nUserIdx		= (UINT8) idx;
			data.head._nByObjectIdx	= (UINT8) KillInfo[ idx].m_KillObjectIdx;

			// 죽인 유저의 위치
			data.head._KillPosition[0]	= getX( pChara->GetPos());
			data.head._KillPosition[1]	= getY( pChara->GetPos());
			data.head._KillPosition[2]	= getZ( pChara->GetPos());

			I3ASSERT( data.head._KillPosition[0] > - 10000.f);
			I3ASSERT( data.head._KillPosition[0] < 10000.f);
			I3ASSERT( data.head._KillPosition[1] > - 10000.f);
			I3ASSERT( data.head._KillPosition[1] < 10000.f);
			I3ASSERT( data.head._KillPosition[2] > - 10000.f);
			I3ASSERT( data.head._KillPosition[2] < 10000.f);

			I3ASSERT( KillInfo[ idx].m_KillerAgainCount <= MAX_KILLUSER_COUNT );

			for( j = 0; j < KillInfo[ idx].m_KillerAgainCount; j++ )
			{
				if( ((INT32)GET_DEATHINFO_IDX_TYPE(KillInfo[ idx].m_KillBuffer[ j ]) == idx ))
				{// 자살한 경우만 체크
					bMyDeath = true;
				}
				data.info[ j ]._ui8Weapon			= KillInfo[ idx].m_ui8Weapon[ j ];
				data.info[ j ]._DeathInfo			= KillInfo[ idx].m_KillBuffer[ j ];
				data.info[ j ]._DeathPosition[0]	= KillInfo[ idx].m_KillPositionBuf[ j ][0];
				data.info[ j ]._DeathPosition[1]	= KillInfo[ idx].m_KillPositionBuf[ j ][1];
				data.info[ j ]._DeathPosition[2]	= KillInfo[ idx].m_KillPositionBuf[ j ][2];

				INT32 assistor = AssistContext::i()->GetAssistor((INT32)GET_DEATHINFO_IDX_TYPE(KillInfo[idx].m_KillBuffer[j]), pChara->getCharaNetIndex());

				// 어시스터 정보가 없을 경우 Killer Index를 넣어보낸다
				if (assistor == -1)
					assistor = pChara->getCharaNetIndex();

				data.info[j]._ui8AssistIdx = (UINT8)assistor;

				I3ASSERT( data.info[ j ]._DeathPosition[0] > -10000.f);
				I3ASSERT( data.info[ j ]._DeathPosition[0] < 10000.f);
				I3ASSERT( data.info[ j ]._DeathPosition[1] > -10000.f);
				I3ASSERT( data.info[ j ]._DeathPosition[1] < 10000.f);
				I3ASSERT( data.info[ j ]._DeathPosition[2] > -10000.f);
				I3ASSERT( data.info[ j ]._DeathPosition[2] < 10000.f);
			}

			if( bMyDeath && ( KillInfo[ idx].m_KillerAgainCount == 2))
			{	// MassKill이 아닙니다.
				data.head._nKillType	= SET_KILLTYPE_LOW( data.head._nKillType, CHARA_KILL_DEFAULT);
			}

			GameEventSender::i()->SetEvent( EVENT_DEATH_BATTLE, &data);
			KillInfo[ idx].m_KillerAgainCount = 0;
		}
	}
}

void UserContext::CreateBattle( void)
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		Death[i] = false;
		KillInfo[i].Reset();
	}

	fRemainingBattleTime = 0.f;

	ISTEST_S
	{
		PlayingSlotFlag = 0xFFFF;
	}
	ISTEST_E
}

void UserContext::InitRoomData( void)
{
	ISTEST_S
	{
		for( UINT32 i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			Death[i] = false;
		}
	}
	ISTEST_E
}
bool UserContext::_IsLiveCharacter(TEAM_TYPE team, INT32 idx)
{
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(idx);

	
	if( SLOT_STATE_BATTLE != pInfo->_State ) return false;
	if( !IsPlayingSlot(idx) ) return false;
	if( Death[idx] )	return false;

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);

	if( pChara != nullptr && pChara->isCharaStateMask( CHARA_STATE_DEATH))
		return false;
	else if( pChara == nullptr)
		return false;

	if( pChara->getTeam() != team) return false;

	return true;
}

INT32 UserContext::GetIndexedRankderIdx(TEAM_TYPE team, INT32 idx)
{
	i3::vector<INT32> playerIdxList;

	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (_IsLiveCharacter(team, i) == false) continue;
		playerIdxList.push_back(i);
	}

	if (playerIdxList.empty()) return -1;

	i3::sort(playerIdxList.begin(), playerIdxList.end(), RankingSortFunc());

	return playerIdxList[idx]; // 내림차순으로 정렬되어 있음.	
}

INT32 UserContext::GetTopRankerIdx(TEAM_TYPE team)
{
	i3::vector<INT32> playerIdxList;

	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (_IsLiveCharacter(team, i) == false) continue;
		playerIdxList.push_back(i);
	}

	if (playerIdxList.empty()) return -1;

	i3::sort(playerIdxList.begin(), playerIdxList.end(), RankingSortFunc());

	return playerIdxList[0]; // 내림차순으로 정렬되어 있음.
}