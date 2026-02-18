#include "pch.h"
#include "AssistContext.h"
#include "BattleSlotContext.h"

AssistContext::AssistContext()
{
	i3mem::Fill(m_Assistor, -1, sizeof(m_Assistor));
}

void AssistContext::SetAssistor(INT32 idx, INT32 assistor)
{
	I3ASSERT(idx >= 0 && idx < SLOT_MAX_COUNT)

	// 타격자, 피격자 둘 중 유저만 게임 중인지 확인
	if (BattleSlotContext::i()->IsMyTeam(idx))
	{
		// 피격자가 유저팀일 경우 피격자의 상태를 확인
		if (BattleSlotContext::i()->getSlotInfo(idx)->_State == SLOT_STATE_BATTLE && m_Assistor[idx][1] != assistor)
		{
			m_Assistor[idx][0] = m_Assistor[idx][1];
			m_Assistor[idx][1] = assistor;
		}
	}
	else
	{
		// 타격자가 유저팀일 경우 타격자의 상태를 확인
		if (BattleSlotContext::i()->getSlotInfo(assistor)->_State == SLOT_STATE_BATTLE && m_Assistor[idx][1] != assistor)
		{
			m_Assistor[idx][0] = m_Assistor[idx][1];
			m_Assistor[idx][1] = assistor;
		}
	}

}

INT32 AssistContext::GetAssistor(INT32 idx, INT32 killer)
{
	I3ASSERT( idx >= 0 && idx < SLOT_MAX_COUNT)

	INT32 nAssistor = m_Assistor[idx][1];

	// 현재 어시스터가 Killer와 같을 경우 이전 어시스터로 출력함
	if (nAssistor == killer && m_Assistor[idx][0] != -1)
		nAssistor = m_Assistor[idx][0];

	// Kill이 발생했으므로 해당 인덱스의 어시스터를 비워줌
	m_Assistor[idx][0] = m_Assistor[idx][1] = -1;

	return nAssistor;
}

void AssistContext::AllResetAssistor()
{
	for (INT32 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		for (INT32 j = 0; j < 2; j++)
		{
			m_Assistor[i][j] = -1;
		}
	}
}

void AssistContext::UpdateAssistorInfo()
{
	for (INT32 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		if (m_Assistor[i][1] == -1)
			continue;

		// 유저가 나갔을 때 나간 유저의 슬롯을 비우고 나간 유저의 인덱스를 보관한 곳을 지워줌
		if (BattleSlotContext::i()->IsMyTeam(i))
		{
			if (BattleSlotContext::i()->getSlotInfo(i)->_State != SLOT_STATE_BATTLE)
				m_Assistor[i][0] = m_Assistor[i][1] = -1;
		}
		else
		{
			if (BattleSlotContext::i()->getSlotInfo(m_Assistor[i][1])->_State != SLOT_STATE_BATTLE)
			{
				m_Assistor[i][1] = m_Assistor[i][0];
				m_Assistor[i][0] = -1;
			}
		}
	}
}