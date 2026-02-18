#include "pch.h"
#include "GiftShopContext.h"

GiftShopContext::GiftShopContext()
{
	for(INT32 idx = 0 ; idx < WEEK_MAX ; ++idx)
		m_GiftRankList[idx].clear();
}

GiftShopContext::~GiftShopContext()
{
	for(INT32 idx = 0 ; idx < WEEK_MAX ; ++idx)
		m_GiftRankList[idx].clear();
}

void GiftShopContext::_PushGiftRankData(INT32 i32Type, GIFT_RANKING & data)
{
	size_t count = m_GiftRankList[i32Type].size();

	for(size_t idx = 0 ; idx < count ; ++idx)
	{
		GIFT_RANKING & data2 = m_GiftRankList[i32Type].at(idx);

		// 중복된 랭킹이 있을 경우, 나중에 들어온 녀석을 다음 순위로 미룹니다.
		if(data.m_ui8Rank == data2.m_ui8Rank)
			data.m_ui8Rank += 1;
	}

	m_GiftRankList[i32Type].push_back(data);
}

GIFT_RANKING * GiftShopContext::_FindGiftRankingDataByIndex(INT32 i32Type, INT32 i32Idx)
{
	GIFT_RANKING * pData = nullptr;
	if( i32Idx < (INT32)(m_GiftRankList[i32Type].size()) )
		pData = &m_GiftRankList[i32Type].at(i32Idx);

	return pData;
}

GIFT_RANKING * GiftShopContext::_FindGiftRankingDataByRank(INT32 i32Type, INT32 i32Rank)
{
	size_t count = m_GiftRankList[i32Type].size();

	for(size_t idx = 0 ; idx < count ; ++idx)
	{
		GIFT_RANKING & data = m_GiftRankList[i32Type].at(idx);

		if( data.m_ui8Rank == i32Rank )
			return &data;
	}

	return nullptr;
}

GIFT_RANKING * GiftShopContext::_FindGiftRankingDataByGiftID(INT32 i32Type, UINT32 GiftID)
{
	size_t count = m_GiftRankList[i32Type].size();

	for(size_t idx = 0 ; idx < count ; ++idx)
	{
		GIFT_RANKING & data = m_GiftRankList[i32Type].at(idx);

		if( data.m_ui32GiftID == GiftID )
			return &data;
	}

	return nullptr;
}

void GiftShopContext::BuildGiftRankingData(GIFT_BUY_RANKING* RankInfo)
{
	if(RankInfo->m_ui8GiftCount == 0)
		return;

	i3::vector<GIFT_RANKING> temp_list;

	UINT32 last_week_date = 0;
	UINT32 this_week_date = 0;

	for( INT32 idx = 0 ; idx < RankInfo->m_ui8GiftCount ; ++idx )
	{
		GIFT_RANKING & Data = RankInfo->m_aGiftRanking[idx];

		if( Data.m_ui32Date > this_week_date )
		{
			last_week_date = this_week_date;
			this_week_date = Data.m_ui32Date;			
		}

		if( Data.m_ui32Date < this_week_date &&	Data.m_ui32Date > last_week_date )
			last_week_date = Data.m_ui32Date;

		temp_list.push_back(Data);
	}

	for( size_t idx = 0 ; idx < temp_list.size() ; ++idx)
	{
		if( temp_list[idx].m_ui32Date == last_week_date )
			_PushGiftRankData(LAST_WEEK, temp_list[idx]);

		if( temp_list[idx].m_ui32Date == this_week_date )
			_PushGiftRankData(THIS_WEEK, temp_list[idx]);
	}
}

UINT32 GiftShopContext::GetRankGiftID(INT32 i32Rank)
{
	GIFT_RANKING * pData = _FindGiftRankingDataByRank(THIS_WEEK, i32Rank);

	if(pData != nullptr)
		return pData->m_ui32GiftID;

	return 0;
}

INT32 GiftShopContext::GetRankType(INT32 i32Rank)
{
	GIFT_RANKING * pthis_data = _FindGiftRankingDataByRank(THIS_WEEK, i32Rank);

	if(pthis_data != nullptr)
	{
		GIFT_RANKING * plast_data = _FindGiftRankingDataByGiftID(LAST_WEEK, pthis_data->m_ui32GiftID);

		if(plast_data == nullptr)
			return GRT_NEW;

		if( pthis_data->m_ui8Rank > plast_data->m_ui8Rank )
			return GRT_DOWN;

		if( pthis_data->m_ui8Rank < plast_data->m_ui8Rank)
			return GRT_UP;

		return GRT_NONE;
	}

	return GRT_INIT;
}

INT32 GiftShopContext::GetRankAmount(INT32 i32Rank)
{
	GIFT_RANKING * pthis_data = _FindGiftRankingDataByRank(THIS_WEEK, i32Rank);

	if(pthis_data != nullptr)
	{
		GIFT_RANKING * plast_data = _FindGiftRankingDataByGiftID(LAST_WEEK, pthis_data->m_ui32GiftID);
		if(plast_data != nullptr)
		{
			INT32 i32Amount = i3Math::abs(plast_data->m_ui8Rank - pthis_data->m_ui8Rank);
			return i32Amount;
		}
	}

	return 0;
}