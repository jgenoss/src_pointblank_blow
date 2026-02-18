#pragma once

struct BATTLE_RESULT
{
	UINT16	m_BattleExp[SLOT_MAX_COUNT] = { 0 }; //all user Exp
	UINT16	m_BattlePoint[ SLOT_MAX_COUNT] = { 0 }; //all user point
	UINT8	m_BattleScore[ SLOT_MAX_COUNT] = { 0 };

	UINT8	m_pBattleResultIcon[ SLOT_MAX_COUNT] = { 0 }; //Event Flag ( PCCafe, Item, Event )
	UINT8	m_AccMissionCount[ SLOT_MAX_COUNT] = { 0 };

	UINT16	m_BonusExp[ SLOT_MAX_COUNT][ TYPE_BATTLE_RESULT_EVENT_COUNT ] = { 0 }; //Event Flag에 따른 bonus 값
	UINT16	m_BonusPoint[SLOT_MAX_COUNT][ TYPE_BATTLE_RESULT_EVENT_COUNT ] = { 0 }; //Event Flag에 따른 bonus 값

	UINT16	m_ui16QuestEndFlag = 0;

	void	Init( INT32 idx)
	{
		m_pBattleResultIcon[ idx] = 0;
		m_BattleExp[ idx] = 0;
		m_BattlePoint[ idx] = 0;
		m_BattleScore[ idx] = 0;
		m_AccMissionCount[ idx] = 0;

		i3mem::Fill(m_BonusExp[ idx], 0 , sizeof(UINT16) * TYPE_BATTLE_RESULT_EVENT_COUNT);
		i3mem::Fill(m_BonusPoint[ idx], 0 , sizeof(UINT16) * TYPE_BATTLE_RESULT_EVENT_COUNT);
		m_ui16QuestEndFlag = 0;
	}

	UINT16 getTotalBonusExp(INT32 idx) const
	{
		I3_BOUNDCHK(idx, SLOT_MAX_COUNT);
		UINT16 total_bonus_exp = 0;

		for(INT32 cnt = 0 ; cnt < TYPE_BATTLE_RESULT_EVENT_COUNT ; ++cnt)
			total_bonus_exp += m_BonusExp[idx][cnt];

		return total_bonus_exp;
	}

	UINT16 getTotalBonusPoint(INT32 idx) const
	{
		UINT16 total_bonus_point = 0;

		for(INT32 cnt = 0 ; cnt < TYPE_BATTLE_RESULT_EVENT_COUNT ; ++cnt)
			total_bonus_point += m_BonusPoint[idx][cnt];

		return total_bonus_point;
	}

};

class BattleResultContext : public i3::shared_ginst<BattleResultContext>
{
public:	
	BATTLE_RESULT&				SetInfo()		{ return m_BattleResult; }
	const BATTLE_RESULT&		GetInfo() const	{ return m_BattleResult; }

	UINT16						getBattleEndUserFlag(void) const	{ return m_BattleEndUserFlag; }
	void						setBattleEndUserFlag( UINT16 v)		{ m_BattleEndUserFlag = v; }

private:
	BATTLE_RESULT			m_BattleResult;
	UINT16					m_BattleEndUserFlag = 0;

};
