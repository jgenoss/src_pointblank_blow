#pragma once

	
class CRankingManager
{
public:
	void 				Initialize(INT32 nClanCount);
	void				InsertRanking(INT32 i32Ranking, CLAN_DETAIL_INFO_SERVER* pClan);

	void				UpdateRanking(CLAN_DETAIL_INFO_SERVER* pClanDetail, bool bRaiseRating);

	bool				MakePacketRankingPage(INT32 i32Page, i3NetworkPacket* pPacket);


private:
	void				ChangeRanking(INT32 i32Ranking, REAL32 r32ClanPoint, bool bRaiseScore);


public:
	CRankingManager();
	virtual ~CRankingManager();


private:
	INT32						m_i32RankCount;
	CLAN_DETAIL_INFO_SERVER*	m_pRankClans[MAX_CLAN_NUMBER];
};
