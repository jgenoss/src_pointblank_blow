#include "pch.h"
#include "CardSet.h"


I3_CLASS_INSTANCE( CCardSet);//, i3ElementBase);


CCardSet::CCardSet()
{
	m_MyIndex		= -1;
	m_ID			= QUEST_CARDSET_TYPE_NONE;
	m_ActiveIndex	= 0;

	m_CardInfo.resize(MAX_CARD_PER_CARDSET);
	for ( int i = 0 ; i < MAX_CARD_PER_CARDSET ; ++i)
		m_CardInfo[i] = CCardInfo::new_object() ;
}

CCardSet::~CCardSet()
{
	i3::cu::for_each_safe_release(m_CardInfo, &CCardInfo::Release);
}

bool CCardSet::Reset( INT32 MyIndex )
{
	m_MyIndex	= MyIndex;

	for( INT32 i = 0; i < MAX_CARD_PER_CARDSET; ++i)
	{
		// 카드 한장의 정보를 넘김
		if( getCard(i)->Reset( i) == false)
		{
			I3FATAL( "Could not ReMake CardInfo at %d\n", i);
			return false;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------
// ccardset
//--------------------------------------------------------------------------------
bool CCardSet::ChangeActiveCard( UINT32 idx)
{
	if( getCard(idx) == NULL)
	{
		return false;
	}

	if( m_ActiveIndex == idx)
		return true;
	
	I3ASSERT(idx < MAX_CARD_PER_CARDSET);

	m_ActiveIndex = (UINT8)idx;

	return true;
}

bool CCardSet::IsComplate()
{
	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		CCardInfo* cardInfo = getCard(i);

		if (NULL == cardInfo || FALSE == cardInfo->IsCompletedCard())
		{
			i3Error::Log( "최종 아이템 보상창 5 [ 임무카드순서[ %d ] ] == NULL", i );
			return false;
		}
	}

	return true;
}

void CCardSet::SetCardQuestItemInfo( INT32 CardIndex, const QUEST_ITEM_INFO_NEW & ItemInfo )
{
	CCardInfo * pInfo = getCard(CardIndex);
	pInfo->SetQuestItemData( ItemInfo );
}

CCardInfo * CCardSet::getCard( UINT32 nidx )
{
	return m_CardInfo[nidx];
}


