#include "pch.h"
#include "QuestTextDictionary.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/strtok_fast.h"
#include "i3Base/string/ext/atoi.h"

namespace 
{
	void	InitCardSetTextArray(SCardSetText cardSetText[])
	{
		for (INT32 i = 0 ; i < QUEST_CARDSET_TYPE_COUNT ; ++i)
		{
			SCardSetText* s = cardSetText + i;

			::memset(s->_title, 0, sizeof(char)* MAX_STRING_COUNT);
			::memset(s->_name, 0, sizeof(char)* MAX_STRING_COUNT);
			::memset(s->_character, 0, sizeof(char)* MAX_STRING_COUNT);
			::memset(s->_FileName, 0, sizeof(char)* MAX_STRING_COUNT);
			s->_totalExp = 0;
			s->_totalPoint = 0;
			s->_bonusExp = 0;
			s->_bonusPoint = 0;
			s->_bonusBadgeMaster = 0;		// 최종 지급 마스터 훈장 개수
			::memset(s->_bonusItemID, 0, sizeof(UINT32) * MAX_QUEST_REWARD_ITEM_COUNT);	// 최종 지급 아이템
			s->_bonusItemCount = 0;
		}
	}

}


I3_CLASS_INSTANCE(CQuestTextDictionary);//, i3ElementBase);


CQuestTextDictionary* g_pQuestTextDictionary = NULL;

CQuestTextDictionary::CQuestTextDictionary()
{
	InitCardSetTextArray(m_cardSetText);
	i3mem::FillZero(m_ribbonText, sizeof(m_ribbonText));
	i3mem::FillZero(m_ensignText, sizeof(m_ensignText));
	i3mem::FillZero(m_medalText, sizeof(m_medalText));
	i3mem::FillZero(m_cardPrizeItem, sizeof(m_cardPrizeItem));

	i3mem::FillZero(m_EventCardPerido, sizeof(m_EventCardPerido));
}

CQuestTextDictionary::~CQuestTextDictionary()
{
	// Do nothing
}

// i3Registry::Init() 후에 호출되어야합니다.
void CQuestTextDictionary::Create(void)
{	
	// 퀘스트 문자열 정보 확인
	i3RegKey* questKey = Config::_GetRegRoot("Quest/Quest");
	I3ASSERT(questKey);

	if (questKey == NULL)
	{
		I3TRACE("[ERROR] Not found RegKey 'Quest' in BC.pef");
        return;
	}


	// 카드셋 문자열 정보 확인
	i3RegKey* cardSetKey = Config::FindKey("CardSet", questKey);

	if (cardSetKey)
	{		
		for(INT32 i = 0; i < cardSetKey->getChildCount(); i++)
		{
			i3RegKey* childKey = (i3RegKey*)cardSetKey->getChild(i);

			INT32 cardSetId = -1;

			GetKeyData(childKey, "CardSetId", &cardSetId);

			if( cardSetId > 0 )
			{
				I3ASSERT(cardSetId < MAX_QUEST_ID_COUNT);

				GetKeyData(childKey, "CardSetTitle", m_cardSetText[cardSetId]._title, MAX_STRING_COUNT );
				GetKeyData(childKey, "CardSetName", m_cardSetText[cardSetId]._name, MAX_STRING_COUNT );
				GetKeyData(childKey, "FileName", m_cardSetText[cardSetId]._FileName, MAX_STRING_COUNT );
				GetKeyData(childKey, "TotalExp", &m_cardSetText[cardSetId]._totalExp);
				GetKeyData(childKey, "TotalPoint", &m_cardSetText[cardSetId]._totalPoint);
				GetKeyData(childKey, "BonusExp", &m_cardSetText[cardSetId]._bonusExp);
				GetKeyData(childKey, "BonusPoint", &m_cardSetText[cardSetId]._bonusPoint);
				GetKeyData(childKey, "BonusBadgeMaster", &m_cardSetText[cardSetId]._bonusBadgeMaster);

				// index 0
				INT32 temp = 0;
				m_cardSetText[cardSetId]._bonusItemCount = 0;
				GetKeyData(childKey, "BonusItemID", &temp);
				m_cardSetText[cardSetId]._bonusItemID[0] = static_cast<UINT32>(temp);

				std::string tmp;
				GetKeyDataString(childKey, "BonusItemID_Time",tmp );
				//m_cardSetText[cardSetId]._vbonusItemTime.push_back(tmp);

				if(temp != 0)
					m_cardSetText[cardSetId]._bonusItemCount = 1;

				for ( int i = 1 ; i < MAX_QUEST_REWARD_ITEM_COUNT ; ++i )
				{
					char str[256] = "";
					temp = 0;
					i3::snprintf( str, 256, "BonusItemID_%d", i+1);
					GetKeyData(childKey, str, &temp);

					i3::snprintf( str, 256, "BonusItemID_Time%d", i+1);
					GetKeyDataString(childKey, str, tmp );
					//m_cardSetText[cardSetId]._vbonusItemTime.push_back(tmp);
					m_cardSetText[cardSetId]._bonusItemID[i] = static_cast<UINT32>(temp);
					if(temp != 0)
						m_cardSetText[cardSetId]._bonusItemCount++;
				}

				for ( int i = 0 ; i < MAX_CARD_PER_CARDSET ; ++i )
				{
					char str[256];
					StringCchPrintf(str, 256, "card_prize_item_%d", i+1);
					INT32 item=0;
					GetKeyData(childKey, str, &item);
					m_cardPrizeItem[cardSetId][i] = static_cast<UINT32>(item);
				}

				//이벤트 임무카드 기간 표시
				if( cardSetId == QUEST_CARDSET_TYPE_EVENT )
				{
					GetKeyData(childKey, "CardSetPeriod", m_EventCardPerido, MAX_STRING_COUNT);
				}
			}
		}
	}


	// 보상 문자열 정보 확인
	i3RegKey* prizeKey = Config::FindKey("Prize", questKey);

	if (prizeKey)
	{
		// 약장, 휘장, 훈장 RegKey 테이블
		i3RegKey* prizeTypeKey[3];

		prizeTypeKey[0] = Config::FindKey("Ribbon", prizeKey);
		prizeTypeKey[1] = Config::FindKey("Ensign", prizeKey);
		prizeTypeKey[2] = Config::FindKey("Medal", prizeKey);


		// 약장, 휘장, 훈장 문자열 테이블
		SPrizeText* prizeText[3];

		prizeText[0] = m_ribbonText;
		prizeText[1] = m_ensignText;
		prizeText[2] = m_medalText;


		// 약장, 휘장, 훈장 문자열 정보 확인
		for(INT32 type = 0; type < 3; type++)
		{
			if (prizeTypeKey[type])
			{
				for(INT32 i = 0; i < prizeTypeKey[type]->getChildCount(); i++)
				{
					i3RegKey* childKey = (i3RegKey*)prizeTypeKey[type]->getChild(i);

					INT32 prizeId;
					if (childKey->GetData("PrizeId", &prizeId))
					{
						I3ASSERT(0 < prizeId && prizeId < MAX_QUEST_ID_COUNT);

						childKey->GetData( "PrizeName", prizeText[type][prizeId]._name, MAX_STRING_COUNT );
						childKey->GetData( "PrizeDescription", prizeText[type][prizeId]._description, MAX_STRING_COUNT );
						childKey->GetData( "PrizeGainWay", prizeText[type][prizeId]._gainWay, MAX_STRING_COUNT );
					}
				}
			}
		}
	}
}

void CQuestTextDictionary::GetCardSetTitleString(char* string, const size_t sizeStrMax, UINT8 cardSetId) const
{
	I3ASSERT(string);
	i3::safe_string_copy( string, m_cardSetText[cardSetId]._title, sizeStrMax );
}

void CQuestTextDictionary::GetCardSetNameString(char* string, const size_t sizeStrMax, UINT8 cardSetId) const
{
	I3ASSERT(string);
	i3::safe_string_copy( string, m_cardSetText[cardSetId]._name, sizeStrMax );
}


const char *	CQuestTextDictionary::GetQuestFileName(ENUM_QUEST_CARDSET_TYPE CardType) const
{
	return m_cardSetText[CardType]._FileName;
}

// CardSetName과 구성 Chracter을 조합하여 카드 이름을 만든다.
//void CQuestTextDictionary::GetCardNameString(char* string,UINT8 cardSetId,UINT8 cardIdx) const
//{
//	//////////////////////////////////////////////////////////////////////////
//	//
//	// m_cardSetText[cardSetId]._character에 글자가 11개 이상 들어가면 문제가 됩니다.
//	// 이 함수는 카드 낱개 구매시에만 사용되므로 낱개 구매를 지원하지 않는다면 11개 이상이어도 문제가 안됩니다.
//	//
//	//////////////////////////////////////////////////////////////////////////
//	I3ASSERT(string);
//	I3ASSERT(cardIdx < MAX_CARD_PER_CARDSET + 1);
//
//	
//	// 같은 문자가 몇개나 되는지 확인한다.
//	char cardChar = m_cardSetText[cardSetId]._character[cardIdx];
//	
//	BOOL beSameChar = FALSE;
//
//	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
//	{
//		if (i == cardIdx)
//		{
//			continue;	// 문자 자신의 idx는 계산하지 않는다.
//		}
//
//		if (cardChar == m_cardSetText[cardSetId]._character[i])
//		{
//			beSameChar = TRUE;
//			break;
//		}
//	}
//
//	// 같은 문자가 있다.
//	if (beSameChar)
//	{
//		// 자신의 순서를 나타내는 숫자를 추가한다.
//		INT32 sameCharCount = 1;
//		for(INT32 i = 0; i < cardIdx; i++)
//		{
//			if (cardChar == m_cardSetText[cardSetId]._character[i])
//			{
//				sameCharCount++;
//			}
//		}
//
//		sprintf_s(string, "%s %c%d", m_cardSetText[cardSetId]._name, cardChar, sameCharCount);
//	}
//	else
//	{
//		// 유일한 문자라면 자체 문자만 출력한다.
//		sprintf_s(string, "%s %c", m_cardSetText[cardSetId]._name, cardChar);
//	}
//}

INT32 CQuestTextDictionary::GetCardSetTotalExp(UINT8 cardSetId) const
{
	return m_cardSetText[cardSetId]._totalExp;
}

INT32 CQuestTextDictionary::GetCardSetTotalPoint(UINT8 cardSetId) const
{
	return m_cardSetText[cardSetId]._totalPoint;
}

INT32 CQuestTextDictionary::GetCardSetBonusExp(UINT8 cardSetId) const
{
	return m_cardSetText[cardSetId]._bonusExp;
}

INT32 CQuestTextDictionary::GetCardSetBonusPoint(UINT8 cardSetId) const
{
	return m_cardSetText[cardSetId]._bonusPoint;
}

INT32 CQuestTextDictionary::GetCardSetBonusMasterBadgeCount(UINT8 cardSetId) const
{
	return m_cardSetText[cardSetId]._bonusBadgeMaster;
}

UINT32 CQuestTextDictionary::GetCardSetBonusItemID(UINT8 cardSetId, INT32 cnt) const
{
	if( cnt < 0 || cnt >= MAX_QUEST_REWARD_ITEM_COUNT)
	{
		I3FATAL( "overflow buffer count. please check");
	}

	return m_cardSetText[cardSetId]._bonusItemID[cnt];
}

i3::string CQuestTextDictionary::GetCardSetBonusItemTime(UINT8 cardSetId, INT32 cnt) const
{
	if( cnt < 0 || cnt >= MAX_QUEST_REWARD_ITEM_COUNT)
	{
		I3FATAL( "overflow buffer count. please check");
	}

	i3::string strText;

	const i3::string& str  = m_cardSetText[cardSetId]._vbonusItemTime[cnt];
	
	if( str.empty() )
	{
		i3::sprintf( strText, "설정 실패! [Q:%d,C:%d]", cardSetId, cnt);
		return i3::move(strText);
	}
	
	i3::vector< i3::string > timelist;
	i3::strtok_fast(timelist, str, '/', true);
//	StringUtil::Split( timelist, str, "/" );

	if( timelist.size() > 2 )
	{
		i3::sprintf( strText, "기간 실패! [Q:%d,C:%d]", cardSetId, cnt);
		return i3::move(strText);
	}

	UINT32 time = (UINT32) i3::atoi(timelist[0]); //		StringUtil::ToParse<UINT32>( timelist[0] );
	i3::to_lower( timelist[1] );
	if( timelist[1] == "day" )
	{
		i3::sprintf( strText, GAME_RCSTRING("STR_POPUP_USABLE_TERM"), time);
	}
	else if( timelist[1] == "time" )
	{
		i3::sprintf( strText, GAME_RCSTRING("STR_POPUP_USABLE_TIME"), time);
	}
	else if( timelist[1] == "count" )
	{
		i3::sprintf( strText, GAME_RCSTRING("STR_POPUP_USABLE_COUNT"), time);
	}
	else if( timelist[1] == "free" )
	{
		i3::sprintf( strText, GAME_RCSTRING("STR_POPUP_USABLE_FREE"));
	}
	else
	{
		i3::sprintf( strText, "종류 실패! [Q:%d,C:%d]", cardSetId, cnt);
	}

	return i3::move(strText);
}

INT32 CQuestTextDictionary::GetCardSetBonusItemIDCount(UINT8 cardSetId) const
{
	return m_cardSetText[cardSetId]._bonusItemCount;
}

void CQuestTextDictionary::GetRibbonNameString(char* string, const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_ribbonText[prizeId]._name, sizeStrMax );
}

void CQuestTextDictionary::GetRibbonDescriptionString(char* string, const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_ribbonText[prizeId]._description, sizeStrMax);
}

void CQuestTextDictionary::GetRibbonGainWayString(char* string, const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_ribbonText[prizeId]._gainWay, sizeStrMax);
}

void CQuestTextDictionary::GetEnsignNameString(char* string,const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_ensignText[prizeId]._name, sizeStrMax);
}

void CQuestTextDictionary::GetEnsignDescriptionString(char* string, const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_ensignText[prizeId]._description, sizeStrMax);
}

void CQuestTextDictionary::GetEnsignGainWayString(char* string, const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_ensignText[prizeId]._gainWay, sizeStrMax);
}

void CQuestTextDictionary::GetMedalNameString(char* string, const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_medalText[prizeId]._name, sizeStrMax);
}

void CQuestTextDictionary::GetMedalDescriptionString(char* string, const size_t sizeStrMax, UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_medalText[prizeId]._description, sizeStrMax);
}

void CQuestTextDictionary::GetMedalGainWayString(char* string, const size_t sizeStrMax,  UINT8 prizeId) const
{
	I3ASSERT(string);
	i3::safe_string_copy(string, m_medalText[prizeId]._gainWay, sizeStrMax);
}

UINT32 CQuestTextDictionary::GetQuestCardPrize( UINT32 cardSetID, UINT32 CardIndex )
{
	return m_cardPrizeItem[cardSetID][CardIndex];
}
