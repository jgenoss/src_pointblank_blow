#pragma once

class MCard;
struct MCardQuestInfo;

class MCardQuest
{
public:
	MCardQuest(const MCard* pCard, INT32 myIndex);
	~MCardQuest();

	bool			IsActive() const { return m_pCardQuestInfo != nullptr; }
	void			Activate( const MCardQuestInfo* pCardQuestInfo);
	void			Deactivate();

	void			SetCompleteCount_OffBattle(INT32 completeCount);
	void			SetCompleteCount_OnBattle(INT32 completeCount);
	void			CancelCompletion();
	void			ConfirmCompletion();
	
	INT32			GetCurrCompleteCount_OffBattle() const { return m_currCompleteCount_OffBattle; }
	INT32			GetCurrCompleteCount_OnBattle() const { return m_currCompleteCount_OnBattle; }
	
	
	const MCardQuestInfo*		GetMCardQuestInfo() const { return m_pCardQuestInfo; }

	i3::rc_wstring	GetCurrHUDDesc(bool bLineFeed = true) const;

private:
	const MCard*			m_pParent;
	INT32					m_myIndex;
	INT32					m_currCompleteCount_OffBattle;
	INT32					m_currCompleteCount_OnBattle;
	const MCardQuestInfo*	m_pCardQuestInfo;					
};
