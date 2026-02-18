#pragma once
#include "Mode/Domination/Domination_Defines.h"

class DomiContext : public i3::shared_ginst<DomiContext>
{
public:	
	void		setDomiWaveCount(UINT32 count)		{ m_domiWaveCount = count;	}
	UINT32		getDomiWaveCount() const			{ return m_domiWaveCount;	}

	DomiDefs::FLOW& SetFlow()								{ return m_flow; }
	const DomiDefs::FLOW& GetFlow() const					{ return m_flow; }

	DomiDefs::INTERMISSION& SetIntermission()				{ return m_intermission; }
	const DomiDefs::INTERMISSION& GetIntermission()	const	{ return m_intermission; }

	DomiDefs::REVIVAL& SetRevival()							{ return m_revival; }
	const DomiDefs::REVIVAL& GetRevival() const				{ return m_revival; }

	i3::vector<DomiDefs::MUTANTREX_HP*>& SetMutantHpList()					{ return m_mutantHpList; }
	const i3::vector<DomiDefs::MUTANTREX_HP*>& GetMutantHpList() const		{ return m_mutantHpList; }

	DomiDefs::MUTANTREX_HOWL& SetMutantHowl()				{ return m_mutantHowl; }
	const DomiDefs::MUTANTREX_HOWL& GetMutantHowl() const	{ return m_mutantHowl; }

	DomiDefs::AWARD& SetAward()								{ return m_award; }
	const DomiDefs::AWARD& GetAward() const					{ return m_award; }

	DomiDefs::ACIDITEM_APPEARENCE& SetAcidItemAppearence()					{ return m_aciditemAppearence; }
	const DomiDefs::ACIDITEM_APPEARENCE& GetAcidItemAppearence() const		{ return m_aciditemAppearence; }

	DomiDefs::GRADE SetGrade()								{ return m_grade; }
	const DomiDefs::GRADE GetGrade() const					{ return m_grade; }

	DomiDefs::WAVE_RESULT_INFO& SetWaveResult(INT32 idx)				{ I3ASSERT(0 <= idx && idx < MAX_COUNT_DOMI_USER_REAL); return m_waveResults[idx]; }
	const DomiDefs::WAVE_RESULT_INFO& GetWaveResult(INT32 idx) const	{ I3ASSERT(0 <= idx && idx < MAX_COUNT_DOMI_USER_REAL); return m_waveResults[idx]; }

	DomiDefs::ROUND_RESULT_INFO& SetRoundResult(INT32 idx)				{ I3ASSERT(0 <= idx && idx < MAX_COUNT_DOMI_USER_REAL); return m_roundResults[idx]; }
	const DomiDefs::ROUND_RESULT_INFO& GetRoundResult(INT32 idx) const	{ I3ASSERT(0 <= idx && idx < MAX_COUNT_DOMI_USER_REAL); return m_roundResults[idx]; }

private:
	UINT32			m_domiWaveCount = 0;

	DomiDefs::FLOW m_flow;
	DomiDefs::INTERMISSION m_intermission;
	DomiDefs::REVIVAL m_revival;
	i3::vector<DomiDefs::MUTANTREX_HP*> m_mutantHpList;
	DomiDefs::MUTANTREX_HOWL m_mutantHowl;
	DomiDefs::AWARD m_award;
	DomiDefs::ACIDITEM_APPEARENCE m_aciditemAppearence;
	DomiDefs::GRADE m_grade;

	DomiDefs::WAVE_RESULT_INFO m_waveResults[MAX_COUNT_DOMI_USER_REAL];
	DomiDefs::ROUND_RESULT_INFO m_roundResults[MAX_COUNT_DOMI_USER_REAL];
};