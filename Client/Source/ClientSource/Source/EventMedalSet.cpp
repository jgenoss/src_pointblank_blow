#include "pch.h"
#include "MedalManager.h"
#include "EventMedalSet.h"

bool EventMedalSet::LoadMedalMQF()
{
	const char* szEventMedalPath = "Quest/Event/*.*";
	return LoadMedalMQFDir(szEventMedalPath);
}

void EventMedalSet::setMedalReward(UINT16 idx, UINT8 lv)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		pInfo->setMedalReward(1,true);
	}
}

INT32 EventMedalSet::getMedalTypeByIndex(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);
	if ( it != m_MedalMap.end() )
	{
		return MEDAL_SET_TYPE_EVENT;
	}
	return -1;
}