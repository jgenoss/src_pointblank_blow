#pragma once

struct MCardSetInfo;
class MCard;

class MCardSet
{
public:
	MCardSet(INT32 myIndex);
	~MCardSet();
	bool				IsActive() const { return m_pCardSetInfo != nullptr; }
	void				Activate(const MCardSetInfo* pCardSetInfo);
	void				Deactivate();

	bool				IsAllCardComplete_OffBattle() const;
	bool				IsAllCardComplete_OnBattle() const;

	void				SelectCardIndex(INT32 selCardIndex);
	INT32				GetSelectCardIndex() const { return m_selCardIndex; }
	INT32				SelectNextAvailableCardIndex();

	MCard*				GetSelectedMCard() const;

	MCard*				GetMCard(INT32 idx) const { return m_apMCard[idx];  }
	const MCardSetInfo*		GetMCardSetInfo() const { return m_pCardSetInfo; }
	INT32				GetMyIndex() const { return m_myIndex; }

private:
	INT32				m_myIndex;
	INT32				m_selCardIndex;			// 10 장 중 현재 카드 한장..( 없거나 한장)
	const MCardSetInfo*	m_pCardSetInfo;				// 지워지지 않을 고정값으로 레퍼런스카운팅 필요 없다. 카드셋아이디는 내장..
	MCard*				m_apMCard[ MAX_CARD_PER_CARDSET ];
	friend class MCardMgr;
};

