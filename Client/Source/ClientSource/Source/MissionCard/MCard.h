#pragma once

class MCardSet;
class MCardQuest;
struct MCardInfo;

class MCard
{
public:
	MCard(const MCardSet* pSet, INT32 myIndex);
	~MCard();
	bool			IsActive() const { return m_pCardInfo != nullptr; }
	void			Activate(const MCardInfo* pCardInfo);
	void			Deactivate();

	void			SetQuestCompletion_OffBattle(INT32 questIdx, bool bComplete);
	void			SetQuestCompletion_OnBattle(INT32 questIdx, bool bComplete);

	MCardQuest*		GetMCardQuest(INT32 idx) const { return m_apMCardQuest[idx]; }

	const MCardInfo*	GetMCardInfo() const { return m_pCardInfo; }

	bool			IsAllQuestCompleted_OffBattle() const;
	bool			IsQuestCompleted_OffBattle(INT32 questIdx) const;	// 비활성화된 것은 완료로 간주..
	bool			IsAllQuestCompleted_OnBattle() const;
	bool			IsQuestCompleted_OnBattle(INT32 questIdx) const;	// 비활성화된 것은 완료로 간주..

	bool			IsDifferentComplete_OnOffBattle() const;

	bool			IsBattleAvailableCard() const;		// 사용 가능성 여부를 물음(대부분재사용없음. 튜터리얼은 재사용가능)
	bool			IsSelectEnableCard() const;			// UI상에서 선택 가능한지를 따짐..(사용가능과 조건이 다름)

	void			CancelCompletion();
	void			ConfirmCompletion();
	INT32			GetMyIndex() const { return m_myIndex; }

private:
	const MCardSet*				m_pParent;
	INT32						m_myIndex;
	UINT8						m_abyQuestComplete_OffBattle[MAX_QUEST_PER_CARD_NEW];
	UINT8						m_abyQuestComplete_OnBattle[MAX_QUEST_PER_CARD_NEW];
	const MCardInfo*			m_pCardInfo;
	MCardQuest*					m_apMCardQuest[MAX_QUEST_PER_CARD_NEW];
};
