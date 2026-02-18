#include "pch.h"
#include "BattleSlotContext.h"
#include "StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleServerContext.h"
#include "GameCharaEquipContext.h"
#include "StageBattle/ScoreContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "UtilBattle.h"

UseItemSlotInfo UIslotInfo[] = 
{
	//		Slot ID							//SlotType							//Value
	{(CASHITEMSKILL_LIST)SLOT_ITEM_INCREASE_INVINCIBLE_TIME,	CASHITEMSKILL_INVINCIBLE_TIME,		VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME },
	{(CASHITEMSKILL_LIST)SLOT_ITEM_C4_SPEED_KIT,				CASHITEMSKILL_C4SPEED,				2	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_INCREASE_GRENADE_SLOT,		CASHITEMSKILL_GRENADE_ADD,			VALUE_CASH_ITEM_INCREASE_GRENADE_SLOT	},

	{(CASHITEMSKILL_LIST)SLOT_ITEM_QUICK_CHANGE_WEAPON,			CASHITEMSKILL_SWAPWEAPON_RATE,		0.50	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_QUICK_CHANGE_MAGAZINE,		CASHITEMSKILL_REBULLET_RATE,		0.25f	},

	{(CASHITEMSKILL_LIST)SLOT_ITEM_MEGA_HP10,					CASHITEMSKILL_HP_RATE,				0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_MEGA_HP5,					CASHITEMSKILL_HP_RATE,				0.05f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_KETUPAT,						CASHITEMSKILL_HP_RATE,				0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_VALENTINE_CHOCOLATE,			CASHITEMSKILL_HP_ADD,				(REAL32)VALUE_CASH_ITEM_VALENTINE_CHOCOLATE	},

	{(CASHITEMSKILL_LIST)SLOT_ITEM_BULLET_PROOF_VEST,			CASHITEMSKILL_DEFENSE_OVERLAP,		0.05f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_BULLET_PROOF_VEST_PLUS,		CASHITEMSKILL_DEFENSE_OVERLAP,		0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_BULLET_PROOF_VEST_METAL,		CASHITEMSKILL_DEFENSE_OVERLAP,		0.20f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_BULLET_PROOF_VEST_GM,		CASHITEMSKILL_DEFENSE_OVERLAP,		0.90f	},

	{(CASHITEMSKILL_LIST)SLOT_ITEM_SHORT_RESPAWN_100,			CASHITEMSKILL_RESPAWN,				1.f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_SHORT_RESPAWN_50,			CASHITEMSKILL_RESPAWN,				VALUE_CASH_ITEM_SHORT_RESPAWN_50	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_SHORT_RESPAWN_30,			CASHITEMSKILL_RESPAWN,				VALUE_CASH_ITEM_SHORT_RESPAWN_30	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_SHORT_RESPAWN_20,			CASHITEMSKILL_RESPAWN,				VALUE_CASH_ITEM_SHORT_RESPAWN_20	},

	{(CASHITEMSKILL_LIST)SLOT_ITEM_FULLMETALJACKETAMMO,			CASHITEMSKILL_ACCURACY,				0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_FULLMETALJACKETAMMO,			CASHITEMSKILL_DAMAGE,				0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_HOLLOW_POINT_AMMO,			CASHITEMSKILL_ACCURACY,				-0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_HOLLOW_POINT_AMMO,			CASHITEMSKILL_DAMAGE,				-0.10f	},
	
	{(CASHITEMSKILL_LIST)SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS,		CASHITEMSKILL_ACCURACY,				-0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS,		CASHITEMSKILL_DAMAGE,				0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO,	CASHITEMSKILL_ACCURACY,				-0.05f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO,	CASHITEMSKILL_MOVE_RATE,			-0.10f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO,	CASHITEMSKILL_DAMAGE,				0.05f	},

	{(CASHITEMSKILL_LIST)SLOT_ITEM_UPGRADE_REINFORCED,			CASHITEMSKILL_RESPAWN,				0.20f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_UPGRADE_REINFORCED,			CASHITEMSKILL_HP_RATE,				0.20f	},
	{(CASHITEMSKILL_LIST)SLOT_ITEM_UPGRADE_REINFORCED,			CASHITEMSKILL_MOVE_RATE,			0.20f	},

	//추가 생성해야 하는 기능
	//관통력	//반동제어 //세트능력치
};

BattleSlotContext::BattleSlotContext()
{
	InitCharaType();
	InitRoomData();	// 방정보 초기화

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_CharaInfoBuffer[i].Reset();
	}

	for(INT32 i = 0; i < MAX_COUNT_DOMI_DINO; i++ )
	{
		m_AICharaInfoBuffer[i].Reset();
	}

	m_getNetCharaInfo[ CHARA::BUFFER_USER ] = &BattleSlotContext::getNetCharaInfo_Normal;
	m_getNetCharaInfo[ CHARA::BUFFER_NEW_AI] = &BattleSlotContext::getNetCharaInfo_AI;

	m_setNetCharaInfo[ CHARA::BUFFER_USER ] = &BattleSlotContext::setNetCharaInfo_Normal;
	m_setNetCharaInfo[ CHARA::BUFFER_NEW_AI] = &BattleSlotContext::setNetCharaInfo_AI;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_CharacterGameInfo[i].Reset();
	}

	for(INT32 i = 0; i < MAX_COUNT_DOMI_DINO; i++ )
	{
		m_CharacterGameInfoForAI[i].Reset();
	}

	m_getGameInfo_Chara[ CHARA::BUFFER_USER ] = &BattleSlotContext::getGameInfo_Chara_Normal;
	m_getGameInfo_Chara[ CHARA::BUFFER_NEW_AI] = &BattleSlotContext::getGameInfo_Chara_AI;

	m_setGameInfo_Chara[ CHARA::BUFFER_USER ] = &BattleSlotContext::setGameInfo_Chara_Normal;
	m_setGameInfo_Chara[ CHARA::BUFFER_NEW_AI] = &BattleSlotContext::setGameInfo_Chara_AI;
}

void	BattleSlotContext::InitCharaType()
{
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_CharaType[ i] = CHARA::TYPE_NA;
	}

}


void	BattleSlotContext::SetAICharaInfo_P2P()
{

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
	// AI용 초기화
		m_AICharaInfoBuffer[ i].Reset();
	}

		// AI용으로 초기화를 해야합니다.
	m_AICharaInfoBuffer[ 0].setWeaponItemID( WEAPON_SLOT_PRIMARY,	DEFAULT_WEAPON_ASSAULT);
	m_AICharaInfoBuffer[ 0].setWeaponItemID( WEAPON_SLOT_SECONDARY,	DEFAULT_WEAPON_HANDGUN);
	m_AICharaInfoBuffer[ 0].setWeaponItemID( WEAPON_SLOT_MELEE,		DEFAULT_WEAPON_KNIFE);
	m_AICharaInfoBuffer[ 0].setWeaponItemID( WEAPON_SLOT_THROWING1,	DEFAULT_WEAPON_THROWING1);
	m_AICharaInfoBuffer[ 0].setWeaponItemID( WEAPON_SLOT_THROWING2,	DEFAULT_WEAPON_THROWING2);

	for(INT32 i = 1; i < SLOT_MAX_COUNT; i++)
	{
		m_AICharaInfoBuffer[ 0].CopyTo( &m_AICharaInfoBuffer[ i]);
	}

}

void BattleSlotContext::InitRoomData(void)
{
	INT32 i;

	for( i = 0; i < USER_SLOT_COUNT; i++)
	{
		m_SlotInfo[ i ].Reset();
		ISTEST_S
		{
			m_SlotInfo[ i]._State = SLOT_STATE_BATTLE;
		}
		ISTEST_E
	}

	m_MainSlotIdx = 0;
	m_MySlotIdx = 0;

	m_killerIdxMostRecent = -1;
}

void	BattleSlotContext::DumpGameInfo(void)
{
	
	I3PRINTLOG(I3LOG_NOTICE,"======================================");
	char strDumpStr[512];
	strDumpStr[0] = 0;
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		I3PRINTLOG(I3LOG_NOTICE,strDumpStr);
		if( BattleSlotContext::getSlotInfo( i)->_State > SLOT_STATE_EMPTY)
		{
			i3::snprintf(strDumpStr, _countof(strDumpStr), "Number %d  State %d Nickname %s ", i, BattleSlotContext::getSlotInfo( i)->_State, getNickForSlot( i));
			for(INT32 j = 0; j < WEAPON_SLOT_COUNT; j++)
			{
				I3PRINTLOG(I3LOG_NOTICE,"Weapon slotNum %d = %d",j, BattleSlotContext::getNetCharaInfo( i)->getWeaponList( (WEAPON_SLOT_TYPE)j));
			}
		}
		else
		{
			i3::snprintf(strDumpStr, _countof(strDumpStr), "Number %d  State %d", i, BattleSlotContext::getSlotInfo( i)->_State);

		}
		I3PRINTLOG(I3LOG_NOTICE,"----------------------------------------");
	}
	I3PRINTLOG(I3LOG_NOTICE,"======================================");
}

bool BattleSlotContext::IsObservingStateMe() const			
{			
	return IsObservingState(getMySlotIdx());		
}			
			
bool BattleSlotContext::IsObservingState(INT32 slotIdx) const			
{			
	bool playing = UserContext::i()->IsPlayingSlot(slotIdx);		
			
	return !UserContext::i()->Death[slotIdx] && playing ? true : false;		
}			
		
bool BattleSlotContext::IsObservingTargetSlot(INT32 slotIdx) const			
{			
	CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);		
			
	if(chara && chara->isCharaStateMask(CHARA_STATE_OBSERVER))		
		return true;	
			
	return false;		
}	

//Chara Nick( User, AI)
i3::rc_wstring	BattleSlotContext::getNickForSlot(INT32 idx, CHARA::BUFFER_TYPE eType ) const
{ 
	if( eType == CHARA::BUFFER_USER)
	{
		I3_BOUNDCHK_RETURN( idx, USER_SLOT_COUNT, i3::rc_wstring());
		return m_wstrNickForSlot[idx];
	}
	if( eType == CHARA::BUFFER_NEW_AI)
	{
		I3_BOUNDCHK_RETURN( idx, MAX_COUNT_DOMI_DINO, i3::rc_wstring());
		return m_wstrAICharaNickForSlot[idx];
	}

	return i3::rc_wstring();
}
void		BattleSlotContext::setNickForSlot(INT32 idx, CHARA::BUFFER_TYPE eType, const i3::rc_wstring& wstrNickName)
{
	if( eType == CHARA::BUFFER_USER)
		BattleSlotContext::setNickForSlot(idx, wstrNickName);
	else
	if (eType == CHARA::BUFFER_NEW_AI)
		BattleSlotContext::setNickForSlot_BufferNewAI(idx, wstrNickName);
}

void		BattleSlotContext::setNickForSlot_BufferNewAI(INT32 idx, const i3::rc_wstring& wstrNickName)
{
	m_wstrAICharaNickForSlot[idx] = wstrNickName;
}

void			BattleSlotContext::setNickForSlot(INT32 idx, const char* szNickName)
{
	I3_BOUNDCHK( idx, USER_SLOT_COUNT);
	i3::mb_to_utf16( szNickName, m_wstrNickForSlot[idx]);
}

void			BattleSlotContext::setNickForSlot(INT32 idx, const i3::rc_wstring& wstrNickName)
{
	m_wstrNickForSlot[idx] = wstrNickName;
}

void		BattleSlotContext::setNickColorForSlot(INT32 idx, UINT8 ui8Color)
{
	I3_BOUNDCHK( idx, USER_SLOT_COUNT);
	m_ui8NickColor[idx] = ui8Color;
}

void		BattleSlotContext::setFireMuzzleColorForSlot(INT32 idx, UINT16 ui16Color)
{
	I3_BOUNDCHK( idx, USER_SLOT_COUNT);
	m_ui16MuzzleFlashColor[idx] = ui16Color;
}

INT32		BattleSlotContext::GetUserIndex(const i3::wliteral_range& wNickNameRng)
{
	for (INT32 idx = 0; idx < 16; idx++)
	{
		if(i3::generic_is_equal(getNickForSlot(idx), wNickNameRng))
			return idx;
	}

	return -1;
}

UINT8		BattleSlotContext::getNickColorForSlot(INT32 idx) const						
{ 
	BOUNDCHK( idx, USER_SLOT_COUNT);

	if(	MyRoomInfoContext::i()->getStage()->IsAiMode() && 
		BattleServerContext::i()->IsP2PActivate()
#if defined( DEF_OBSERVER_MODE)
		&& !BattleSlotContext::i()->isObserver( idx)
#endif
		)
	{// 챌린지모드 이고
		if( (m_MySlotIdx % 2) == (idx % 2) )
		{	// 유저
			return m_ui8NickColor[idx];
		}
		else
		{	// AI 
			return 0;
		}
	}
	else
	{	// 일반 모드
		return m_ui8NickColor[idx];
	}
}

UINT16 BattleSlotContext::getFireMuzzleColorForSlot(INT32 idx) const
{
	I3_BOUNDCHK(idx, USER_SLOT_COUNT);
	return m_ui16MuzzleFlashColor[idx];
}

INT32 BattleSlotContext::GetBattleUserCount() const
{
	INT32 i = 0;
	INT32 iCnt = 0;

	for(i=0; i<SLOT_MAX_COUNT; i++) {
		if(m_SlotInfo[i]._State ==SLOT_STATE_BATTLE && UserContext::i()->IsPlayingSlot(i)) {
			iCnt++;
		}
	}

	return iCnt;
}

INT32 BattleSlotContext::GetBattleRedTeamCount() const
{
	INT32 i = 0;
	INT32 iCnt = 0;

	for(i=0; i<SLOT_MAX_COUNT; i++) {
	const GLOBAL_CHARA_INFO* charaInfo = g_pCharaManager->getCharaInfo(i);
		if(m_SlotInfo[i]._State ==SLOT_STATE_BATTLE 
			&& UserContext::i()->IsPlayingSlot(i)
			&& IsRedTeam(i)
			&& charaInfo->GetHP() > 0) 
		{
			iCnt++;
		}
	}

	return iCnt;
}


CHARACTER_TEAM_TYPE BattleSlotContext::getTeam(INT32 slotIdx)
{
	if(slotIdx == -1)
		return CHARACTER_TEAM_NONE;

	I3_BOUNDCHK(slotIdx, SLOT_MAX_COUNT);
	const GLOBAL_CHARA_INFO* charaInfo = g_pCharaManager->getCharaInfo(slotIdx);
	CHARACTER_TEAM_TYPE teamType = charaInfo->GetTeam();

	// slotIdx가 SLOT_BATTLE이 아닌슬롯들은 팀이없는걸로 나온다.
	if(teamType == CHARACTER_TEAM_NONE)
		return CHARACTER_TEAM_NONE;

#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
	if(charaInfo->GetNetIdx() == -1)
		return teamType; // 빈슬롯입니다. 팀값이 의미없습니다.

	bool autoTeamChangeExecuted = ScoreContext::i()->IsAutoTeamChangeExecuted();
	if(autoTeamChangeExecuted) // 자동공수교대가 실행되었나
	{
		if( (slotIdx % 2) )
		{
			// blue
			I3ASSERT( (teamType & CHARACTER_TEAM_RED) );
		}
		else
		{
			// red
			I3ASSERT( (teamType & CHARACTER_TEAM_BLUE) );
		}
	}
#endif

	return teamType;
}

CHARACTER_TEAM_TYPE BattleSlotContext::getMyTeam( void) const
{
#if defined( DEF_OBSERVER_MODE)
	if( isObserverMe())
		return true;
#endif

	return getTeam(getMySlotIdx());
}

CHARACTER_TEAM_TYPE BattleSlotContext::getEnemyTeamType() const
{
	return (getMyTeam() == CHARACTER_TEAM_RED) ? CHARACTER_TEAM_BLUE : CHARACTER_TEAM_RED;
}

bool BattleSlotContext::IsMyTeam(INT32 idx) const
{
	return IsRedTeam() == IsRedTeam(idx);
}

bool BattleSlotContext::IsRedTeam() const
{

	return IsRedTeam(getMySlotIdx());
}

bool BattleSlotContext::IsRedTeam(INT32 idx) const
{
	bool bteam = true;

#if defined( DEF_OBSERVER_MODE)
	if( isObserverMe())
		return true;
#endif

	I3_BOUNDCHK(idx, SLOT_MAX_COUNT);

#if defined(_DEBUG)
	{
		if(getSlotInfo(idx)->_State == SLOT_STATE_BATTLE)
		{
			I3ASSERT(getTeam(idx) != CHARACTER_TEAM_NONE);
		}
	}
#endif

	if(getTeam(idx) & CHARACTER_TEAM_RED) // getTeam(idx)은 공수교대 여부가 적용된 결과값을 줍니다.
		bteam = true;	//	레드 팀
	else
		bteam = false;	//	블루 팀

	return bteam;
}

bool BattleSlotContext::CheckAnyoneAlive(const CHARACTER_TEAM_TYPE teamType) const
{
	return (FindAnyoneAlive(teamType) != -1) ? true : false;
}

INT32 BattleSlotContext::CheckTotalNumOfPlayers(const CHARACTER_TEAM_TYPE teamType) const
{
	INT32 num = 0;

	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (getTeam(i) != teamType) continue;
		if (!UserContext::i()->IsPlayingSlot(i)) continue;
		if (IsGMObserverForClient(i)) continue;

		++num;
	}

	return num;
}

INT32 BattleSlotContext::CheckTotalNumOfPlayers(const CHARACTER_TEAM_TYPE teamType, const INT32 begin, const INT32 end) const
{
	INT32 num = 0;

	for (INT32 i=begin; i<end; i++)
	{
		if (getTeam(i) != teamType) continue;
		if (!UserContext::i()->IsPlayingSlot(i)) continue;
		if (IsGMObserverForClient(i)) continue;

		++num;
	}

	return num;
}

INT32 BattleSlotContext::CheckTotalNumOfAlive(const CHARACTER_TEAM_TYPE teamType) const
{
	INT32 num = 0;

	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (getTeam(i) != teamType) continue;
		if (!ub::is_live_character(i)) continue;

		++num;
	}

	return num;
}

INT32 BattleSlotContext::CheckTotalNumOfAlive(const CHARACTER_TEAM_TYPE teamType, const INT32 begin, const INT32 end) const
{
	INT32 num = 0;

	for (INT32 i=begin; i<end; i++)
	{
		if (getTeam(i) != teamType) continue;
		if (!ub::is_live_character(i)) continue;

		++num;
	}

	return num;
}

INT32 BattleSlotContext::CheckPlayerOrder(const INT32 idx) const
{
	return CheckTotalNumOfPlayers(getTeam(idx), 0, idx+1);
}

INT32 BattleSlotContext::FindAnyoneAlive() const
{
	INT32 idx = FindAnyoneAlive(getMyTeam());	// 로컬캐릭터 팀을 우선 순위로 검색.
	if (idx == -1)
		idx = FindAnyoneAlive(getEnemyTeamType());

	return idx;
}

INT32 BattleSlotContext::FindAnyoneAlive(const CHARACTER_TEAM_TYPE teamType) const
{
	for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if (getTeam(i) == teamType)
		{
			if (ub::is_live_character(i))
				return i;
		}
	}

	return -1;
}

INT32 BattleSlotContext::FindNextPlayerAlive(const INT32 currIdx) const
{
	INT32 idx = currIdx+1;

	for (; idx<SLOT_MAX_COUNT; idx++)
	{
		if (ub::is_live_character(idx))
			return idx;
	}

	for (idx=0; idx<=currIdx; idx++)
	{
		if (ub::is_live_character(idx))
			return idx;
	}

	return -1;
}

INT32 BattleSlotContext::FindPrevPlayerAlive(const INT32 currIdx) const
{
	INT32 idx = currIdx-1;

	for (; idx>=0; idx--)
	{
		if (ub::is_live_character(idx))
			return idx;
	}

	for (idx=SLOT_MAX_COUNT-1; idx>=currIdx; idx--)
	{
		if (ub::is_live_character(idx))
			return idx;
	}

	return -1;
}

INT32 BattleSlotContext::FindNextPlayerAlive(const CHARACTER_TEAM_TYPE teamType, const INT32 currIdx) const
{
	INT32 idx = currIdx+1;

	for (; idx<SLOT_MAX_COUNT; idx++)
	{
		if (getTeam(idx) != teamType) continue;

		if (ub::is_live_character(idx))
			return idx;
	}

	for (idx=0; idx<=currIdx; idx++)
	{
		if (getTeam(idx) != teamType) continue;

		if (ub::is_live_character(idx))
			return idx;
	}

	return -1;
}

INT32 BattleSlotContext::FindPrevPlayerAlive(const CHARACTER_TEAM_TYPE teamType, const INT32 currIdx) const
{
	INT32 idx = currIdx-1;

	for (; idx>=0; idx--)
	{
		if (getTeam(idx) != teamType) continue;

		if (ub::is_live_character(idx))
			return idx;
	}

	for (idx=SLOT_MAX_COUNT-1; idx>=currIdx; idx--)
	{
		if (getTeam(idx) != teamType) continue;

		if (ub::is_live_character(idx))
			return idx;
	}

	return -1;
}

void	BattleSlotContext::SetKillerIdxMostRecent(const INT32 idx)
{
	m_killerIdxMostRecent = idx;
}

INT32	BattleSlotContext::GetKillerIdxMostRecent() const
{
	return m_killerIdxMostRecent;
}

bool	BattleSlotContext::IsLocalPlayerDead() const
{
	return ub::is_live_character(getMySlotIdx()) ? false : true;
}

bool	BattleSlotContext::isUseCashItemInSlot( INT32 idxSlot, UINT32 useSlotItemFlag) const
{
	return (getSlotInfo( idxSlot)->_Item & useSlotItemFlag) != 0;
}

REAL32	BattleSlotContext::GetUseCashItemValue( INT32 idxSlot, CASHITEMSKILL_TYPE itemtype) const
{
	I3_BOUNDCHK_RETURN(idxSlot, USER_SLOT_COUNT, 0.f);
	return m_UseCashItemInfo[idxSlot][itemtype];
}

void	BattleSlotContext::SetUseCashItem( INT32 idxSlot)
{
	INT32 SlotCount = GET_ARRAY_COUNT(UIslotInfo);
	i3mem::FillZero(m_UseCashItemInfo[idxSlot],			sizeof(m_UseCashItemInfo[idxSlot]));

	for ( INT32 i = 0 ; i < SlotCount ; ++i )
	{
		if( isUseCashItemInSlot( idxSlot, UIslotInfo[i].SlotID ))
		{
			switch(UIslotInfo[i].SlotType)
			{
				case CASHITEMSKILL_DEFENSE_OVERLAP:
				case CASHITEMSKILL_RESPAWN:
				{
					m_UseCashItemInfo[idxSlot][UIslotInfo[i].SlotType] = max(m_UseCashItemInfo[idxSlot][UIslotInfo[i].SlotType], UIslotInfo[i].iValue);
				}
				break;
				default:
				{
					m_UseCashItemInfo[idxSlot][UIslotInfo[i].SlotType] += UIslotInfo[i].iValue;
				}
				break;
			}
		}
	}

	//중복사용체크 제외
	if( ( isUseCashItemInSlot(idxSlot, SLOT_ITEM_MEGA_HP10)) && ( isUseCashItemInSlot(idxSlot, SLOT_ITEM_MEGA_HP5)))
	{
		m_UseCashItemInfo[idxSlot][CASHITEMSKILL_HP_RATE] -= 0.05f;
	}

	if( isUseCashItemInSlot(idxSlot, SLOT_ITEM_KETUPAT) && ( isUseCashItemInSlot(idxSlot, SLOT_ITEM_VALENTINE_CHOCOLATE)))
	{
		m_UseCashItemInfo[idxSlot][CASHITEMSKILL_HP_ADD] -= VALUE_CASH_ITEM_VALENTINE_CHOCOLATE;
	}

	// 보유탄수 증가권이 있는지를 확인하여 보유탄수 40% 증가 (자신 사용)
	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_AMMO_UP) == true )
	{
		m_UseCashItemInfo[idxSlot][CASHITEMSKILL_BULLETADD] = WEAPON_BULLET_INCREASING_COUNT_40;
	}


	for(INT32 i = 0; i < USER_SLOT_COUNT; i++)
	{
		// 예외로 Eps값이 필요없는 것들을 따로 빼기 전 임시로 무적시간은 eps값을 주지 않습니다.
		if(i == CASHITEMSKILL_INVINCIBLE_TIME) continue;

		m_UseCashItemInfo[idxSlot][i] += (m_UseCashItemInfo[idxSlot][i] > 0 ? I3_EPS : -I3_EPS);
	}
	

}

REAL32	BattleSlotContext::getMissionBomb_SetTime_UsingSkill(INT32 idxSlot) const
{
	const NET_CHARA_INFO * pChara = const_cast<BattleSlotContext*>(this)->BattleSlotContext::getNetCharaInfo(idxSlot);
	REAL32 fVal = 0.0f;
	if(pChara)
	{
		//fVal = pChara->getBattleApply()->getSkillValue(WEAPON_MODE_BOMB_INSTALL);
		//fVal+= pChara->getBattleApply()->getSkillValue(WEAPON_MODE_BOMB_INSTALL2);
	}
	return fVal;
}

REAL32	BattleSlotContext::getMissionBomb_DisarmTime_UsingSkill(INT32 idxSlot) const
{	//	C4 SpeedKit 캐시템 사용시 해체 시간 감소
	const NET_CHARA_INFO * pChara = const_cast<BattleSlotContext*>(this)->BattleSlotContext::getNetCharaInfo(idxSlot);
	REAL32 fVal = 0.0f;
	if(pChara)
	{
		//fVal = pChara->getBattleApply()->getSkillValue(WEAPON_MODE_BOMB_UNINSTALL);
		//fVal+= pChara->getBattleApply()->getSkillValue(WEAPON_MODE_BOMB_UNINSTALL2);
	}
	return fVal;
}


REAL32	BattleSlotContext::getMissionBomb_SetTime_UsingSpeedKit( INT32 idxSlot) const
{
	REAL32 time = WEAPON_BOMB_SETTIME;

	if (roomOption.m_ui8Mode == STAGE_MODE_BOMB)
	{
		time = roomOption.m_r16BombInstallDelayTime;
	}
	
	if( WEAPON_BOMB_SETTIME > time )
	{
		//해당 상수값이 기본 타임입니다. 기획 요청으로 기본 타임이 아닐경우 밑의 기능은 무시 되어야 합니다.
		//추후에 플래그가 왔으면 좋겠으나, 지금은 이렇게 처리합니다.
		return time;
	}

	//	C4 SpeedKit 캐시템 사용시 설치 시간 감소
	if (getSlotInfo( idxSlot)->_Item & SLOT_ITEM_C4_SPEED_KIT)
	{
		time += (REAL32) VALUE_CASH_ITEM_C4SPEEDKIT_INSTALL;
	}

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idxSlot);
	if( pChara != nullptr && pChara->getEquipContext() != nullptr)
	{
		time -= pChara->getEquipContext()->GetAbility( EQUIP::eABILITY_C4SPEEDKIT);
	}

	return time;
}

REAL32	BattleSlotContext::getMissionBomb_DisarmTime_UsingSpeedKit(INT32 idxSlot) const
{
	REAL32 time = WEAPON_BOMB_DISARMTIME;

	if (roomOption.m_ui8Mode == STAGE_MODE_BOMB)
	{	
		time = roomOption.m_r16BombUnInstallDelayTime;
	}
	
	if( WEAPON_BOMB_DISARMTIME > time )
	{
		//해당 상수값이 기본 타임입니다. 기획 요청으로 기본 타임이 아닐경우 밑의 기능은 무시 되어야 합니다.
		//추후에 플래그가 왔으면 좋겠으나, 지금은 이렇게 처리합니다.
		return time;
	}

	//	C4 SpeedKit 캐시템 사용시 해체 시간 감소
	if (getSlotInfo( idxSlot)->_Item & SLOT_ITEM_C4_SPEED_KIT)
	{
		time += (REAL32) VALUE_CASH_ITEM_C4SPPEDKIT_DISARM;
	}

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idxSlot);
	if( pChara != nullptr && pChara->getEquipContext() != nullptr)
	{
		time -= pChara->getEquipContext()->GetAbility( EQUIP::eABILITY_C4SPEEDKIT);
	}

	return time;
}

void BattleSlotContext::CreateBattle( void)
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		setGameInfo_Chara( i )->Reset();
		SetUseCashItem(i);
	}

	InitCharaType();
	SetAICharaInfo_P2P(); 
}

		

