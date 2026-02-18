#include "stdafx.h"
#include "ItemIDMgr.h"

#include "ItemInfoMgr.h"
#include "ItemInfo.h"
#include "SubItemInfo.h"
#include "ItemData.h"
#include "PreprocessMessageCallback.h"

//
// 현재 ItemID의 해시분포가 썩 좋은 편은 아닌데..일단 방치하고...
// 문제가 될것 같다는 추정이 설때에만 해쉬함수를 변경한다..
//

ItemIDMgr::ItemIDMgr() : m_ItemIDMap( i3::unordered_map<INT32, ItemData*>::calculate_bucket_size(1000))
{
	
	
}

ItemIDMgr::~ItemIDMgr()
{

}

void	ItemIDMgr::Reset()
{
	m_ItemIDMap.clear();
}

// 분리 처리할까 하다가, 그냥 ItemInfoMgr 구현에 종속시킨다...이쪽이 일단 편할것 같다..
void	ItemIDMgr::Activate(PreprocessMessageCallback* cb)
{
	ItemIDMgr::Reset();

	cb->OnTextMessage(L"Gathering ITEMID from every item pef files.");
////////////////////////////////////////////////////////////////////////////////
	ItemInfoMgr* info_mgr =	ItemInfoMgr::i();

	for (int i = 0; i < NUM_ITEMINFO ; ++i)
	{
		ItemInfo* first_info = info_mgr->GetItemInfo((eItemInfo)i);
		
		const size_t num_second_info = first_info->GetNumSubItemInfo();

		for (size_t j = 0 ; j < num_second_info ; ++j)
		{
			SubItemInfo* second_info = first_info->GetSubItemInfo(j);

			const size_t num_item_data = second_info->GetNumItemData();
			
			for (size_t k = 0 ; k < num_item_data ; ++k)
			{
				second_info->GetItemData(k)->OnInsertItemIDMap(m_ItemIDMap);		// private변수에 기인해 그쪽 함수호출로 일단 처리한다..
			}
		}
	}
///////////////////////////////////////////////////////////////////////////		
	cb->OnTextMessage(L"Making relatedDataList using ITEMID list.");	
	
	for (int i = 0; i < NUM_ITEMINFO ; ++i)
	{
		ItemInfo* first_info = info_mgr->GetItemInfo((eItemInfo)i);

		const size_t num_second_info = first_info->GetNumSubItemInfo();

		for (size_t j = 0 ; j < num_second_info ; ++j)
		{
			SubItemInfo* second_info = first_info->GetSubItemInfo(j);

			const size_t num_item_data = second_info->GetNumItemData();

			for (size_t k = 0 ; k < num_item_data ; ++k)
			{
				second_info->GetItemData(k)->OnBuildRelatedDataList();		// private변수에 기인해 그쪽 함수호출로 일단 처리한다..
			}
		}
	}

}

// 아주 간단하게 처리된다...



ItemData*		ItemIDMgr::FindItemData(INT32 itemID) const
{
	i3::unordered_map<INT32, ItemData*>::const_iterator it = m_ItemIDMap.find(itemID);
	return ( it != m_ItemIDMap.end() ) ? it->second : NULL;
}

