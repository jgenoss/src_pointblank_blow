#include "pch.h"
#include "Ranking.h"


CRankingManager::CRankingManager()
: m_i32RankCount(0)
{
	for( INT32 i = 0; i < MAX_CLAN_NUMBER; ++i )
		m_pRankClans[i] = nullptr;
}

CRankingManager::~CRankingManager()
{

}

void CRankingManager::Initialize(INT32 nClanCount)
{
	m_i32RankCount = nClanCount;
}

void CRankingManager::InsertRanking(INT32 i32Ranking, CLAN_DETAIL_INFO_SERVER* pClan)
{
	if( i32Ranking < 0 || MAX_CLAN_NUMBER <= i32Ranking )
		return;

	m_i32RankCount++;
	m_pRankClans[i32Ranking] = pClan;
}

bool CRankingManager::MakePacketRankingPage(INT32 i32Page, i3NetworkPacket* pPacket)
{
	INT32 i32StartIdx = (i32Page - 1) * CLAN_NUM_PER_PAGE;
	for( INT32 i = 0; i < CLAN_NUM_PER_PAGE; ++i )
	{
		if( MAX_CLAN_NUMBER <= i32StartIdx + i )
			break;

		CLAN_DETAIL_INFO_SERVER* pClanInfo = m_pRankClans[i32StartIdx+i];

		CLAN_RANKING stClanRanking;
		stClanRanking.m_i32ClanID	= pClanInfo->_ClanDetailInfo.m_ui32CID;
		stClanRanking.m_i32Ranking	= (i32Page - 1) * CLAN_NUM_PER_PAGE + i + 1;
		stClanRanking.m_ui16OnlineMember = pClanInfo->_ClanDetailInfo.m_currentPerson;
		stClanRanking.m_ui16TotalMember = pClanInfo->_ClanDetailInfo.m_maxPerson;

		pPacket->WriteData(&stClanRanking, sizeof(stClanRanking));
	}

	return true;
}


void CRankingManager::UpdateRanking(CLAN_DETAIL_INFO_SERVER* pClanDetail, bool bRaiseRating)
{
	for( INT32 i = 0; i < m_i32RankCount; ++i )
	{
		if( m_pRankClans[i] != pClanDetail )
			continue;

		if( bRaiseRating )
		{
			for( INT32 j = i; j > 0; --j )
			{
				if( m_pRankClans[j]->_ClanDetailInfo.GetClanRating() < m_pRankClans[j-1]->_ClanDetailInfo.GetClanRating() )
					break;

				CLAN_DETAIL_INFO_SERVER* pTempClan = m_pRankClans[j];
				m_pRankClans[j] = m_pRankClans[j-1];
				m_pRankClans[j-1] = pTempClan;
			}
		}
		else
		{
			for( INT32 j = i; j < m_i32RankCount-1; ++j )
			{
				if( m_pRankClans[j+1]->_ClanDetailInfo.GetClanRating() < m_pRankClans[j]->_ClanDetailInfo.GetClanRating() )
					break;

				CLAN_DETAIL_INFO_SERVER* pTempClan = m_pRankClans[j];
				m_pRankClans[j] = m_pRankClans[j+1];
				m_pRankClans[j+1] = pTempClan;
			}
		}
	}
}
