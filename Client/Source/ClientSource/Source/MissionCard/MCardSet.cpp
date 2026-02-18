#include "pch.h"
#include "MCardSet.h"
#include "MCardSetInfo.h"
#include "MCard.h"


MCardSet::MCardSet(INT32 myIndex) : m_pCardSetInfo(nullptr), m_myIndex(myIndex), m_selCardIndex(-1)
{
	for (INT32 i = 0 ; i < MAX_CARD_PER_CARDSET ; ++i)
	{
		m_apMCard[i] = new MCard(this, i);
	}
}

MCardSet::~MCardSet()
{
	i3::for_each(m_apMCard, m_apMCard+MAX_CARD_PER_CARDSET, i3::fu::delete_fun());
}

void	MCardSet::Activate(const MCardSetInfo* pCardSetInfo)
{
	m_pCardSetInfo = pCardSetInfo;
	m_selCardIndex = -1;

	if (pCardSetInfo)
	{
		for (INT32 i = 0; i < MAX_CARD_PER_CARDSET ; ++i)
		{
			MCardInfo* pCardInfo = pCardSetInfo->mCardInfo[i].get();
			m_apMCard[i]->Activate(pCardInfo);
		}
	}
	else
	{
		for (INT32 i = 0; i < MAX_CARD_PER_CARDSET ; ++i)
		{	
			m_apMCard[i]->Deactivate();
		}
	}
}

void	MCardSet::Deactivate()
{
	MCardSet::Activate(nullptr);
}

void	MCardSet::SelectCardIndex(INT32 selCardIndex) 
{ 
	if (selCardIndex < 0 || selCardIndex >= MAX_CARD_PER_CARDSET)
	{
		m_selCardIndex = -1;
		//		I3ASSERT(selCardIndex < -1 || selCardIndex >= MAX_CARD_PER_CARDSET);
	}
	else
	{
		m_selCardIndex = selCardIndex; 
	}
}

// 다음번 사용가능 카드를 못찾으면 바뀌는 것없이 그대로 자기 자신의 인덱스를 리턴...
INT32	MCardSet::SelectNextAvailableCardIndex()
{
	if (m_pCardSetInfo == nullptr )
		return -1;

	if (m_selCardIndex == -1)			// 이경우는 그냥 한바퀴만 계산하고 없으면 리턴하면 됨..
	{
		for (INT32 i = 0 ; i < MAX_CARD_PER_CARDSET ; ++i)
		{
			if ( m_apMCard[i]->IsBattleAvailableCard() )
			{
				m_selCardIndex = i;
				return i;
			}
		}
		return -1;
	}

	INT32 nextCandidate = m_selCardIndex + 1;			//
	if (nextCandidate >= MAX_CARD_PER_CARDSET)
		nextCandidate = 0;

	while ( nextCandidate != m_selCardIndex )			// 도로 자기 자신한테 온 경우이다..
	{
		if ( m_apMCard[nextCandidate]->IsBattleAvailableCard() )
		{
			m_selCardIndex = nextCandidate;
			return nextCandidate;
		}
		++nextCandidate;
		if (nextCandidate >= MAX_CARD_PER_CARDSET)
			nextCandidate = 0;
	}

	return m_selCardIndex;			
}

MCard*	MCardSet::GetSelectedMCard() const
{
	if (m_selCardIndex == -1) return nullptr;
	return m_apMCard[m_selCardIndex];
}

bool		MCardSet::IsAllCardComplete_OffBattle() const
{
	for (INT32 i = 0 ; i < MAX_CARD_PER_CARDSET ; ++i)
	{
		MCard* pCard = m_apMCard[i];

		if (pCard->IsActive() && pCard->IsAllQuestCompleted_OffBattle() == false)
			return false;
	}
	return true;
}

bool		MCardSet::IsAllCardComplete_OnBattle() const
{
	for (INT32 i = 0 ; i < MAX_CARD_PER_CARDSET ; ++i)
	{
		MCard* pCard = m_apMCard[i];

		if (pCard->IsActive() && pCard->IsAllQuestCompleted_OnBattle() == false)
			return false;
	}
	return true;
}
